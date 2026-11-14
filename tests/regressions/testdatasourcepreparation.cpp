// SPDX-License-Identifier: GPL-2.0-or-later
#include <QtTest>
#include <QSemaphore>
#include <QThread>
#include <atomic>

#include "datasourcepreparation.h"

namespace {
std::atomic_int liveResults{0};
struct Result : Kst::PreparedDataSource {
  Result() { ++liveResults; }
  ~Result() override { --liveResults; }
};
}

class PreparationPlugin : public QObject, public Kst::DataSourcePluginInterface,
                          public Kst::PreparedDataSourcePluginInterface {
  Q_OBJECT
  Q_INTERFACES(Kst::DataSourcePluginInterface Kst::PreparedDataSourcePluginInterface)
public:
  QSemaphore entered;
  QSemaphore release;
  std::atomic_int calls{0};
  std::atomic_bool usedGuiThread{false};
  Kst::PreparedDataSourceOptions prepareOptions(QSettings *, const QString &) const override { return {}; }
  Kst::PreparedDataSourcePtr prepare(const Kst::PreparedDataSourceOptions &options,
      const QString &numeric, qint64 budget, const std::atomic_bool &cancelled,
      QString &error) const override {
    auto *self = const_cast<PreparationPlugin *>(this);
    ++self->calls;
    self->usedGuiThread = QThread::currentThread() == qApp->thread();
    self->entered.release();
    if (options.settings.value("block").toBool()) self->release.tryAcquire(1, int(budget));
    if (cancelled) return {};
    if (options.settings.value("fail").toBool()) { error = "injected failure"; return {}; }
    if (!numeric.startsWith("127.0.0.1:")) { error = "not numeric"; return {}; }
    return std::make_shared<Result>();
  }
  Kst::DataSource *createPrepared(Kst::ObjectStore *, QSettings *, const QString &,
      const Kst::PreparedDataSourceOptions &, Kst::PreparedDataSourcePtr) const override { return nullptr; }
  QString pluginName() const override { return "Preparation fixture"; }
  QString pluginDescription() const override { return {}; }
  Kst::DataSource *create(Kst::ObjectStore *, QSettings *, const QString &, const QString &,
      const QDomElement &) const override { return nullptr; }
  QStringList matrixList(QSettings *, const QString &, const QString &, QString *, bool *) const override { return {}; }
  QStringList scalarList(QSettings *, const QString &, const QString &, QString *, bool *) const override { return {}; }
  QStringList stringList(QSettings *, const QString &, const QString &, QString *, bool *) const override { return {}; }
  QStringList fieldList(QSettings *, const QString &, const QString &, QString *, bool *) const override { return {}; }
  int understands(QSettings *, const QString &) const override { return 0; }
  bool supportsTime(QSettings *, const QString &) const override { return false; }
  QStringList provides() const override { return {}; }
  Kst::DataSourceConfigWidget *configWidget(QSettings *, const QString &) const override { return nullptr; }
};

class TestDataSourcePreparation : public QObject {
  Q_OBJECT
  Kst::DataSourcePreparationPtr job(PreparationPlugin *plugin, int id, bool block = false) {
    auto value = std::make_shared<Kst::DataSourcePreparation>();
    value->plugin = plugin;
    value->options.endpoint = "127.0.0.1:43234";
    value->options.settings["block"] = block;
    value->requestId = id;
    value->filename = "ssdb://127.0.0.1:43234";
    return value;
  }
private Q_SLOTS:
  void cleanup() { QTRY_COMPARE(liveResults.load(), 0); }

  void successfulAndFailedPreparation() {
    Kst::SharedPtr<PreparationPlugin> plugin(new PreparationPlugin);
    Kst::DataSourcePreparationController controller;
    QSignalSpy finished(&controller, &Kst::DataSourcePreparationController::finished);
    auto request = job(plugin.data(), 1);
    controller.start(request);
    QTRY_COMPARE(finished.count(), 1);
    QVERIFY(request->prepared);
    QVERIFY(request->error.isEmpty());
    QVERIFY(!plugin->usedGuiThread);
    finished.clear();
    request.reset();
    auto failed = job(plugin.data(), 2);
    failed->options.settings["fail"] = true;
    controller.start(failed);
    QTRY_COMPARE(finished.count(), 1);
    QVERIFY(!failed->prepared);
    QCOMPARE(failed->error, QString("injected failure"));
  }

  void stalledOpeningKeepsEventLoopResponsiveAndTimesOut() {
    Kst::SharedPtr<PreparationPlugin> plugin(new PreparationPlugin);
    Kst::DataSourcePreparationController controller;
    QSignalSpy finished(&controller, &Kst::DataSourcePreparationController::finished);
    int ticks = 0;
    QTimer heartbeat;
    heartbeat.setInterval(5);
    connect(&heartbeat, &QTimer::timeout, this, [&] { ++ticks; });
    heartbeat.start();
    auto request = job(plugin.data(), 1, true);
    QElapsedTimer elapsed;
    elapsed.start();
    controller.start(request, 150);
    QTRY_COMPARE(finished.count(), 1);
    QVERIFY(elapsed.elapsed() < 1000);
    QVERIFY(ticks >= 3);
    QVERIFY(!request->prepared);
    QVERIFY(!request->error.isEmpty());
    // Wait for the old worker's queued completion, not merely the GUI timeout.
    auto next = job(plugin.data(), 2);
    controller.start(next);
    QTRY_COMPARE(finished.count(), 2);
    QVERIFY(next->prepared);
  }

  void rapidEditsOnlyPrepareLatestPendingRequest() {
    Kst::SharedPtr<PreparationPlugin> plugin(new PreparationPlugin);
    Kst::DataSourcePreparationController controller;
    QSignalSpy finished(&controller, &Kst::DataSourcePreparationController::finished);
    auto first = job(plugin.data(), 1, true);
    controller.start(first);
    QTRY_VERIFY(plugin->entered.available() > 0);
    for (int id = 2; id <= 20; ++id) controller.start(job(plugin.data(), id));
    QVERIFY(first->cancelled);
    plugin->release.release();
    QTRY_COMPARE(finished.count(), 1);
    const auto result = qvariant_cast<Kst::DataSourcePreparationPtr>(finished.first().first());
    QCOMPARE(result->requestId, 20);
    QVERIFY(result->prepared);
    QCOMPARE(plugin->calls.load(), 2);
  }

  void destructionDoesNotWaitForWorker() {
    Kst::SharedPtr<PreparationPlugin> plugin(new PreparationPlugin);
    auto *controller = new Kst::DataSourcePreparationController;
    auto request = job(plugin.data(), 1, true);
    controller->start(request);
    QTRY_VERIFY(plugin->entered.available() > 0);
    QElapsedTimer elapsed;
    elapsed.start();
    delete controller;
    QVERIFY(elapsed.elapsed() < 100);
    QVERIFY(request->cancelled);
    plugin->release.release();
    // A new controller can complete while the destroyed receiver is ignored.
    Kst::DataSourcePreparationController next;
    QSignalSpy finished(&next, &Kst::DataSourcePreparationController::finished);
    next.start(job(plugin.data(), 2));
    QTRY_COMPARE(finished.count(), 1);
  }

  void invalidEndpointFailsWithoutWorker() {
    Kst::SharedPtr<PreparationPlugin> plugin(new PreparationPlugin);
    Kst::DataSourcePreparationController controller;
    QSignalSpy finished(&controller, &Kst::DataSourcePreparationController::finished);
    auto request = job(plugin.data(), 1);
    request->options.endpoint.clear();
    controller.start(request);
    QCOMPARE(finished.count(), 1);
    QCOMPARE(plugin->calls.load(), 0);
    QVERIFY(!request->error.isEmpty());
  }

  void hostnameResolutionAndAddressFallbackAreAsynchronous() {
    Kst::SharedPtr<PreparationPlugin> plugin(new PreparationPlugin);
    Kst::DataSourcePreparationController controller;
    QSignalSpy finished(&controller, &Kst::DataSourcePreparationController::finished);
    auto request = job(plugin.data(), 1);
    request->options.endpoint = "localhost:43234";
    controller.start(request);
    QTRY_COMPARE(finished.count(), 1);
    QVERIFY(request->prepared);
    QVERIFY(!plugin->usedGuiThread);
  }
};
QTEST_MAIN(TestDataSourcePreparation)
#include "testdatasourcepreparation.moc"