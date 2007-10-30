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


class PluginInterface : public Shared {
  public:
    PluginInterface() {}

    virtual ~PluginInterface() {}

    virtual QString pluginName() const { return QString::null; }

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


class DataObjectPluginInterface : public PluginInterface {
  public:
    virtual ~DataObjectPluginInterface() {}

    virtual QWidget *configWidget(const QString& name) const = 0;
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


typedef ObjectList<PluginInterface> PluginList;

}

Q_DECLARE_INTERFACE(Kst::PluginInterface, "com.kst.PluginInterface/1.0")
Q_DECLARE_INTERFACE(Kst::DataSourcePluginInterface, "com.kst.DataSourcePluginInterface/1.0")
Q_DECLARE_INTERFACE(Kst::DataObjectPluginInterface, "com.kst.DataObjectPluginInterface/1.0")
Q_DECLARE_INTERFACE(Kst::BasicPluginInterface, "com.kst.BasicPluginInterface/1.0")

#endif
