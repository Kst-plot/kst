/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2016 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* this class can be used to apply a filter to many vectors */
/* it will optionally replace the vectors in curves that use it */

#ifndef FILTERMULTIPLEDIALOG_H
#define FILTERMULTIPLEDIALOG_H

#include "dataobject.h"

#include <QDialog>
#include <QGridLayout>

namespace Ui {
class FilterMultipleDialog;
}

class QListWidgetItem;

namespace Kst {

class ObjectStore;

class FilterMultipleDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FilterMultipleDialog(QWidget *parent = 0);
  ~FilterMultipleDialog();

  void updateVectorLists();

  virtual void show();

Q_SIGNALS:
  void modified();

private Q_SLOTS:
  void updateButtons();

  void addButtonClicked();
  void smartButtonClicked();
  void removeButtonClicked();
  void addAll();
  void removeAll();

  void availableDoubleClicked(QListWidgetItem * item);
  void selectedDoubleClicked(QListWidgetItem * item);

  void pluginChanged(QString plugin = QString());

  void OKClicked();
  void apply();

private:
  Ui::FilterMultipleDialog *ui;
  DataObjectConfigWidget* _configWidget;
  QGridLayout* _layout;
  ObjectStore *_store;


};

}
#endif // FILTERMULTIPLEDIALOG_H

// vim: ts=2 sw=2 et

