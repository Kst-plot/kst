#include <QtTest>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "objectstore.h"
#include "ssdbsource.h"
#include "testnextcolor.h"

extern "C" {
#include <flightapi.h>
}

namespace {
int selectCalls = 0;
int matrixCalls = 0;
int64_t selectedTime = 0;
bool failRead = false;
constexpr int64_t latestTime = 10000000000LL;
}

// Only the C API functions used by the actual SSDB adapter are stubbed.
extern "C" {
const char *telemetry_get_last_error() { return "injected read failure"; }
consumer_t *consumer_new(const char *, const char *) {
  return static_cast<consumer_t *>(std::malloc(1));
}
metadata_t *consumer_get_field_list(consumer_t *, uint32_t *count) {
  *count = 1;
  auto *metadata = static_cast<metadata_t *>(std::calloc(1, sizeof(metadata_t)));
  metadata->name = ::strdup("image");
  metadata->structure = 2;
  metadata->last_time = latestTime;
  return metadata;
}
matrix_token_t *consumer_register_matrix(consumer_t *, const char *) {
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

class TestSsdbMatrix : public QObject {
  Q_OBJECT
  TestNextColor colors;
private Q_SLOTS:
  void init() {
    selectCalls = matrixCalls = 0;
    selectedTime = 0;
    failRead = false;
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