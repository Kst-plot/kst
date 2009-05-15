/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
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

class KST_EXPORT DataDialog : public Dialog {
  Q_OBJECT
  public:
    enum EditMode { New, Edit, EditMultiple };
    DataDialog(ObjectPtr dataObject = 0, QWidget *parent = 0);
    virtual ~DataDialog();

    EditMode editMode() const { return _mode; }
    void addDataTab(DataTab *tab);
    QString dataObjectName() const;

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

  private Q_SLOTS:
    void slotApply();
    void slotEditMultiple();
    void modified();
    void clearModified();

  Q_SIGNALS:
    void editMultipleMode();
    void editSingleMode();

  private:
    void createGui();
    void updateApplyButton();

  private:
    QLineEdit *_tagString;
    QCheckBox *_tagStringAuto;
    QLabel *_shortName;
    QLabel *_nameLabel;
    QPushButton *_expand;
    ObjectPtr _dataObject;
    EditMode _mode;
    bool _modified;
};

}

#endif

// vim: ts=2 sw=2 et
