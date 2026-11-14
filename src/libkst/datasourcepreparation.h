// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef KST_DATASOURCEPREPARATION_H
#define KST_DATASOURCEPREPARATION_H

#include "dataplugin.h"
#include <QHostAddress>
#include <QObject>
#include <QTimer>
#include <chrono>

namespace Kst {

struct DataSourcePreparation {
  SharedPtr<DataSourcePluginInterface> plugin;
  PreparedDataSourceOptions options;
  QString filename;
  int requestId = 0;
  std::atomic_bool cancelled{false};
  std::chrono::steady_clock::time_point deadline;
  // Result fields are only modified on the owning GUI thread.
  PreparedDataSourcePtr prepared;
  QString error;
};
using DataSourcePreparationPtr = std::shared_ptr<DataSourcePreparation>;

// A persistent controller keeps at most one running preparation and the latest
// pending request. Cancellation never blocks the GUI or dereferences ObjectStore.
class KSTCORE_EXPORT DataSourcePreparationController : public QObject {
  Q_OBJECT
public:
  explicit DataSourcePreparationController(QObject *parent = nullptr);
  ~DataSourcePreparationController() override;
  void start(DataSourcePreparationPtr, int timeoutMs = 5000);
  void cancel();

Q_SIGNALS:
  void finished(Kst::DataSourcePreparationPtr);

private:
  void resolved(DataSourcePreparationPtr, const QList<QHostAddress> &);
  void launchPending();
  void complete(DataSourcePreparationPtr, PreparedDataSourcePtr, const QString &);
  DataSourcePreparationPtr _current;
  DataSourcePreparationPtr _pending;
  QList<QHostAddress> _addresses;
  QTimer _timeout;
  int _lookupId = -1;
  bool _workerBusy = false;
};

}
Q_DECLARE_METATYPE(Kst::DataSourcePreparationPtr)
#endif