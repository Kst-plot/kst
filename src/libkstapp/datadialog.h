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

#include "kstdataobject.h"

namespace Kst {

class DialogTab;

class KST_EXPORT DataDialog : public Dialog {
  Q_OBJECT
  public:
    DataDialog(QWidget *parent = 0);
    DataDialog(KstObjectPtr dataObject, QWidget *parent = 0);
    virtual ~DataDialog();

    void addDataTab(DialogTab *tab);

  protected:
    KstObjectPtr dataObject() const { return _dataObject; }
    void setDataObject(KstObjectPtr dataObject) { _dataObject = dataObject; }

    virtual KstObjectPtr createNewDataObject() const = 0;
    virtual KstObjectPtr editExistingDataObject() const = 0;

  private Q_SLOTS:
    void slotOk();

  private:
    void createGui();

  private:
    KstObjectPtr _dataObject;
};

}

#endif

// vim: ts=2 sw=2 et
