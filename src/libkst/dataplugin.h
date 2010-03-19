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

#ifndef DATAPLUGIN_H
#define DATAPLUGIN_H

#include <QtPlugin>
#include <QSettings>

#include "sharedptr.h"
#include "datasource.h"

namespace Kst {

class ObjectStore;
class DataSource;
class DataSourceConfigWidget;

class PluginInterface : public Shared {
  public:
    PluginInterface() {}

    virtual ~PluginInterface() {}

    virtual QString pluginName() const { return QString::null; }
    virtual QString pluginDescription() const { return QString::null; }

    virtual bool hasConfigWidget() const { return false; }
};


class DataSourcePluginInterface : public PluginInterface {
  public:
    virtual ~DataSourcePluginInterface() {}

    virtual DataSource *create(ObjectStore *store,
                            QSettings *cfg,
                                  const QString &filename,
                                  const QString &type,
                                  const QDomElement &element) const = 0;

    virtual QStringList matrixList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList scalarList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList stringList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList fieldList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual int understands(QSettings *cfg, const QString& filename) const = 0;

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const = 0;

    virtual QStringList provides() const = 0;

    bool provides(const QString& type) const { return provides().contains(type); }

    virtual DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const = 0;
};


}


Q_DECLARE_INTERFACE(Kst::PluginInterface, "com.kst.PluginInterface/1.0")
Q_DECLARE_INTERFACE(Kst::DataSourcePluginInterface, "com.kst.DataSourcePluginInterface/1.0")


#endif
