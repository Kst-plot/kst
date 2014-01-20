/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2013 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sourcelist.h"
#include "datasourcepluginmanager.h"

#include <QXmlStreamWriter>
#include <QImageReader>
#include <qcolor.h>
#include <QFile>
#include <QFileInfo>



using namespace Kst;

static const QString sourceListTypeString = "List of Datasources";


//
// Vector interface
//

class DataInterfaceSourceListVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceSourceListVector(SourceListSource& d) : sourcelist(d) {}

  // read one element
  int read(const QString& field, DataVector::ReadInfo& p) {return sourcelist.readField(field, p);}

  // named elements
  QStringList list() const { return sourcelist._fieldList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return sourcelist._fieldList.contains( field ); }

  // T specific
  const DataVector::DataInfo dataInfo(const QString&) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }

  //QMap<QString, double> metaScalars(const QString&);
  //QMap<QString, QString> metaStrings(const QString&);

  SourceListSource& sourcelist;
};


const DataVector::DataInfo DataInterfaceSourceListVector::dataInfo(const QString &field) const
{
  if (!sourcelist._fieldList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(sourcelist._frameCount, sourcelist.samplesPerFrame(field));
}

/**********************
SourceListSource::Config - This class defines the config widget that will be added to the
Dialog Config Button for configuring the plugin.  This is only needed for special handling required
by the plugin.  Many plugins will not require configuration.  See plugins/sampleplugin for additional
details.

***********************/
class SourceListSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup("Source List");
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
SourceListSource - This class defines the main DataSource which derives from DataSource.
The key functions that this class must provide is the ability to create the source, provide details about the source
be able to process the data.

***********************/
SourceListSource::SourceListSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
  : Kst::DataSource(store, cfg, filename, type), _config(0L), iv(new DataInterfaceSourceListVector(*this)) {

  setInterface(iv);
  //setInterface(ix);
  //setInterface(is);
  //setInterface(im);

  startUpdating(None);

  _store = store;

  _valid = false;
  if (!type.isEmpty() && type != "Source List") {
    return;
  }

  _config = new SourceListSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  registerChange();
}



SourceListSource::~SourceListSource() {
}


void SourceListSource::reset() {
  init();
  Object::reset();
}


// If the datasource has any predefined fields they should be populated here.
bool SourceListSource::init() {
  _fieldList.clear();
  _scalarList.clear();
  _stringList.clear();
  _matrixList.clear();

  _frameCount = 0;

  // create the child sources, and create the list
  _sources.clear(); // also deletes the smart pointers...
  _sizeList.clear();

  if (!QFile::exists(_filename)) {
    return 0;
  }

  QFile f(_filename);
  if (!f.open(QIODevice::ReadOnly)) {
    return 0;
  }

  QByteArray line;

  _frameCount = 0;

  while (1) {
    line = f.readLine(5000).trimmed();
    if (line.isEmpty()) {
      break;
    }
    // create the data source and add it to _sources
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_store, line);
    if (ds) {
      _sources.append(ds);
    }
  }

  if (_sources.size()>0) {
    _fieldList.append(_sources.at(0)->vector().list());
    foreach (DataSourcePtr ds, _sources) {
      DataVector::DataInfo info = ds->vector().dataInfo(ds->vector().list().at(0));
      _sizeList.append(info.frameCount);
      _frameCount += info.frameCount;
    }
  }

  startUpdating(Timer);

  registerChange();
  return true; // false if something went wrong
}

int SourceListSource::samplesPerFrame(const QString &field) {
  if (_sources.size()>0) {
    DataSourcePtr ds = _sources.at(0);
    DataVector::DataInfo info = ds->vector().dataInfo(field);
    return info.samplesPerFrame;
  }
  return 1;
}


// Check if the data in the from the source has updated.
// For source list, we only check
//   -if a new file has been added
//   -if the last file has grown.
Kst::Object::UpdateType SourceListSource::internalDataSourceUpdate() {
  QFile f(_filename);
  if (!f.open(QIODevice::ReadOnly)) {
    return Kst::Object::NoChange;
  }

  QByteArray line;

  // check if the list has changed
  for (int i = 0; i<_sources.size(); i++) {
    line = f.readLine(5000).trimmed();
    if (line!=_sources.at(i)->fileName()) { // error: better reset
      qDebug() << "source list internal ds update: file list changed";
      reset();
      return (Kst::Object::Updated);
    }
  }
  line = f.readLine(5000).trimmed();
  if (!line.isEmpty()) {
    // create the data source and add it to _sources
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(_store, line);
    if (ds) {
      _sources.append(ds);
    }
  }

  if (_sources.size()>0) {
    if (_fieldList.size()<1) {
      _fieldList.append(_sources.at(0)->vector().list());
    }
    _sizeList.clear();
    int oldFrameCount = _frameCount;
    _frameCount = 0;
    foreach (DataSourcePtr ds, _sources) {
      DataVector::DataInfo info = ds->vector().dataInfo(ds->vector().list().at(0));
      _sizeList.append(info.frameCount);
      _frameCount += info.frameCount;
    }
    if (_frameCount != oldFrameCount) {
      return Kst::Object::Updated;
    }
  }

  return Kst::Object::NoChange;
}


// TODO a DataSource::DataInterface implementation as example




QString SourceListSource::fileType() const {
  return "Source List";
}


void SourceListSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}

// 0 1000 2000 3000
// 0 1    2    3

int SourceListSource::readField(const QString& field, DataVector::ReadInfo& p) {
  int f0 = p.startingFrame;
  int nf = p.numberOfFrames;
  int i_file = 0;
  DataVector::ReadInfo ri;
  int samp_read = 0;
  int f_offset = 0;
  //if (f0<0 && nf >0) {
  //  f0 = qMax(_frameCount - nf, 0);
  //} else if (nf<0 && f0>=0) {
  //  nf = qMax(_frameCount-f0, 0);
  //}

  if (f0 >= 0) {
    while ((f0 >= _sizeList.at(i_file)) && (i_file < _sizeList.size()-1)) {
      f0 -= _sizeList.at(i_file);
      f_offset += _sizeList.at(i_file);
      i_file++;
    }
    if (nf>0) {
      while ((nf>0) && (i_file < _sizeList.size())) {
        int nr = qMin(nf, _sizeList.at(i_file)-f0);
        ri = p;
        ri.startingFrame = f0;
        ri.numberOfFrames = nr;
        ri.data = p.data+samp_read;
        if (field == "INDEX") {
          for (int i=0; i<nr; i++) {
            ri.data[i] = i+f_offset + f0;
          }
          samp_read += nr;
        } else {
          samp_read += _sources[i_file]->vector().read(field, ri);
        }
        nf -= nr;
        f0 = 0;
        f_offset += _sizeList.at(i_file);
        i_file++;
      }
    } else if (nf == -1) { // read one sample
      ri = p;
      ri.startingFrame = f0;
      ri.numberOfFrames = nf;
      samp_read += _sources[i_file]->vector().read(field, ri);
    }
  }
  return samp_read;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString SourceListPlugin::pluginName() const { return "Source List Reader"; }
QString SourceListPlugin::pluginDescription() const { return "Source List Reader"; }

/**********************
SourceListPlugin - This class defines the plugin interface to the DataSource defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

Kst::DataSource *SourceListPlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new SourceListSource(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of matrices.
QStringList SourceListPlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = "Source List";
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
QStringList SourceListPlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = "Source List";
  }

  scalarList.append("FRAMES");
  return scalarList;

}


// Provides the string list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of strings if necessary.
QStringList SourceListPlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = "Source List";
  }

  stringList.append("FILENAME");
  return stringList;

}


// Provides the field list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of fields if necessary.
QStringList SourceListPlugin::fieldList(QSettings *cfg,
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
    *typeSuggestion = "Source List";
  }

  QStringList fieldList;
  return fieldList;
}


// The main function used to determine if this plugin knows how to process the provided file.
// Each datasource plugin should check the file and return a number between 0 and 100 based 
// on the likelyhood of the file being this type.  100 should only be returned if there is no way
// that the file could be any datasource other than this one.
int SourceListPlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)

  if (!QFile::exists(filename)) {
    return 0;
  }

  QFile f(filename);
  if (f.open(QIODevice::ReadOnly)) {

    QByteArray line;
    int i;

    for (i=0; i<5; i++) {
      line = f.readLine(5000).trimmed();
      if (line.isEmpty()) {
        break;
      }
      if (!QFile::exists(line) && !QFileInfo(line).isDir()) {
        return 0;
      }
    }

    if (i>0) {  // all lines (1 or more) listed files
      return 80;
    }
  }
  return 0;
}



bool SourceListPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList SourceListPlugin::provides() const {
  QStringList rc;
  rc += sourceListTypeString;
  return rc;
}


// Request for this plugins configuration widget.  
Kst::DataSourceConfigWidget *SourceListPlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstdata_SourceList, SourceListPlugin)
#endif

// vim: ts=2 sw=2 et
