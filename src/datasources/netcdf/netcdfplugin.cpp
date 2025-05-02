/***************************************************************************
                netcdf.cpp  -  netCDF file data source reader
                             -------------------
    begin                : 17/06/2004
    copyright            : (C) 2004 Nicolas Brisset <nicodev@users.sourceforge.net>
    email                : kst@kde.org
    modified             : 03/14/05 by K. Scott
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "netcdfplugin.h"
#include "netcdfsource.h"

#include <QFile>

//
// NetCdfPlugin
//

QString NetCdfPlugin::pluginName() const        { return tr("netCDF Reader"); }
QString NetCdfPlugin::pluginDescription() const { return tr("netCDF Reader"); }


Kst::DataSource *NetCdfPlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const
{
  return new NetcdfSource(store, cfg, filename, type, element);
}


QStringList NetCdfPlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const
{
  return QStringList();
}


QStringList NetCdfPlugin::scalarList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const
{

  Q_UNUSED(cfg);
  Q_UNUSED(type)
  QStringList scalarList;
  return scalarList;
}


QStringList NetCdfPlugin::stringList(QSettings *cfg,
                                      const QString& filename,
                                      const QString& type,
                                      QString *typeSuggestion,
                                      bool *complete) const {
  Q_UNUSED(cfg);
  Q_UNUSED(type)
  QStringList stringList;

  return stringList;
}


QStringList NetCdfPlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(cfg);
  Q_UNUSED(type)

  QStringList fieldList;

  return fieldList;
}


bool NetCdfPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList NetCdfPlugin::provides() const
{
  return QStringList() << NetcdfSource::netcdfTypeKey();
}


Kst::DataSourceConfigWidget *NetCdfPlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}


/** understands_netcdf: returns true if:
  - the file is readable (!)
  - the file can be opened by the netcdf library **/
int NetCdfPlugin::understands(QSettings *cfg, const QString& filename) const
{
    QFile f(filename);

    if (!f.open(QFile::ReadOnly)) {
      KST_DBG qDebug() << "Unable to read file !" << endl;
      return 0;
    }

    netCDF::NcFile ncfile(filename.toUtf8().data(), netCDF::NcFile::read);
    if (!ncfile.isNull()) {
      KST_DBG qDebug() << filename << " looks like netCDF !" << endl;
      return 80;
    } else {
      return 0;
    }
  }

