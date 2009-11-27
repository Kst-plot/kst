/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qimagesource.h"

#include <QXmlStreamWriter>
#include <QImageReader>
#include <qcolor.h>

#include "kst_i18n.h"

static const QString qimageTypeString = I18N_NOOP("QImage image");

class QImageSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString::null) {
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


QImageSource::QImageSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type, None), _config(0L) {
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


bool QImageSource::reset() {
  init();
  return true;
}


bool QImageSource::init() {
  _image = QImage();
  _matrixList.clear();
  _fieldList.clear();
  _frameCount = 0;
  if ( _image.load( _filename ) ) {
    _fieldList.append("INDEX");
    _fieldList.append( "GRAY" );
    _fieldList.append( "RED" );
    _fieldList.append( "GREEN" );
    _fieldList.append( "BLUE" );
    _matrixList.append( "GRAY" );
    _matrixList.append( "RED" );
    _matrixList.append( "GREEN" );
    _matrixList.append( "BLUE" );
    registerChange();
    return true;
  } else {
    _image = QImage();
    return false;
  }
}


Kst::Object::UpdateType QImageSource::internalDataSourceUpdate() {
  int newNF = _image.width()*_image.height();
  bool isnew = newNF != _frameCount;

  _frameCount = newNF;

  return (isnew ? Kst::Object::Updated : Kst::Object::NoChange);
}


bool QImageSource::matrixDimensions( const QString& matrix, int* xDim, int* yDim) {
  if ( _image.isNull() ) {
    return false;
  }

  if ( !_matrixList.contains( matrix ) ) {
    return false;
  }

  *xDim = _image.width();
  *yDim = _image.height();
  return true;
}


int QImageSource::readMatrix(Kst::MatrixData* data, const QString& field, int xStart,
                                     int yStart, int xNumSteps,
                                     int yNumSteps) {
  int i,  px, py;
  int y0, y1, x0, x1;
  double *z;

  if ( _image.isNull() ) {
    return 0;
  }

  y0 = yStart;
  y1 = yStart+yNumSteps;

  x0 = xStart;
  x1 = xStart+xNumSteps;

  i=0;

  z = data->z;
  if ( field=="GRAY") {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qGray( _image.pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="RED" ) {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qRed( _image.pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="GREEN" ) {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qGreen( _image.pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="BLUE" ) {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qBlue( _image.pixel( px, py ) );
        i++;
      }
    }
  }

    // set the suggested matrix transform params: pixel index....
  data->xMin = x0;
  data->yMin = y0;
  data->xStepSize = 1;
  data->yStepSize = 1;

  return( i );
}


int QImageSource::readField(double *v, const QString& field, int s, int n) {
  int i=0, px,  py;

  if ( field=="INDEX" ) {
    for ( i=0; i<n; i++ ) {
      v[i] = i+s;
    }
  } else if ( field=="GRAY" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qGray( _image.pixel( px, py ) );
    }
  } else if ( field=="RED" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qRed( _image.pixel( px, py ) );
    }
  } else if ( field=="GREEN" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qGreen( _image.pixel( px, py ) );
    }
  } else if ( field=="BLUE" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qBlue( _image.pixel( px, py ) );
    }
  }

  return( i );
}


bool QImageSource::isValidField(const QString& field) const {
  return  _fieldList.contains( field );
}


bool QImageSource::isValidMatrix(const QString& field) const {
  return  _matrixList.contains( field );
}


int QImageSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int QImageSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


bool QImageSource::isEmpty() const {
  return _frameCount < 1;
}


QString QImageSource::fileType() const {
  return qimageTypeString;
}


void QImageSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


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
  QString ftype( QImageReader::imageFormat( filename ) );

  if ( ftype.isEmpty() ) 
    return 0;

  if ( ftype == "TIFF" ) {
    if ( !filename.toLower().endsWith(".tif") ) return 0;
  }


  //QImageReader is incorrectly identifying a single column ascii file with
  // the first row blank as a pcx file... so... enforce filename...
  if ( ftype == "pcx" ) {
    if ( !filename.toLower().endsWith(".pcx") ) return 0;
  }

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
