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

#ifndef FILTERFITDIALOG_H
#define FILTERFITDIALOG_H

#include "datadialog.h"
#include "datatab.h"
#include "dataobjectplugin.h"

#include "kst_export.h"

#include "ui_filterfittab.h"

namespace Kst {

class KST_EXPORT FilterFitTab : public DataTab, Ui::FilterFitTab {
  Q_OBJECT
  public:
    FilterFitTab(QString& pluginName, QWidget *parent = 0);
    virtual ~FilterFitTab();

    void setObjectStore(ObjectStore *store);

    DataObjectConfigWidget* configWidget() { return _configWidget; }

    void loadSettings();
    void saveSettings();

  public Q_SLOTS:
    void pluginChanged(const QString&);

  private:
    DataObjectConfigWidget* _configWidget;
    QGridLayout* _layout;
    DataObjectPluginInterface::PluginTypeID _type;
    ObjectStore *_store;

};

class KST_EXPORT FilterFitDialog : public DataDialog {
  Q_OBJECT
  public:
    FilterFitDialog(QString& pluginName, ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~FilterFitDialog();

  protected:
    virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr editExistingDataObject() const;

  private:
    FilterFitTab *_filterFitTab;
    QString _pluginName;
};

}

#endif

// vim: ts=2 sw=2 et
