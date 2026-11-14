/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2026 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtTest>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "objectstore.h"
#include "ssdbsource.h"
#include "datasourcepluginmanager.h"
#include "scalar.h"
#include "string_kst.h"
#include "testnextcolor.h"

extern "C" {
#include <flightapi.h>
}

namespace {
int selectCalls = 0;
int matrixCalls = 0;
int64_t selectedTime = 0;
bool failRead = false;
bool failConnect = false;
bool failList = false;
bool failLatest = false;
bool failRegister = false;
bool emptyList = false;
int connections = 0;
int consumers = 0;
int tokens = 0;
constexpr int64_t latestTime = 10000000000LL;
}

// Only the C API functions used by the actual SSDB adapter are stubbed.
extern "C" {
const char *telemetry_get_last_error() { return "injected read failure"; }
consumer_t *consumer_new(const char *, const char *) {
  ++connections;
  if (failConnect) return nullptr;
  ++consumers;
  return static_cast<consumer_t *>(std::malloc(1));
}
consumer_t *consumer_new_numeric(const char *name, const char *address, uint64_t opening, uint64_t operation) {
  if (!opening || !operation) return nullptr;
  return consumer_new(name, address);
}
void consumer_destroy(consumer_t *consumer) {
  if (consumer) { --consumers; std::free(consumer); }
}
void scalar_token_destroy(scalar_token_t *token) { if (token) { --tokens; std::free(token); } }
void matrix_token_destroy(matrix_token_t *token) { if (token) { --tokens; std::free(token); } }
int32_t consumer_finish_opening(consumer_t *) { return 1; }
int32_t consumer_get_latest_time_checked(consumer_t *, int64_t *value) {
  if (failLatest) return 0;
  *value = emptyList ? 0 : latestTime;
  return 1;
}
metadata_t *consumer_get_field_list(consumer_t *, uint32_t *count) {
  if (failList) { *count = 0; return nullptr; }
  // Same non-allocatable, non-null sentinel as an empty Rust Vec.
  if (emptyList) { *count = 0; return reinterpret_cast<metadata_t *>(alignof(metadata_t)); }
  *count = 1;
  auto *metadata = static_cast<metadata_t *>(std::calloc(1, sizeof(metadata_t)));
  metadata->name = ::strdup("image");
  metadata->structure = 2;
  metadata->last_time = latestTime;
  return metadata;
}
matrix_token_t *consumer_register_matrix(consumer_t *, const char *) {
  if (failRegister) return nullptr;
  ++tokens;
  return static_cast<matrix_token_t *>(std::malloc(1));
}
scalar_token_t *consumer_register_scalar(consumer_t *, const char *) { return nullptr; }
int64_t consumer_get_latest_time(consumer_t *) { return latestTime; }
int32_t consumer_select_resampled_window(consumer_t *, uint64_t, int64_t time) {
  ++selectCalls;
  selectedTime = time;
  return 1;
}
double *consumer_get_selected_scalar(consumer_t *, scalar_token_t *, uintptr_t) {
  return nullptr;
}
matrix_t *consumer_get_selected_matrix(consumer_t *, matrix_token_t *, uintptr_t size) {
  ++matrixCalls;
  if (failRead) return nullptr;
  auto *result = static_cast<matrix_t *>(std::calloc(size, sizeof(matrix_t)));
  // The database has no measurement at the epoch or before its first sample.
  if (selectedTime >= 1000000000LL) {
    result->nrows = 2;
    result->ncols = 3;
    result->data = static_cast<double *>(std::malloc(6 * sizeof(double)));
    for (int i = 0; i < 6; ++i) result->data[i] = i + 1;
  }
  return result;
}
}

class InvalidPreparedPlugin : public SsdbSourcePlugin {
public:
  Kst::DataSource *createPrepared(Kst::ObjectStore *store, QSettings *cfg,
      const QString &, const Kst::PreparedDataSourceOptions &, Kst::PreparedDataSourcePtr) const override {
    // Interfaces still advertise FILE/FRAMES, but construction is invalid.
    return new SsdbSource(store, cfg, "invalid endpoint", QString(), QDomElement());
  }
};

class TestSsdbMatrix : public QObject {
  Q_OBJECT
  TestNextColor colors;
private Q_SLOTS:
  void init() {
    selectCalls = matrixCalls = 0;
    selectedTime = 0;
    failRead = false;
    failConnect = failList = failLatest = failRegister = emptyList = false;
    connections = 0;
    QCOMPARE(consumers, 0);
    QCOMPARE(tokens, 0);
  }

  void cleanup() {
    QCOMPARE(consumers, 0);
    QCOMPARE(tokens, 0);
  }

  void failedOpening_data() {
    QTest::addColumn<int>("failure");
    QTest::newRow("connection") << 0;
    QTest::newRow("field list") << 1;
    QTest::newRow("registration") << 2;
    QTest::newRow("latest timestamp") << 3;
  }

  void failedOpening() {
    QFETCH(int, failure);
    failConnect = failure == 0;
    failList = failure == 1;
    failRegister = failure == 2;
    failLatest = failure == 3;
    SsdbSourcePlugin plugin;
    const auto options = plugin.prepareOptions(nullptr, "127.0.0.1:43234");
    QString error;
    std::atomic_bool cancelled(false);
    auto prepared = plugin.prepare(options, options.endpoint, 5000, cancelled, error);
    QVERIFY(!prepared);
    QVERIFY(!error.isEmpty());
    QCOMPARE(consumers, 0);
    QCOMPARE(tokens, 0);
    Kst::ObjectStore store;
    SsdbSource source(&store, nullptr, options.endpoint, QString(), QDomElement());
    QVERIFY(!source.isValid());
  }

  void preparedAdoptionDoesNotReconnect() {
    Kst::ObjectStore store;
    Kst::SharedPtr<Kst::DataSourcePluginInterface> plugin(new SsdbSourcePlugin);
    auto *capability = dynamic_cast<SsdbSourcePlugin *>(plugin.data());
    auto options = capability->prepareOptions(nullptr, "127.0.0.1:43234");
    options.settings["frameRateHz"] = 10.0;
    QString error;
    std::atomic_bool cancelled(false);
    auto prepared = capability->prepare(options, options.endpoint, 5000, cancelled, error);
    QVERIFY(prepared);
    QCOMPARE(connections, 1);
    // Any accidental GUI-thread reconnect would fail this adoption.
    failConnect = true;
    auto source = Kst::DataSourcePluginManager::adoptPreparedSource(&store,
        options.endpoint, plugin, options, std::move(prepared), true);
    QVERIFY(source);
    QVERIFY(source->isValid());
    QCOMPARE(source->matrixFrameCount("image"), qint64(100));
    QCOMPARE(connections, 1);
    QCOMPARE(store.dataSourceList().count(), 1);
    QVERIFY(!capability->createPrepared(&store, nullptr, options.endpoint, options, {}));
  }

  void emptyDatabaseIsValidAndCancellationReleasesState() {
    Kst::ObjectStore store;
    SsdbSourcePlugin plugin;
    const auto options = plugin.prepareOptions(nullptr, "127.0.0.1:43234");
    QString error;
    std::atomic_bool cancelled(false);
    emptyList = true;
    auto prepared = plugin.prepare(options, options.endpoint, 5000, cancelled, error);
    QVERIFY(prepared);
    Kst::DataSourcePtr source(plugin.createPrepared(&store, nullptr, options.endpoint, options, std::move(prepared)));
    QVERIFY(source->isValid());
    QVERIFY(source->isEmpty());
    QCOMPARE(source->vector().list(), QStringList({"INDEX", "TIME"}));
    source = nullptr;
    QCOMPARE(consumers, 0);
    cancelled = true;
    QVERIFY(!plugin.prepare(options, options.endpoint, 5000, cancelled, error));
    QCOMPARE(consumers, 0);
  }

  void invalidAdoptionPublishesNoSourceOrSlaveObjects() {
    Kst::ObjectStore store;
    Kst::SharedPtr<Kst::DataSourcePluginInterface> plugin(new InvalidPreparedPlugin);
    auto *capability = dynamic_cast<SsdbSourcePlugin *>(plugin.data());
    const auto options = capability->prepareOptions(nullptr, "127.0.0.1:43234");
    QString error;
    std::atomic_bool cancelled(false);
    auto prepared = capability->prepare(options, options.endpoint, 5000, cancelled, error);
    QVERIFY(prepared);
    QVERIFY(!Kst::DataSourcePluginManager::adoptPreparedSource(&store, options.endpoint,
        plugin, options, std::move(prepared), true));
    QVERIFY(store.dataSourceList().isEmpty());
    QVERIFY(store.getObjects<Kst::Scalar>().isEmpty());
    QVERIFY(store.getObjects<Kst::String>().isEmpty());
  }

  void fieldDiscoveryDistinguishesFailureFromEmptyDatabase() {
    SsdbSourcePlugin plugin;
    bool complete = true;
    failList = true;
    QVERIFY(plugin.fieldList(nullptr, "127.0.0.1:43234", {}, nullptr, &complete).isEmpty());
    QVERIFY(!complete);
    failList = false;
    emptyList = true;
    QCOMPARE(plugin.fieldList(nullptr, "127.0.0.1:43234", {}, nullptr, &complete),
        QStringList({"INDEX", "TIME"}));
    QVERIFY(complete);
  }

  void fileLengthNeverReadsEpochSample() {
    Kst::ObjectStore store;
    Kst::DataSourcePtr source(new SsdbSource(&store, nullptr,
        "ssdb://127.0.0.1:43234", QString(), QDomElement()));
    source->setUpdateType(Kst::DataSource::None);
    QVERIFY(source->isValid());
    auto matrix = store.createObject<Kst::DataMatrix>();
    matrix->change(source, "image", 0, 0, -1, -1, false, false, 1, -1,
                   false, 0, 0, 1, 1);
    selectCalls = matrixCalls = 0;
    for (qint64 frame : {qint64(-1), qint64(48), qint64(0)}) {
      matrix->setFrame(frame);
      QCOMPARE(matrix->fileLength(), qint64(50));
    }
    QCOMPARE(source->matrixFrameCount("missing"), qint64(0));
    QCOMPARE(selectCalls, 0);
    QCOMPARE(matrixCalls, 0);
  }

  void missingEpochMatrixIsEmptyAndClearsReadBuffer() {
    Kst::ObjectStore store;
    SsdbSource source(&store, nullptr, "127.0.0.1:43234", QString(), QDomElement());
    source.setUpdateType(Kst::DataSource::None);
    const auto info = source.matrixDataInfo("image", 0);
    QCOMPARE(info.frameCount, qint64(50));
    QCOMPARE(info.xSize, 0);
    QCOMPARE(info.ySize, 0);
    QCOMPARE(selectedTime, int64_t(0));
    double values[6] = {1, 2, 3, 4, 5, 6};
    Kst::MatrixData data{};
    data.z = values;
    Kst::DataMatrix::ReadInfo read{&data, 0, 0, 3, 2, 0, 0};
    QCOMPARE(source.readMatrix(&data, "image", read), 0);
    for (double value : values) QVERIFY(std::isnan(value));
    QCOMPARE(matrixCalls, 1); // Dimension and data requests share the cached sample.
  }

  void historicalMatrixRetainsShapeOrientationAndCropping() {
    Kst::ObjectStore store;
    SsdbSource source(&store, nullptr, "127.0.0.1:43234", QString(), QDomElement());
    source.setUpdateType(Kst::DataSource::None);
    const auto info = source.matrixDataInfo("image", 10);
    QCOMPARE(info.xSize, 3);
    QCOMPARE(info.ySize, 2);
    QCOMPARE(selectedTime, int64_t(2000000000LL));
    double values[6] = {};
    Kst::MatrixData data{};
    data.z = values;
    Kst::DataMatrix::ReadInfo read{&data, 0, 0, 3, 2, 0, 10};
    QCOMPARE(source.readMatrix(&data, "image", read), 6);
    const double expected[] = {1, 4, 2, 5, 3, 6};
    for (int i = 0; i < 6; ++i) QCOMPARE(values[i], expected[i]);
    read.xStart = 1;
    read.xNumSteps = 2;
    QCOMPARE(source.readMatrix(&data, "image", read), 4);
    const double cropped[] = {2, 5, 3, 6};
    for (int i = 0; i < 4; ++i) QCOMPARE(values[i], cropped[i]);
    QCOMPARE(matrixCalls, 1);
    const auto latest = source.matrixDataInfo("image", -1);
    QCOMPARE(latest.xSize, 3);
    QCOMPARE(selectedTime, int64_t(9800000000LL));
  }

  void failedReadClearsBufferAndDoesNotCacheFailure() {
    Kst::ObjectStore store;
    SsdbSource source(&store, nullptr, "127.0.0.1:43234", QString(), QDomElement());
    source.setUpdateType(Kst::DataSource::None);
    failRead = true;
    double values[6] = {1, 2, 3, 4, 5, 6};
    Kst::MatrixData data{};
    data.z = values;
    Kst::DataMatrix::ReadInfo read{&data, 0, 0, 3, 2, 0, 10};
    QCOMPARE(source.readMatrix(&data, "image", read), 0);
    for (double value : values) QVERIFY(std::isnan(value));
    failRead = false;
    QCOMPARE(source.readMatrix(&data, "image", read), 6);
    QCOMPARE(matrixCalls, 2);
  }
};

QTEST_MAIN(TestSsdbMatrix)
#include "testssdbmatrix.moc"