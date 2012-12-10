/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QByteArray>
#include <QString>
#include <QSizeF>
#include <QList>

#include "scriptinterface.h"
#include "basicplugin.h"
#include "objectstore.h"

#ifndef PLUGINSCRIPTINTERFACE_H
#define PLUGINSCRIPTINTERFACE_H

namespace Kst {

class PluginSI : public ScriptInterface
{    
    Q_OBJECT
public:
    PluginSI(BasicPluginPtr plugin, ObjectStore *store);
    QByteArrayList commands();
    QString doCommand(QString);
    bool isValid();
    QByteArray getHandle();
    void endEditUpdate() {}
  private:
    BasicPluginPtr _plugin;
    ObjectStore *_store;
};


}
#endif // PLUGINSCRIPTINTERFACE_H
