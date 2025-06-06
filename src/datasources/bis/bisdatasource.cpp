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

#include "bisdatasource.h"
#include "datamatrix.h"
#include "bis.h"

#include <QFileInfo>

using namespace Kst;

static const QString BISTypeString = "BIT Image Stream";


/**********************
BISSource::Config - default config widget.  Not used.

***********************/
class BISSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup(BISTypeString);
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

class DataInterfaceBISMatrix : public DataSource::DataInterface<DataMatrix>
{
public:

  DataInterfaceBISMatrix(BISSource& bis) : _bis(bis) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return _bis._matrixHash.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  // no interface
  BISSource& _bis;

  void init();
  void clear();
};

void DataInterfaceBISMatrix::clear()
{
}

void DataInterfaceBISMatrix::init()
{
}



const DataMatrix::DataInfo DataInterfaceBISMatrix::dataInfo(const QString& matrix, int frame) const
{
  if (_bis._bisfile->status != BIS_OK) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;

  if (!_bis._matrixHash.contains(matrix)) {
    return DataMatrix::DataInfo();
  }

  info.frameCount = BISnframes(_bis._bisfile);
  if (frame>=info.frameCount) {
    return DataMatrix::DataInfo();
  }

  int i_img = _bis._matrixHash[matrix];
  BISreadimage(_bis._bisfile, frame, i_img, &_bis._bisImage);

  info.xSize = _bis._bisImage.w;
  info.ySize = _bis._bisImage.h;
  info.invertXHint = false;
  info.invertYHint = false;

  return info;
}


int DataInterfaceBISMatrix::read(const QString& field, DataMatrix::ReadInfo& p)
{

  int y0 = p.yStart;
  int y1 = p.yStart + p.yNumSteps;
  int x0 = p.xStart;
  int x1 = p.xStart + p.xNumSteps;
  int w;
  double* z = p.data->z;
  int frame = p.frame;

  /* verify validity */
  if (_bis._bisfile->status != BIS_OK) {
    return 0;
  }
  if (!_bis._matrixHash.contains(field)) {
    return 0;
  }

  /* read the BISimage */
  int i_img = _bis._matrixHash[field];
  BISreadimage(_bis._bisfile, frame, i_img, &_bis._bisImage);
  w = _bis._bisImage.w;
  x1 = qMin(x1, w);
  y1 = qMin((unsigned short)y1, _bis._bisImage.h);

  /* copy the image */
  int i = 0;
  for (int px = x0; px<x1; ++px ) {
    for (int py=y1-1; py>=y0; --py ) {
      z[i] = _bis._bisImage.img[px + py*w];
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


bool DataInterfaceBISMatrix::isValid(const QString& field) const {
  return  _bis._matrixHash.keys().contains( field );
}


//
// Vector interface
//

class DataInterfaceBISVector : public DataSource::DataInterface<DataVector>
{
public:

  DataInterfaceBISVector(BISSource& bis) : _bis(bis) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return _bis._vectorList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataVector::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  BISSource& _bis;

  void init();
  void clear();
};

void DataInterfaceBISVector::clear()
{
}

void DataInterfaceBISVector::init()
{
}



const DataVector::DataInfo DataInterfaceBISVector::dataInfo(const QString& vector, int) const
{
  if (_bis._bisfile->status != BIS_OK) {
    return DataVector::DataInfo();
  }

  if (!_bis._vectorList.contains(vector)) {
    return DataVector::DataInfo();
  }

  return DataVector::DataInfo(_bis._nframes, 1);
}


int DataInterfaceBISVector::read(const QString& field, DataVector::ReadInfo& p)
{

  int f0 = p.startingFrame;
  int nr = p.numberOfFrames;
  int nf = _bis._nframes;

  if (f0>nf) {
    return 0;
  }

  if (f0 + nr > nf) {
    nr = nf - f0;
  }

  if (!_bis._vectorList.contains(field)) {
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


bool DataInterfaceBISVector::isValid(const QString& field) const {
  return  _bis._vectorList.contains( field );
}


/**********************
BISSource - the reader for streams of images developed for the BIT telescope.

***********************/
BISSource::BISSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type), _config(0L),
 im(new DataInterfaceBISMatrix(*this)), iv(new DataInterfaceBISVector(*this)),_bisfile(0), _nframes(0)
{

  _updateCheckType = None;

  BISInitImage(&_bisImage);

  setInterface(im);
  setInterface(iv);

  startUpdating(None);

  _valid = false;
  if (!type.isEmpty() && type != BISTypeString) {
    return;
  }

  _config = new BISSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  registerChange();
}



BISSource::~BISSource() {
  BISFreeImage(&_bisImage);
  BISclose(_bisfile);
  if (_config) {
    delete _config;
  }
}


void BISSource::reset() {
  if (_bisfile) {
    BISclose(_bisfile);
  }
  _valid = false;
  if (init()) {
    _valid = true;
  }
  Object::reset();
}


// If the datasource has any predefined fields they should be populated here.
bool BISSource::init() {

  _bisfile = BISopen(_filename.toLatin1().data());
  if (_bisfile->status != BIS_OK) {
    BISclose(_bisfile);
    _bisfile = 0;
    return false;
  }

  _matrixHash.clear();

  _matrixHash["IMG1"] = 0;
  _matrixHash["IMG2"] = 1;
  _matrixHash["IMG3"] = 2;
  _matrixHash["IMG4"] = 3;
  _matrixHash["IMG5"] = 4;

  _vectorList.clear();
  _vectorList.append("INDEX");

  _nframes = BISnframes(_bisfile);

  _updateCheckType = Timer;

  registerChange();
  return true; // false if something went wrong
}


// Check if the data in the from the source has updated.  Typically done by checking the frame count of the datasource for 
// changes.
Kst::Object::UpdateType BISSource::internalDataSourceUpdate() {
  
  if (_valid) {
    int newNF = BISnframes(_bisfile);
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

QString BISSource::fileType() const {
  return BISTypeString;
}


void BISSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString BISSourcePlugin::pluginName() const { return tr("BIT Image Stream Reader"); }
QString BISSourcePlugin::pluginDescription() const { return tr("BIT Image Stream Reader"); }

/**********************
BISSourcePlugin - This class defines the plugin interface to the DataSource defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

Kst::DataSource *BISSourcePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new BISSource(store, cfg, filename, type, element);
}


// Provides the matrix list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of matrices.
QStringList BISSourcePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = BISTypeString;
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
QStringList BISSourcePlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = BISTypeString;
  }

  //scalarList.append("FRAMES");
  return scalarList;

}


// Provides the string list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of strings if necessary.
QStringList BISSourcePlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = BISTypeString;
  }

  //stringList.append("FILENAME");
  return stringList;

}


// Provides the field list that this dataSource can provide from the provided filename.
// This function should use understands to validate the file and then open and calculate the 
// list of fields if necessary.
QStringList BISSourcePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {  
  if (typeSuggestion) {
    *typeSuggestion = BISTypeString;
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
    *typeSuggestion = BISTypeString;
  }

  QStringList fieldList;

  fieldList.append("INDEX");
  return fieldList;
}


// The main function used to determine if this plugin knows how to process the provided file.
// Each datasource plugin should check the file and return a number between 0 and 100 based 
// on the likelyhood of the file being this type.  100 should only be returned if there is no way
// that the file could be any datasource other than this one.
int BISSourcePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)

  if (!QFile::exists(filename) || QFileInfo(filename).isDir()) {
    return 0;
  }

  if (isBISfile(filename.toLatin1().data())) {
    return (99);
  }
  return 0;
}


bool BISSourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return false;
}


QStringList BISSourcePlugin::provides() const {
  QStringList rc;
  rc += BISTypeString;
  return rc;
}


// Request for this plugins configuration widget.  
Kst::DataSourceConfigWidget *BISSourcePlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

// vim: ts=2 sw=2 et
