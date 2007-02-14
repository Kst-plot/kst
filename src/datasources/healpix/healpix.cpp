/***************************************************************************
                    healpix.cpp  -  Healpix data source
                             -------------------
    begin                : Wed June 01 2005
    copyright            : (C) 2005 Ted Kisner
    email                : tskisner.public@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qcheckbox.h>

#include <klineedit.h>
#include <kcombobox.h>
#include <knuminput.h>

#include <ksdebug.h>
#include "kststring.h"
#include "healpix_source.h"
#include "healpixconfig.h"

#define DEFAULT_XDIM 800
#define DEFAULT_YDIM 600
#define HPUNIT_RAD 0
#define HPUNIT_DEG 1
#define HPUNIT_RADEC 2
#define HPUNIT_LATLON 3
 
HealpixSource::HealpixSource(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e) : KstDataSource(cfg, filename, type) {
  int ret;
  size_t poff;
   
  _valid = false;
  if (type.isEmpty( ) || type == "HEALPIX") {
    strncpy(_healpixfile, filename.latin1(), HEALPIX_STRNL);
    
    _names = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
    _units = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
    _keys = healpix_keys_alloc();
    
    ret = healpix_fits_map_info(_healpixfile, &_mapNside, &_mapOrder, &_mapCoord, &_mapType, &_nMaps, _creator, _extname, _names, _units, _keys);
    
    if (_mapType == HEALPIX_FITS_CUT) {
      poff = 1;
    } else {
      poff = 0;
    }
    if (ret) {
      _valid = true;
      _mapNpix = 12 * _mapNside * _mapNside;
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
      loadConfig(cfg);
      if (!e.isNull()) {
        load(e);
      }
      
      // populate the metadata
      QString metaVal;
      QString metaName;
      KstString *metaString;

      metaName = "NSIDE";
      metaVal.sprintf("%lu", (long unsigned int)_mapNside);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "NPIX";
      metaVal.sprintf("%lu", (long unsigned int)_mapNpix);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "ORDER";
      metaVal.sprintf("%d", _mapOrder);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "COORD";
      metaVal.sprintf("%d", _mapCoord);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "TYPE";
      metaVal.sprintf("%d", _mapType);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "NMAPS";
      metaVal.sprintf("%lu", (long unsigned int)_nMaps);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "CREATOR";
      metaVal.sprintf("%s", _creator);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      metaName = "EXTNAME";
      metaVal.sprintf("%s", _extname);
      metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
      _metaData.insert(metaName, metaString);
      
      for (size_t j = 0; j < _keys->nskeys; j++) {
        metaName.sprintf("%s", _keys->skeynames[j]);
        metaVal.sprintf("%s", _keys->skeyvals[j]);
        metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
        _metaData.insert(metaName, metaString);
      }

      for (size_t j = 0; j < _keys->nikeys; j++) {
        metaName.sprintf("%s", _keys->ikeynames[j]);
        metaVal.sprintf("%d", _keys->ikeyvals[j]);
        metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
        _metaData.insert(metaName, metaString);
      }

      for (size_t j = 0; j < _keys->nfkeys; j++) {
        metaName.sprintf("%s", _keys->fkeynames[j]);
        metaVal.sprintf("%e", _keys->fkeyvals[j]);
        metaString = new KstString(KstObjectTag(metaName, tag()), this, metaVal);
        _metaData.insert(metaName, metaString);
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
          mapName.sprintf("%s (%s)",mapName.ascii(),"Unknown Units");
        } else {
          mapName.sprintf("%s (%s)",mapName.ascii(),_units[i+poff]);
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
          mapName.sprintf("%s (%s)",mapName.ascii(),"Unknown Units");
        } else {
          mapName.sprintf("%s (%s)",mapName.ascii(),_units[_nMaps+2]);
        }
        _matrixList.append(mapName);
      }
      _fieldList.append("1 - Vector Field Head Theta");
      _fieldList.append("2 - Vector Field Head Phi");
      _fieldList.append("3 - Vector Field Tail Theta");
      _fieldList.append("4 - Vector Field Tail Phi");
    } else {
      healpix_keys_free(_keys);
      healpix_strarr_free(_names, HEALPIX_FITS_MAXCOL);
      healpix_strarr_free(_units, HEALPIX_FITS_MAXCOL);
    }
  }
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

KstObject::UpdateType HealpixSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }
  // updates not supported yet
  // we should check to see if the FITS file has changed on disk
  return setLastUpdateResult(KstObject::NO_CHANGE);
}

//FIXME we should really change readField so that we can compute 
// a vector field using component maps from different files.
// this will have to wait for the future...

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
    for (int i = 0; i < _vecDegrade; i++) {
      vecNside = (size_t)(vecNside/2);
    }
    
    if (_mapType == HEALPIX_FITS_CUT) {
      // cut-sphere files have extra pixel/hits/error columns
      coltheta = _vecTheta + 2;
      colphi = _vecPhi + 2;
      ncol = (int)_nMaps + 3;
    } else {
      coltheta = _vecTheta + 1;
      colphi = _vecPhi + 1;
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
        if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
          /*must at least have FIRSTPIX key*/
          fits_close_file(fp, &ret);
          return 0;
        } else {
          if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
            ret = 0;
            /*might be using LASTPIX instead*/
            if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
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
        if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
          /*must at least have FIRSTPIX key*/
          fits_close_file(fp, &ret);
          return 0;
        } else {
          if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
            ret = 0;
            /*might be using LASTPIX instead*/
            if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
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
    
    if (_autoMag) {
      double vecMag = 0.0;
      for (size_t i = 0; i < 12*vecNside*vecNside; i++) {
        if (sqrt((double)(comptheta[i]*comptheta[i] + compphi[i]*compphi[i])) > vecMag) {
          vecMag = sqrt((double)(comptheta[i]*comptheta[i] + compphi[i]*compphi[i]));
        }
      }
      _maxMag = vecMag;
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
        if (_vecQU) { //theta is really Q, phi really U
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
        thetapart *= maxang / _maxMag;
        phipart *= maxang / _maxMag;
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

int HealpixSource::readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps) {
  // If the file is _valid, then we already 
  // have all the header information- no need to read it again.
  // We also know that the matrix index is not out-of-range.
  
  if (_valid && isValidMatrix(matrix)) {
    fitsfile *fp;
    int colnum;
    int fieldnum = _matrixList.findIndex(matrix);
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
    
    if (_matrixList.contains(matrix)) {
      fieldnum = _matrixList.findIndex(matrix);
    } else {
      fieldnum = matrix.toInt();
      fieldnum--;
    }

    // check range
    if ((xStart >= _nX) || (yStart >= _nY)) {
      return -1;
    }
    int nxread = xNumSteps;
    if (nxread < 0) {
      nxread = 1;
    }
    if (xStart + nxread > _nX) {
      nxread = _nX - xStart;
    } 
    int nyread = yNumSteps;
    if (nyread < 0) {
      nyread = 1;
    }
    if (yStart + nyread > _nY) {
      nyread = _nY - yStart;
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
        if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
          /*must at least have FIRSTPIX key*/
          fits_close_file(fp, &ret);
          return 0;
        } else {
          if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
            ret = 0;
            /*might be using LASTPIX instead*/
            if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
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
    
    if (_autoTheta || _autoPhi) {
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
      if (_autoTheta) {
        _thetaMin = mapMinTheta;
        _thetaMax = mapMaxTheta;
      }
      if (_autoPhi) {
        _phiMin = mapMinPhi;
        _phiMax = mapMaxPhi;
      }
      //kstdDebug() << "HEALPIX autorange is Theta=[" << mapMinTheta << "..." << mapMaxTheta << "] Phi=[" << mapMinPhi << "..." << mapMaxPhi << "]" << endl;
    }
    //kstdDebug() << "HEALPIX using range Theta=[" << _thetaMin << "..." << _thetaMax << "] Phi=[" << _phiMin << "..." << _phiMax << "]" << endl;
    
    // copy sphere data to matrix.
    
    for (int i = xStart; i < nxread; i++) {
      for (int j = yStart; j < nyread; j++) {
        healpix_proj_rev_car(_thetaMin, _thetaMax, _phiMin, _phiMax, (double)_nX, (double)_nY, (double)i, (double)j, &theta, &phi);
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
    
    double tMin = _thetaMin;
    double tMax = _thetaMax;
    double pMin = _phiMin;
    double pMax = _phiMax;
    theta2External(_thetaUnits, tMin);
    theta2External(_thetaUnits, tMax);
    phi2External(_phiUnits, pMin);
    phi2External(_phiUnits, pMax);
    //kstdDebug() << "HEALPIX exported range Theta=[" << tMin << "..." << tMax << "] Phi=[" << pMin << "..." << pMax << "]" << endl;
    
    switch (_thetaUnits) {
      case HPUNIT_RAD: case HPUNIT_DEG:
        data->yMin = tMax;
        data->yStepSize = (tMin - tMax)/(double)_nY;
        break;
      case HPUNIT_RADEC: case HPUNIT_LATLON: 
        data->yMin = tMax;
        data->yStepSize = (tMin - tMax)/(double)_nY;
        break;
      default:
        break;
    }
    
    data->xMin = pMin;
    switch (_phiUnits) {
      case HPUNIT_RAD:
        if (pMin > pMax) {
          data->xStepSize = (pMax + (2.0*HEALPIX_PI-pMin)) / (double)_nX;
        } else {
          data->xStepSize = (pMax - pMin) / (double)_nX;
        }
        break;
      case HPUNIT_DEG: case HPUNIT_RADEC:
        if (pMin > pMax) {
          data->xStepSize = (pMax + (360.0-pMin)) / (double)_nX;
        } else {
          data->xStepSize = (pMax - pMin) / (double)_nX;
        }
        break;
      case HPUNIT_LATLON: 
        if (pMin > pMax) {
          data->xStepSize = ((180.0+pMax) + (180.0-pMin)) / (double)_nX;
        } else {
          data->xStepSize = (pMax - pMin) / (double)_nX;
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
    for (int i = 0; i < _vecDegrade; i++) {
      vecNside = (size_t)(vecNside/2);
    }
    return (int)(12*vecNside*vecNside);
  } else {
    return 0;
  }
}

bool HealpixSource::matrixDimensions( const QString& matrix, int* xDim, int* yDim) 
{
  Q_UNUSED(matrix)
  if (_valid) {
    (*xDim) = _nX;
    (*yDim) = _nY;
    return true;
  }
  return false;
}


QString HealpixSource::fileType() const {
  return "HEALPIX";
}


void HealpixSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
  
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
  
  ts << indent << "<dim x=\"" << _nX << "\" y=\"" << _nY << "\"/>" << endl;
  ts << indent << "<theta auto=\"" << _autoTheta << "\" units=\"" << _thetaUnits << "\" min=\"" << confThetaMin << "\" max=\"" << confThetaMax << "\"/>" << endl;
  ts << indent << "<phi auto=\"" << _autoPhi << "\" units=\"" << _phiUnits << "\" min=\"" << confPhiMin << "\" max=\"" << confPhiMax << "\"/>" << endl;
  ts << indent << "<vector theta=\"" << _vecTheta << "\" phi=\"" << _vecPhi << "\" degrade=\"" << _vecDegrade << "\" auto=\"" << _autoMag << "\" max=\"" << _maxMag << "\" QU=\"" << _vecQU << "\"/>" << endl;
  return;
}

void HealpixSource::load(const QDomElement& e) {
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
  
  /*
  QString str;
  QString indent = "  ";
  QTextStream ts(&str, IO_WriteOnly);
  save(ts, indent);
  kstdDebug() << str << endl;
  */
  return;
}

bool HealpixSource::isEmpty() const {
  if (!_valid) {
    return true;
  } else {
    return false;
  }
}

void HealpixSource::theta2Internal(int units, double& theta) {
  
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

void HealpixSource::theta2External(int units, double& theta) {
  
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
    
void HealpixSource::phi2Internal(int units, double& phi) {
  
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
    
void HealpixSource::phi2External(int units, double& phi) {
  
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

void HealpixSource::checkDegrade(int& degrade) {
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

void HealpixSource::loadConfig(KConfig *cfg) {
  double confThetaMin;
  double confThetaMax;
  double confPhiMin;
  double confPhiMax;
  int tempdegrade;
  
  cfg->setGroup("Healpix General");
  cfg->setGroup(fileName());
  _nX = (int)cfg->readNumEntry("Matrix X Dimension", DEFAULT_XDIM);
  _nY = (int)cfg->readNumEntry("Matrix Y Dimension", DEFAULT_YDIM);
  _autoTheta = cfg->readBoolEntry("Theta Autoscale", true);
  _thetaUnits = cfg->readNumEntry("Theta Units", HPUNIT_RADEC);
  confThetaMin = (cfg->readEntry("Theta Min", QString::null)).toDouble();
  confThetaMax = (cfg->readEntry("Theta Max", QString::null)).toDouble();
  _autoPhi = cfg->readBoolEntry("Phi Autoscale", true);
  _phiUnits = cfg->readNumEntry("Phi Units", HPUNIT_RADEC);
  confPhiMin = (cfg->readEntry("Phi Min", QString::null)).toDouble();
  confPhiMax = (cfg->readEntry("Phi Max", QString::null)).toDouble();
  _vecTheta = cfg->readNumEntry("Vector Theta", 0);
  _vecPhi = cfg->readNumEntry("Vector Phi", 0);
  tempdegrade = (int)cfg->readNumEntry("Vector Degrade Factor", 1);
  _autoMag = cfg->readBoolEntry("Vector Magnitude Autoscale", true);
  _maxMag = (cfg->readEntry("Vector Max Magnitude", QString::null)).toDouble();
  _vecQU = cfg->readBoolEntry("Vector is QU", false);
  
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
  
  return;
}

void HealpixSource::saveConfig(KConfig *cfg) {
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
  
  cfg->setGroup("Healpix General");
  cfg->setGroup(fileName());
  cfg->writeEntry("Matrix X Dimension", _nX);
  cfg->writeEntry("Matrix Y Dimension", _nY);
  cfg->writeEntry("Theta Autoscale", _autoTheta);
  cfg->writeEntry("Theta Units", _thetaUnits);
  cfg->writeEntry("Theta Min", confThetaMin);
  cfg->writeEntry("Theta Max", confThetaMax);
  cfg->writeEntry("Phi Autoscale", _autoPhi);
  cfg->writeEntry("Phi Units", _phiUnits);
  cfg->writeEntry("Phi Min", confPhiMin);
  cfg->writeEntry("Phi Max", confPhiMax);
  cfg->writeEntry("Vector Theta", _vecTheta);
  cfg->writeEntry("Vector Phi", _vecPhi);
  cfg->writeEntry("Vector Degrade Factor", _vecDegrade);
  cfg->writeEntry("Vector Magnitude Autoscale", _autoMag);
  cfg->writeEntry("Vector Max Magnitude", _maxMag);
  cfg->writeEntry("Vector is QU", _vecQU);
  
  return;
}

class ConfigWidgetHealpix : public KstDataSourceConfigWidget {
  public:
    ConfigWidgetHealpix() : KstDataSourceConfigWidget() {
      QGridLayout *layout = new QGridLayout(this, 1, 1);
      _hc = new HealpixConfig(this);
      layout->addWidget(_hc, 0, 0);
      layout->activate();
    }

    virtual ~ConfigWidgetHealpix() {}

    virtual void setConfig(KConfig *cfg) {
      KstDataSourceConfigWidget::setConfig(cfg);
      return;
    }

    virtual void load() {
      QStringList unitList;
      //FIXME add back in when we have a renderer
      //unitList.append("(Radians)");
      //unitList.append("(Degrees)");
      unitList.append("(RA/DEC)");
      //unitList.append("(Lat/Long)");
      _cfg->setGroup("Healpix General");
      _hc->matThetaUnits->clear();
      _hc->matPhiUnits->clear();
      _hc->vecTheta->clear();
      _hc->vecPhi->clear();
      _hc->matThetaUnits->insertStringList(unitList);
      _hc->matPhiUnits->insertStringList(unitList);
      //FIXME change to HPUNIT_RAD
      _hc->matThetaUnits->setCurrentItem(0);
      _hc->matPhiUnits->setCurrentItem(0);
      _hc->matDimX->setValue(_cfg->readNumEntry("Matrix X Dimension", DEFAULT_XDIM));
      _hc->matDimY->setValue(_cfg->readNumEntry("Matrix Y Dimension", DEFAULT_YDIM));
      _hc->matThetaAuto->setChecked(_cfg->readBoolEntry("Theta Autoscale", true));
      //FIXME Only one combobox item for now...
      //_hc->matThetaUnits->setCurrentItem(_cfg->readNumEntry("Theta Units", HPUNIT_RAD));
      _hc->matThetaUnits->setCurrentItem(0);
      _hc->matThetaMin->setText(_cfg->readEntry("Theta Min", QString::null));
      _hc->matThetaMax->setText(_cfg->readEntry("Theta Max", QString::null));
      _hc->matPhiAuto->setChecked(_cfg->readBoolEntry("Phi Autoscale", true));
      //FIXME Only one combobox item for now...
      //_hc->matPhiUnits->setCurrentItem(_cfg->readNumEntry("Phi Units", HPUNIT_RAD));
      _hc->matPhiUnits->setCurrentItem(0);
      _hc->matPhiMin->setText(_cfg->readEntry("Phi Min", QString::null));
      _hc->matPhiMax->setText(_cfg->readEntry("Phi Max", QString::null));
      _hc->vecTheta->setCurrentItem(_cfg->readNumEntry("Vector Theta", 0));
      _hc->vecPhi->setCurrentItem(_cfg->readNumEntry("Vector Phi", 0));
      _hc->vecDegrade->setValue(_cfg->readNumEntry("Vector Degrade Factor", 1));
      _hc->vecMagAuto->setChecked(_cfg->readBoolEntry("Vector Magnitude Autoscale", true));
      _hc->vecMag->setText(_cfg->readEntry("Vector Max Magnitude", QString::null));
      _hc->vecQU->setChecked(_cfg->readBoolEntry("Vector is QU", false));
      
      bool hasInstance = (_instance != 0L);
      
      if (hasInstance) {
        _hc->vecTheta->insertStringList(_instance->matrixList());
        _hc->vecPhi->insertStringList(_instance->matrixList());
        KstSharedPtr<HealpixSource> src = kst_cast<HealpixSource>(_instance);
        assert(src);        
        _cfg->setGroup(src->fileName());
        // set the config to the current datasource values
        src->saveConfig(_cfg);
        _hc->matDimX->setValue(_cfg->readNumEntry("Matrix X Dimension", DEFAULT_XDIM));
        _hc->matDimY->setValue(_cfg->readNumEntry("Matrix Y Dimension", DEFAULT_YDIM));
        _hc->matThetaAuto->setChecked(_cfg->readBoolEntry("Theta Autoscale", true));
        //FIXME Only one combobox item for now...
        //_hc->matThetaUnits->setCurrentItem(_cfg->readNumEntry("Theta Units", HPUNIT_RAD));
        _hc->matThetaUnits->setCurrentItem(0);
        _hc->matThetaMin->setText(_cfg->readEntry("Theta Min", QString::null));
        _hc->matThetaMax->setText(_cfg->readEntry("Theta Max", QString::null));
        _hc->matPhiAuto->setChecked(_cfg->readBoolEntry("Phi Autoscale", true));
        //FIXME Only one combobox item for now...
        //_hc->matPhiUnits->setCurrentItem(_cfg->readNumEntry("Phi Units", HPUNIT_RAD));
        _hc->matPhiUnits->setCurrentItem(0);
        _hc->matPhiMin->setText(_cfg->readEntry("Phi Min", QString::null));
        _hc->matPhiMax->setText(_cfg->readEntry("Phi Max", QString::null));
        _hc->vecTheta->setCurrentItem(_cfg->readNumEntry("Vector Theta", 0));
        _hc->vecPhi->setCurrentItem(_cfg->readNumEntry("Vector Phi", 0));
        _hc->vecDegrade->setValue(_cfg->readNumEntry("Vector Degrade Factor", 1));
        _hc->vecMagAuto->setChecked(_cfg->readBoolEntry("Vector Magnitude Autoscale", true));
        _hc->vecMag->setText(_cfg->readEntry("Vector Max Magnitude", QString::null));
        _hc->vecQU->setChecked(_cfg->readBoolEntry("Vector is QU", false));
      }
      return;
    }

    virtual void save() {
      assert(_cfg);
      _cfg->setGroup("Healpix General");
      // If we have an instance, save settings for that 
      // instance only
      KstSharedPtr<HealpixSource> src = kst_cast<HealpixSource>(_instance);
      if (src) {
        _cfg->setGroup(src->fileName());
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
      
      // Update the instance from our new settings
      // Load the config and then save it again, in case
      // the datasource adjusted any of the parameters.
      if (src && src->reusable()) {
        src->loadConfig(_cfg);
        src->saveConfig(_cfg);
      }
      return;
    }

    HealpixConfig *_hc;
};


extern "C" {
KstDataSource *create_healpix(KConfig *cfg, const QString& filename, const QString& type) {
  return new HealpixSource(cfg, filename, type);
}

KstDataSource *load_healpix(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e) {
  return new HealpixSource(cfg, filename, type, e);
}

QStringList provides_healpix() {
  QStringList rc;
  rc += "HEALPIX";
  return rc;  
}

int understands_healpix(KConfig *cfg, const QString& filename) {
  Q_UNUSED(cfg)
      
  int ret;
  char thealpixfile[HEALPIX_STRNL];
  int tOrder;
  int tCoord;
  int tType;
  size_t tNside;
  size_t tMaps;
   
  strncpy(thealpixfile, filename.latin1(), HEALPIX_STRNL);
  ret = healpix_fits_map_test(thealpixfile, &tNside, &tOrder, &tCoord, &tType, &tMaps);
    
  if (ret) {
    // MUST return 100, since LFIIO datasource returns 99
    // for *all* valid FITS files
    return 100;
  } else {
    return 0;
  }
}

QStringList matrixList_healpix(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(cfg)
  if (!type.isEmpty() && !provides_healpix().contains(type)) {
    return QStringList();
  }

  int ret;
  char thealpixfile[HEALPIX_STRNL];
  int tOrder;
  int tCoord;
  int tType;
  size_t tNside;
  size_t tMaps;
  char creator[HEALPIX_STRNL];
  char extname[HEALPIX_STRNL];
  char **names;
  char **units;
  healpix_keys *keys;
  size_t poff;
   
  strncpy(thealpixfile, filename.latin1(), HEALPIX_STRNL);
  
  names = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
  units = healpix_strarr_alloc(HEALPIX_FITS_MAXCOL);
  keys = healpix_keys_alloc();
    
  ret = healpix_fits_map_info(thealpixfile, &tNside, &tOrder, &tCoord, &tType, &tMaps, creator, extname, names, units, keys);
    
  QString mapName;
  QStringList matrices;
  
  if (tType == HEALPIX_FITS_CUT) {
    poff = 1;
  } else {
    poff = 0;
  }
  if (ret) {
    for (size_t i = 0; i < tMaps; i++) {
      if (strlen(names[i+poff]) == 0) {
        mapName.sprintf("%d - %s",(int)(i+1),"MAP");
      } else {
        mapName.sprintf("%d - %s",(int)(i+1),names[i+poff]);
      }
      if (strlen(units[i+poff]) == 0) {
        mapName.sprintf("%s (%s)",mapName.ascii(),"Unknown Units");
      } else {
        mapName.sprintf("%s (%s)",mapName.ascii(),units[i+poff]);
      }
      matrices.append(mapName);
    }
    if (tType == HEALPIX_FITS_CUT) {
      if (strlen(names[tMaps+1]) == 0) {
        mapName.sprintf("%s","HITS");
      } else {
        mapName.sprintf("%s",names[tMaps+1]);
      }
      matrices.append(mapName);
      if (strlen(names[tMaps+2]) == 0) {
        mapName.sprintf("%s","ERRORS");
      } else {
        mapName.sprintf("%s",names[tMaps+2]);
      }
      if (strlen(units[tMaps+2]) == 0) {
        mapName.sprintf("%s (%s)",mapName.ascii(),"Unknown Units");
      } else {
        mapName.sprintf("%s (%s)",mapName.ascii(),units[tMaps+2]);
      }
      matrices.append(mapName);
    }
  } else {
    healpix_keys_free(keys);
    healpix_strarr_free(names, HEALPIX_FITS_MAXCOL);
    healpix_strarr_free(units, HEALPIX_FITS_MAXCOL);
    return QStringList();
  }
    
  if (complete) {
    *complete = true;
  }
  
  if (typeSuggestion) {
    *typeSuggestion = "HEALPIX";
  }
  
  return matrices;  
}

QStringList fieldList_healpix(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(cfg)
      
  if (!type.isEmpty() && !provides_healpix().contains(type)) {
    return QStringList();
  }

  int ret;
  char thealpixfile[HEALPIX_STRNL];
  int tOrder;
  int tCoord;
  int tType;
  size_t tNside;
  size_t tMaps;
  QStringList fields;
   
  strncpy(thealpixfile, filename.latin1(), HEALPIX_STRNL);
  ret = healpix_fits_map_test(thealpixfile, &tNside, &tOrder, &tCoord, &tType, &tMaps);
  
  if (ret) {
    fields.append("1 - Vector Field Head Theta");
    fields.append("2 - Vector Field Head Phi");
    fields.append("3 - Vector Field Tail Theta");
    fields.append("4 - Vector Field Tail Phi");
  } else {
    return QStringList();
  }
    
  if (complete) {
    *complete = true;
  }
  
  if (typeSuggestion) {
    *typeSuggestion = "HEALPIX";
  }
  
  return fields; 
}

QWidget* widget_healpix(const QString& filename) {
  Q_UNUSED(filename)
  return new ConfigWidgetHealpix;
}

}

KST_KEY_DATASOURCE_PLUGIN(healpix)


// vim: ts=2 sw=2 et
