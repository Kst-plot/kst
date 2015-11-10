/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2015 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tiff16source.h"
#include <tiffio.h>


#include <QXmlStreamWriter>
#include <QColor>


using namespace Kst;

static const QString tiff16TypeString = "Tiff16 image";

class Tiff16Source::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup(tiff16TypeString);
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
// Vector interface
//

class DataInterfaceTiff16Vector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceTiff16Vector(unsigned short **z) : _z(z) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return _vectorList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataVector::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  // no interface

  unsigned short **_z;

  QStringList _vectorList;
  int _frameCount;

  void init();
  void clear();
};


void DataInterfaceTiff16Vector::clear()
{
  _vectorList.clear();
  _frameCount = 0;
}


void DataInterfaceTiff16Vector::init()
{
  _vectorList.append( "GRAY" );
}


const DataVector::DataInfo DataInterfaceTiff16Vector::dataInfo(const QString &field, int frame) const
{
  Q_UNUSED(frame)
  if (!_vectorList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(_frameCount, 1);
}



int DataInterfaceTiff16Vector::read(const QString& field, DataVector::ReadInfo& p)
{
  int i = 0;
  int s = p.startingFrame;
  int n = p.numberOfFrames;

  if ( field=="INDEX" ) {
    for ( i=0; i<n; ++i ) {
      p.data[i] = i + s;
    }
  } else if ( field=="GRAY" ) {
    for ( i = s; i<s+n; ++i ) {
      p.data[i-s] = (*_z)[i];
    }
  }

  return i;
}


bool DataInterfaceTiff16Vector::isValid(const QString& field) const {
  return  _vectorList.contains( field );
}

// TODO FRAMES only in vector?
QMap<QString, double> DataInterfaceTiff16Vector::metaScalars(const QString&)
{
  QMap<QString, double> m;
  m["FRAMES"] = _frameCount;
  return m;
}


//
// Matrix interface
//

class DataInterfaceTiff16Matrix : public DataSource::DataInterface<DataMatrix>
{
public:

  DataInterfaceTiff16Matrix(unsigned short **z, int *width, int *height) : _image(z),_width(width),_height(height) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return _matrixList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  // no interface
  unsigned short **_image;
  int *_width;
  int *_height;

  QStringList _matrixList;

  void init();
  void clear();
};

void DataInterfaceTiff16Matrix::clear()
{
  _matrixList.clear();
}

void DataInterfaceTiff16Matrix::init()
{
  _matrixList.append( "GRAY" );
}



const DataMatrix::DataInfo DataInterfaceTiff16Matrix::dataInfo(const QString& matrix, int frame) const
{
  Q_UNUSED(frame)
  if ( !*_image || (*_width * *_height == 0) || !_matrixList.contains( matrix ) ) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;
  info.xSize = *_width;
  info.ySize = *_height;

  return info;
}


int DataInterfaceTiff16Matrix::read(const QString& field, DataMatrix::ReadInfo& p)
{
  if ( !*_image ) {
    return 0;
  }

  int y0 = p.yStart;
  int y1 = p.yStart + p.yNumSteps;
  int x0 = p.xStart;
  int x1 = p.xStart + p.xNumSteps;
  double* z = p.data->z;

  int i = 0, j;

  if ( field=="GRAY") {
    for (int px = p.xStart; px<x1; ++px ) {
      for (int py=y1-1; py>=p.yStart; py-- ) {
        j = py * *_width + px;
        z[i] = (*_image)[j];
        ++i;
      }
    }
  }
    // set the suggested matrix transform params: pixel index....
  p.data->xMin = x0;
  p.data->yMin = y0;
  p.data->xStepSize = 1;
  p.data->yStepSize = 1;

  return i;
}


bool DataInterfaceTiff16Matrix::isValid(const QString& field) const {
  return  _matrixList.contains( field );
}





//
// Tiff16Source
//

Tiff16Source::Tiff16Source(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e) :
  Kst::DataSource(store, cfg, filename, type),
  _z(NULL), _config(0L),
  iv(new DataInterfaceTiff16Vector(&_z)),
  im(new DataInterfaceTiff16Matrix(&_z, &_width, &_height))
{
  setInterface(iv);
  setInterface(im);

  startUpdating(None);

  _valid = false;
  if (!type.isEmpty() && type != tiff16TypeString) {
    return;
  }

  _config = new Tiff16Source::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }
  registerChange();
}


Tiff16Source::~Tiff16Source() {
  delete _config;
  delete [] _z;
  _z = 0L;
  _config = 0L;
}


const QString& Tiff16Source::typeString() const {
  return tiff16TypeString;
}


void Tiff16Source::reset() {
  init();
  Object::reset();
}


bool Tiff16Source::init()
{
  //_image = Tiff16();
  if (_z) {
    delete [] _z;
    _z = 0L;
  }

  iv->clear();
  im->clear();

  TIFF *tif=TIFFOpen(_filename.toAscii(), "r");
  if (tif) {
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &_width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &_height);

    _z = new unsigned short[_width*_height];

    char *buf = (char *)_z;

    int linesize = TIFFScanlineSize(tif);
    for (int i = 0; i < _height; i++) {
      TIFFReadScanline(tif, &buf[i * linesize], i, 0);
    }
  } else {
    _width = _height = 0;
  }

  iv->init();
  im->init();
  registerChange();
  return true;
}


Kst::Object::UpdateType Tiff16Source::internalDataSourceUpdate()
{
  int newNF = _width * _height;
  bool isnew = newNF != iv->_frameCount;

  iv->_frameCount = newNF;

  return (isnew ? Kst::Object::Updated : Kst::Object::NoChange);
}


bool Tiff16Source::isEmpty() const {
  return iv->_frameCount < 1;
}


QString Tiff16Source::fileType() const {
  return tiff16TypeString;
}


void Tiff16Source::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}

/* TODO needed?
int Tiff16Source::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = _frameCount;
    return 1;
  }
  return 0;
}


int Tiff16Source::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  }
  return 0;
}
*/




//
// Tiff16SourcePlugin
//

QString Tiff16SourcePlugin::pluginName() const { return tr("Tiff16 Source Reader"); }
QString Tiff16SourcePlugin::pluginDescription() const { return tr("Tiff16 Source Reader"); }


Kst::DataSource *Tiff16SourcePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new Tiff16Source(store, cfg, filename, type, element);
}



QStringList Tiff16SourcePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = tiff16TypeString;
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  QStringList matrixList;

  if (complete) {
    *complete = true;
  }

  //if ( !Tiff16Reader::imageFormat( filename ).isEmpty() ) {
    matrixList.append( "GRAY" );
  //}
  return matrixList;

}


QStringList Tiff16SourcePlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = tiff16TypeString;
  }

  scalarList.append("FRAMES");
  return scalarList;

}


QStringList Tiff16SourcePlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = tiff16TypeString;
  }

  stringList.append("FILENAME");
  return stringList;

}

QStringList Tiff16SourcePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(type)
  Q_UNUSED(cfg)
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = tiff16TypeString;
  }

  fieldList.append("INDEX");
  fieldList.append( "GRAY" );

  return fieldList;
}


int Tiff16SourcePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)

  quint16 spp, bpp, is_tiled;

  if (filename.toLower().endsWith(".tiff") ||
      filename.toLower().endsWith(".tif")) {

    TIFF *tif=TIFFOpen(filename.toAscii(), "r");

    if (tif) {
      TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
      TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
      is_tiled = TIFFIsTiled(tif);

      TIFFClose(tif);


      if ((bpp == 16) && (spp = 1) && (is_tiled == 0)) {
          return 91;
      }
    }
  }
  return 0;
}



bool Tiff16SourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList Tiff16SourcePlugin::provides() const {
  QStringList rc;
  rc += tiff16TypeString;
  return rc;
}


Kst::DataSourceConfigWidget *Tiff16SourcePlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstdata_tiff16source, Tiff16SourcePlugin)
#endif

// vim: ts=2 sw=2 et
