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

#ifndef DATADIALOG_H
#define DATADIALOG_H

#include "dialog.h"

#include "kst_export.h"

#include "dataobject.h"

class QLineEdit;
class QCheckBox;
class QLabel;

namespace Kst {

class DataTab;
class Document;
class EditMultipleWidget;

class DataDialog : public Dialog {
  Q_OBJECT
  public:
    enum EditMode { New, Edit, EditMultiple };
    explicit DataDialog(ObjectPtr dataObject = 0, QWidget *parent = 0, bool edit_multiple = true);
    virtual ~DataDialog();

    EditMode editMode() const { return _mode; }
    void addDataTab(DataTab *tab);
    QString dataObjectName() const;

    void editMultiple(const QList<ObjectPtr> &objects);

  protected:
    virtual QString tagString() const;
    void setTagString(const QString &tagString);
    void setShortName(const QString &name);

    bool tagStringAuto() const;

    ObjectPtr dataObject() const { return _dataObject; }
    void setDataObject(ObjectPtr dataObject) { _dataObject = dataObject; }

    virtual ObjectPtr createNewDataObject() = 0;
    virtual ObjectPtr editExistingDataObject() const = 0;

    Document *_document;
    EditMultipleWidget *_editMultipleWidget;
    QString _dataObjectName;

  public Q_SLOTS:
    void slotEditMultiple();

  private Q_SLOTS:
    void slotApply();
    void modified();
    void clearModified();

  Q_SIGNALS:
    void editMultipleMode();
    void editSingleMode();

  private:
    void createGui(bool edit_multiple = true);
    void updateApplyButton();

  private:
    QLineEdit *_tagString;
    QCheckBox *_tagStringAuto;
    QLabel *_shortName;
    QLabel *_nameLabel;
    QPushButton *_editMultipleButton;
    ObjectPtr _dataObject;
    EditMode _mode;
    bool _modified;
};

}

#endif

// vim: ts=2 sw=2 et
