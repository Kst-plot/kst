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
#include "updatemanager.h"
#include "updateserver.h"

#ifndef PLUGINSCRIPTINTERFACE_H
#define PLUGINSCRIPTINTERFACE_H

namespace Kst {

class KSTMATH_EXPORT DataObjectSI : public ScriptInterface
{    
    Q_OBJECT
public:
    explicit DataObjectSI(DataObjectPtr plugin);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newPlugin(ObjectStore *store, QByteArray pluginName);
  private:
    DataObjectPtr _plugin;
};


}
#endif // PLUGINSCRIPTINTERFACE_H
