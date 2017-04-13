#ifndef DATASOURCEDIALOG_H
#define DATASOURCEDIALOG_H

#include <QDialog>

#include "ui_datasourcedialog.h"
#include "datasource.h"
#include "kst_export.h"

namespace Kst {

class DataSourceDialog : public QDialog,  Ui::DataSourceDialog
{
  Q_OBJECT
public:
  explicit DataSourceDialog(ObjectPtr op, QWidget *parent = 0);
  ~DataSourceDialog();

public slots:
  void showConfigWidget();
  void updateUpdateBox();
  void updateTypeActivated(int idx);

private:
  DataSourcePtr _datasource;
};
}

#endif // DATASOURCEDIALOG_H

// vim: ts=2 sw=2 et
