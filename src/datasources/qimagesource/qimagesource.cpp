/***************************************************************************
                 qimagesource.cpp  -  data source for qimagesources
                             -------------------
    begin                : Tue Oct 21 2003
    copyright            : (C) 2003 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qimagesource.h"
#include <QImageReader>
#include <qcolor.h>


QimagesourceSource::QimagesourceSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type) {
  _image = QImage();
  if (init()) {
    _valid = true;
  }
}


QimagesourceSource::~QimagesourceSource() {
}


bool QimagesourceSource::reset() {
  init();
  return true;
}


bool QimagesourceSource::init() {

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
    _fieldList.append( "1" );
    _fieldList.append( "2" );
    _fieldList.append( "3" );
    _fieldList.append( "4" );
    _matrixList.append( "GRAY" );
    _matrixList.append( "RED" );
    _matrixList.append( "GREEN" );
    _matrixList.append( "BLUE" );
    _matrixList.append( "1" );
    _matrixList.append( "2" );
    _matrixList.append( "3" );
    _matrixList.append( "4" );
    return update() == KstObject::UPDATE;
  } else {
    _image = QImage();
    return false;
  }
}

KstObject::UpdateType QimagesourceSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }
  int newNF = _image.width()*_image.height();
  bool isnew = newNF != _frameCount;

  _frameCount = newNF;

  updateNumFramesScalar();
  return setLastUpdateResult(isnew ? KstObject::UPDATE : KstObject::NO_CHANGE);
}

bool QimagesourceSource::matrixDimensions( const QString& matrix,
                                           int* xDim, int* yDim) {
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

int QimagesourceSource::readMatrix(KstMatrixData* data,
                                     const QString& field, int xStart,
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
  if ( field=="GRAY" || field == "1") {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qGray( _image.pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="RED" || field == "2" ) {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qRed( _image.pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="GREEN" || field == "3" ) {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[i] = qGreen( _image.pixel( px, py ) );
        i++;
      }
    }
  } else if ( field=="BLUE" || field == "4" ) {
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

int QimagesourceSource::readField(double *v, const QString& field, int s, int n) {
  int i=0, px,  py;

  if ( field=="INDEX" ) {
    for ( i=0; i<n; i++ ) {
      v[i] = i+s;
    }
  } else if ( field=="GRAY" || field == "1" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qGray( _image.pixel( px, py ) );
    }
  } else if ( field=="RED" || field == "2" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qRed( _image.pixel( px, py ) );
    }
  } else if ( field=="GREEN" || field == "3" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qGreen( _image.pixel( px, py ) );
    }
  } else if ( field=="BLUE" || field == "4" ) {
    for ( i=s; i<s+n; i++ ) {
      px = i%_image.width();
      py = i/_image.width();
      v[i-s] = qBlue( _image.pixel( px, py ) );
    }
  }

  return( i );
}


bool QimagesourceSource::isValidField(const QString& field) const {
  return  _fieldList.contains( field );
}

bool QimagesourceSource::isValidMatrix(const QString& field) const {
  return  _matrixList.contains( field );
}

int QimagesourceSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int QimagesourceSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


bool QimagesourceSource::isEmpty() const {
  return _frameCount < 1;
}


QString QimagesourceSource::fileType() const {
  return "QImage compatible Image";
}


void QimagesourceSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}

//#include <kdebug.h>

extern "C" {
KstDataSource *create_qimagesource(KConfig *cfg, const QString& filename, const QString& type) {
  return new QimagesourceSource(cfg, filename, type);
}

QStringList provides_qimagesource() {
  QStringList rc;
  rc += "QImage compatible Image";
  return rc;
}

int understands_qimagesource(KConfig*, const QString& filename) {
  QString ftype( QImageReader::imageFormat( filename ) );

  if ( ftype.isEmpty() ) return 0;

  if ( ftype == "TIFF" ) {
    if ( !filename.endsWith(".tif") ) return 0;
  }

  return 90;
}


QStringList fieldList_qimagesource(KConfig*, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(type)
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "QImage compatible Image";
  }
  if ( !QImageReader::imageFormat( filename ).isEmpty() ) {
    fieldList.append("INDEX");
    fieldList.append( "GRAY" );
    fieldList.append( "RED" );
    fieldList.append( "GREEN" );
    fieldList.append( "BLUE" );
    fieldList.append( "1" );
    fieldList.append( "2" );
    fieldList.append( "3" );
    fieldList.append( "4" );
  }
  return fieldList;
}

}

KST_KEY_DATASOURCE_PLUGIN(qimagesource)

// vim: ts=2 sw=2 et
