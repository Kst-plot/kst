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

#ifndef BASICPLUGINDIALOG_H
#define BASICPLUGINDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "kst_export.h"

#include "ui_basicplugintab.h"

namespace Kst {

class KST_EXPORT BasicPluginTab : public DataTab, Ui::BasicPluginTab {
  Q_OBJECT
  public:
    BasicPluginTab(QWidget *parent = 0);
    virtual ~BasicPluginTab();

};

class KST_EXPORT BasicPluginDialog : public DataDialog {
  Q_OBJECT
  public:
    BasicPluginDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~BasicPluginDialog();

  protected:
    virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private:
    BasicPluginTab *_basicPluginTab;
};

}

#endif

// vim: ts=2 sw=2 et
