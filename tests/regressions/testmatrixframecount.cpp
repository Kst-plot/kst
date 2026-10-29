#include <QtTest>

#include "datamatrix.h"
#include "datasource.h"
#include "objectstore.h"
#include "testnextcolor.h"

namespace {

const qint64 largeFrameCount = (qint64(1) << 53) + 1;

class MatrixInterface : public Kst::DataSource::DataInterface<Kst::DataMatrix> {
public:
  mutable int infoCalls = 0;
  int readCalls = 0;

  int read(const QString &, Kst::DataMatrix::ReadInfo &) override {
    ++readCalls;
    return 0;
  }
  QStringList list() const override { return {"matrix"}; }
  bool isListComplete() const override { return true; }
  bool isValid(const QString &name) const override { return name == "matrix"; }
  const Kst::DataMatrix::DataInfo dataInfo(const QString &name, double = 0) const override {
    ++infoCalls;
    Kst::DataMatrix::DataInfo info;
    info.frameCount = isValid(name) ? largeFrameCount : 0;
    info.xSize = info.ySize = 0; // No sample exists at the Unix epoch.
    return info;
  }
  void setDataInfo(const QString &, const Kst::DataMatrix::DataInfo &) override {}
  QMap<QString, double> metaScalars(const QString &) override { return {}; }
  QMap<QString, QString> metaStrings(const QString &) override { return {}; }
};

class LegacySource : public Kst::DataSource {
public:
  explicit LegacySource(Kst::ObjectStore *store)
      : DataSource(store, nullptr, "matrix-test", "matrix-test"),
        interface(new MatrixInterface) {
    setInterface(interface);
    setUpdateType(None);
    _valid = true;
  }
  UpdateType internalDataSourceUpdate() override { return NoChange; }
  MatrixInterface *interface;
};

class MetadataSource : public LegacySource {
public:
  using LegacySource::LegacySource;
  qint64 matrixFrameCount(const QString &name) const override {
    return interface->isValid(name) ? largeFrameCount : 0;
  }
};

void attach(Kst::DataMatrixPtr matrix, Kst::DataSourcePtr source,
            const QString &field = "matrix") {
  matrix->change(source, field, 0, 0, -1, -1, false, false, 1, -1,
                 false, 0.0, 0.0, 1.0, 1.0);
}

} // namespace

class TestMatrixFrameCount : public QObject {
  Q_OBJECT
  TestNextColor colors;
private Q_SLOTS:
  void missingSource() {
    Kst::ObjectStore store;
    auto matrix = store.createObject<Kst::DataMatrix>();
    QCOMPARE(matrix->fileLength(), qint64(0));
  }

  void metadataLengthDoesNotSelectSample() {
    Kst::ObjectStore store;
    auto *source = new MetadataSource(&store);
    Kst::DataSourcePtr ownedSource(source);
    auto matrix = store.createObject<Kst::DataMatrix>();
    attach(matrix, ownedSource);
    // Construction legitimately queries dimensions. fileLength must not.
    source->interface->infoCalls = 0;
    for (qint64 frame : {qint64(-1), qint64(100), qint64(0)}) {
      matrix->setFrame(frame);
      QCOMPARE(matrix->fileLength(), largeFrameCount);
      QCOMPARE(matrix->frame(), frame);
    }
    QCOMPARE(source->interface->infoCalls, 0);
    QCOMPARE(source->interface->readCalls, 0);
  }

  void unknownFieldHasNoFrames() {
    Kst::ObjectStore store;
    auto *source = new MetadataSource(&store);
    Kst::DataSourcePtr ownedSource(source);
    auto matrix = store.createObject<Kst::DataMatrix>();
    attach(matrix, ownedSource, "missing");
    source->interface->infoCalls = 0;
    QCOMPARE(matrix->fileLength(), qint64(0));
    QCOMPARE(source->interface->infoCalls, 0);
  }

  void legacyFallbackRemainsAvailable() {
    Kst::ObjectStore store;
    auto *source = new LegacySource(&store);
    Kst::DataSourcePtr ownedSource(source);
    auto matrix = store.createObject<Kst::DataMatrix>();
    attach(matrix, ownedSource);
    source->interface->infoCalls = 0;
    QCOMPARE(matrix->fileLength(), largeFrameCount);
    QCOMPARE(source->interface->infoCalls, 1);
    QCOMPARE(source->interface->readCalls, 0);
  }
};

QTEST_MAIN(TestMatrixFrameCount)
#include "testmatrixframecount.moc"