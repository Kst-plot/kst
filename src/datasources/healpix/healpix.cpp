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

#include "healpix.h"

#include <assert.h>
#include <QXmlStreamWriter>
#include <math.h>

#include "ui_healpixconfig.h"

#define DEFAULT_XDIM 800
#define DEFAULT_YDIM 600
#define HPUNIT_RAD 0
#define HPUNIT_DEG 1
#define HPUNIT_RADEC 2
#define HPUNIT_LATLON 3

#include "kst_i18n.h"

static const QString healpixTypeString = I18N_NOOP("HEALPIX image");

class HealpixSource::Config {
  public:
    Config() {
      _nX = 800;
      _nY = 600;
      _autoTheta = true;
      _autoPhi = true;
      //FIXME switch to radians default later
      _thetaUnits = HPUNIT_RADEC;
      _phiUnits = HPUNIT_RADEC;
      _vecUnits = HPUNIT_RADEC;
      _autoMag = true;
      _vecDegrade = 0;
      _vecTheta = 0;
      _vecPhi = 0;
      _vecQU = false;
    }

    void read(QSettings *cfg, const QString& fileName = QString::null) {
      Q_UNUSED(fileName);
      cfg->beginGroup(healpixTypeString);
      double confThetaMin;
      double confThetaMax;
      double confPhiMin;
      double confPhiMax;
      int tempdegrade;

      if (!fileName.isEmpty()) {
        cfg->endGroup();
        cfg->beginGroup(fileName);
      }
      _nX = cfg->value("Matrix X Dimension", DEFAULT_XDIM).toInt();
      _nY = cfg->value("Matrix Y Dimension", DEFAULT_YDIM).toInt();
      _autoTheta = cfg->value("Theta Autoscale", true).toBool();
      _thetaUnits = cfg->value("Theta Units", HPUNIT_RADEC).toInt();
      confThetaMin = cfg->value("Theta Min", 0).toDouble();
      confThetaMax = cfg->value("Theta Max", 0).toDouble();
      _autoPhi = cfg->value("Phi Autoscale", true).toBool();
      _phiUnits = cfg->value("Phi Units", HPUNIT_RADEC).toInt();
      confPhiMin = cfg->value("Phi Min", 0).toDouble();
      confPhiMax = cfg->value("Phi Max", 0).toDouble();
      _vecTheta = cfg->value("Vector Theta", 0).toInt();
      _vecPhi = cfg->value("Vector Phi", 0).toInt();
      tempdegrade = cfg->value("Vector Degrade Factor", 1).toInt();
      _autoMag = cfg->value("Vector Magnitude Autoscale", true).toBool();
      _maxMag = cfg->value("Vector Max Magnitude", 0).toDouble();
      _vecQU = cfg->value("Vector is QU", false).toBool();

      // check degrade factor
      checkDegrade(tempdegrade);
      _vecDegrade = tempdegrade;

      // convert the entered range values into radians and 
      // force them to the correct range.
      theta2Internal(_thetaUnits, confThetaMin);
      theta2Internal(_thetaUnits, confThetaMax);
      phi2Internal(_phiUnits, confPhiMin);
      phi2Internal(_phiUnits, confPhiMax);

      // swap theta min/max if coordinate system requires it
      if (confThetaMax < confThetaMin) {
        double temp = confThetaMax;
        confThetaMax = confThetaMin;
        confThetaMin = temp;
      }

      _thetaMin = confThetaMin;
      _thetaMax = confThetaMax;
      _phiMin = confPhiMin;
      _phiMax = confPhiMax;

      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      double confThetaMin = _thetaMin;
      double confThetaMax = _thetaMax;
      double confPhiMin = _phiMin;
      double confPhiMax = _phiMax;

      // export the internal range (in radians) to the
      // selected coordinate system
      theta2External(_thetaUnits, confThetaMin);
      theta2External(_thetaUnits, confThetaMax);
      phi2External(_phiUnits, confPhiMin);
      phi2External(_phiUnits, confPhiMax);

      // swap theta min/max if coordinate system requires it
      if (confThetaMax < confThetaMin) {
        double temp = confThetaMax;
        confThetaMax = confThetaMin;
        confThetaMin = temp;
      }
      s.writeStartElement("properties");
      s.writeAttribute("dim-x", QString::number(_nX));
      s.writeAttribute("dim-y", QString::number(_nY));
      s.writeAttribute("theta-auto", QVariant(_autoTheta).toString());
      s.writeAttribute("theta-units", QString::number(_thetaUnits));
      s.writeAttribute("theta-min", QString::number(confThetaMin));
      s.writeAttribute("theta-max", QString::number(confThetaMax));
      s.writeAttribute("phi-auto", QVariant(_autoPhi).toString());
      s.writeAttribute("phi-units", QString::number(_phiUnits));
      s.writeAttribute("phi-min", QString::number(confPhiMin));
      s.writeAttribute("phi-max", QString::number(confPhiMax));
      s.writeAttribute("vector-theta", QString::number(_vecTheta));
      s.writeAttribute("vector-phi", QString::number(_vecPhi));
      s.writeAttribute("vector-degrade", QVariant(_vecDegrade).toString());
      s.writeAttribute("vector-auto", QVariant(_autoMag).toString());
      s.writeAttribute("vector-max", QString::number(_maxMag));
      s.writeAttribute("vector-QU", QVariant(_vecQU).toString());
      s.writeEndElement();
    }

    void parseProperties(QXmlStreamAttributes &properties) {
      double confThetaMin;
      double confThetaMax;
      double confPhiMin;
      double confPhiMax;
      int tempdegrade;

      _nX = properties.value("dim-x").toString().toInt();
      _nY = properties.value("dim-y").toString().toInt();
      _autoTheta = QVariant(properties.value("theta-auto").toString()).toBool();
      _thetaUnits = properties.value("theta-units").toString().toInt();
      confThetaMin = properties.value("theta-min").toString().toDouble();
      confThetaMax = properties.value("theta-max").toString().toDouble();
      _autoPhi = QVariant(properties.value("phi-auto").toString()).toBool();
      _phiUnits = properties.value("phi-units").toString().toInt();
      confPhiMin = properties.value("phi-min").toString().toDouble();
      confPhiMax = properties.value("phi-max").toString().toDouble();
      _vecTheta = properties.value("vector-theta").toString().toInt();
      _vecPhi = properties.value("vector-phi").toString().toInt();
      tempdegrade = QVariant(properties.value("vector-degrade").toString()).toBool();
      _autoMag = QVariant(properties.value("vector-auto").toString()).toBool();
      _vecQU = QVariant(properties.value("vector-QU").toString()).toBool();
      _maxMag = properties.value("vector-max").toString().toDouble();

      checkDegrade(tempdegrade);
      _vecDegrade = tempdegrade;

      // convert the entered range values into radians and 
      // force them to the correct range.
      theta2Internal(_thetaUnits, confThetaMin);
      theta2Internal(_thetaUnits, confThetaMax);
      phi2Internal(_phiUnits, confPhiMin);
      phi2Internal(_phiUnits, confPhiMax);

      // swap theta min/max if coordinate system requires it
      if (confThetaMax < confThetaMin) {
        double temp = confThetaMax;
        confThetaMax = confThetaMin;
        confThetaMin = temp;
      }

      _thetaMin = confThetaMin;
      _thetaMax = confThetaMax;
      _phiMin = confPhiMin;
      _phiMax = confPhiMax;
    }


    void load(const QDomElement& e) {
      double confThetaMin;
      double confThetaMax;
      double confPhiMin;
      double confPhiMax;
      int tempdegrade;
      QDomNode n = e.firstChild();
      while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
          if (e.tagName() == "dim") {
            if (e.hasAttribute("x")) {
              _nX = e.attribute("x").toInt();
            }
            if (e.hasAttribute("y")) {
              _nX = e.attribute("y").toInt();
            }
          } else if (e.tagName() == "theta") {
            if (e.hasAttribute("auto")) {
              _autoTheta = e.attribute("auto").toInt();
            }
            if (e.hasAttribute("units")) {
              _thetaUnits = e.attribute("units").toInt();
            }
            if (e.hasAttribute("min")) {
              confThetaMin = e.attribute("min").toDouble();
            }
            if (e.hasAttribute("max")) {
              confThetaMax = e.attribute("max").toDouble();
            }
          } else if (e.tagName() == "phi") {
            if (e.hasAttribute("auto")) {
              _autoPhi = e.attribute("auto").toInt();
            }
            if (e.hasAttribute("units")) {
              _phiUnits = e.attribute("units").toInt();
            }
            if (e.hasAttribute("min")) {
              confPhiMin = e.attribute("min").toDouble();
            }
            if (e.hasAttribute("max")) {
              confPhiMax = e.attribute("max").toDouble();
            }
          } else if (e.tagName() == "vector") {
            if (e.hasAttribute("auto")) {
              _autoMag = e.attribute("auto").toInt();
            }
            if (e.hasAttribute("degrade")) {
              tempdegrade = e.attribute("degrade").toInt();
              checkDegrade(tempdegrade);
              _vecDegrade = tempdegrade;
            }
            if (e.hasAttribute("theta")) {
              _vecTheta = e.attribute("theta").toInt();
            }
            if (e.hasAttribute("phi")) {
              _vecPhi = e.attribute("phi").toInt();
            }
            if (e.hasAttribute("QU")) {
              _vecQU = e.attribute("QU").toInt();
            }
            if (e.hasAttribute("max")) {
              _maxMag = e.attribute("max").toDouble();
            }
          }
        }
        n = n.nextSibling();
      }

      // convert the entered range values into radians and 
      // force them to the correct range.
      theta2Internal(_thetaUnits, confThetaMin);
      theta2Internal(_thetaUnits, confThetaMax);
      phi2Internal(_phiUnits, confPhiMin);
      phi2Internal(_phiUnits, confPhiMax);

      // swap theta min/max if coordinate system requires it
      if (confThetaMax < confThetaMin) {
        double temp = confThetaMax;
        confThetaMax = confThetaMin;
        confThetaMin = temp;
      }

      _thetaMin = confThetaMin;
      _thetaMax = confThetaMax;
      _phiMin = confPhiMin;
      _phiMax = confPhiMax;
    }

    // data range
    int _nX;
    int _nY;
    double _thetaMin;
    double _phiMin;
    double _thetaMax;
    double _phiMax;
    bool _autoTheta;
    bool _autoPhi;

    //values are
    // 0 : Radians 
    // 1 : Degrees
    // 2 : Degrees RA/DEC
    // 3 : Degrees Lat/Long
    int _thetaUnits;
    int _phiUnits;
    int _vecUnits;

    // vector field
    int _vecDegrade;
    int _vecTheta;
    int _vecPhi;
    bool _autoMag;
    double _maxMag;
    bool _vecQU;

    size_t _mapNside;

    void checkDegrade(int& degrade) {
      int nside = _mapNside;
      if (!degrade) {
        return;
      }
      if ((nside == 1) && (degrade != 0)) {
        degrade = 0;
        return;
      }
      for (int i = 0; i < degrade; i++) {
        nside = (int)(nside/2);
        if (nside == 1) {
          degrade = i+1;
          return;
        }
      }
      return;
    }

    void theta2Internal(int units, double& theta) {
      switch (units) {
        case HPUNIT_RAD: 
          break;
        case HPUNIT_DEG:
          theta *= HEALPIX_PI/180.0;
          break;
        case HPUNIT_RADEC: case HPUNIT_LATLON: 
          theta = (90.0 - theta) * HEALPIX_PI/180.0;
          break;
        default:
          break;
      }
      while (theta < 0.0) {
        theta += HEALPIX_PI;
      }
      while (theta > HEALPIX_PI) {
        theta -= HEALPIX_PI;
      }
      return;
    }

    void theta2External(int units, double& theta) {
      switch (units) {
        case HPUNIT_RAD: 
          break;
        case HPUNIT_DEG:
          theta *= 180.0/HEALPIX_PI;
          break;
        case HPUNIT_RADEC: case HPUNIT_LATLON: 
          theta = 90.0 - theta*180.0/HEALPIX_PI;
          break;
        default:
          break;
      }
      return;
    }

    void phi2Internal(int units, double& phi) {
      switch (units) {
        case HPUNIT_RAD: 
          break;
        case HPUNIT_DEG: case HPUNIT_RADEC: case HPUNIT_LATLON:
          phi *= HEALPIX_PI/180.0;
          break;
        default:
          break;
      }
      while (phi < 0.0) {
        phi += 2.0*HEALPIX_PI;
      }
      while (phi > 2.0*HEALPIX_PI) {
        phi -= 2.0*HEALPIX_PI;
      }
      return;
    }

    void phi2External(int units, double& phi) {
      switch (units) {
        case HPUNIT_RAD: 
          break;
        case HPUNIT_DEG: case HPUNIT_RADEC:
          phi *= 180.0/HEALPIX_PI;
          break;
        case HPUNIT_LATLON: 
          phi *= 180.0/HEALPIX_PI;
          if (phi >= 180.0) {
            phi -= 360.0;
          }
          break;
        default:
          break;
      }
      return;
    }
};


HealpixSource::HealpixSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type, None), _config(0L) {
  _valid = false;

  if (!type.isEmpty() && type != "HEALPIX Source") {
    return;
  }

  _config = new HealpixSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  if (init()) {
    _valid = true;
  }

  update();
}


HealpixSource::~HealpixSource() {
  if (_keys) {
    healpix_keys_free(_keys);
  }
  if (_names) {
    healpix_strarr_free(_names, HEALPIX_FITS_MAXCOL);
  }
  if (_units) {
    healpix_strarr_free(_units, HEALPIX_FITS_MAXCOL);
  }
}


bool HealpixSource::reset() {
  return true;
}


bool HealpixSource::init() {
  size_t poff;

  strncpy(_healpixfile, _filename.toLatin1().data(), HEALPIX_STRNL);

  _names = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
  _units = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
  _keys = healpix_keys_alloc();

  if (!healpix_fits_map_info(_healpixfile, &_mapNside, &_mapOrder, &_mapCoord, &_mapType, &_nMaps, _creator, _extname, _names, _units, _keys)) {
    healpix_keys_free(_keys);
    healpix_strarr_free(_names, HEALPIX_FITS_MAXCOL);
    healpix_strarr_free(_units, HEALPIX_FITS_MAXCOL);

    return false;
  }

  _config->_mapNside = _mapNside;
  if (_mapType == HEALPIX_FITS_CUT) {
    poff = 1;
  } else {
    poff = 0;
  }

  _mapNpix = 12 * _mapNside * _mapNside;

  // populate the metadata
  QString metaVal;
  QString metaName;

  metaName = "NSIDE";
  metaVal.sprintf("%lu", (long unsigned int)_mapNside);
  _metaData.insert(metaName, metaVal);

  metaName = "NPIX";
  metaVal.sprintf("%lu", (long unsigned int)_mapNpix);
  _metaData.insert(metaName, metaVal);

  metaName = "ORDER";
  metaVal.sprintf("%d", _mapOrder);
  _metaData.insert(metaName, metaVal);

  metaName = "COORD";
  metaVal.sprintf("%d", _mapCoord);
  _metaData.insert(metaName, metaVal);

  metaName = "TYPE";
  metaVal.sprintf("%d", _mapType);
  _metaData.insert(metaName, metaVal);

  metaName = "NMAPS";
  metaVal.sprintf("%lu", (long unsigned int)_nMaps);
  _metaData.insert(metaName, metaVal);

  metaName = "CREATOR";
  metaVal.sprintf("%s", _creator);
  _metaData.insert(metaName, metaVal);

  metaName = "EXTNAME";
  metaVal.sprintf("%s", _extname);
  _metaData.insert(metaName, metaVal);

  for (size_t j = 0; j < _keys->nskeys; j++) {
    metaName.sprintf("%s", _keys->skeynames[j]);
    metaVal.sprintf("%s", _keys->skeyvals[j]);
    _metaData.insert(metaName, metaVal);
  }

  for (size_t j = 0; j < _keys->nikeys; j++) {
    metaName.sprintf("%s", _keys->ikeynames[j]);
    metaVal.sprintf("%d", _keys->ikeyvals[j]);
    _metaData.insert(metaName, metaVal);
  }

  for (size_t j = 0; j < _keys->nfkeys; j++) {
    metaName.sprintf("%s", _keys->fkeynames[j]);
    metaVal.sprintf("%e", _keys->fkeyvals[j]);
    _metaData.insert(metaName, metaVal);
  }

  // populate the field list
  QString mapName;
  for (size_t i = 0; i < _nMaps; i++) {
    if (strlen(_names[i+poff]) == 0) {
      mapName.sprintf("%d - %s",(int)(i+1),"MAP");
    } else {
      mapName.sprintf("%d - %s",(int)(i+1),_names[i+poff]);
    }
    if (strlen(_units[i+poff]) == 0) {
      mapName.sprintf("%s (%s)",mapName.toLatin1().data(),"Unknown Units");
    } else {
      mapName.sprintf("%s (%s)",mapName.toLatin1().data(),_units[i+poff]);
    }
    _matrixList.append(mapName);
  }

  if (_mapType == HEALPIX_FITS_CUT) {
    if (strlen(_names[_nMaps+1]) == 0) {
      mapName.sprintf("%s","HITS");
    } else {
      mapName.sprintf("%s",_names[_nMaps+1]);
    }
    _matrixList.append(mapName);
    if (strlen(_names[_nMaps+2]) == 0) {
      mapName.sprintf("%s","ERRORS");
    } else {
      mapName.sprintf("%s",_names[_nMaps+2]);
    }
    if (strlen(_units[_nMaps+2]) == 0) {
      mapName.sprintf("%s (%s)",mapName.toLatin1().data(),"Unknown Units");
    } else {
      mapName.sprintf("%s (%s)",mapName.toLatin1().data(),_units[_nMaps+2]);
    }
    _matrixList.append(mapName);
  }

  _fieldList.append("1 - Vector Field Head Theta");
  _fieldList.append("2 - Vector Field Head Phi");
  _fieldList.append("3 - Vector Field Tail Theta");
  _fieldList.append("4 - Vector Field Tail Phi");

  return true;
}


Kst::Object::UpdateType HealpixSource::update() {
  // updates not supported yet
  // we should check to see if the FITS file has changed on disk
  return Kst::Object::NO_CHANGE;
}


bool HealpixSource::matrixDimensions( const QString& matrix, int* xDim, int* yDim) {
  Q_UNUSED(matrix)
  if (_valid) {
    (*xDim) = _config->_nX;
    (*yDim) = _config->_nY;
    return true;
  }
  return false;
}


int HealpixSource::readMatrix(Kst::MatrixData* data, const QString& field, int xStart,
                                     int yStart, int xNumSteps,
                                     int yNumSteps) {
  // If the file is _valid, then we already
  // have all the header information- no need to read it again.
  // We also know that the matrix index is not out-of-range.
  if (_valid && isValidMatrix(field)) {
    fitsfile *fp;
    int colnum;
    int fieldnum;
    int ret = 0;
    int ncol;
    int ttype;
    long nrows;
    long pcount;
    int tfields;
    char extname[HEALPIX_STRNL]; 
    char comment[HEALPIX_STRNL];
    float *datavec;
    int *pixvec;
    float *mapdata;
    float nullval = 0.0;
    int nnull = 0;
    int keynpix;
    int keyfirst;
    int ischunk;
    long nelem;

    if (_matrixList.contains(field)) {
      fieldnum = _matrixList.findIndex(field);
    } else {
      fieldnum = field.toInt();
      fieldnum--;
    }

    // check range
    if ((xStart >= _config->_nX) || (yStart >= _config->_nY)) {
      return -1;
    }
    int nxread = xNumSteps;
    if (nxread < 0) {
      nxread = 1;
    }
    if (xStart + nxread > _config->_nX) {
      nxread = _config->_nX - xStart;
    }
    int nyread = yNumSteps;
    if (nyread < 0) {
      nyread = 1;
    }
    if (yStart + nyread > _config->_nY) {
      nyread = _config->_nY - yStart;
    }

    if (_mapType == HEALPIX_FITS_CUT) {
      // cut-sphere files have extra pixel/hits/error columns
      colnum = fieldnum + 2;
      ncol = (int)_nMaps + 3;
    } else {
      colnum = fieldnum + 1;
      ncol = (int)_nMaps;
    }

    // open file and move to second header unit
    if (fits_open_file(&fp, _healpixfile, READONLY, &ret)) {
      return -1;
    }

    if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
      return -1;
    }

    // read the number of rows
    if (fits_read_btblhdr(fp, ncol, &nrows, &tfields, NULL, NULL, NULL, extname, &pcount, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return -1;
    }

    mapdata = (float*)calloc(_mapNpix, sizeof(float));
    //initialize data to HEALPIX_NULL
    for (size_t j = 0; j < _mapNpix; j++) {
      mapdata[j] = HEALPIX_NULL;
    }

    if (_mapType == HEALPIX_FITS_CUT) {
      // For a cut-sphere file, we must read the entire
      // file and then re-map the data onto a full-sphere
      // vector.

      datavec = (float*)calloc((size_t) nrows, sizeof(float));
      pixvec = (int*)calloc((size_t) nrows, sizeof(int));

      if (fits_read_col(fp, TINT, 1, 1, 1, nrows, &nullval, pixvec, &nnull, &ret)) {
        free(pixvec);
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }

      if (fits_read_col(fp, TFLOAT, colnum, 1, 1, nrows, &nullval, datavec, &nnull, &ret)) {
        free(pixvec);
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }

      for (long j = 0; j < nrows; j++) {
        if ((pixvec[j] >= 0) && (pixvec[j] < (int)_mapNpix)) {
          mapdata[pixvec[j]] = datavec[j];
        }
      }
      free(pixvec);
      free(datavec);
    } else {
      /* is this a chunk? */
      if ((nrows != (long)(_mapNpix))&&(1024*nrows != (long)(_mapNpix))) {
        /*this must be a chunk file*/
        char charFirstPix[] = "FIRSTPIX";
        if (fits_read_key(fp, TLONG, charFirstPix, &keyfirst, comment, &ret)) {
          /*must at least have FIRSTPIX key*/
          fits_close_file(fp, &ret);
          return 0;
        } else {
          char charNPix[] = "NPIX";
          if (fits_read_key(fp, TLONG, charNPix, &keynpix, comment, &ret)) {
            ret = 0;
            /*might be using LASTPIX instead*/
            char charLastPix[] = "LASTPIX";
            if (fits_read_key(fp, TLONG, charLastPix, &keynpix, comment, &ret)) {
              fits_close_file(fp, &ret);
              return 0;
            } else {
              keynpix = keynpix - keyfirst + 1;
              ischunk = 1;
            }
          } else {
            ischunk = 1;
          }
        }
      } else {
        ischunk = 0;
      }
      if (ischunk) {
        datavec = (float*)calloc((size_t)keynpix, sizeof(float));
        nelem = (long)keynpix;
      } else {
        datavec = (float*)calloc(_mapNpix, sizeof(float));
        nelem = (long)(_mapNpix);
      }
      if (fits_read_col(fp, TFLOAT, colnum, 1, 1, nelem, &nullval, datavec, &nnull, &ret)) {
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      if (ischunk) {
        for (long j = 0; j < nelem; j++) {
          mapdata[j+keyfirst] = datavec[j];
        }
      } else {
        for (long j = 0; j < nelem; j++) {
          mapdata[j] = datavec[j];
        }
      }
      free(datavec);
    }

    fits_close_file(fp, &ret);

    // set the matrix to NULL
    for (int i = 0; i < nxread*nyread; i++) {
      data->z[i] = NAN;
    }

    double theta, phi;
    size_t ppix;

    // compute autorange parameters if necessary
    if (_config->_autoTheta || _config->_autoPhi) {
      double mapMinTheta = HEALPIX_PI;
      double mapMaxTheta = 0.0;
      double mapMinPhi = 2.0*HEALPIX_PI;
      double mapMaxPhi = 0.0;

      for (size_t i = 0; i < _mapNpix; i++) {
        if (!healpix_is_fnull(mapdata[i])) {
          if (_mapOrder == HEALPIX_RING) {
            healpix_pix2ang_ring(_mapNside, i, &theta, &phi);
          } else {
            healpix_pix2ang_nest(_mapNside, i, &theta, &phi);
          }
          if (theta < mapMinTheta) {
            mapMinTheta = theta;
          }
          if (theta > mapMaxTheta) {
            mapMaxTheta = theta;
          }
          if (phi < mapMinPhi) {
            mapMinPhi = phi;
          }
          if (phi > mapMaxPhi) {
            mapMaxPhi = phi;
          }
        }
      }
      if (mapMaxTheta < mapMinTheta) { // no valid data in map
        mapMaxTheta = HEALPIX_PI;
        mapMinTheta = 0.0;
        mapMaxPhi = 2.0*HEALPIX_PI;
        mapMinPhi = 0.0;
      } 
      if (_config->_autoTheta) {
        _config->_thetaMin = mapMinTheta;
        _config->_thetaMax = mapMaxTheta;
      }
      if (_config->_autoPhi) {
        _config->_phiMin = mapMinPhi;
        _config->_phiMax = mapMaxPhi;
      }
      //qDebug() << "HEALPIX autorange is Theta=[" << mapMinTheta << "..." << mapMaxTheta << "] Phi=[" << mapMinPhi << "..." << mapMaxPhi << "]";
    }
    //qDebug() << "HEALPIX using range Theta=[" << _thetaMin << "..." << _thetaMax << "] Phi=[" << _phiMin << "..." << _phiMax << "]";

    // copy sphere data to matrix.
    for (int i = xStart; i < nxread; i++) {
      for (int j = yStart; j < nyread; j++) {
        healpix_proj_rev_car(_config->_thetaMin, _config->_thetaMax, _config->_phiMin, _config->_phiMax, (double)_config->_nX, (double)_config->_nY, (double)i, (double)j, &theta, &phi);
        if ((!healpix_is_dnull(theta)) && (!healpix_is_dnull(phi))) { 
          if (_mapOrder == HEALPIX_RING) {
            healpix_ang2pix_ring(_mapNside, theta, phi, &ppix);
          } else {
            healpix_ang2pix_nest(_mapNside, theta, phi, &ppix);
          }
          if (!healpix_is_fnull(mapdata[ppix])) {
            data->z[i*nyread+j] = (double)mapdata[ppix];
          }
        }
      }
    }
    free(mapdata);

    // FIXME
    // Eventually, we can just always use radians for the
    // matrix range, since actual display units will be
    // handled by the Kst2DRenderer.  For now, set the matrix
    // range to the same units used in the config dialog.
    // Note that this behaviour is broken, since 2 maps
    // from different files could be put on the same plot
    // in different units.  It's the best we can do for now.
    //
    //data->yMin = _thetaMax;
    //data->yStepSize = -(_thetaMax - _thetaMin)/(double)_nY;
    //data->xMin = _phiMin;
    //if (_phiMin > _phiMax) {
    //  data->xStepSize = (_phiMax + (2.0*HEALPIX_PI - _phiMin)) / (double)_nX;
    //} else {
    //  data->xStepSize = ((_phiMax - _phiMin) / (double)_nX;
    //}

    double tMin = _config->_thetaMin;
    double tMax = _config->_thetaMax;
    double pMin = _config->_phiMin;
    double pMax = _config->_phiMax;
    _config->theta2External(_config->_thetaUnits, tMin);
    _config->theta2External(_config->_thetaUnits, tMax);
    _config->phi2External(_config->_phiUnits, pMin);
    _config->phi2External(_config->_phiUnits, pMax);
    //qDebug() << "HEALPIX exported range Theta=[" << tMin << "..." << tMax << "] Phi=[" << pMin << "..." << pMax << "]";

    switch (_config->_thetaUnits) {
      case HPUNIT_RAD: case HPUNIT_DEG:
        data->yMin = tMax;
        data->yStepSize = (tMin - tMax)/(double)_config->_nY;
        break;
      case HPUNIT_RADEC: case HPUNIT_LATLON: 
        data->yMin = tMax;
        data->yStepSize = (tMin - tMax)/(double)_config->_nY;
        break;
      default:
        break;
    }

    data->xMin = pMin;
    switch (_config->_phiUnits) {
      case HPUNIT_RAD:
        if (pMin > pMax) {
          data->xStepSize = (pMax + (2.0*HEALPIX_PI-pMin)) / (double)_config->_nX;
        } else {
          data->xStepSize = (pMax - pMin) / (double)_config->_nX;
        }
        break;
      case HPUNIT_DEG: case HPUNIT_RADEC:
        if (pMin > pMax) {
          data->xStepSize = (pMax + (360.0-pMin)) / (double)_config->_nX;
        } else {
          data->xStepSize = (pMax - pMin) / (double)_config->_nX;
        }
        break;
      case HPUNIT_LATLON: 
        if (pMin > pMax) {
          data->xStepSize = ((180.0+pMax) + (180.0-pMin)) / (double)_config->_nX;
        } else {
          data->xStepSize = (pMax - pMin) / (double)_config->_nX;
        }
        break;
      default:
        break;
    }
    return nxread*nyread;
  } else {
    return -1;
  }
}


int HealpixSource::readField(double *v, const QString& field, int s, int n) {
  Q_UNUSED(s)
  if (_valid && isValidField(field)) {
    fitsfile *fp;
    int coltheta;
    int colphi;
    int ret = 0;
    int ncol;
    int ttype;
    long nrows;
    long pcount;
    int tfields;
    char extname[HEALPIX_STRNL];  
    char comment[HEALPIX_STRNL];  
    float *datavec;
    int *pixvec;
    int *hits;
    float *mapdata;
    float *comptheta;
    float *compphi;
    float nullval;
    int nnull;
    size_t vecNside;
    size_t temppix;
    int fieldnum;
    double theta, phi;
    long nearest[8];
    int keynpix;
    int keyfirst;
    int ischunk;
    long nelem;

    if (_fieldList.contains(field)) {
      fieldnum = _fieldList.findIndex(field);
    } else {
      fieldnum = field.toInt();
      fieldnum--;
    }

    // check range
    if (n <= 0) {
      return -1;
    }

    vecNside = _mapNside;
    for (int i = 0; i < _config->_vecDegrade; i++) {
      vecNside = (size_t)(vecNside/2);
    }

    if (_mapType == HEALPIX_FITS_CUT) {
      // cut-sphere files have extra pixel/hits/error columns
      coltheta = _config->_vecTheta + 2;
      colphi = _config->_vecPhi + 2;
      ncol = (int)_nMaps + 3;
    } else {
      coltheta = _config->_vecTheta + 1;
      colphi = _config->_vecPhi + 1;
      ncol = (int)_nMaps;
    }

    // open file and move to second header unit
    if (fits_open_file(&fp, _healpixfile, READONLY, &ret)) {
      return -1;
    }

    if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
      return -1;
    }

    // read the number of rows
    if (fits_read_btblhdr(fp, ncol, &nrows, &tfields, NULL, NULL, NULL, extname, &pcount, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return -1;
    }

    mapdata = (float*)calloc(_mapNpix, sizeof(float));
    hits = (int*)calloc(12*vecNside*vecNside, sizeof(int));

    //-----------------------------------------
    // Read in the first component and degrade
    //-----------------------------------------
    for (size_t j = 0; j < _mapNpix; j++) {
      mapdata[j] = HEALPIX_NULL;
    }    
    if (_mapType == HEALPIX_FITS_CUT) {
      datavec = (float*)calloc((size_t) nrows, sizeof(float));
      pixvec = (int*)calloc((size_t) nrows, sizeof(int));
      if (fits_read_col(fp, TINT, 1, 1, 1, nrows, &nullval, pixvec, &nnull, &ret)) {
        free(pixvec);
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      if (fits_read_col(fp, TFLOAT, coltheta, 1, 1, nrows, &nullval, datavec, &nnull, &ret)) {
        free(pixvec);
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      for (long j = 0; j < nrows; j++) {
        if ((pixvec[j] >= 0) && (pixvec[j] < (int)_mapNpix)) {
          mapdata[pixvec[j]] = datavec[j];
        }
      }
      free(pixvec);
      free(datavec);
    } else {
      /* is this a chunk? */
      if ((nrows != (long)(_mapNpix))&&(1024*nrows != (long)(_mapNpix))) {
        /*this must be a chunk file*/
        char charFirstPix[] = "FIRSTPIX";
        if (fits_read_key(fp, TLONG, charFirstPix, &keyfirst, comment, &ret)) {
          /*must at least have FIRSTPIX key*/
          fits_close_file(fp, &ret);
          return 0;
        } else {
          char charNPix[] = "NPIX";
          if (fits_read_key(fp, TLONG, charNPix, &keynpix, comment, &ret)) {
            ret = 0;
            /*might be using LASTPIX instead*/
            char charLastPix[] = "LASTPIX";
            if (fits_read_key(fp, TLONG, charLastPix, &keynpix, comment, &ret)) {
              fits_close_file(fp, &ret);
              return 0;
            } else {
              keynpix = keynpix - keyfirst + 1;
              ischunk = 1;
            }
          } else {
            ischunk = 1;
          }
        }
      } else {
        ischunk = 0;
      }
      if (ischunk) {
        datavec = (float*)calloc((size_t)keynpix, sizeof(float));
        nelem = (long)keynpix;
      } else {
        datavec = (float*)calloc(_mapNpix, sizeof(float));
        nelem = (long)(_mapNpix);
      }
      if (fits_read_col(fp, TFLOAT, coltheta, 1, 1, nelem, &nullval, datavec, &nnull, &ret)) {
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      if (ischunk) {
        for (long j = 0; j < nelem; j++) {
          mapdata[j+keyfirst] = datavec[j];
        }
      } else {
        for (long j = 0; j < nelem; j++) {
          mapdata[j] = datavec[j];
        }
      }
      free(datavec);
    }
    comptheta = (float*)calloc(12*vecNside*vecNside, sizeof(float));
    for (size_t i = 0; i < 12*vecNside*vecNside; i++) {
      comptheta[i] = HEALPIX_NULL;
      hits[i] = 0;
    }
    for (size_t i = 0; i < _mapNpix; i++) {
      if (!healpix_is_fnull(mapdata[i])) {
        if (_mapOrder == HEALPIX_NEST) {
          healpix_degrade_nest(_mapNside, i, vecNside, &temppix);
        } else {
          healpix_degrade_ring(_mapNside, i, vecNside, &temppix);
        }
        if (!healpix_is_fnull(comptheta[temppix])) {
          comptheta[temppix] += mapdata[i];
        } else {
          comptheta[temppix] = mapdata[i];
        }
        hits[temppix] += 1;
      }
    }
    for (size_t i = 0; i < 12*vecNside*vecNside; i++) {
      if (!healpix_is_fnull(comptheta[i])) {
        comptheta[i] = comptheta[i] / (double)hits[i];
      }
    }

    //-----------------------------------------
    // Read in the second component and degrade
    //-----------------------------------------
    for (size_t j = 0; j < _mapNpix; j++) {
      mapdata[j] = HEALPIX_NULL;
    }
    if (_mapType == HEALPIX_FITS_CUT) {
      datavec = (float*)calloc((size_t) nrows, sizeof(float));
      pixvec = (int*)calloc((size_t) nrows, sizeof(int));
      if (fits_read_col(fp, TINT, 1, 1, 1, nrows, &nullval, pixvec, &nnull, &ret)) {
        free(pixvec);
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      if (fits_read_col(fp, TFLOAT, colphi, 1, 1, nrows, &nullval, datavec, &nnull, &ret)) {
        free(pixvec);
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      for (long j = 0; j < nrows; j++) {
        if ((pixvec[j] >= 0) && (pixvec[j] < (int)_mapNpix)) {
          mapdata[pixvec[j]] = datavec[j];
        }
      }
      free(pixvec);
      free(datavec);
    } else {
      /* is this a chunk? */
      if ((nrows != (long)(_mapNpix))&&(1024*nrows != (long)(_mapNpix))) {
        /*this must be a chunk file*/
        char charFirstPix[] = "FIRSTPIX";
        if (fits_read_key(fp, TLONG, charFirstPix, &keyfirst, comment, &ret)) {
          /*must at least have FIRSTPIX key*/
          fits_close_file(fp, &ret);
          return 0;
        } else {
          char charNPix[] = "NPIX";
          if (fits_read_key(fp, TLONG, charNPix, &keynpix, comment, &ret)) {
            ret = 0;
            /*might be using LASTPIX instead*/
            char charLastPix[] = "LASTPIX";
            if (fits_read_key(fp, TLONG, charLastPix, &keynpix, comment, &ret)) {
              fits_close_file(fp, &ret);
              return 0;
            } else {
              keynpix = keynpix - keyfirst + 1;
              ischunk = 1;
            }
          } else {
            ischunk = 1;
          }
        }
      } else {
        ischunk = 0;
      }
      if (ischunk) {
        datavec = (float*)calloc((size_t)keynpix, sizeof(float));
        nelem = (long)keynpix;
      } else {
        datavec = (float*)calloc(_mapNpix, sizeof(float));
        nelem = (long)(_mapNpix);
      }
      if (fits_read_col(fp, TFLOAT, colphi, 1, 1, nelem, &nullval, datavec, &nnull, &ret)) {
        free(datavec);
        free(mapdata);
        ret = 0;
        fits_close_file(fp, &ret);
        return -1;
      }
      if (ischunk) {
        for (long j = 0; j < nelem; j++) {
          mapdata[j+keyfirst] = datavec[j];
        }
      } else {
        for (long j = 0; j < nelem; j++) {
          mapdata[j] = datavec[j];
        }
      }
      free(datavec);
    }

    compphi = (float*)calloc(12*vecNside*vecNside, sizeof(float));
    for (size_t i = 0; i < 12*vecNside*vecNside; i++) {
      compphi[i] = HEALPIX_NULL;
      hits[i] = 0;
    }
    for (size_t i = 0; i < _mapNpix; i++) {
      if (!healpix_is_fnull(mapdata[i])) {
        if (_mapOrder == HEALPIX_NEST) {
          healpix_degrade_nest(_mapNside, i, vecNside, &temppix);
        } else {
          healpix_degrade_ring(_mapNside, i, vecNside, &temppix);
        }
        if (!healpix_is_fnull(compphi[temppix])) {
          compphi[temppix] += mapdata[i];
        } else {
          compphi[temppix] = mapdata[i];
        }
        hits[temppix] += 1;
      }
    }
    for (size_t i = 0; i < 12*vecNside*vecNside; i++) {
      if (!healpix_is_fnull(compphi[i])) {
        compphi[i] = compphi[i] / (double)hits[i];
      }
    }

    free(hits);
    free(mapdata);
    fits_close_file(fp, &ret);

    //----------------------------------------------
    // compute head and tail coordinates in radians
    //----------------------------------------------

    // autoscale magnitude if necessary
    double vecMag = 0.0;
    if (_config->_autoMag) {
      for (size_t i = 0; i < 12*vecNside*vecNside; i++) {
        if (sqrt((double)(comptheta[i]*comptheta[i] + compphi[i]*compphi[i])) > vecMag) {
          vecMag = sqrt((double)(comptheta[i]*comptheta[i] + compphi[i]*compphi[i]));
        }
      }
    } else {
      vecMag = _config->_maxMag;
    }

    // find distance to 4 nearest neighbor pixels and 
    // use the average to compute how "long" the max
    // valued vector should be for a given pixel.
    // This is *expensive*, but we only load vector 
    // fields infrequently, so should be ok.

    // only actually compute the component of the 
    // vectorfield that is requested.

    double maxang;
    double thetapart;
    double phipart;
    double headtheta;
    double tailtheta;
    double headphi;
    double tailphi;
    double pmag;
    double alpha;
    // only compute as many elements as requested
    for (size_t i = 0; i < (size_t)n; i++) {
      if (!healpix_is_fnull(comptheta[i]) && !healpix_is_fnull(compphi[i])) {
        if (_config->_vecQU) { //theta is really Q, phi really U
          pmag = sqrt((double)(comptheta[i]*comptheta[i] + compphi[i]*compphi[i]));
          alpha = 0.5 * atan((double)compphi[i] / (double)comptheta[i]);
          thetapart = pmag * cos(alpha);
          phipart = pmag * sin(alpha);
        } else {
          thetapart = (double)comptheta[i];
          phipart = (double)compphi[i];
        }
        if (_mapOrder == HEALPIX_RING) {
          healpix_pix2ang_ring(vecNside, i, &theta, &phi);
        } else {
          healpix_pix2ang_nest(vecNside, i, &theta, &phi);
        }
        //find max angle for this pixel
        healpix_neighbors(vecNside, _mapOrder, i, nearest);
        maxang = healpix_loc_dist(vecNside, _mapOrder, i, nearest[0]);
        maxang += healpix_loc_dist(vecNside, _mapOrder, i, nearest[1]);
        maxang += healpix_loc_dist(vecNside, _mapOrder, i, nearest[2]);
        maxang += healpix_loc_dist(vecNside, _mapOrder, i, nearest[3]);
        maxang /= 4.0;
        thetapart *= maxang / vecMag;
        phipart *= maxang / vecMag;
        //find coordinates of head and tail
        headtheta = theta + 0.5 * thetapart;
        tailtheta = theta - 0.5 * thetapart;
        headphi = phi + 0.5 * phipart;
        tailphi = phi - 0.5 * phipart;
        while (headtheta > HEALPIX_PI) {
          headtheta -= HEALPIX_PI;
        }
        while (headtheta < 0.0) {
          headtheta += HEALPIX_PI;
        }
        while (tailtheta > HEALPIX_PI) {
          tailtheta -= HEALPIX_PI;
        }
        while (tailtheta < 0.0) {
          tailtheta += HEALPIX_PI;
        }
        while (headphi > 2.0 * HEALPIX_PI) {
          headphi -= 2.0 * HEALPIX_PI;
        }
        while (headphi < 0.0) {
          headphi += 2.0 * HEALPIX_PI;
        }
        while (tailphi > 2.0 * HEALPIX_PI) {
          tailphi -= 2.0 * HEALPIX_PI;
        }
        while (tailphi < 0.0) {
          tailphi += 2.0 * HEALPIX_PI;
        }
        switch (fieldnum) {
          case 0: //head theta
            v[i] = headtheta;
            break;
          case 1: //head phi
            v[i] = headphi;
            break;
          case 2: //tail theta
            v[i] = tailtheta;
            break;
          case 3: //tail phi
            v[i] = tailphi;
            break;
          default:
            break;
        }
      }
    }
    return n;
  } else {
    return -1;
  }
}


bool HealpixSource::isValidField(const QString& field) const {
  // need to allow for referencing fields by number, so
  // that command line options work.
  if (_fieldList.contains(field)) {
    return true;
  } else {
    bool ok = false;
    int num = field.toInt(&ok);
    if (ok && num <= (int)_fieldList.count() && num != 0) {
      return true;
    } else {
      return false;
    }
  }

  return _fieldList.contains(field);
}


bool HealpixSource::isValidMatrix(const QString& field) const {
  // need to allow for referencing fields by number, so
  // that command line options work.
  if (_matrixList.contains(field)) {
    return true;
  } else {
    bool ok = false;
    int num = field.toInt(&ok);
    if (ok && num <= (int)_matrixList.count() && num != 0) {
      return true;
    } else {
      return false;
    }
  }
}


int HealpixSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int HealpixSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  if (_valid) {
    // return the number of pixels in the sky for
    // the degraded vectorfield
    size_t vecNside = _mapNside;
    for (int i = 0; i < _config->_vecDegrade; i++) {
      vecNside = (size_t)(vecNside/2);
    }
    return (int)(12*vecNside*vecNside);
  } else {
    return 0;
  }
}


bool HealpixSource::isEmpty() const {
  return !_valid;
}


QString HealpixSource::fileType() const {
  return healpixTypeString;
}


void HealpixSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


void HealpixSource::parseProperties(QXmlStreamAttributes &properties) {
  _config->parseProperties(properties);
}


int HealpixSource::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = 1;
    return 1;
  }
  return 0;
}


int HealpixSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  } else if (_metaData.contains(string)) {
    S = _metaData[string];
    return 1;
  }
  return 0;
}


class ConfigWidgetHealpixInternal : public QWidget, public Ui_HealpixConfig {
  public:
    ConfigWidgetHealpixInternal(QWidget *parent) : QWidget(parent), Ui_HealpixConfig() { setupUi(this); }
};


class ConfigWidgetHealpix : public Kst::DataSourceConfigWidget {
  public:
    ConfigWidgetHealpix() : Kst::DataSourceConfigWidget() {
      QGridLayout *layout = new QGridLayout(this);
      _hc = new ConfigWidgetHealpixInternal(this);
      layout->addWidget(_hc, 0, 0);
      layout->activate();
    }

    ~ConfigWidgetHealpix() {}

    void setConfig(QSettings *cfg) {
      Kst::DataSourceConfigWidget::setConfig(cfg);
    }

    void load() {
       QStringList unitList;
      //FIXME add back in when we have a renderer
      //unitList.append("(Radians)");
      //unitList.append("(Degrees)");
      unitList.append("(RA/DEC)");
      //unitList.append("(Lat/Long)");
      _cfg->beginGroup("Healpix General");
      _hc->matThetaUnits->clear();
      _hc->matPhiUnits->clear();
      _hc->vecTheta->clear();
      _hc->vecPhi->clear();
      _hc->matThetaUnits->insertStringList(unitList);
      _hc->matPhiUnits->insertStringList(unitList);
      //FIXME change to HPUNIT_RAD
      _hc->matThetaUnits->setCurrentItem(0);
      _hc->matPhiUnits->setCurrentItem(0);
      _hc->matDimX->setValue(_cfg->value("Matrix X Dimension", DEFAULT_XDIM).toInt());
      _hc->matDimY->setValue(_cfg->value("Matrix Y Dimension", DEFAULT_YDIM).toInt());
      _hc->matThetaAuto->setChecked(_cfg->value("Theta Autoscale", true).toBool());
      //FIXME Only one combobox item for now...
      //_hc->matThetaUnits->setCurrentItem(_cfg->readEntry("Theta Units", HPUNIT_RAD));
      _hc->matThetaUnits->setCurrentItem(0);
      _hc->matThetaMin->setText(_cfg->value("Theta Min").toString());
      _hc->matThetaMax->setText(_cfg->value("Theta Max").toString());
      _hc->matPhiAuto->setChecked(_cfg->value("Phi Autoscale", true).toBool());
      //FIXME Only one combobox item for now...
      //_hc->matPhiUnits->setCurrentItem(_cfg->readEntry("Phi Units", HPUNIT_RAD));
      _hc->matPhiUnits->setCurrentItem(0);
      _hc->matPhiMin->setText(_cfg->value("Phi Min").toString());
      _hc->matPhiMax->setText(_cfg->value("Phi Max").toString());
      _hc->vecTheta->setCurrentItem(_cfg->value("Vector Theta", 0).toInt());
      _hc->vecPhi->setCurrentItem(_cfg->value("Vector Phi", 0).toInt());
      _hc->vecDegrade->setValue(_cfg->value("Vector Degrade Factor", 1).toInt());
      _hc->vecMagAuto->setChecked(_cfg->value("Vector Magnitude Autoscale", true).toBool());
      _hc->vecMag->setText(_cfg->value("Vector Max Magnitude").toString());
      _hc->vecQU->setChecked(_cfg->value("Vector is QU", false).toBool());

      bool hasInstance = (_instance != 0L);

      if (hasInstance) {
        _hc->vecTheta->insertStringList(_instance->matrixList());
        _hc->vecPhi->insertStringList(_instance->matrixList());
        Kst::SharedPtr<HealpixSource> src = Kst::kst_cast<HealpixSource>(_instance);
        assert(src);
        _cfg->endGroup();
        _cfg->beginGroup(src->fileName());
        _hc->matDimX->setValue(_cfg->value("Matrix X Dimension", DEFAULT_XDIM).toInt());
        _hc->matDimY->setValue(_cfg->value("Matrix Y Dimension", DEFAULT_YDIM).toInt());
        _hc->matThetaAuto->setChecked(_cfg->value("Theta Autoscale", true).toBool());
        //FIXME Only one combobox item for now...
        //_hc->matThetaUnits->setCurrentItem(_cfg->readEntry("Theta Units", HPUNIT_RAD));
        _hc->matThetaUnits->setCurrentItem(0);
        _hc->matThetaMin->setText(_cfg->value("Theta Min").toString());
        _hc->matThetaMax->setText(_cfg->value("Theta Max").toString());
        _hc->matPhiAuto->setChecked(_cfg->value("Phi Autoscale", true).toBool());
        //FIXME Only one combobox item for now...
        //_hc->matPhiUnits->setCurrentItem(_cfg->readEntry("Phi Units", HPUNIT_RAD));
        _hc->matPhiUnits->setCurrentItem(0);
        _hc->matPhiMin->setText(_cfg->value("Phi Min").toString());
        _hc->matPhiMax->setText(_cfg->value("Phi Max").toString());
        _hc->vecTheta->setCurrentItem(_cfg->value("Vector Theta", 0).toInt());
        _hc->vecPhi->setCurrentItem(_cfg->value("Vector Phi", 0).toInt());
        _hc->vecDegrade->setValue(_cfg->value("Vector Degrade Factor", 1).toInt());
        _hc->vecMagAuto->setChecked(_cfg->value("Vector Magnitude Autoscale", true).toBool());
        _hc->vecMag->setText(_cfg->value("Vector Max Magnitude").toString());
        _hc->vecQU->setChecked(_cfg->value("Vector is QU", false).toBool());
        _cfg->endGroup();
      }
    }

    void save() {
      assert(_cfg);
      _cfg->beginGroup("Healpix General");
      // If we have an instance, save settings for that instance only
      Kst::SharedPtr<HealpixSource> src = Kst::kst_cast<HealpixSource>(_instance);
      if (src) {
        _cfg->endGroup();
        _cfg->beginGroup(src->fileName());
      }
      _cfg->writeEntry("Matrix X Dimension", _hc->matDimX->value());
      _cfg->writeEntry("Matrix Y Dimension", _hc->matDimY->value());
      _cfg->writeEntry("Theta Autoscale", _hc->matThetaAuto->isChecked());
      //FIXME override for now
      //_cfg->writeEntry("Theta Units", _hc->matThetaUnits->currentItem());
      _cfg->writeEntry("Theta Units", HPUNIT_RADEC);
      _cfg->writeEntry("Theta Min", _hc->matThetaMin->text());
      _cfg->writeEntry("Theta Max", _hc->matThetaMax->text());
      _cfg->writeEntry("Phi Autoscale", _hc->matPhiAuto->isChecked());
      //FIXME override for now
      //_cfg->writeEntry("Phi Units", _hc->matPhiUnits->currentItem());
      _cfg->writeEntry("Phi Units", HPUNIT_RADEC);
      _cfg->writeEntry("Phi Min", _hc->matPhiMin->text());
      _cfg->writeEntry("Phi Max", _hc->matPhiMax->text());
      _cfg->writeEntry("Vector Theta", _hc->vecTheta->currentItem());
      _cfg->writeEntry("Vector Phi", _hc->vecPhi->currentItem());
      _cfg->writeEntry("Vector Degrade Factor", _hc->vecDegrade->value());
      _cfg->writeEntry("Vector Magnitude Autoscale", _hc->vecMagAuto->isChecked());
      _cfg->writeEntry("Vector Max Magnitude", _hc->vecMag->text());
      _cfg->writeEntry("Vector is QU", _hc->vecQU->isChecked());
      _cfg->endGroup();

      // Update the instance from our new settings
      if (src && src->reusable()) {
        src->_config->read(_cfg, src->fileName());
        src->reset();
      }
    }

    ConfigWidgetHealpixInternal *_hc;
};


QString HealpixPlugin::pluginName() const { return "HEALPIX Source Reader"; }
QString HealpixPlugin::pluginDescription() const { return "HEALPIX Source Reader"; }


Kst::DataSource *HealpixPlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new HealpixSource(store, cfg, filename, type, element);
}



QStringList HealpixPlugin::matrixList(QSettings *cfg,
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
    *typeSuggestion = healpixTypeString;
  }
  if ( understands(cfg, filename) ) {
    size_t poff;

    int mapType;
    int mapOrder;
    int mapCoord;
    size_t mapNside;
    size_t nMaps;
    char healpixfile[HEALPIX_STRNL];
    healpix_keys *keys;
    char creator[HEALPIX_STRNL];
    char extname[HEALPIX_STRNL];
    char **names;
    char **units;

    strncpy(healpixfile, filename.toLatin1().data(), HEALPIX_STRNL);

    names = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
    units = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
    keys = healpix_keys_alloc();

    if (healpix_fits_map_info(healpixfile, &mapNside, &mapOrder, &mapCoord, &mapType, &nMaps, creator, extname, names, units, keys)) {
      if (mapType == HEALPIX_FITS_CUT) {
        poff = 1;
      } else {
        poff = 0;
      }

      // populate the field list
      QString mapName;
      for (size_t i = 0; i < nMaps; i++) {
        if (strlen(names[i+poff]) == 0) {
          mapName.sprintf("%d - %s",(int)(i+1),"MAP");
        } else {
          mapName.sprintf("%d - %s",(int)(i+1),names[i+poff]);
        }
        if (strlen(units[i+poff]) == 0) {
          mapName.sprintf("%s (%s)",mapName.toLatin1().data(),"Unknown Units");
        } else {
          mapName.sprintf("%s (%s)",mapName.toLatin1().data(),units[i+poff]);
        }
        matrixList.append(mapName);
      }

      if (mapType == HEALPIX_FITS_CUT) {
        if (strlen(names[nMaps+1]) == 0) {
          mapName.sprintf("%s","HITS");
        } else {
          mapName.sprintf("%s",names[nMaps+1]);
        }
        matrixList.append(mapName);
        if (strlen(names[nMaps+2]) == 0) {
          mapName.sprintf("%s","ERRORS");
        } else {
          mapName.sprintf("%s",names[nMaps+2]);
        }
        if (strlen(units[nMaps+2]) == 0) {
          mapName.sprintf("%s (%s)",mapName.toLatin1().data(),"Unknown Units");
        } else {
          mapName.sprintf("%s (%s)",mapName.toLatin1().data(),units[nMaps+2]);
        }
        matrixList.append(mapName);
      }
    }

    healpix_keys_free(keys);
    healpix_strarr_free(names, HEALPIX_FITS_MAXCOL);
    healpix_strarr_free(units, HEALPIX_FITS_MAXCOL);
  }

  return matrixList;

}


QStringList HealpixPlugin::scalarList(QSettings *cfg,
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
    *typeSuggestion = healpixTypeString;
  }

  scalarList.append("FRAMES");
  return scalarList;

}


QStringList HealpixPlugin::stringList(QSettings *cfg,
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
    *typeSuggestion = healpixTypeString;
  }

  stringList.append("FILENAME");

  int mapType;
  int mapOrder;
  int mapCoord;
  size_t mapNside;
  size_t nMaps;
  char healpixfile[HEALPIX_STRNL];
  healpix_keys *keys;
  char creator[HEALPIX_STRNL];
  char extname[HEALPIX_STRNL];
  char **names;
  char **units;

  strncpy(healpixfile, filename.toLatin1().data(), HEALPIX_STRNL);

  names = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
  units = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
  keys = healpix_keys_alloc();

  if (healpix_fits_map_info(healpixfile, &mapNside, &mapOrder, &mapCoord, &mapType, &nMaps, creator, extname, names, units, keys)) {
    // populate the metadata
    QString metaVal;
    QString metaName;

    metaName = "NSIDE";
    metaVal.sprintf("%lu", (long unsigned int)mapNside);
    stringList.append(metaName);

    metaName = "NPIX";
    size_t mapNpix = 12 * mapNside * mapNside;
    metaVal.sprintf("%lu", (long unsigned int)mapNpix);
    stringList.append(metaName);

    metaName = "ORDER";
    metaVal.sprintf("%d", mapOrder);
    stringList.append(metaName);

    metaName = "COORD";
    metaVal.sprintf("%d", mapCoord);
    stringList.append(metaName);

    metaName = "TYPE";
    metaVal.sprintf("%d", mapType);
    stringList.append(metaName);

    metaName = "NMAPS";
    metaVal.sprintf("%lu", (long unsigned int)nMaps);
    stringList.append(metaName);

    metaName = "CREATOR";
    metaVal.sprintf("%s", creator);
    stringList.append(metaName);

    metaName = "EXTNAME";
    metaVal.sprintf("%s", extname);
    stringList.append(metaName);

    for (size_t j = 0; j < keys->nskeys; j++) {
      metaName.sprintf("%s", keys->skeynames[j]);
      metaVal.sprintf("%s", keys->skeyvals[j]);
      stringList.append(metaName);
    }

    for (size_t j = 0; j < keys->nikeys; j++) {
      metaName.sprintf("%s", keys->ikeynames[j]);
      metaVal.sprintf("%d", keys->ikeyvals[j]);
      stringList.append(metaName);
    }

    for (size_t j = 0; j < keys->nfkeys; j++) {
      metaName.sprintf("%s", keys->fkeynames[j]);
      metaVal.sprintf("%e", keys->fkeyvals[j]);
      stringList.append(metaName);
    }
  }

  healpix_keys_free(keys);
  healpix_strarr_free(names, HEALPIX_FITS_MAXCOL);
  healpix_strarr_free(units, HEALPIX_FITS_MAXCOL);

  return stringList;

}

QStringList HealpixPlugin::fieldList(QSettings *cfg,
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
    *typeSuggestion = healpixTypeString;
  }
  if ( understands(cfg, filename) ) {
    fieldList.append("1 - Vector Field Head Theta");
    fieldList.append("2 - Vector Field Head Phi");
    fieldList.append("3 - Vector Field Tail Theta");
    fieldList.append("4 - Vector Field Tail Phi");
  }
  return fieldList;
}


int HealpixPlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg)

  int ret;
  char thealpixfile[HEALPIX_STRNL];
  int tOrder;
  int tCoord;
  int tType;
  size_t tNside;
  size_t tMaps;

  strncpy(thealpixfile, filename.toLatin1().data(), HEALPIX_STRNL);
  ret = healpix_fits_map_test(thealpixfile, &tNside, &tOrder, &tCoord, &tType, &tMaps);

  if (ret) {
    // MUST return 100, since LFIIO datasource returns 99
    // for *all* valid FITS files
    return 100;
  } else {
    return 0;
  }
}


bool HealpixPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList HealpixPlugin::provides() const {
  QStringList rc;
  rc += "HEALPIX Source";
  return rc;
}


Kst::DataSourceConfigWidget *HealpixPlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(filename)
  ConfigWidgetHealpix *config = new ConfigWidgetHealpix;
  config->setConfig(cfg);
  config->load();
  return config;
}

Q_EXPORT_PLUGIN2(kstdata_qimagesource, HealpixPlugin)


// vim: ts=2 sw=2 et
