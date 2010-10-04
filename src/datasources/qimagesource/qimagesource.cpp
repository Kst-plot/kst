/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2005 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qimagesource.h"
#include "kst_i18n.h"

#include <QXmlStreamWriter>
#include <QImageReader>
#include <QColor>


using namespace Kst;

static const QString qimageTypeString = I18N_NOOP("QImage image");

class QImageSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup(qimageTypeString);
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

class DataInterfaceQImageVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceQImageVector(QImage* img) : _image(img) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return _vectorList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataVector::DataInfo dataInfo(const QString&) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  // no interface

  QImage* _image;
  QStringList _vectorList;
  int _frameCount;

  void init();
  void clear();
};


void DataInterfaceQImageVector::clear()
{
  _vectorList.clear();
  _frameCount = 0;
}


void DataInterfaceQImageVector::init()
{
  _vectorList.append( "GRAY" );
  _vectorList.append( "RED" );
  _vectorList.append( "GREEN" );
  _vectorList.append( "BLUE" );
}


const DataVector::DataInfo DataInterfaceQImageVector::dataInfo(const QString &field) const
{
  if (!_vectorList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(_frameCount, 1);
}



int DataInterfaceQImageVector::read(const QString& field, DataVector::ReadInfo& p)
{
  int i = 0;
  int s = p.startingFrame;
  int n = p.numberOfFrames;

  if ( field=="INDEX" ) {
    for ( i=0; i<n; i++ ) {
      p.data[i] = i + s;
    }
  } else if ( field=="GRAY" ) {
    for ( i = s; i<s+n; i++ ) {
      int px = i%_image->width();
      int py = i/_image->width();
      p.data[i-s] = qGray( _image->pixel( px, py ) );
    }
  } else if ( field=="RED" ) {
    for ( i=s; i<s+n; i++ ) {
      int px = i%_image->width();
      int py = i/_image->width();
      p.data[i-s] = qRed( _image->pixel( px, py ) );
    }
  } else if ( field=="GREEN" ) {
    for ( i=s; i<s+n; i++ ) {
      int px = i%_image->width();
      int py = i/_image->width();
      p.data[i-s] = qGreen( _image->pixel( px, py ) );
    }
  } else if ( field=="BLUE" ) {
    for ( i=s; i<s+n; i++ ) {
      int px = i%_image->width();
      int py = i/_image->width();
      p.data[i-s] = qBlue( _image->pixel( px, py ) );
    }
  }

  return i;
}


bool DataInterfaceQImageVector::isValid(const QString& field) const {
  return  _vectorList.contains( field );
}

// TODO FRAMES only in vector?
QMap<QString, double> DataInterfaceQImageVector::metaScalars(const QString&)
{
  QMap<QString, double> m;
  m["FRAMES"] = _frameCount;
  return m;
}


//
// Matrix interface
//

class DataInterfaceQImageMatrix : public DataSource::DataInterface<DataMatrix>
{
public:

  DataInterfaceQImageMatrix(QImage* img) : _image(img) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return _matrixList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo(const QString&) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  // no interface
  QImage* _image;
  QStringList _matrixList;

  void init();
  void clear();
};

void DataInterfaceQImageMatrix::clear()
{
  _matrixList.clear();
}

void DataInterfaceQImageMatrix::init()
{
  _matrixList.append( "GRAY" );
  _matrixList.append( "RED" );
  _matrixList.append( "GREEN" );
  _matrixList.append( "BLUE" );
}



const DataMatrix::DataInfo DataInterfaceQImageMatrix::dataInfo(const QString& matrix) const
{
  if ( !_image || _image->isNull() || !_matrixList.contains( matrix ) ) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;
  info.samplesPerFrame = 1;
  info.xSize = _image->width();
  info.ySize = _image->height();

  return info;
}


int DataInterfaceQImageMatrix::read(const QString& field, DataMatrix::ReadInfo& p)
{
  if ( _image->isNull() ) {
    return 0;
  }

  int y0 = p.yStart;
  int y1 = p.yStart + p.yNumSteps;
  int x0 = p.xStart;
  int x1 = p.xStart + p.xNumSteps;
  double* z = p.data->z;

  int i = 0;

  if ( field=="GRAY") {
    for (int px = p.xStart; px<x1; px++ ) {
      for (int py=y1-1; py>=p.yStart; py-- ) {
        z[i] = qGray( _image->pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="RED" ) {
    for (int px = p.xStart; px<x1; px++ ) {
      for (int py=y1-1; py>=p.yStart; py-- ) {
        z[i] = qRed( _image->pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="GREEN" ) {
    for (int px = p.xStart; px<x1; px++ ) {
      for (int py=y1-1; py>=p.yStart; py-- ) {
        z[i] = qGreen( _image->pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="BLUE" ) {
    for (int px = p.xStart; px<x1; px++ ) {
      for (int py=y1-1; py>=p.yStart; py-- ) {
        z[i] = qBlue( _image->pixel( px, py ) );
        i++;
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


bool DataInterfaceQImageMatrix::isValid(const QString& field) const {
  return  _matrixList.contains( field );
}





//
// QImageSource
//

QImageSource::QImageSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e) :
  Kst::DataSource(store, cfg, filename, type),
  _config(0L),
  iv(new DataInterfaceQImageVector(&_image)),
  im(new DataInterfaceQImageMatrix(&_image))
{
  setInterface(iv);
  setInterface(im);

  setUpdateType(None);

  _valid = false;
  if (!type.isEmpty() && type != qimageTypeString) {
    return;
  }

  _config = new QImageSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  _image = QImage();
  if (init()) {
    _valid = true;
  }
  registerChange();
}


QImageSource::~QImageSource() {
  delete _config;
  _config = 0L;
}


const QString& QImageSource::typeString() const {
  return qimageTypeString;
}


void QImageSource::reset() {
  init();
  Object::reset();
}


bool QImageSource::init()
{
  _image = QImage();
  iv->clear();
  im->clear();
  if (!_image.load( _filename ) ) {
    return false;
  }
  iv->init();
  im->init();
  registerChange();
  return true;
}


Kst::Object::UpdateType QImageSource::internalDataSourceUpdate()
{
  int newNF = _image.width()*_image.height();
  bool isnew = newNF != iv->_frameCount;

  iv->_frameCount = newNF;

  return (isnew ? Kst::Object::Updated : Kst::Object::NoChange);
}


bool QImageSource::isEmpty() const {
  return iv->_frameCount < 1;
}


QString QImageSource::fileType() const {
  return qimageTypeString;
}


void QImageSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}

/* TODO needed?
int QImageSource::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = _frameCount;
    return 1;
  }
  return 0;
}


int QImageSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  }
  return 0;
}
*/




//
// QImageSourcePlugin
//

QString QImageSourcePlugin::pluginName() const { return "QImage Source Reader"; }
QString QImageSourcePlugin::pluginDescription() const { return "QImage Source Reader"; }


Kst::DataSource *QImageSourcePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new QImageSource(store, cfg, filename, type, element);
}



QStringList QImageSourcePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = qimageTypeString;
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

  if ( !QImageReader::imageFormat( filename ).isEmpty() ) {
    matrixList.append( "GRAY" );
    matrixList.append( "RED" );
    matrixList.append( "GREEN" );
    matrixList.append( "BLUE" );
  }
  return matrixList;

}


QStringList QImageSourcePlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = qimageTypeString;
  }

  scalarList.append("FRAMES");
  return scalarList;

}


QStringList QImageSourcePlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = qimageTypeString;
  }

  stringList.append("FILENAME");
  return stringList;

}

QStringList QImageSourcePlugin::fieldList(QSettings *cfg,
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
    *typeSuggestion = qimageTypeString;
  }
  if ( !QImageReader::imageFormat( filename ).isEmpty() ) {
    fieldList.append("INDEX");
    fieldList.append( "GRAY" );
    fieldList.append( "RED" );
    fieldList.append( "GREEN" );
    fieldList.append( "BLUE" );
  }
  return fieldList;
}


int QImageSourcePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)

  QList<QByteArray> formats = QImageReader::supportedImageFormats();

  bool matches = false;
  foreach (QByteArray ext, formats) {
    if (filename.toLower().endsWith(ext.toLower())) {
      matches = true;
      break;
    }
  }
  if (!matches) {
    return 0;
  }

  QString ftype( QImageReader::imageFormat( filename ) );

  if ( ftype.isEmpty() )
    return 0;

  //QImageReader is incorrectly identifying some ascii files with
  // images.  Enforce filenames (may not be needed anymore, since
  // we already know that the extension matches *some* image format.

  //if ( ftype == "TIFF" ) {
  //  if ( !filename.toLower().endsWith(".tif") ) return 0;
  //}
  //if ( ftype == "pcx" ) {
  //  if ( !filename.toLower().endsWith(".pcx") ) return 0;
  //}

  return 90;
}



bool QImageSourcePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList QImageSourcePlugin::provides() const {
  QStringList rc;
  rc += qimageTypeString;
  return rc;
}


Kst::DataSourceConfigWidget *QImageSourcePlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

Q_EXPORT_PLUGIN2(kstdata_qimagesource, QImageSourcePlugin)


// vim: ts=2 sw=2 et
