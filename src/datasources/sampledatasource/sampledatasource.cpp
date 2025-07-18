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

#include "sampledatasource.h"

#include <QXmlStreamWriter>
#include <QImageReader>
#include <qcolor.h>

/**********************
SampleDatasourceSource::Config - This class defines the config widget that will be added to the 
Dialog Config Button for configuring the plugin.  This is only needed for special handling required
by the plugin.  Many plugins will not require configuration.  See plugins/sampleplugin for additional
details.

***********************/
class SampleDatasourceSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup("Sample Datasource");
      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      Q_UNUSED(s);
    }

    void load(const QDomElement& e) {
      Q_UNUSED(e);
    }
};


/**********************
SampleDatasourceSource - This class defines the main DataSource which derives from DataSource.
The key functions that this class must provide is the ability to create the source, provide details about the source
be able to process the data.

***********************/
SampleDatasourceSource::SampleDatasourceSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type), _config(0L) {

  startUpdating(None);

  _valid = false;
  if (!type.isEmpty() && type != "Sample Datasource") {
    return;
  }

  _config = new SampleDatasourceSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  registerChange();
}



SampleDatasourceSource::~SampleDatasourceSource() {
}


void SampleDatasourceSource::reset() {
  init();
  Object::reset();
}


// If the datasource has any predefined fields they should be populated here.
bool SampleDatasourceSource::init() {
  registerChange();
  return true; // false if something went wrong
}


// Check if the data in the from the source has updated.  Typically done by checking the frame count of the datasource for 
// changes.
Kst::Object::UpdateType SampleDatasourceSource::internalDataSourceUpdate() {
  return Kst::Object::NoChange;
}


// TODO a DataSource::DataInterface implementation as example




QString SampleDatasourceSource::fileType() const {
  return "Sample Datasource";
}


void SampleDatasourceSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}





// Name used to identify the plugin.  Used when loading the plugin.
QString SampleDatasourcePlugin::pluginName() const { return tr("Sample Datasource Reader"); }
QString SampleDatasourcePlugin::pluginDescription() const { return tr("Sample Datasource Reader"); }

/**********************
SampleDatasourcePlugin - This class defines the plugin interface to the DataSource defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

Kst::DataSource *SampleDatasourcePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new SampleDatasourceSource(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of matrices.
QStringList SampleDatasourcePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = "Sample Datasource";
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  QStringList matrixList;

  return matrixList;

}


// Provides the scalar list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of scalars if necessary.
QStringList SampleDatasourcePlugin::scalarList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  QStringList scalarList;

  if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = "Sample Datasource";
  }

  scalarList.append("FRAMES");
  return scalarList;

}


// Provides the string list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of strings if necessary.
QStringList SampleDatasourcePlugin::stringList(QSettings *cfg,
                                      const QString& filename,
                                      const QString& type,
                                      QString *typeSuggestion,
                                      bool *complete) const {

  QStringList stringList;

  if ((!type.isEmpty() && !provides().contains(type)) || 0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = "Sample Datasource";
  }

  stringList.append("FILENAME");
  return stringList;

}


// Provides the field list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of fields if necessary.
QStringList SampleDatasourcePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  Q_UNUSED(type)

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "Sample Datasource";
  }

  QStringList fieldList;
  return fieldList;
}


// The main function used to determine if this plugin knows how to process the provided file.
// Each datasource plugin should check the file and return a number between 0 and 100 based 
// on the likelyhood of the file being this type.  100 should only be returned if there is no way
// that the file could be any datasource other than this one.
int SampleDatasourcePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;
}



bool SampleDatasourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList SampleDatasourcePlugin::provides() const {
  QStringList rc;
  rc += "Sample Datasource";
  return rc;
}


// Request for this plugins configuration widget.  
Kst::DataSourceConfigWidget *SampleDatasourcePlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

// vim: ts=2 sw=2 et
