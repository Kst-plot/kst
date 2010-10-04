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

#ifndef STRINGDIALOG_H
#define STRINGDIALOG_H

#include "datadialog.h"
#include "datatab.h"
#include "datasource.h"

#include "ui_stringtab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class StringTab : public DataTab, Ui::StringTab {
  Q_OBJECT
  public:
    enum StringMode { DataString, GeneratedString };

    explicit StringTab(ObjectStore *store, QWidget *parent = 0);
    virtual ~StringTab();

    StringMode stringMode() const { return _mode; }
    void setStringMode(StringMode mode);

    QString value() const;
    void setValue(const QString &value);

    DataSourcePtr dataSource() const;
    void setDataSource(DataSourcePtr dataSource);

    QString file() const;
    void setFile(const QString &file);

    QString field() const;
    void setField(const QString &field);

    void setFieldList(const QStringList &fieldList);

    void hideGeneratedOptions();
    void hideDataOptions();
  Q_SIGNALS:
    void valueChanged();
    void sourceChanged();

  private Q_SLOTS:
    void readFromSourceClicked();
    void generateClicked();
    void textChanged();
    void fileNameChanged(const QString &file);
    void showConfigWidget();
    void sourceValid(QString filename, int requestID);

  private:
    void updateDataSource();
    StringMode _mode;
    ObjectStore *_store;
    DataSourcePtr _dataSource;
    int _requestID;
};

class StringDialog : public DataDialog {
  Q_OBJECT
  public:
    explicit StringDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~StringDialog();

  protected:
//     virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr createNewGeneratedString();
    virtual ObjectPtr createNewDataString();
    virtual ObjectPtr editExistingDataObject() const;

  private:
    void configureTab(ObjectPtr object);

  private Q_SLOTS:
    void updateButtons();

  private:
    StringTab *_stringTab;

};

}

#endif

// vim: ts=2 sw=2 et
