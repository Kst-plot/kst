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

#include "fitsimage.h"

#include <QXmlStreamWriter>
#include <fitsio.h>
#include <math.h>

class FitsImageSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString::null) {
      Q_UNUSED(fileName);
      cfg->beginGroup("FITS Image Source");
      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      Q_UNUSED(s);
    }

    void load(const QDomElement& e) {
      Q_UNUSED(e);
    }
};


FitsImageSource::FitsImageSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type, None), _config(0L) {
  _fptr = 0L;
  _valid = false;

  if (!type.isEmpty() && type != "FITS Image Source") {
    return;
  }

  _config = new FitsImageSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  update();
}



FitsImageSource::~FitsImageSource() {
  int status;
  if (_fptr) {
    fits_close_file( _fptr, &status );
    _fptr = 0L;
  }
}


bool FitsImageSource::reset() {
  init();
  return true;
}


bool FitsImageSource::init() {
  int status = 0;

  _matrixList.clear();
  _fieldList.clear();
  _frameCount = 0;

  fits_open_image( &_fptr, _filename.latin1( ), READONLY, &status );
  if (status == 0) {
    _fieldList.append("INDEX");
    _fieldList.append("1");
    _matrixList.append("1");
    return update() == Kst::Object::UPDATE;
  } else {
    fits_close_file( _fptr, &status );
    _fptr = 0L;
  }
  return false;
}


Kst::Object::UpdateType FitsImageSource::update() {
  long n_axes[3];
  int status = 0;

  fits_get_img_size( _fptr,  2,  n_axes,  &status );

  int newNF = n_axes[0]*n_axes[1];
  bool isnew = newNF != _frameCount;

  _frameCount = newNF;

  return (isnew ? Kst::Object::UPDATE : Kst::Object::NO_CHANGE);
}


bool FitsImageSource::matrixDimensions( const QString& matrix, int* xDim, int* yDim) {
  long n_axes[3];
  int status = 0;

  if (!_matrixList.contains(matrix)) {
    return false;
  }

  fits_get_img_size( _fptr,  2,  n_axes,  &status );

  if (status) {
    return false;
  }

  *xDim = n_axes[0];
  *yDim = n_axes[1];

  return true;
}


int FitsImageSource::readMatrix(Kst::MatrixData* data, const QString& field, int xStart,
                                     int yStart, int xNumSteps, int yNumSteps) {
  long n_axes[2],  fpixel[2] = {1, 1};
  double nullval = NAN;
  double blank = 0.0;
  long n_elements;
  int i,  px, py,  anynull,  ni;
  int y0, y1, x0, x1;
  double *z;
  int status = 0;
  double *buffer;

  if (!_matrixList.contains(field)) {
    return false;
  }

  fits_get_img_size( _fptr,  2,  n_axes,  &status );

  if (status) {
    return false;
  }

  n_elements = n_axes[0]*n_axes[1];
  buffer = (double*)malloc(n_elements*sizeof(double));

  fits_read_pix( _fptr,  TDOUBLE, fpixel, n_elements, &nullval, buffer, &anynull,  &status );

  // Check to see if the file is using the BLANK keyword
  // to indicate the NULL value for the image.  This is
  // not correct useage for floating point images, but 
  // it is used frequently nonetheless... 
  char charBlank[] = "BLANK";
  fits_read_key(_fptr, TDOUBLE, charBlank, &blank, NULL, &status);
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
  x1 = xStart + xNumSteps;
  ni = xNumSteps * yNumSteps - 1;

  i = 0;

  z = data->z;
  if (field=="1") {
    for (px = xStart; px < x1; px++) {
      for (py = y1-1; py >= yStart; py--) {
        z[ni - i] = buffer[px + py*n_axes[0]];
        i++;
      }
    }
  }

  free(buffer);

// NOTE: This code results in invalid stepsize / mins.  Should be reviewed.
// temporarily disabled.

  // set the suggested matrix transform params: pixel index....
//   double x, y, dx, dy, cx, cy;
//   char charCRVal1[] = "CRVAL1";
//   char charCRVal2[] = "CRVAL2";
//   char charCDelt1[] = "CDELT1";
//   char charCDelt2[] = "CDELT2";
//   char charCRPix1[] = "CRPIX1";
//   char charCRPix2[] = "CRPIX2";
//   fits_read_key(_fptr, TDOUBLE, charCRVal1, &x, NULL, &status);
//   fits_read_key(_fptr, TDOUBLE, charCRVal2, &y, NULL, &status);
//   fits_read_key(_fptr, TDOUBLE, charCDelt1, &dx, NULL, &status);
//   fits_read_key(_fptr, TDOUBLE, charCDelt2, &dy, NULL, &status);
//   fits_read_key(_fptr, TDOUBLE, charCRPix1, &cx, NULL, &status);
//   fits_read_key(_fptr, TDOUBLE, charCRPix2, &cy, NULL, &status);
// 
//   if (status) {
    data->xMin = x0;
    data->yMin = y0;
    data->xStepSize = 1;
    data->yStepSize = 1;
//   } else {
//     dx = fabs(dx);
//     dy = fabs(dy);
//     data->xStepSize = dx;
//     data->yStepSize = dy;
//     data->xMin = x - cx*dx;
//     data->yMin = y - cy*dy;
//   }

  return(i);
}


int FitsImageSource::readField(double *v, const QString& field, int s, int n) {
  int i = 0;

  if (!_fieldList.contains(field)) {
    return false;
  }

  if (field=="INDEX") {
    for (i = 0; i < n; i++) {
      v[i] = i+s;
    }
  } else if (field=="1") {
    double *buffer;
    long fpixel[2] = {1, 1};
    double nullval = 0;
    int status = 0, anynull;

    buffer = (double*)malloc(_frameCount*sizeof(double));

    fits_read_pix( _fptr,  TDOUBLE, fpixel, _frameCount, &nullval, buffer, &anynull,  &status );

    for (i = 0; i < n; i++) {
      v[i] = buffer[i+s];
    }

    free( buffer );

  }

  return(i);
}


bool FitsImageSource::isValidField(const QString& field) const {
  return  _fieldList.contains( field );
}


bool FitsImageSource::isValidMatrix(const QString& field) const {
  return  _matrixList.contains( field );
}


int FitsImageSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int FitsImageSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


bool FitsImageSource::isEmpty() const {
  return _frameCount < 1;
}


QString FitsImageSource::fileType() const {
  return "FITS Image";
}


void FitsImageSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


int FitsImageSource::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = _frameCount;
    return 1;
  }
  return 0;
}


int FitsImageSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  }
  return 0;
}


QString FitsImagePlugin::pluginName() const { return "FITS Image Source Reader"; }
QString FitsImagePlugin::pluginDescription() const { return "FITS Image Source Reader"; }


Kst::DataSource *FitsImagePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new FitsImageSource(store, cfg, filename, type, element);
}



QStringList FitsImagePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {
  Q_UNUSED(type)
  QStringList matrixList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "FITS Image";
  }
  if ( understands(cfg, filename) ) {
    matrixList.append( "1" );
  }
  return matrixList;

}


QStringList FitsImagePlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = "FITS Image";
  }

  scalarList.append("FRAMES");
  return scalarList;

}


QStringList FitsImagePlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = "FITS Image";
  }

  stringList.append("FILENAME");
  return stringList;

}

QStringList FitsImagePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(type)
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "FITS Image";
  }
  if (understands(cfg, filename)) {
    fieldList.append("INDEX");
    fieldList.append( "1" );
  }
  return fieldList;
}


int FitsImagePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  fitsfile* ffits;
  int status = 0;
  int ret_val = 0;
  int naxis;

  fits_open_image( &ffits, filename.latin1( ), READONLY, &status );
  fits_get_img_dim( ffits, &naxis,  &status);

  if ((status == 0) && (naxis > 1)) {
    ret_val = 95;
  } else {
    ret_val = 0;
  }

  // status !=0 should prevent close from having trouble...
  fits_close_file( ffits ,  &status );

  return ret_val;
}



bool FitsImagePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList FitsImagePlugin::provides() const {
  QStringList rc;
  rc += "FITS Image Source";
  return rc;
}


Kst::DataSourceConfigWidget *FitsImagePlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

Q_EXPORT_PLUGIN2(kstdata_qimagesource, FitsImagePlugin)


// vim: ts=2 sw=2 et
