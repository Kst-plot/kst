/***************************************************************************
                   datasourcemetadialog.h
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

#ifndef DATASOURCEMETADATADIALOG_H
#define DATASOURCEMETADATADIALOG_H

#include <QDialog>
#include "ui_datasourcemetadatadialog4.h"

class DataSourceMetaDataDialog : public QDialog, public Ui::DataSourceMetaDataDialog {
  Q_OBJECT

public:
  DataSourceMetaDataDialog(QWidget *parent = 0);
  ~DataSourceMetaDataDialog();

public slots:
    void setDataSource(KstDataSourcePtr dsp);

protected slots:
    void updateMetadata(const QString &tag);
    void init();

private:
  KstDataSourcePtr _dsp;
};

#endif
// vim: ts=2 sw=2 et
