/***************************************************************************
                     dmcplugin.cpp
                  -------------------
    copyright            : (C) 2010 The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dmcplugin.h"


//
// DmcPlugin
//
static const char* plugin_name= "PLANCK DMC I/O";
QString DmcPlugin::pluginName()        const { return plugin_name; }
QString DmcPlugin::pluginDescription() const { return plugin_name; }



Kst::DataSource *DmcPlugin::create( Kst::ObjectStore *store,
                                    QSettings *cfg,
                                    const QString &filename,
                                    const QString &type,
                                    const QDomElement &element) const
{
  return 0;// new DmcSource(store, cfg, filename, type, element);
}


QStringList DmcPlugin::matrixList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const
{
  Q_UNUSED(cfg);
  Q_UNUSED(filename);
  Q_UNUSED(type);
  Q_UNUSED(typeSuggestion);
  Q_UNUSED(complete);
  return QStringList();
}


QStringList DmcPlugin::scalarList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const
{
  Q_UNUSED(cfg);
  Q_UNUSED(filename);
  Q_UNUSED(type);
  Q_UNUSED(typeSuggestion);
  Q_UNUSED(complete);
  return QStringList();
}


QStringList DmcPlugin::stringList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const
{
  Q_UNUSED(cfg);
  Q_UNUSED(filename);
  Q_UNUSED(type);
  Q_UNUSED(typeSuggestion);
  Q_UNUSED(complete);
  QStringList stringList;

  return stringList;
}


QStringList DmcPlugin::fieldList( QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const

{
  Q_UNUSED(cfg);
  Q_UNUSED(filename);
  Q_UNUSED(type);
  Q_UNUSED(typeSuggestion);
  Q_UNUSED(complete);
  return QStringList();
}


bool DmcPlugin::supportsTime( QSettings *cfg,
                              const QString& filename) const 
{
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList DmcPlugin::provides() const
{
  return QStringList() ;//<< DmcSource::dmcTypeKey();
}


Kst::DataSourceConfigWidget *DmcPlugin::configWidget( QSettings *cfg,
                                                      const QString& filename) const
{

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}





int DmcPlugin::understands( QSettings *cfg,
                            const QString& filename) const
{
  return 0;
}


Q_EXPORT_PLUGIN2(kstdata_dmcsource, DmcPlugin)


// vim: ts=2 sw=2 et
