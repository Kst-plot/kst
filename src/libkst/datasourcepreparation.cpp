// SPDX-License-Identifier: GPL-2.0-or-later
#include "datasourcepreparation.h"

#include <QFutureWatcher>
#include <QHostInfo>
#include <QThreadPool>
#include <QUrl>
#include <QtConcurrentRun>

namespace Kst {
namespace {
struct PreparationOutcome {
  PreparedDataSourcePtr prepared;
  QString error;
};
qint64 remaining(const DataSourcePreparationPtr &job) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      job->deadline - std::chrono::steady_clock::now()).count();
}

QThreadPool &preparationPool() {
  // Do not queue blocking requests on the validation pool or its global mutex.
  static QThreadPool pool;
  static const bool configured = [] { pool.setMaxThreadCount(2); return true; }();
  Q_UNUSED(configured)
  return pool;
}
}

DataSourcePreparationController::DataSourcePreparationController(QObject *parent)
    : QObject(parent) {
  qRegisterMetaType<DataSourcePreparationPtr>();
  _timeout.setSingleShot(true);
  _timeout.setTimerType(Qt::PreciseTimer);
  connect(&_timeout, &QTimer::timeout, this, [this] {
    auto job = _current;
    if (!job) return;
    cancel();
    job->error = tr("Source opening timed out");
    emit finished(job);
  });
}

DataSourcePreparationController::~DataSourcePreparationController() { cancel(); }

void DataSourcePreparationController::cancel() {
  _timeout.stop();
  if (_lookupId >= 0) QHostInfo::abortHostLookup(_lookupId);
  _lookupId = -1;
  if (_current) _current->cancelled = true;
  if (_pending) _pending->cancelled = true;
  _current.reset();
  _pending.reset();
  _addresses.clear();
}

void DataSourcePreparationController::start(DataSourcePreparationPtr job, int timeoutMs) {
  cancel();
  _current = job;
  job->deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(qMax(1, timeoutMs));
  _timeout.start(qMax(1, timeoutMs));
  const QUrl url("ssdb://" + job->options.endpoint);
  if (!url.isValid() || url.host().isEmpty() || url.port() <= 0) {
    complete(job, {}, tr("Invalid source endpoint"));
    return;
  }
  QHostAddress address;
  if (address.setAddress(url.host())) {
    resolved(job, {address});
  } else {
    _lookupId = QHostInfo::lookupHost(url.host(), this, [this, job](const QHostInfo &info) {
      if (job != _current || job->cancelled) return;
      _lookupId = -1;
      if (info.error() != QHostInfo::NoError || info.addresses().isEmpty())
        complete(job, {}, info.errorString());
      else
        resolved(job, info.addresses());
    });
  }
}

void DataSourcePreparationController::resolved(DataSourcePreparationPtr job,
    const QList<QHostAddress> &addresses) {
  if (job != _current || job->cancelled) return;
  _pending = job;
  _addresses = addresses;
  launchPending();
}

void DataSourcePreparationController::launchPending() {
  if (_workerBusy || !_pending) return;
  const auto job = _pending;
  const auto addresses = _addresses;
  _pending.reset();
  _workerBusy = true;
  // Watcher destruction disconnects completion without waiting for the future.
  // Workers never retain raw pointers to the wizard or QApplication, including
  // when the application itself is destroyed before a pending request returns.
  auto *watcher = new QFutureWatcher<PreparationOutcome>(this);
  connect(watcher, &QFutureWatcher<PreparationOutcome>::finished, this, [this, watcher, job] {
    const auto outcome = watcher->result();
    watcher->deleteLater();
    _workerBusy = false;
    complete(job, outcome.prepared, outcome.error);
    launchPending();
  });
  watcher->setFuture(QtConcurrent::run(&preparationPool(), [job, addresses] {
    PreparedDataSourcePtr result;
    QString error;
    auto *capability = qobject_cast<PreparedDataSourcePluginInterface *>(
        dynamic_cast<QObject *>(job->plugin.data()));
    if (!capability) error = QStringLiteral("Source does not support preparation");
    else {
      const int port = QUrl("ssdb://" + job->options.endpoint).port();
      for (const auto &address : addresses) {
        const qint64 budget = remaining(job);
        if (job->cancelled || budget <= 0) break;
        QString host = address.toString();
        if (host.contains(':')) host = '[' + host + ']';
        result = capability->prepare(job->options, host + ':' + QString::number(port),
            budget, job->cancelled, error);
        if (result) break;
      }
    }
    if (error.isEmpty() && !result) error = QStringLiteral("Source opening cancelled or timed out");
    return PreparationOutcome{std::move(result), error};
  }));
}

void DataSourcePreparationController::complete(DataSourcePreparationPtr job,
    PreparedDataSourcePtr result, const QString &error) {
  if (job != _current || job->cancelled) return;
  _timeout.stop();
  _current.reset();
  if (remaining(job) <= 0) {
    job->cancelled = true;
    job->error = tr("Source opening timed out");
  } else {
    job->prepared = std::move(result);
    job->error = job->prepared ? QString() : error;
  }
  emit finished(job);
}
}