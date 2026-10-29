// SSDB datasource: exposes database scalar streams and indexed matrix samples
// on a shared, epoch-based frame grid. The server resamples values with
// zero-order hold (ZOH); Kst rereads a short tail when late field writes arrive.
// INDEX (frame number) and TIME (Unix seconds at the frame boundary) are
// synthetic vectors and never request sample data from the database.
#include "ssdbsource.h"
#include "objectstore.h"

extern "C" {
#include <flightapi.h>
}

#include <QDoubleSpinBox>
#include <QDebug>
#include <QDomElement>
#include <QElapsedTimer>
#include <QFormLayout>
#include <QRegularExpression>
#include <QSettings>
#include <QUrl>
#include <QXmlStreamWriter>

#include <algorithm>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <limits>

using namespace Kst;

static const QString ssdbTypeString = "SST Telemetry Database Reader (SSDB)";
static const QString indexFieldString = "INDEX";
static const QString timeFieldString = "TIME";
static const bool profileSsdb = qEnvironmentVariableIsSet("KST_SSDB_PROFILE");

namespace {

// Release metadata and its C-owned strings, except for the empty-Vec sentinel.
void freeMetadata(metadata_t *meta, uint32_t count) {
  // Rust's zero-length Vec may return a non-null, non-allocatable dangling pointer.
  if (!meta || count == 0) return;
  for (uint32_t i = 0; i < count; ++i) {
    free(meta[i].name);
    free(meta[i].units);
    free(meta[i].quantity);
  }
  free(meta);
}

// Report a failing C API call without allocating an error string on success.
void logError(const QString &operation) {
  // telemetry_get_last_error currently allocates a new string on each call;
  // avoid calling it on the routine success path.
  const char *error = telemetry_get_last_error();
  qWarning() << "SSDB:" << operation << (error ? error : "unknown error");
}

class SsdbVector : public DataSource::DataInterface<DataVector> {
  public:
    // Adapt the source's vector implementation to Kst's data interface.
    explicit SsdbVector(SsdbSource &source) : _source(source) {}

    // Fetch frames, treating a failed read as an empty Kst read.
    int read(const QString &name, DataVector::ReadInfo &info) override {
      return qMax(0, _source.readVector(info.data, name, info));
    }

    // Include synthetic vectors alongside discovered scalar fields.
    QStringList list() const override { return _source.vectorList(); }

    // Field discovery returns the complete list for this connection.
    bool isListComplete() const override { return true; }

    // Check synthetic and registered vector names.
    bool isValid(const QString &name) const override { return _source.isVectorField(name); }

    // Report each vector's shared frame count and samples per frame.
    const DataVector::DataInfo dataInfo(const QString &name, double frame = 0) const override {
      Q_UNUSED(frame)
      return _source.vectorInfo(name);
    }

    // Vector shape is controlled by the datasource, not by callers.
    void setDataInfo(const QString &, const DataVector::DataInfo &) override {}

    // Supply frame count and, for database fields, nominal sampling metadata.
    QMap<QString, double> metaScalars(const QString &name) override {
      if (name == indexFieldString || name == timeFieldString)
        return {{"FRAMES", _source.frameCount()}};
      const SsdbSource::Field *field = _source.field(name);
      if (!field) return {};
      return {{"FRAMES", _source.frameCount()}, {"rate", field->rate},
              {"samplesPerFrame", double(field->samplesPerFrame)}};
    }

    // Attach units and quantity to database fields and the TIME vector.
    QMap<QString, QString> metaStrings(const QString &name) override {
      if (name == timeFieldString) return {{"units", "s"}, {"quantity", "Time"}};
      const SsdbSource::Field *field = _source.field(name);
      if (!field) return {};
      return {{"units", field->units}, {"quantity", field->quantity}};
    }

  private:
    SsdbSource &_source;
};

class SsdbMatrix : public DataSource::DataInterface<DataMatrix> {
  public:
    explicit SsdbMatrix(SsdbSource &source) : _source(source) {}

    int read(const QString &name, DataMatrix::ReadInfo &info) override {
      return _source.readMatrix(info.data, name, info);
    }

    QStringList list() const override { return _source.matrixNames(); }
    bool isListComplete() const override { return true; }
    bool isValid(const QString &name) const override { return _source.isMatrixField(name); }

    const DataMatrix::DataInfo dataInfo(const QString &name, double frame = 0) const override {
      if (!std::isfinite(frame) || std::floor(frame) != frame ||
          frame < double(LLONG_MIN) || frame >= -double(LLONG_MIN))
        return DataMatrix::DataInfo();
      return _source.matrixInfo(name, qint64(frame));
    }

    void setDataInfo(const QString &, const DataMatrix::DataInfo &) override {}

    QMap<QString, double> metaScalars(const QString &name) override {
      return _source.isMatrixField(name) ? QMap<QString, double>{{"FRAMES", _source.frameCount()}}
                                         : QMap<QString, double>{};
    }

    QMap<QString, QString> metaStrings(const QString &name) override {
      const SsdbSource::MatrixField *field = _source.matrixField(name);
      if (!field) return {};
      QMap<QString, QString> values;
      if (!field->units.isEmpty()) values.insert("z_units", field->units);
      if (!field->quantity.isEmpty()) values.insert("z_quantity", field->quantity);
      return values;
    }

  private:
    SsdbSource &_source;
};

class SsdbScalar : public DataSource::DataInterface<DataScalar> {
  public:
    // Expose the frame count as a Kst scalar.
    explicit SsdbScalar(SsdbSource &source) : _source(source) {}

    // Read the current number of frames.
    int read(const QString &name, DataScalar::ReadInfo &info) override {
      if (name != "FRAMES") return 0;
      *info.value = _source.frameCount();
      return 1;
    }

    // Advertise the frame-count scalar.
    QStringList list() const override { return {"FRAMES"}; }

    // The scalar set is fixed.
    bool isListComplete() const override { return true; }

    // Reject names other than FRAMES.
    bool isValid(const QString &name) const override { return name == "FRAMES"; }

    // FRAMES has no per-frame data description.
    const DataScalar::DataInfo dataInfo(const QString &, double frame = 0) const override {
      Q_UNUSED(frame)
      return {};
    }

    // Scalar shape cannot be changed by callers.
    void setDataInfo(const QString &, const DataScalar::DataInfo &) override {}

    // FRAMES has no additional numeric metadata.
    QMap<QString, double> metaScalars(const QString &) override { return {}; }

    // FRAMES has no additional string metadata.
    QMap<QString, QString> metaStrings(const QString &) override { return {}; }

  private:
    SsdbSource &_source;
};

class SsdbString : public DataSource::DataInterface<DataString> {
  public:
    // Expose the connection address as a Kst string.
    explicit SsdbString(SsdbSource &source) : _source(source) {}

    // Read the resolved server address.
    int read(const QString &name, DataString::ReadInfo &info) override {
      if (name != "FILE") return 0;
      *info.value = _source.address();
      return 1;
    }

    // Advertise the address string.
    QStringList list() const override { return {"FILE"}; }

    // The string set is fixed.
    bool isListComplete() const override { return true; }

    // Reject names other than FILE.
    bool isValid(const QString &name) const override { return name == "FILE"; }

    // FILE has no per-frame data description.
    const DataString::DataInfo dataInfo(const QString &, double frame = 0) const override {
      Q_UNUSED(frame)
      return {};
    }

    // String shape cannot be changed by callers.
    void setDataInfo(const QString &, const DataString::DataInfo &) override {}

    // FILE has no additional numeric metadata.
    QMap<QString, double> metaScalars(const QString &) override { return {}; }

    // FILE has no additional string metadata.
    QMap<QString, QString> metaStrings(const QString &) override { return {}; }

  private:
    SsdbSource &_source;
};

class SsdbConfigWidget : public DataSourceConfigWidget {
  public:
    // Build controls for the shared grid and late-correction window.
    explicit SsdbConfigWidget(QSettings &settings, const QString &address)
        : DataSourceConfigWidget(settings), _address(address) {
      auto *layout = new QFormLayout(this);
      _rate = new QDoubleSpinBox(this);
      _rate->setRange(0.001, 1000000.0);
      _rate->setDecimals(3);
      _rate->setValue(5.0);
      _rate->setSuffix(" Hz");
      layout->addRow(tr("Frame rate:"), _rate);

      _delay = new QDoubleSpinBox(this);
      _delay->setRange(0.0, 3600000.0);
      _delay->setDecimals(3);
      _delay->setValue(100.0);
      _delay->setSuffix(" ms");
      layout->addRow(tr("End delay:"), _delay);

      _repair = new QDoubleSpinBox(this);
      _repair->setRange(0.0, 60000.0);
      _repair->setDecimals(0);
      _repair->setValue(2000.0);
      _repair->setSuffix(" ms");
      layout->addRow(tr("Late correction window:"), _repair);
    }

    // Load saved settings, then prefer a connected source's active settings.
    void load() override {
      settings().beginGroup("SSDB");
      settings().beginGroup(_address);
      _rate->setValue(settings().value("frameRateHz", 5.0).toDouble());
      _delay->setValue(settings().value("endDelayMs", 100.0).toDouble());
      _repair->setValue(settings().value("repairWindowMs", 2000.0).toDouble());
      settings().endGroup();
      settings().endGroup();

      if (hasInstance()) {
        auto src = Kst::kst_cast<SsdbSource>(instance());
        _rate->setValue(src->frameRate());
        _delay->setValue(src->delayMs());
        _repair->setValue(src->repairWindowMs());
      }
    }

    // Persist settings and apply them to the active source if one exists.
    void save() override {
      if (!isOkAcceptabe()) return;
      settings().beginGroup("SSDB");
      settings().beginGroup(_address);
      settings().setValue("frameRateHz", _rate->value());
      settings().setValue("endDelayMs", _delay->value());
      settings().setValue("repairWindowMs", _repair->value());
      settings().endGroup();
      settings().endGroup();

      if (hasInstance()) {
        auto src = Kst::kst_cast<SsdbSource>(instance());
        src->setGrid(_rate->value(), _delay->value());
        src->setRepairWindow(_repair->value());
      }
    }

    // Accept only rates that can be represented by a nanosecond grid.
    bool isOkAcceptabe() const override { return SsdbSource::validRate(_rate->value()); }

  private:
    QString _address;
    QDoubleSpinBox *_rate;
    QDoubleSpinBox *_delay;
    QDoubleSpinBox *_repair;
};

// Accept either automatic type detection or the explicit SSDB plugin type.
bool validType(const QString &type) { return type.isEmpty() || type == ssdbTypeString; }

// Identify SSDB addresses during datasource discovery.
bool available(const QString &name, const QString &type, QString *suggestion, bool *complete) {
  if (suggestion) *suggestion = ssdbTypeString;
  const bool ok = validType(type) && !SsdbSource::endpoint(name).isEmpty();
  if (complete) *complete = ok;
  return ok;
}
} // namespace

// Normalize URL or host:port syntax for the Rust consumer.
QString SsdbSource::endpoint(const QString &source) {
  QString host;
  int port = -1;
  if (source.startsWith("ssdb://", Qt::CaseInsensitive)) {
    QUrl url(source);
    if (!url.isValid() || url.scheme().compare("ssdb", Qt::CaseInsensitive) ||
        !url.userInfo().isEmpty() || !url.path().isEmpty() ||
        !url.query().isEmpty() || !url.fragment().isEmpty()) return {};
    host = url.host();
    port = url.port(-1);
  } else {
    static const QRegularExpression pattern("^((?:\\[[0-9a-fA-F:]+\\])|(?:[A-Za-z0-9.-]+)):([0-9]{1,5})$");
    auto match = pattern.match(source);
    if (!match.hasMatch()) return {};
    host = match.captured(1);
    port = match.captured(2).toInt();
  }
  if (host.isEmpty() || port < 1 || port > 65535) return {};
  // Consumer::new picks only the first resolved address. On dual-stack hosts
  // localhost often resolves to ::1 first, whereas SSDB listens on IPv4.
  if (host.compare("localhost", Qt::CaseInsensitive) == 0) host = "127.0.0.1";
  if (host.contains(':') && !host.startsWith('[')) host = '[' + host + ']';
  return host + ':' + QString::number(port);
}

// Require a positive frame period that fits the supported rate range.
bool SsdbSource::validRate(double rate) {
  return std::isfinite(rate) && rate >= 0.001 && rate <= 1000000.0 &&
         std::llround(1.0e9 / rate) >= 1;
}

// Restore per-address/session settings before connecting and discovering fields.
SsdbSource::SsdbSource(ObjectStore *store, QSettings *cfg, const QString &filename,
                       const QString &type, const QDomElement &element)
    : DataSource(store, cfg, filename, type), _address(endpoint(filename)) {
  setInterface(new SsdbVector(*this));
  setInterface(new SsdbMatrix(*this));
  setInterface(new SsdbScalar(*this));
  setInterface(new SsdbString(*this));
  _valid = false;
  if (!validType(type) || _address.isEmpty()) return;
  if (cfg) {
    cfg->beginGroup("SSDB");
    cfg->beginGroup(_address);
    const double rate = cfg->value("frameRateHz", 5.0).toDouble();
    const double delay = cfg->value("endDelayMs", 100.0).toDouble();
    const double repair = cfg->value("repairWindowMs", 2000.0).toDouble();
    cfg->endGroup();
    cfg->endGroup();
    setGrid(rate, delay);
    setRepairWindow(repair);
  }

  if (!element.isNull()) {
    QXmlStreamAttributes attrs;
    attrs.append("frameRateHz", element.attribute("frameRateHz"));
    attrs.append("endDelayMs", element.attribute("endDelayMs"));
    attrs.append("repairWindowMs", element.attribute("repairWindowMs"));
    parseProperties(attrs);
  }
  _valid = init();
  registerChange();
}

// Release the database consumer and field handles.
SsdbSource::~SsdbSource() { close(); }

// Drop the current connection and cached field registration state.
void SsdbSource::close() {
  for (auto &field : _fields) free(field.token);
  for (auto &field : _matrixFields) free(field.token);
  _fields.clear();
  _matrixFields.clear();
  _matrixCache.clear();
  _changedFields.clear();
  free(_consumer);
  _consumer = nullptr;
  _frameCount = 0;
  _latestTime = 0;
}

// Connect and register supported scalar and matrix fields.
bool SsdbSource::init() {
  close();
  _consumer = consumer_new("kst ssdb reader", _address.toUtf8().constData());
  if (!_consumer) {
    logError("connecting to " + _address);
    return false;
  }

  uint32_t count = 0;
  metadata_t *metadata = consumer_get_field_list(_consumer, &count);
  if (!metadata) {
    logError("listing fields");
    close();
    return false;
  }

  for (uint32_t i = 0; i < count; ++i) {
    const metadata_t &meta = metadata[i];
    if (!meta.name) continue;
    QString name = QString::fromUtf8(meta.name);
    if (meta.structure == 0) {
      if (name == indexFieldString || name == timeFieldString) {
        qWarning() << "SSDB: field" << name << "is reserved for Kst's synthetic vectors";
        continue;
      }
      if (_fields.contains(name)) continue;
      scalar_token_t *token = consumer_register_scalar(_consumer, meta.name);
      if (!token) {
        logError("registering " + name);
        continue;
      }

      Field field;
      field.token = token;
      field.rate = meta.rate;
      // Write-time statistics drive field-change detection, not sample age.
      field.lastTime = meta.last_time;
      field.units = meta.units ? QString::fromUtf8(meta.units) : QString();
      field.quantity = meta.quantity ? QString::fromUtf8(meta.quantity) : QString();
      if (std::isfinite(meta.rate) && meta.rate > 0) {
        // Match nominal field density to the common frame grid.
        const double spf = std::round(meta.rate / _frameRate);
        field.samplesPerFrame = int(qBound(1.0, spf, 1000000.0));
      }
      _fields.insert(name, field);
    } else if (meta.structure == 2 && !_matrixFields.contains(name)) {
      matrix_token_t *token = consumer_register_matrix(_consumer, meta.name);
      if (!token) {
        logError("registering matrix " + name);
        continue;
      }
      MatrixField field;
      field.token = token;
      field.lastTime = meta.last_time;
      field.units = meta.units ? QString::fromUtf8(meta.units) : QString();
      field.quantity = meta.quantity ? QString::fromUtf8(meta.quantity) : QString();
      _matrixFields.insert(name, field);
    }
  }
  freeMetadata(metadata, count);

  const qint64 latest = consumer_get_latest_time(_consumer);
  if (latest > 0) updateFrameCount(latest);
  return true;
}

// Reconnect and tell Kst that cached datasource state is invalid.
void SsdbSource::reset() {
  _valid = init();
  Object::reset();
}

// Map the latest database timestamp, less the display delay, onto epoch frames.
void SsdbSource::updateFrameCount(qint64 latest) {
  _latestTime = latest;
  const qint64 end = latest - _delayNs;
    _frameCount = end >= _originNs && latest > 0
      ? (end - _originNs) / _framePeriodNs + 1 : 0;
}

// Advance the grid and flag fields whose writes may change plotted values.
DataSource::UpdateType SsdbSource::internalDataSourceUpdate() {
  if (!_consumer) return NoChange;
  QElapsedTimer timer;
  if (profileSsdb) timer.start();
  _changedFields.clear();
  const qint64 latest = consumer_get_latest_time(_consumer);
  const qint64 latestUs = profileSsdb ? timer.nsecsElapsed() / 1000 : 0;
  if (latest <= 0) return NoChange; // No data, or connection failure; don't erase live data.

  const double previous = _frameCount;
  if (latest < _latestTime) {
    reset();
    return Updated;
  }
  updateFrameCount(latest);
  _matrixCache.clear();

  // Field data can arrive after the latest frame was first exposed. Check
  // published field progress even when the global frame count has not moved.
  // Newly registered fields still wait for reconnect.
  uint32_t count = 0;
  metadata_t *metadata = consumer_get_field_list(_consumer, &count);
  for (uint32_t i = 0; metadata && i < count; ++i) {
    if (metadata[i].name) {
      const QString name = QString::fromUtf8(metadata[i].name);
      auto it = _fields.find(name);
      if (it != _fields.end() && it->lastTime != metadata[i].last_time) {
        it->lastTime = metadata[i].last_time;
        _changedFields.insert(name);
      }
      auto matrixIt = _matrixFields.find(name);
      if (matrixIt != _matrixFields.end() && matrixIt->lastTime != metadata[i].last_time) {
        matrixIt->lastTime = metadata[i].last_time;
        _changedFields.insert(name);
        _matrixCache.remove(name);
      }
    }
  }
  freeMetadata(metadata, count);

  if (profileSsdb) {
    qInfo() << "SSDB update" << _address << "latest ns" << latest
            << "frames" << _frameCount << "changed fields" << _changedFields.size()
          << "latest us" << latestUs << "metadata us" << timer.nsecsElapsed() / 1000 - latestUs;
  }
  return _frameCount != previous || !_changedFields.isEmpty() ? Updated : NoChange;
}

// Rewind only changed fields by a bounded number of already plotted frames.
int SsdbSource::framesToRecheck(const QString &name) const {
  if (!_changedFields.contains(name) || _repairWindowMs == 0) return 0;
  // This is a repair window, not a display delay. It bounds the amount of
  // previously plotted data revisited when a field arrives asynchronously.
  return int(qBound(1.0, std::ceil(_repairWindowMs * _frameRate / 1000.0), 4096.0));
}

// Validate the maximum age of cached frames eligible for rereading.
void SsdbSource::setRepairWindow(double windowMs) {
  if (std::isfinite(windowMs) && windowMs >= 0 && windowMs <= 60000.0)
    _repairWindowMs = windowMs;
}

// Apply grid settings; a new rate changes frame indexing and requires a reset.
void SsdbSource::setGrid(double rate, double delayMs) {
  if (!validRate(rate) || !std::isfinite(delayMs) || delayMs < 0 || delayMs > 3600000.0) return;
  const qint64 period = qint64(std::llround(1.0e9 / rate));
  const bool changed = period != _framePeriodNs;
  _frameRate = rate;
  _framePeriodNs = period;
  _delayMs = delayMs;
  _delayNs = qint64(std::llround(delayMs * 1000000.0));
  if (changed && _consumer) {
    reset();
    store()->resetDataSourceDependents(fileName());
  } else {
    _matrixCache.clear();
    updateFrameCount(_latestTime);
    registerChange();
  }
}

// Return the name used to identify this datasource type.
QString SsdbSource::fileType() const { return ssdbTypeString; }

// Return the human-readable datasource type.
QString SsdbSource::typeString() const { return ssdbTypeString; }

// Save per-source settings with the Kst session.
void SsdbSource::save(QXmlStreamWriter &writer) {
  writer.writeAttribute("frameRateHz", QString::number(_frameRate, 'g', 16));
  writer.writeAttribute("endDelayMs", QString::number(_delayMs, 'g', 16));
  writer.writeAttribute("repairWindowMs", QString::number(_repairWindowMs, 'g', 16));
}

// Restore settings from the Kst session, retaining defaults for missing keys.
void SsdbSource::parseProperties(QXmlStreamAttributes &properties) {
  bool rateOk = false, delayOk = false, repairOk = false;
  const double rate = properties.value("frameRateHz").toString().toDouble(&rateOk);
  const double delay = properties.value("endDelayMs").toString().toDouble(&delayOk);
  const double repair = properties.value("repairWindowMs").toString().toDouble(&repairOk);
  setGrid(rateOk ? rate : _frameRate, delayOk ? delay : _delayMs);
  if (repairOk) setRepairWindow(repair);
}

// Report whether the grid has any available frames.
bool SsdbSource::isEmpty() const { return _frameCount == 0; }

// List synthetic vectors first, then registered database scalar fields.
QStringList SsdbSource::vectorList() const {
  QStringList names = _fields.keys();
  names.prepend(timeFieldString);
  names.prepend(indexFieldString);
  return names;
}

// Recognize synthetic names without requiring a matching database token.
bool SsdbSource::isVectorField(const QString &name) const {
  return name == indexFieldString || name == timeFieldString || _fields.contains(name);
}

// Look up a registered database scalar field.
const SsdbSource::Field *SsdbSource::field(const QString &name) const {
  auto it = _fields.constFind(name);
  return it == _fields.cend() ? nullptr : &it.value();
}

QStringList SsdbSource::matrixNames() const { return _matrixFields.keys(); }

bool SsdbSource::isMatrixField(const QString &name) const {
  return _matrixFields.contains(name);
}

const SsdbSource::MatrixField *SsdbSource::matrixField(const QString &name) const {
  auto it = _matrixFields.constFind(name);
  return it == _matrixFields.cend() ? nullptr : &it.value();
}

// Expose the common frame count to Kst's vector and scalar interfaces.
double SsdbSource::frameCount() const { return _frameCount; }

// Synthetic vectors have one value per frame; fields use their nominal density.
DataVector::DataInfo SsdbSource::vectorInfo(const QString &name) const {
  if (name == indexFieldString || name == timeFieldString)
    return DataVector::DataInfo(_frameCount, 1);
  const Field *f = field(name);
  return f ? DataVector::DataInfo(_frameCount, f->samplesPerFrame) : DataVector::DataInfo();
}

bool SsdbSource::frameTime(qint64 frame, qint64 *time) const {
  if (!time || frame < 0 || _framePeriodNs <= 0 ||
      frame > (LLONG_MAX - _originNs) / _framePeriodNs) return false;
  *time = _originNs + frame * _framePeriodNs;
  return true;
}

bool SsdbSource::selectMatrix(const QString &name, qint64 frame, MatrixSample *sample) const {
  if (!sample || !_consumer) return false;
  const MatrixField *field = matrixField(name);
  if (!field) return false;

  auto cached = _matrixCache.constFind(name);
  if (cached != _matrixCache.cend() && cached->frame == frame) {
    *sample = cached->sample;
    return true;
  }

  qint64 time = 0;
  if (!frameTime(frame, &time)) return false;
  if (!consumer_select_resampled_window(_consumer, 1, time)) {
    logError("selecting matrix sample");
    return false;
  }

  matrix_t *selected = consumer_get_selected_matrix(_consumer, field->token, 1);
  if (!selected) {
    logError("reading matrix sample");
    return false;
  }

  MatrixSample result;
  const uintptr_t rows = selected[0].nrows;
  const uintptr_t columns = selected[0].ncols;
  const bool sizeValid = rows <= uintptr_t(INT_MAX) && columns <= uintptr_t(INT_MAX) &&
      (!rows || columns <= std::numeric_limits<uintptr_t>::max() / rows) &&
      rows * columns <= uintptr_t(INT_MAX);
  if (sizeValid && rows && columns && selected[0].data) {
    result.nrows = int(rows);
    result.ncols = int(columns);
    const int count = int(rows * columns);
    result.values.resize(count);
    std::copy_n(selected[0].data, count, result.values.data());
  } else if (!sizeValid || (rows && columns && !selected[0].data)) {
    if (selected[0].data && rows && columns) free(selected[0].data);
    free(selected);
    logError("invalid matrix result");
    return false;
  }

  if (selected[0].data && rows && columns) free(selected[0].data);
  free(selected);
  _matrixCache.insert(name, CachedMatrix{frame, result});
  *sample = result;
  return true;
}

DataMatrix::DataInfo SsdbSource::matrixInfo(const QString &name, qint64 frame) const {
  DataMatrix::DataInfo info;
  if (!isMatrixField(name) || _frameCount <= 0 || frame >= _frameCount) return info;

  info.frameCount = _frameCount;
  const qint64 selectedFrame = frame < 0 ? _frameCount - 1 : frame;
  if (selectedFrame < 0 || selectedFrame >= _frameCount) return info;

  MatrixSample sample;
  if (selectMatrix(name, selectedFrame, &sample)) {
    info.xSize = sample.ncols;
    info.ySize = sample.nrows;
  } else {
    // A 0x0 ZOH result means this matrix has no sample at or before the frame.
    info.xSize = 0;
    info.ySize = 0;
  }
  return info;
}

DataMatrix::DataInfo SsdbSource::matrixDataInfo(const QString &name, qint64 frame) const {
  return matrixInfo(name, frame);
}

qint64 SsdbSource::matrixFrameCount(const QString &name) const {
  return isMatrixField(name) ? _frameCount : 0;
}

int SsdbSource::readMatrix(MatrixData *data, const QString &name,
                           const DataMatrix::ReadInfo &info) {
  if (info.skip > 0) return -9999;
  const qint64 requested = qint64(info.xNumSteps) * info.yNumSteps;
  const auto clearRequested = [&]() {
    if (data && data->z && requested > 0 && requested <= INT_MAX)
      std::fill_n(data->z, int(requested), std::numeric_limits<double>::quiet_NaN());
    if (data) {
      data->xMin = info.xStart;
      data->yMin = info.yStart;
      data->xStepSize = 1;
      data->yStepSize = 1;
    }
  };
  if (!data || !data->z || info.frame < 0 || info.xStart < 0 || info.yStart < 0 ||
      info.xNumSteps <= 0 || info.yNumSteps <= 0 || !_consumer ||
      !isMatrixField(name) || info.frame >= _frameCount) {
    clearRequested();
    return 0;
  }

  MatrixSample sample;
  if (!selectMatrix(name, info.frame, &sample) || sample.nrows == 0 || sample.ncols == 0 ||
      info.xStart >= sample.ncols || info.yStart >= sample.nrows) {
    clearRequested();
    return 0;
  }

  const int width = qMin(info.xNumSteps, sample.ncols - info.xStart);
  const int height = qMin(info.yNumSteps, sample.nrows - info.yStart);
  int copied = 0;
  // Kst's Matrix stores z values as x * ySize + y; SSDB's Matrix is row-major.
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      const int source = (info.yStart + y) * sample.ncols + info.xStart + x;
      data->z[copied++] = sample.values[source];
    }
  }

  data->xMin = info.xStart;
  data->yMin = info.yStart;
  data->xStepSize = 1;
  data->yStepSize = 1;
  return copied;
}

// Fetch one server-resampled (ZOH) value for a trailing local hold.
bool SsdbSource::selectValue(scalar_token_t *token, qint64 time, double *value) {
  if (!consumer_select_resampled_window(_consumer, 1, time)) {
    logError("selecting scalar sample");
    return false;
  }
  double *result = consumer_get_selected_scalar(_consumer, token, 1);
  if (!result) {
    logError("reading scalar sample");
    return false;
  }
  *value = result[0];
  free(result);
  return true;
}

// Read frame-aligned vectors: synthesize INDEX/TIME or query scalar samples.
int SsdbSource::readVector(double *data, const QString &name, const DataVector::ReadInfo &info) {
  if (!data || !std::isfinite(info.startingFrame) ||
      info.startingFrame < 0) return -1;
  const bool one = info.singleSample || info.numberOfFrames < 0;
  if (!one && (!std::isfinite(info.numberOfFrames) || info.numberOfFrames <= 0)) return -1;
  if (info.startingFrame >= _frameCount) return 0;
  // SSDB samples live on integer frame boundaries. Truncate fractional starts
  // and lengths to whole frames so reads stay aligned to the shared grid.
  const qint64 first = qint64(info.startingFrame);
  const double frames = one ? 1 : qMin(std::trunc(info.numberOfFrames),
                                       double(_frameCount - first));
  if (frames <= 0) return 0;

  // INDEX and TIME share frame boundaries and need no database requests.
  if (name == indexFieldString || name == timeFieldString) {
    if (frames > INT_MAX) return -1;
    const int count = int(frames);
    if (name == indexFieldString) {
      for (int i = 0; i < count; ++i) data[i] = first + i;
    } else {
      if (first > (LLONG_MAX - _originNs) / _framePeriodNs - (count - 1)) return -1;
      for (int i = 0; i < count; ++i)
        data[i] = double(_originNs + (first + i) * _framePeriodNs) / 1.0e9;
    }
    return count;
  }

  const Field *f = field(name);
  if (!_consumer || !f) return -1;
  const double values = one ? 1 : frames * f->samplesPerFrame;
  if (values > INT_MAX ||
      double(first) > double(LLONG_MAX / _framePeriodNs) - frames) return -1;

  const int spf = f->samplesPerFrame;
  const qint64 step = _framePeriodNs / spf;
  if (!step) return -1;
  const int frameCount = int(frames);
  int copied = 0;

  QElapsedTimer timer;
  if (profileSsdb) timer.start();
  qint64 selectUs = 0;
  qint64 readUs = 0;
  int requests = 0;

  // Keep remote reads bounded; each request resamples at most 4096 values.
  const int chunkFrames = _framePeriodNs % spf == 0 ? qMax(1, 4096 / spf) : 1;
  for (int frame = 0; frame < frameCount;) {
    const int nframes = one ? 1 : qMin(chunkFrames, frameCount - frame);
    const int n = one ? 1 : nframes * spf;
    if (first > (LLONG_MAX - _originNs) / _framePeriodNs - frame) return -1;
    const qint64 time = _originNs + (first + frame) * _framePeriodNs;
    if (qint64(n - 1) > (LLONG_MAX - time) / step) return -1;
    const qint64 last = f->lastTime;
    const int available = last > 0 ?
        (time > last ? 0 : int(qMin(qint64(n), (last - time) / step + 1))) : n;
    // After the last reported field write, query once and hold locally.
    if (!available) {
      double held = 0;
      if (!selectValue(f->token, last, &held)) return -1;
      std::fill_n(data + copied, n, held);
      copied += n;
      frame += nframes;
      continue;
    }

    const qint64 end = time + qint64(available - 1) * step;
    const uint64_t duration = uint64_t(available) * uint64_t(step);
    // The database does ZOH over its stored samples at these grid times.
    const qint64 selectStarted = profileSsdb ? timer.nsecsElapsed() / 1000 : 0;
    if (!consumer_select_resampled_window(_consumer, duration, end)) {
      logError("selecting scalar window");
      return -1;
    }
    if (profileSsdb) selectUs += timer.nsecsElapsed() / 1000 - selectStarted;
    const qint64 readStarted = profileSsdb ? timer.nsecsElapsed() / 1000 : 0;
    double *samples = consumer_get_selected_scalar(_consumer, f->token, available);
    if (!samples) {
      logError("reading scalar window");
      return -1;
    }
    if (profileSsdb) {
      readUs += timer.nsecsElapsed() / 1000 - readStarted;
      ++requests;
    }
    std::copy_n(samples, available, data + copied);
    free(samples);
    if (available < n) {
      double held = 0;
      if (!selectValue(f->token, last, &held)) return -1;
      std::fill_n(data + copied + available, n - available, held);
    }
    copied += n;
    frame += nframes;
  }

  if (profileSsdb) {
    qInfo() << "SSDB read" << name << "frames" << frameCount << "values" << copied
          << "windows" << requests << "select us" << selectUs
          << "read us" << readUs << "total us" << timer.nsecsElapsed() / 1000;
  }
  return copied;
}

namespace {

QStringList fieldsAtStructure(const QString &source, uint8_t structure,
                              bool includeSyntheticVectors) {
  QString addr = SsdbSource::endpoint(source);
  if (addr.isEmpty()) return {};
  consumer_t *consumer = consumer_new("kst ssdb field discovery", addr.toUtf8().constData());
  if (!consumer) {
    logError("connecting to " + addr);
    return {};
  }

  uint32_t count = 0;
  metadata_t *meta = consumer_get_field_list(consumer, &count);
  if (!meta) logError("listing fields at " + addr);
  QStringList fields;
  for (uint32_t i = 0; meta && i < count; ++i)
    if (meta[i].structure == structure && meta[i].name &&
        QString::fromUtf8(meta[i].name) != indexFieldString &&
        QString::fromUtf8(meta[i].name) != timeFieldString)
      fields << QString::fromUtf8(meta[i].name);
  if (meta && includeSyntheticVectors) {
    fields.prepend(timeFieldString);
    fields.prepend(indexFieldString);
  }

  freeMetadata(meta, count);
  free(consumer);
  return fields;
}

} // namespace

// Discover scalar fields and synthetic vectors for Kst's field chooser.
QStringList SsdbSource::fieldsAt(const QString &source) {
  return fieldsAtStructure(source, 0, true);
}

// Discover matrix streams without mixing them into the vector field list.
QStringList SsdbSource::matrixFieldsAt(const QString &source) {
  return fieldsAtStructure(source, 2, false);
}

// Identify the plugin in Kst's datasource menu.
QString SsdbSourcePlugin::pluginName() const { return tr("SSDB Reader"); }

// Summarize the datasource's scalar and indexed matrix behavior.
QString SsdbSourcePlugin::pluginDescription() const {
  return tr("SSDB scalar time-series and indexed matrices (zero-order hold resampling)");
}

// Construct a datasource for an SSDB endpoint.
DataSource *SsdbSourcePlugin::create(ObjectStore *store, QSettings *cfg,
                                    const QString &name, const QString &type,
                                    const QDomElement &element) const {
  return new SsdbSource(store, cfg, name, type, element);
}

QStringList SsdbSourcePlugin::matrixList(QSettings *, const QString &name, const QString &type,
                                         QString *suggestion, bool *complete) const {
  return available(name, type, suggestion, complete) ? SsdbSource::matrixFieldsAt(name)
                                                     : QStringList();
}

// Discover scalar fields and synthetic vectors for Kst's field chooser.
QStringList SsdbSourcePlugin::fieldList(QSettings *, const QString &name, const QString &type,
                                        QString *suggestion, bool *complete) const {
  return available(name, type, suggestion, complete) ? SsdbSource::fieldsAt(name) : QStringList();
}

// Advertise the FRAMES scalar for recognized SSDB endpoints.
QStringList SsdbSourcePlugin::scalarList(QSettings *, const QString &name, const QString &type,
                                         QString *suggestion, bool *complete) const {
  return available(name, type, suggestion, complete) ? QStringList{"FRAMES"} : QStringList();
}

// Advertise the FILE string for recognized SSDB endpoints.
QStringList SsdbSourcePlugin::stringList(QSettings *, const QString &name, const QString &type,
                                         QString *suggestion, bool *complete) const {
  return available(name, type, suggestion, complete) ? QStringList{"FILE"} : QStringList();
}

// Rank well-formed SSDB URLs above generic datasource plugins.
int SsdbSourcePlugin::understands(QSettings *, const QString &name) const {
  return SsdbSource::endpoint(name).isEmpty() ? 0 : 95;
}

bool SsdbSourcePlugin::isNonFileSource(const QString &name) const {
  return !SsdbSource::endpoint(name).isEmpty();
}

// Declare that the source supplies a time axis to Kst.
bool SsdbSourcePlugin::supportsTime(QSettings *, const QString &) const { return true; }

// Report this plugin's supported datasource type.
QStringList SsdbSourcePlugin::provides() const { return {ssdbTypeString}; }

// Open the endpoint-specific configuration controls when settings are available.
DataSourceConfigWidget *SsdbSourcePlugin::configWidget(QSettings *cfg, const QString &name) const {
  return cfg ? new SsdbConfigWidget(*cfg, SsdbSource::endpoint(name)) : nullptr;
}
