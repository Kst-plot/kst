#ifndef SSDBSOURCE_H
#define SSDBSOURCE_H

#include <datasource.h>
#include <dataplugin.h>
#include <QMap>
#include <QSet>
#include <QVector>

struct consumer_t;
struct scalar_token_t;
struct matrix_token_t;

class SsdbSource : public Kst::DataSource {
  Q_OBJECT

  public:
    struct Field {
      scalar_token_t *token = nullptr;
      int samplesPerFrame = 1;
      double rate = 0;
      QString units;
      QString quantity;
      qint64 lastTime = 0;
    };

    struct MatrixField {
      matrix_token_t *token = nullptr;
      QString units;
      QString quantity;
      qint64 lastTime = 0;
    };

    struct MatrixSample {
      int nrows = 0;
      int ncols = 0;
      QVector<double> values;
    };

    SsdbSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename,
               const QString& type, const QDomElement& element);
    ~SsdbSource() override;

    bool init();
    void reset() override;
    UpdateType internalDataSourceUpdate() override;
    int framesToRecheck(const QString &field) const override;
    QString fileType() const override;
    QString typeString() const override;
    void save(QXmlStreamWriter &writer) override;
    void parseProperties(QXmlStreamAttributes &properties) override;
    bool isEmpty() const override;
    bool isImageStream(QString field) override { return isMatrixField(field); }
    Kst::DataMatrix::DataInfo matrixDataInfo(const QString &field, qint64 frame) const override;
    qint64 matrixFrameCount(const QString &field) const override;

    QStringList vectorList() const;
    bool isVectorField(const QString &name) const;
    const Field *field(const QString& name) const;
    QStringList matrixNames() const;
    bool isMatrixField(const QString &name) const;
    const MatrixField *matrixField(const QString &name) const;
    double frameCount() const;
    double frameRate() const { return _frameRate; }
    double delayMs() const { return _delayMs; }
    double repairWindowMs() const { return _repairWindowMs; }
    QString address() const { return _address; }
    void setGrid(double rate, double delayMs);
    void setRepairWindow(double windowMs);
    int readVector(double *data, const QString &field, const Kst::DataVector::ReadInfo &info);
    Kst::DataVector::DataInfo vectorInfo(const QString& field) const;
    int readMatrix(Kst::MatrixData *data, const QString &field,
             const Kst::DataMatrix::ReadInfo &info);
    Kst::DataMatrix::DataInfo matrixInfo(const QString &field, qint64 frame) const;

    static QString endpoint(const QString &source);
    static QStringList fieldsAt(const QString &source);
    static QStringList matrixFieldsAt(const QString &source);
    static bool validRate(double rate);

  private:
    void close();
    void updateFrameCount(qint64 latest);
    bool selectValue(scalar_token_t *token, qint64 time, double *value);
    bool selectMatrix(const QString &field, qint64 frame, MatrixSample *sample) const;
    bool frameTime(qint64 frame, qint64 *time) const;

    consumer_t *_consumer = nullptr;
    QString _address;
    QMap<QString, Field> _fields;
    QMap<QString, MatrixField> _matrixFields;
    QSet<QString> _changedFields;
    struct CachedMatrix {
      qint64 frame = -1;
      MatrixSample sample;
    };
    mutable QMap<QString, CachedMatrix> _matrixCache;
    qint64 _originNs = 0; // Other origin policies can be added without changing frame indexing.
    qint64 _framePeriodNs = 200000000;
    qint64 _delayNs = 100000000;
    qint64 _latestTime = 0;
    double _frameRate = 5.0;
    double _delayMs = 100.0;
    double _repairWindowMs = 2000.0;
    qint64 _frameCount = 0;
};

class SsdbSourcePlugin : public QObject, public Kst::DataSourcePluginInterface,
                         public Kst::NonFileDataSourcePluginInterface {
  Q_OBJECT
  Q_INTERFACES(Kst::DataSourcePluginInterface Kst::NonFileDataSourcePluginInterface)
  Q_PLUGIN_METADATA(IID KST_DATASOURCE_IID)
  public:
    ~SsdbSourcePlugin() override = default;
    QString pluginName() const override;
    QString pluginDescription() const override;
    bool hasConfigWidget() const override { return true; }
    Kst::DataSource *create(Kst::ObjectStore *, QSettings *, const QString&,
                            const QString&, const QDomElement&) const override;
    QStringList matrixList(QSettings *, const QString&, const QString&, QString *, bool *) const override;
    QStringList fieldList(QSettings *, const QString&, const QString&, QString *, bool *) const override;
    QStringList scalarList(QSettings *, const QString&, const QString&, QString *, bool *) const override;
    QStringList stringList(QSettings *, const QString&, const QString&, QString *, bool *) const override;
    int understands(QSettings *, const QString&) const override;
    bool isNonFileSource(const QString&) const override;
    bool supportsTime(QSettings *, const QString&) const override;
    QStringList provides() const override;
    Kst::DataSourceConfigWidget *configWidget(QSettings *, const QString&) const override;
};

#endif
