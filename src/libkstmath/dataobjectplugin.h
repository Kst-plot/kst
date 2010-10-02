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

#ifndef DATAOBJECTPLUGIN_H
#define DATAOBJECTPLUGIN_H

#include <QtPlugin>
#include <QSettings>

#include "sharedptr.h"
#include "dataobject.h"
#include "dataplugin.h"

namespace Kst {

class ObjectStore;

class DataObjectPluginInterface : public PluginInterface {
  public:
    enum PluginTypeID { Generic, Fit, Filter };

    virtual ~DataObjectPluginInterface() {}

    virtual DataObject *create(ObjectStore *store, DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true) const = 0;

    virtual DataObjectConfigWidget *configWidget(QSettings *settingsObject) const = 0;

    virtual PluginTypeID pluginType() const = 0;
};


class BasicPluginInterface : public DataObjectPluginInterface {
  public:
    virtual ~BasicPluginInterface() {}

    virtual QStringList inputVectorList() const = 0;

    virtual QStringList inputScalarList() const = 0;

    virtual QStringList inputStringList() const = 0;

    virtual QStringList outputVectorList() const = 0;

    virtual QStringList outputScalarList() const = 0;

    virtual QStringList outputStringList() const = 0;
};


typedef ObjectList<DataObjectPluginInterface> DataObjectPluginList;

}

Q_DECLARE_INTERFACE(Kst::DataObjectPluginInterface, "com.kst.DataObjectPluginInterface/1.0")
Q_DECLARE_INTERFACE(Kst::BasicPluginInterface, "com.kst.BasicPluginInterface/1.0")

#endif
