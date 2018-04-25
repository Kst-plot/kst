/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2015 C. Barth Netterfield
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "itsdatasource.h"
#include "datamatrix.h"
#include "its.h"

#include <QFileInfo>

#define MAX_IMAGES 16

using namespace Kst;

static const QString ITSTypeString = "Indexed Thumbnail Stream";


/**********************
ITSSource::Config - default config widget.  Not used.

***********************/
class ITSSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup(ITSTypeString);
      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      Q_UNUSED(s);
    }

    void load(const QDomElement& e) {
      Q_UNUSED(e);
    }
};


//
// Matrix interface
//

class DataInterfaceITSMatrix : public DataSource::DataInterface<DataMatrix>
{
public:

  DataInterfaceITSMatrix(ITSSource& its) : _its(its) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return _its._matrixNames.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  // no interface
  ITSSource& _its;

  void init();
  void clear();
};

void DataInterfaceITSMatrix::clear()
{
}

void DataInterfaceITSMatrix::init()
{
}



const DataMatrix::DataInfo DataInterfaceITSMatrix::dataInfo(const QString& matrix, int frame) const
{
  if (_its._itsfile->status != ITS_OK) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;

  if (!_its._matrixNames.contains(matrix)) {
    return DataMatrix::DataInfo();
  }

  info.frameCount = ITSnframes(_its._itsfile);
  if (frame>=info.frameCount) {
    return DataMatrix::DataInfo();
  }

  int i_img = _its._matrixNames[matrix];
  ITSreadimage(_its._itsfile, frame, i_img, &_its._itsImage);

  info.xSize = _its._itsImage.w;
  info.ySize = _its._itsImage.h;
  info.invertXHint = false;
  info.invertYHint = false;

  return info;
}


int DataInterfaceITSMatrix::read(const QString& field, DataMatrix::ReadInfo& p)
{

  int y0 = p.yStart;
  int y1 = p.yStart + p.yNumSteps;
  int x0 = p.xStart;
  int x1 = p.xStart + p.xNumSteps;
  int w;
  double* z = p.data->z;
  int frame = p.frame;

  /* verify validity */
  if (_its._itsfile->status != ITS_OK) {
    return 0;
  }
  if (!_its._matrixNames.contains(field)) {
    return 0;
  }

  /* read the ITSimage */
  int i_img = _its._matrixNames[field];
  ITSreadimage(_its._itsfile, frame, i_img, &_its._itsImage);
  w = _its._itsImage.w;
  x1 = qMin(x1, w);
  y1 = qMin((unsigned short)y1, _its._itsImage.h);

  /* copy the image */
  int i = 0;
  for (int px = x0; px<x1; ++px ) {
    for (int py=y1-1; py>=y0; --py ) {
      z[i] = _its._itsImage.img[px + py*w];
      ++i;
    }
  }

  // set the suggested matrix transform params: pixel index....
  p.data->xMin = x0;
  p.data->yMin = y0;
  p.data->xStepSize = 1;
  p.data->yStepSize = 1;

  return i;
}


bool DataInterfaceITSMatrix::isValid(const QString& field) const {
  return  _its._matrixNames.keys().contains( field );
}


//
// Vector interface
//

class DataInterfaceITSVector : public DataSource::DataInterface<DataVector>
{
public:

  DataInterfaceITSVector(ITSSource& its) : _its(its) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return _its._vectorList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataVector::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  ITSSource& _its;

  void init();
  void clear();
};

void DataInterfaceITSVector::clear()
{
}

void DataInterfaceITSVector::init()
{
}



const DataVector::DataInfo DataInterfaceITSVector::dataInfo(const QString& vector, int) const
{
  if (_its._itsfile->status != ITS_OK) {
    return DataVector::DataInfo();
  }

  if (!_its._vectorList.contains(vector)) {
    return DataVector::DataInfo();
  }

  return DataVector::DataInfo(_its._nframes, 1);
}


int DataInterfaceITSVector::read(const QString& field, DataVector::ReadInfo& p)
{

  int f0 = p.startingFrame;
  int nr = p.numberOfFrames;
  int nf = _its._nframes;

  if (f0>nf) {
    return 0;
  }

  if (f0 + nr > nf) {
    nr = nf - f0;
  }

  if (!_its._vectorList.contains(field)) {
    return 0;
  }

  if ( field=="INDEX" ) {
    for (int i=0; i<nr; ++i ) {
      p.data[i] = i + f0;
    }
    return nr;
  }

  return 0;
}


bool DataInterfaceITSVector::isValid(const QString& field) const {
  return  _its._vectorList.contains( field );
}


/**********************
ITSSource - the reader for streams of images developed for the Spider telescope.

***********************/
ITSSource::ITSSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type), _config(0L),
 im(new DataInterfaceITSMatrix(*this)), iv(new DataInterfaceITSVector(*this)),_itsfile(0), _nframes(0)
{

  _updateCheckType = None;

  ITSInitImage(&_itsImage);

  setInterface(im);
  setInterface(iv);

  startUpdating(None);

  _valid = false;
  if (!type.isEmpty() && type != ITSTypeString) {
    return;
  }

  _config = new ITSSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  registerChange();
}



ITSSource::~ITSSource() {
  ITSFreeImage(&_itsImage);
  ITSclose(_itsfile);
  if (_config) {
    delete _config;
  }
}


void ITSSource::reset() {
  if (_itsfile) {
    ITSclose(_itsfile);
  }
  _valid = false;
  if (init()) {
    _valid = true;
  }
  Object::reset();
}


// If the datasource has any predefined fields they should be populated here.
bool ITSSource::init() {


  _itsfile = ITSopen(_filename.toAscii().data());
  if (_itsfile->status != ITS_OK) {
    ITSclose(_itsfile);
    _itsfile = 0;
    return false;
  }

  _matrixNames.clear();

  for (int i=0; i < MAX_IMAGES; i++) {
    _matrixNames[QString("IMG%1").arg(int(i+1), 2, 10, QLatin1Char('0'))] = i;
  }

  _vectorList.clear();
  _vectorList.append("INDEX");

  _nframes = ITSnframes(_itsfile);

  _updateCheckType = Timer;

  registerChange();
  return true; // false if something went wrong
}


// Check if the data in the from the source has updated.  Typically done by checking the frame count of the datasource for 
// changes.
Kst::Object::UpdateType ITSSource::internalDataSourceUpdate() {
  
  if (_valid) {
    int newNF = ITSnframes(_itsfile);
    bool isnew = newNF != _nframes;

    if (_nframes>newNF) {
      reset();
    }

    _nframes = newNF;

    return (isnew ? Updated : NoChange);
  }

  return Kst::Object::NoChange;
}


// TODO a DataSource::DataInterface implementation as example

QString ITSSource::fileType() const {
  return ITSTypeString;
}


void ITSSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString ITSSourcePlugin::pluginName() const { return tr("Indexed Thumbnail Stream Reader"); }
QString ITSSourcePlugin::pluginDescription() const { return tr("Indexed Thumbnail Stream Reader"); }

/**********************
ITSSourcePlugin - This class defines the plugin interface to the DataSource defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

Kst::DataSource *ITSSourcePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new ITSSource(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of matrices.
QStringList ITSSourcePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = ITSTypeString;
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  QStringList matrixList;

  matrixList += "IMG1";
  matrixList += "IMG2";
  matrixList += "IMG3";
  matrixList += "IMG4";
  matrixList += "IMG5";

  return matrixList;

}


// Provides the scalar list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of scalars if necessary.
QStringList ITSSourcePlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = ITSTypeString;
  }

  //scalarList.append("FRAMES");
  return scalarList;

}


// Provides the string list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of strings if necessary.
QStringList ITSSourcePlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = ITSTypeString;
  }

  //stringList.append("FILENAME");
  return stringList;

}


// Provides the field list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of fields if necessary.
QStringList ITSSourcePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {  
  if (typeSuggestion) {
    *typeSuggestion = ITSTypeString;
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = ITSTypeString;
  }

  QStringList fieldList;

  fieldList.append("INDEX");
  return fieldList;
}


// The main function used to determine if this plugin knows how to process the provided file.
// Each datasource plugin should check the file and return a number between 0 and 100 based 
// on the likelyhood of the file being this type.  100 should only be returned if there is no way
// that the file could be any datasource other than this one.
int ITSSourcePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)

  if (!QFile::exists(filename) || QFileInfo(filename).isDir()) {
    return 0;
  }

  if (isITSfile(filename.toAscii().data())) {
    return (99);
  }
  return 0;
}


bool ITSSourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return false;
}


QStringList ITSSourcePlugin::provides() const {
  QStringList rc;
  rc += ITSTypeString;
  return rc;
}


// Request for this plugins configuration widget.  
Kst::DataSourceConfigWidget *ITSSourcePlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstdata_itssource, ITSSourcePlugin)
#endif

// vim: ts=2 sw=2 et
