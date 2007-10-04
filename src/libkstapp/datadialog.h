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

namespace Kst {

class DataTab;

class KST_EXPORT DataDialog : public Dialog {
  Q_OBJECT
  public:
    enum EditMode { New, Edit };
    DataDialog(Kst::ObjectPtr dataObject = 0, QWidget *parent = 0);
    virtual ~DataDialog();

    EditMode editMode() const { return _mode; }
    void addDataTab(DataTab *tab);

  protected:
    virtual QString tagName() const;
    void setTagName(const QString &tagName);

    QString defaultTag() const { return _defaultTag; }

    Kst::ObjectPtr dataObject() const { return _dataObject; }
    void setDataObject(Kst::ObjectPtr dataObject) { _dataObject = dataObject; }

    virtual Kst::ObjectPtr createNewDataObject() const = 0;
    virtual Kst::ObjectPtr editExistingDataObject() const = 0;

  private Q_SLOTS:
    void slotOk();
    void slotEditMultiple();

  private:
    void createGui();

  private:
    QString _defaultTag;
    QLineEdit *_tagName;
    Kst::ObjectPtr _dataObject;
    EditMode _mode;
};

}

#endif

// vim: ts=2 sw=2 et
