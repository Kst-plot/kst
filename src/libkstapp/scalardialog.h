/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCALARDIALOG_H
#define SCALARDIALOG_H

#include "datadialog.h"
#include "datatab.h"
#include "datasource.h"

#include "ui_scalartab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class ScalarTab : public DataTab, Ui::ScalarTab {
  Q_OBJECT
  public:
    enum ScalarMode { DataScalar, GeneratedScalar, RVectorScalar };

    explicit ScalarTab(ObjectStore *store, QWidget *parent = 0);
    virtual ~ScalarTab();

    ScalarMode scalarMode() const { return _mode; }

    QString value() const;
    void setValue(const QString &value);

    DataSourcePtr dataSource() const;
    void setDataSource(DataSourcePtr dataSource);

    QString file() const;
    void setFile(const QString &file);

    QString field() const;
    void setField(const QString &field);

    QString fieldRV() const;
    void setFieldRV(const QString &field);

    int F0() const;
    void setF0(int f0);

    void hideGeneratedOptions();
    void hideDataOptions();
    void setRVOptions();
    void setDataOptions();

  Q_SIGNALS:
    void valueChanged();
    void sourceChanged();

  private Q_SLOTS:
    void readFromSourceChanged();
    void entryChanged();
    void fileNameChanged(const QString &file);
    void showConfigWidget();
    void sourceValid(QString filename, int requestID);

  private:
    void updateDataSource();
    ScalarMode _mode;
    ObjectStore *_store;
    DataSourcePtr _dataSource;
    int _requestID;
};

class ScalarDialog : public DataDialog {
  Q_OBJECT
  public:
    explicit ScalarDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~ScalarDialog();

  protected:
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr createNewGeneratedScalar();
    virtual ObjectPtr createNewDataScalar();
    virtual ObjectPtr createNewVScalar();
    virtual ObjectPtr editExistingDataObject() const;

  private:
    void configureTab(ObjectPtr object);

  private Q_SLOTS:
    void updateButtons();

  private:
    ScalarTab *_scalarTab;

};

}

#endif

// vim: ts=2 sw=2 et
