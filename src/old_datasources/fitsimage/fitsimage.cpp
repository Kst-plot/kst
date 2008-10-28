/***************************************************************************
                 fitsimage.cpp  -  data source for fitsimages
                             -------------------
    begin                : August 19, 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "fitsimage.h"
#include <fitsio.h>

#include <stdio.h>
#include <math.h>
#include <ksdebug.h>


FitsimageSource::FitsimageSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type) {
  _fptr = 0L;
  if( type.isEmpty( ) || type == "FITS" ) {
    if (init()) {
      _valid = true;
    }
  }
}


FitsimageSource::~FitsimageSource() {
  int status;
  if ( _fptr )
    fits_close_file( _fptr, &status );
  _fptr = 0L;
}


bool FitsimageSource::reset() {
  init();
  return true;
}


bool FitsimageSource::init() {
  int status = 0;

  _matrixList.clear();
  _fieldList.clear();
  _frameCount = 0;

  fits_open_image( &_fptr, _filename.latin1( ), READONLY, &status );
  if ( status == 0 ) {
    _fieldList.append( "INDEX" );
    _fieldList.append( "1" );
    _matrixList.append( "1" );
    return update() == KstObject::UPDATE;
  } else {
    fits_close_file( _fptr, &status );
    _fptr = 0L;
  }
  return false;
}

KstObject::UpdateType FitsimageSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  long n_axes[3];
  int status = 0;

  fits_get_img_size( _fptr,  2,  n_axes,  &status );

  int newNF = n_axes[0]*n_axes[1];
  bool isnew = newNF != _frameCount;

  _frameCount = newNF;

  updateNumFramesScalar();
  return setLastUpdateResult(isnew ? KstObject::UPDATE : KstObject::NO_CHANGE);
}

bool FitsimageSource::matrixDimensions( const QString& matrix,
                                           int* xDim, int* yDim) {
  long n_axes[3];
  int status = 0;

  if ( !_matrixList.contains( matrix ) ) {
    return false;
  }

  fits_get_img_size( _fptr,  2,  n_axes,  &status );

  if ( status ) return false;

  *xDim = n_axes[0];
  *yDim = n_axes[1];
  return true;
}

int FitsimageSource::readMatrix(KstMatrixData* data,
                                     const QString& field, int xStart,
                                     int yStart, int xNumSteps,
                                     int yNumSteps) {
  long n_axes[2],  fpixel[2] = {1, 1};
  double nullval = NAN;
  double blank = 0.0;
  long n_elements;
  int i,  px, py,  anynull,  ni;
  int y0, y1, x0, x1;
  double *z;
  int status = 0;
  double *buffer;

  if ( !_matrixList.contains( field ) ) {
    return false;
  }

  fits_get_img_size( _fptr,  2,  n_axes,  &status );

  if ( status ) return false;

  n_elements = n_axes[0]*n_axes[1];
  buffer = ( double * )malloc( n_elements*sizeof( double ) );

  fits_read_pix( _fptr,  TDOUBLE, fpixel, n_elements,
                 &nullval, buffer, &anynull,  &status );
  
  // Check to see if the file is using the BLANK keyword
  // to indicate the NULL value for the image.  This is
  // not correct useage for floating point images, but 
  // it is used frequently nonetheless... 
  fits_read_key(_fptr, TDOUBLE, "BLANK", &blank, NULL, &status );
  if (status) { //keyword does not exist, ignore it
    status = 0;
  } else { //keyword is used, replace pixels with this value
    double epsilon = fabs(1e-4 * blank);
    for (long j = 0; j < n_elements; j++) {
      if (fabs(buffer[j]-blank) < epsilon) {
        buffer[j] = NAN;
      }
    }
  }

  y0 = yStart;
  y1 = yStart+yNumSteps;

  x0 = xStart;
  x1 = xStart+xNumSteps;
  ni = xNumSteps*yNumSteps-1;

  i=0;

  z = data->z;
  if ( field=="1" ) {
    for ( px = xStart; px<x1; px++ ) {
      for ( py=y1-1; py>=yStart; py-- ) {
        z[ni - i] = buffer[px + py*n_axes[0]];
        i++;
      }
    }
  }

  free( buffer );
  // set the suggested matrix transform params: pixel index....
  double x, y, dx, dy, cx, cy;
  fits_read_key(_fptr, TDOUBLE, "CRVAL1", &x, NULL, &status );
  fits_read_key(_fptr, TDOUBLE, "CRVAL2", &y, NULL, &status );
  fits_read_key(_fptr, TDOUBLE, "CDELT1", &dx, NULL, &status );
  fits_read_key(_fptr, TDOUBLE, "CDELT2", &dy, NULL, &status );
  fits_read_key(_fptr, TDOUBLE, "CRPIX1", &cx, NULL, &status );
  fits_read_key(_fptr, TDOUBLE, "CRPIX2", &cy, NULL, &status );


  if ( status ) {
    data->xMin = x0;
    data->yMin = y0;
    data->xStepSize = 1;
    data->yStepSize = 1;
  } else {
    dx = fabs( dx );
    dy = fabs( dy );
    data->xStepSize =dx;
    data->yStepSize =dy;
    data->xMin = x - cx*dx;
    data->yMin = y - cy*dy;
  }
  return( i );
}

int FitsimageSource::readField(double *v, const QString& field, int s, int n) {
  int i=0;

  if ( !_matrixList.contains( field ) ) {
    return false;
  }

  if ( field=="INDEX" ) {
    for ( i=0; i<n; i++ ) {
      v[i] = i+s;
    }
  } else if ( field=="1" ) {
    double *buffer;
    long fpixel[2] = {1, 1};
    double nullval = 0;
    int status = 0, anynull;


    buffer = ( double * )malloc( _frameCount*sizeof( double ) );

    fits_read_pix( _fptr,  TDOUBLE, fpixel, _frameCount,
                   &nullval, buffer, &anynull,  &status );

    for ( i=0; i<n; i++ ) {
      v[i] = buffer[i+s];
    }

    free( buffer );

  }

  return( i );
}


bool FitsimageSource::isValidField(const QString& field) const {
  return  _fieldList.contains( field );
}

bool FitsimageSource::isValidMatrix(const QString& field) const {
  return  _matrixList.contains( field );
}

int FitsimageSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int FitsimageSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


bool FitsimageSource::isEmpty() const {
  return _frameCount < 1;
}


QString FitsimageSource::fileType() const {
  return "FITS Image";
}


void FitsimageSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}

//#include <kdebug.h>

extern "C" {
KstDataSource *create_fitsimage(KConfig *cfg, const QString& filename, const QString& type) {
  return new FitsimageSource(cfg, filename, type);
}

QStringList provides_fitsimage() {
  QStringList rc;
  rc += "FITS Image";
  return rc;
}

int understands_fitsimage(KConfig*, const QString& filename) {

  fitsfile* ffits;
  int status = 0;
  int ret_val = 0;
  int naxis;

  fits_open_image( &ffits, filename.latin1( ), READONLY, &status );
  fits_get_img_dim( ffits, &naxis,  &status);

  if( ( status == 0 ) && ( naxis>1 ) )
    ret_val = 95;
  else
    ret_val = 0;

  // status !=0 should prevent close from having trouble...
  fits_close_file( ffits ,  &status );

  return ret_val;
}


QStringList fieldList_fitsimage(KConfig*, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(type)
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "FITS Image";
  }
  if ( understands_fitsimage(NULL, filename) ) {
    fieldList.append("INDEX");
    fieldList.append( "1" );
  }
  return fieldList;
}

}

KST_KEY_DATASOURCE_PLUGIN(fitsimage)

// vim: ts=2 sw=2 et
