#include "datasourcedialog.h"
#include "datasourceconfiguredialog.h"


namespace Kst {

DataSourceDialog::DataSourceDialog(ObjectPtr op, QWidget *parent) :
  QDialog(parent) {

  setAttribute(Qt::WA_DeleteOnClose, true);

  setupUi(this);
  _datasource = kst_cast<DataSource>(op);
  _fileName->setText(_datasource->fileName());

  _configure->setEnabled(_datasource->hasConfigWidget());
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));

  _updateBox->addItem(tr("Time Interval", "update periodically"));
  _updateBox->addItem(tr("Change Detection", "update when a change is detected"));
  _updateBox->addItem(tr("No Update", "do not update the file"));
  updateUpdateBox();
  connect(_updateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTypeActivated(int)));
}

DataSourceDialog::~DataSourceDialog() {
}

void DataSourceDialog::showConfigWidget() {
  QPointer<DataSourceConfigureDialog> dialog = new DataSourceConfigureDialog(DataDialog::Edit, _datasource, this);
  dialog->exec();
  delete dialog;
}

void DataSourceDialog::updateUpdateBox()
{
  if (_datasource) {
    switch (_datasource->updateType()) {
      case DataSource::Timer: _updateBox->setCurrentIndex(0); break;
      case DataSource::File:  _updateBox->setCurrentIndex(1); break;
      case DataSource::None:  _updateBox->setCurrentIndex(2); break;
      default: break;
    };
  } else {
    _updateBox->setEnabled(false);
  }
}

void DataSourceDialog::updateTypeActivated(int idx)
{
  if (!_datasource) {
    _updateBox->setEnabled(false);
    return;
  }
  switch (idx) {
    _updateBox->setEnabled(true);
    case 0: _datasource->startUpdating(DataSource::Timer); break;
    case 1: _datasource->startUpdating(DataSource::File);  break;
    case 2: _datasource->startUpdating(DataSource::None);  break;
    default: break;
  };
}

}
