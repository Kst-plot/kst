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

#include "fitsimage.h"

#include <QXmlStreamWriter>

#include <math.h>
#include <QHash>



using namespace Kst;

static const QString fitsTypeString = "FITS image";
static const QString DefaultMatrixName = "1";

class FitsImageSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup(fitsTypeString);
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
// String interface
//

class DataInterfaceFitsImageString : public DataSource::DataInterface<DataString>
{
public:
  DataInterfaceFitsImageString(FitsImageSource& s) : source(s) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return source._strings.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataString::DataInfo dataInfo(const QString&, int frame=0) const { Q_UNUSED(frame) return DataString::DataInfo(); }
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  FitsImageSource& source;
};


//-------------------------------------------------------------------------------------------
int DataInterfaceFitsImageString::read(const QString& string, DataString::ReadInfo& p)
{
  // TODO read strings from file
  if (isValid(string) && p.value) {
    *p.value = source._strings[string];
    return 1;
  }
  return 0;
}


//-------------------------------------------------------------------------------------------
bool DataInterfaceFitsImageString::isValid(const QString& string) const
{
  // TODO read strings from file
  return  source._strings.contains( string );
}



//
// Matrix interface
//


class DataInterfaceFitsImageMatrix : public DataSource::DataInterface<DataMatrix> {
public:

  DataInterfaceFitsImageMatrix(fitsfile **fitsfileptr) : _fitsfileptr(fitsfileptr) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return _matrixHash.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString &m);
  QMap<QString, QString> metaStrings(const QString &m);


  // no interface
  fitsfile **_fitsfileptr;
  QHash<QString,int> _matrixHash;

  void init();
  void clear();
};

void DataInterfaceFitsImageMatrix::clear()
{
  _matrixHash.clear();
}

void DataInterfaceFitsImageMatrix::init()
{
  int hdu;
  int nhdu;
  int status=0;
  int type;
  QString name;
  char instr[32];
  char tmpstr[1024];

  fits_get_hdu_num(*_fitsfileptr, &hdu);

  _matrixHash.insert(DefaultMatrixName, hdu);

  fits_get_num_hdus(*_fitsfileptr, &nhdu, &status);
  for (hdu = 1; hdu <= nhdu; ++hdu) {
    fits_movabs_hdu(*_fitsfileptr, hdu, &type, &status);
    fits_get_hdu_type(*_fitsfileptr, &type, &status);
    if (type == IMAGE_HDU) {
      fits_read_key_str(*_fitsfileptr, "EXTNAME", instr, tmpstr, &status);
      if (status) {
        name = QString("HDU%1").arg(hdu);
      } else {
        name = QString(instr).trimmed();
      }
      _matrixHash.insert(name, hdu);
    }
  }
}

const DataMatrix::DataInfo DataInterfaceFitsImageMatrix::dataInfo(const QString& matrix, int frame) const
{
  Q_UNUSED(frame)
  long n_axes[3];
  int status = 0;
  int type;

  if ( !*_fitsfileptr || !_matrixHash.contains( matrix ) ) {
    return DataMatrix::DataInfo();
  }

  fits_movabs_hdu(*_fitsfileptr, _matrixHash[matrix], &type, &status);

  fits_get_img_size( *_fitsfileptr,  2,  n_axes,  &status );

  if (status) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;
  info.xSize = n_axes[0];
  info.ySize = n_axes[1];

  char charCDelt1[] = "CDELT1";
  char charCDelt2[] = "CDELT2";
  double dx,dy;
  fits_read_key(*_fitsfileptr, TDOUBLE, charCDelt1, &dx, NULL, &status);
  fits_read_key(*_fitsfileptr, TDOUBLE, charCDelt2, &dy, NULL, &status);

  if (!status) {
    info.invertXHint = (dx<0);
    info.invertYHint = (dy<0);
  }

  return info;
}

QMap<QString, double> DataInterfaceFitsImageMatrix::metaScalars(const QString &matrix) {
  qDebug() << "metascalars for " << matrix;
  QMap<QString, double> M;

  return M;
}

QMap<QString, QString> DataInterfaceFitsImageMatrix::metaStrings(const QString &matrix) {

  QMap<QString, QString> M;
  int status = 0;
  int type;
  char instr[128];

  M.clear();

  if ( !*_fitsfileptr || !_matrixHash.contains( matrix ) ) {
    return M;
  }

  fits_movabs_hdu(*_fitsfileptr, _matrixHash[matrix], &type, &status);

  fits_read_key(*_fitsfileptr, TSTRING, "CTYPE1", instr, NULL, &status);
  if (!status) {
    M.insert("x_quantity", QString(instr).trimmed());
  }
  status = 0;
  fits_read_key(*_fitsfileptr, TSTRING, "CTYPE2", instr, NULL, &status);
  if (!status) {
    M.insert("y_quantity", QString(instr).trimmed());
  }
  status = 0;
  fits_read_key(*_fitsfileptr, TSTRING, "CRUNIT1", instr, NULL, &status);
  if (!status) {
    M.insert("x_units", QString(instr).trimmed());
  }
  status = 0;
  fits_read_key(*_fitsfileptr, TSTRING, "CRUNIT2", instr, NULL, &status);
  if (!status) {
    M.insert("y_units", QString(instr).trimmed());
  }
  status = 0;
  fits_read_key(*_fitsfileptr, TSTRING, "BUNIT", instr, NULL, &status);
  if (!status) {
    M.insert("z_units", QString(instr).trimmed());
  }
  status = 0;

  return M;
}

int DataInterfaceFitsImageMatrix::read(const QString& field, DataMatrix::ReadInfo& p) {
  long n_axes[2],  fpixel[2] = {1, 1};
  double nullval = NAN;
  double blank = 0.0;
  long n_elements;
  int px, py,  anynull;
  int status = 0, type;
  double *buffer;

  if ((!*_fitsfileptr) || (!_matrixHash.contains(field))) {
    return 0;
  }

  fits_movabs_hdu(*_fitsfileptr, _matrixHash[field], &type, &status);

  fits_get_img_size( *_fitsfileptr,  2,  n_axes,  &status );

  if (status) {
    return 0;
  }

  n_elements = n_axes[0]*n_axes[1];
  buffer = (double*)malloc(n_elements*sizeof(double));

  if (fits_read_pix( *_fitsfileptr,  TDOUBLE, fpixel, n_elements, &nullval, buffer, &anynull,  &status )) {
      char errmsg[80];
      fits_get_errstatus(status, errmsg);
      fprintf(stderr, "cannot read pixel data: %s\n", errmsg);
      fflush(stderr);
  }

  // Check to see if the file is using the BLANK keyword
  // to indicate the NULL value for the image.  This is
  // not correct useage for floating point images, but
  // it is used frequently nonetheless...
  char charBlank[] = "BLANK";
  fits_read_key(*_fitsfileptr, TDOUBLE, charBlank, &blank, NULL, &status);
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

  int y0 = p.yStart;
  int y1 = p.yStart + p.yNumSteps;
  int x0 = p.xStart;
  int x1 = p.xStart + p.xNumSteps;
  double* z = p.data->z;

  int ni = p.xNumSteps * p.yNumSteps - 1;
  // set the suggested matrix transform params: pixel index....
  double x, y, dx, dy, cx, cy;
  char charCRVal1[] = "CRVAL1";
  char charCRVal2[] = "CRVAL2";
  char charCDelt1[] = "CDELT1";
  char charCDelt2[] = "CDELT2";
  char charCRPix1[] = "CRPIX1";
  char charCRPix2[] = "CRPIX2";
  fits_read_key(*_fitsfileptr, TDOUBLE, charCRVal1, &x, NULL, &status);
  fits_read_key(*_fitsfileptr, TDOUBLE, charCRVal2, &y, NULL, &status);
  fits_read_key(*_fitsfileptr, TDOUBLE, charCDelt1, &dx, NULL, &status);
  fits_read_key(*_fitsfileptr, TDOUBLE, charCDelt2, &dy, NULL, &status);
  fits_read_key(*_fitsfileptr, TDOUBLE, charCRPix1, &cx, NULL, &status);
  fits_read_key(*_fitsfileptr, TDOUBLE, charCRPix2, &cy, NULL, &status);

  if (status) {
    dx = 1;
    dy = 1;
  }

  int i = 0;

  if ((dx<0) && (dy>0)) {
    for (px = p.xStart; px < x1; ++px) {
      for (py = y1-1; py >= p.yStart; --py) {
        z[ni - i] = buffer[px + py*n_axes[0]];
        i++;
      }
    }
  } else if ((dx>0) && (dy>0)) {
    for (px = x1-1; px >= p.xStart; --px) {
      for (py = y1-1; py >= p.yStart; --py) {
        z[ni - i] = buffer[px + py*n_axes[0]];
        i++;
      }
    }
  } else if ((dx>0) && (dy<0)) {
    for (px = x1-1; px >= p.xStart; --px) {
      for (py = p.yStart; py < y1; ++py) {
        z[ni - i] = buffer[px + py*n_axes[0]];
        i++;
      }
    }
  } else if ((dx<0) && (dy<0)) {
    for (px = p.xStart; px < x1; ++px) {
      for (py = p.yStart; py < y1; ++py) {
        z[ni - i] = buffer[px + py*n_axes[0]];
        i++;
      }
    }
  }
  free(buffer);

  if (status) {
    p.data->xMin = x0;
    p.data->yMin = y0;
    p.data->xStepSize = 1;
    p.data->yStepSize = 1;
  } else {
    dx = fabs(dx);
    dy = fabs(dy);
    p.data->xStepSize = dx;
    p.data->yStepSize = dy;
    p.data->xMin = x - cx*dx;
    p.data->yMin = y - cy*dy;
  }

  return(i);
}

bool DataInterfaceFitsImageMatrix::isValid(const QString& field) const {
  return  _matrixHash.contains( field );
}


FitsImageSource::FitsImageSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type),
  _config(0L),
  is(new DataInterfaceFitsImageString(*this)),
  im(new DataInterfaceFitsImageMatrix(&_fptr))
{
  setInterface(is);
  setInterface(im);

  setUpdateType(None);

  _fptr = 0L;
  _valid = false;

  if (!type.isEmpty() && type != fitsTypeString) {
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

  registerChange();
}



FitsImageSource::~FitsImageSource() {
  int status = 0;
  if (_fptr) {
    fits_close_file( _fptr, &status );
    _fptr = 0L;
  }
  delete _config;
  _config = 0L;
}

const QString& FitsImageSource::typeString() const {
  return fitsTypeString;
}



void FitsImageSource::reset() {
  init();
  Object::reset();
}

bool FitsImageSource::init() {
  int status = 0;
  fits_open_image( &_fptr, _filename.toAscii(), READONLY, &status );

  im->clear();
  _strings = fileMetas();
  if (status == 0) {
    im->init();

    registerChange();
    return true;
  } else {
    fits_close_file( _fptr, &status );
    _fptr = 0L;
    return false;
  }
}


Kst::Object::UpdateType FitsImageSource::internalDataSourceUpdate() {
  return (Kst::Object::NoChange);
}


bool FitsImageSource::isEmpty() const {
  return im->dataInfo(DefaultMatrixName,0).xSize < 1;
}


QString FitsImageSource::fileType() const {
  return fitsTypeString;
}


void FitsImageSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


QString FitsImagePlugin::pluginName() const { return tr("FITS Image Source Reader"); }
QString FitsImagePlugin::pluginDescription() const { return tr("FITS Image Source Reader"); }


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
    *typeSuggestion = fitsTypeString;
  }

  if ( understands(cfg, filename) ) {
    fitsfile* ffits;
    int status = 0;
    int hdu;
    int nhdu;
    int type;
    QString name;
    char instr[32];
    char tmpstr[1024];


    fits_open_image( &ffits, filename.toAscii(), READONLY, &status );
    matrixList.append( DefaultMatrixName );

    fits_get_num_hdus(ffits, &nhdu, &status);
    for (hdu = 1; hdu <= nhdu; ++hdu) {
      fits_movabs_hdu(ffits, hdu, &type, &status);
      fits_get_hdu_type(ffits, &type, &status);
      if (type == IMAGE_HDU) {
        fits_read_key_str(ffits, "EXTNAME", instr, tmpstr, &status);
        if (status) {
          name = QString("HDU%1").arg(hdu);
        } else {
          name = QString(instr).trimmed();
        }
        matrixList.append(name);
      }
    }

    fits_close_file( ffits ,  &status );
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
    *typeSuggestion = fitsTypeString;
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
    *typeSuggestion = fitsTypeString;
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
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = fitsTypeString;
  }

  return fieldList;
}


int FitsImagePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)
  fitsfile* ffits;
  int status = 0;
  int ret_val = 0;
  int naxis;

  fits_open_image( &ffits, filename.toAscii(), READONLY, &status );
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
  return false;
}


QStringList FitsImagePlugin::provides() const {
  QStringList rc;
  rc += fitsTypeString;
  return rc;
}


Kst::DataSourceConfigWidget *FitsImagePlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

// vim: ts=2 sw=2 et
