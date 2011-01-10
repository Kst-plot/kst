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
#include "dmcsource.h"


using namespace Kst;


//
// DmcPlugin
//
static const char* plugin_name= "PLANCK DMC I/O  plugin";
QString DmcPlugin::pluginName()        const { return plugin_name; }
QString DmcPlugin::pluginDescription() const { return plugin_name; }



Kst::DataSource *DmcPlugin::create( Kst::ObjectStore *store,
                                    QSettings *cfg,
                                    const QString &filename,
                                    const QString &type,
                                    const QDomElement &element) const
{
  return new DmcSource(store, cfg, filename, type, element);
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
  /* old code
QStringList fieldList_dmc(KConfig*, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
    if (!type.isEmpty() && !provides_dmc().contains(type)) {
      return QStringList();
    }

    KstSharedPtr<DMC::Object> pobj = new DMC::Object;
    if (!pobj->setGroup(filename) || !pobj->isValid()) {
      return QStringList();
    }

    if (complete) {
      *complete = true;
    }

    if (typeSuggestion) {
      *typeSuggestion = "PLANCK DMC I/O";
    }

    QStringList rc = pobj->fields();
    rc.prepend("INDEX");
    return rc;
  }
*/
  Q_UNUSED(cfg);
  Q_UNUSED(filename);
  Q_UNUSED(type);
  Q_UNUSED(typeSuggestion);
  Q_UNUSED(complete);

  if (!type.isEmpty() && !provides().contains(type)) {
    return QStringList();
  }

  SharedPtr<DMC::Object> pobj = new DMC::Object;
  if (!pobj->setGroup(filename) || !pobj->isValid()) {
    return QStringList();
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = DmcSource::dmcTypeKey();
  }

  QStringList rc = pobj->fields();
  rc.prepend("INDEX");
  return rc;
}


bool DmcPlugin::supportsTime( QSettings *cfg,
                              const QString& filename) const 
{
  /* old code
bool supportsTime_dmc(KConfig*, const QString& filename) {
    KstSharedPtr<DMC::Object> pobj = new DMC::Object;
    if (!pobj->setGroup(filename) || !pobj->isValid()) {
      return false;
    }
    return pobj->fields().contains("TIMES_OF_SAMPLES") || pobj->fields().contains("TIMESEC");
  }*/

  Q_UNUSED(cfg)
  Q_UNUSED(filename)

  SharedPtr<DMC::Object> pobj = new DMC::Object;
  if (!pobj->setGroup(filename) || !pobj->isValid()) {
    return false;
  }
  return pobj->fields().contains("TIMES_OF_SAMPLES") || pobj->fields().contains("TIMESEC");
}


QStringList DmcPlugin::provides() const
{
  return QStringList() << DmcSource::dmcTypeKey();
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
  /* old code
int understands_dmc(KConfig*, const QString& filename) {
  bool rc = DMC::validDatabase(filename);
  qDebug() << "-> Valid dmc database? " << rc << endl;
  return rc ? 100 : 0;
}*/

  bool rc = DMC::validDatabase(filename);
  qDebug() << "-> Valid dmc database? " << rc << endl;
  return rc ? 100 : 0;
}


Q_EXPORT_PLUGIN2(kstdata_dmcsource, DmcPlugin)


// vim: ts=2 sw=2 et
