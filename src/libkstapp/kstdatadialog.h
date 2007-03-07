/***************************************************************************
                   kstdatadialog.h
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTDATADIALOG_H
#define KSTDATADIALOG_H

#include <QDialog>
#include "ui_kstdatadialog4.h"

#include "kstdataobject.h"

class KstDataDialog : public QDialog, public Ui::KstDataDialog {
  Q_OBJECT

public:
  KstDataDialog(QWidget *parent = 0, Qt::WindowFlags fl = 0);
  ~KstDataDialog();

public slots:
  void ok();
  void close();
  void reject();
  void init();
  void update();
  void show();
  void showNew(const QString &field);
  void showEdit(const QString &field);
  bool newObject();
  bool editObject();
  bool multiple();
  void toggleEditMultiple();

signals:
  void modified();

public:
  bool _newDialog;
  bool _multiple;
  bool _editMultipleMode;
  KstObjectPtr _dp;

protected:
  QString objectName();
  void fillFieldsForEdit();
  void fillFieldsForNew();
  KstObjectPtr findObject(const QString &name);
  void populateEditMultiple();
  void setMultiple(bool multiple);
  void closeEvent(QCloseEvent *e);
  void cleanup();
};

#endif
// vim: ts=2 sw=2 et
