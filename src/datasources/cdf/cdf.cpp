/***************************************************************************
                     cdf.cpp  -  CDF file data source
                             -------------------
    begin                : 17/06/2004
    copyright            : (C) 2004 Nicolas Brisset <nicodev@users.sourceforge.net>
    email                : kst@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cdf_source.h" // Local header for the kst CDF datasource
#include <cdf.h>
#include "cdfdefs.h" // Macros to handle rVars and zVars transparently

#include <ksdebug.h>

#include <qfile.h>
#include <qfileinfo.h>

#include <ctype.h>
#include <stdlib.h>

CdfSource::CdfSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type) {
  // kstdDebug() << "Entering CdfSource constructor for " << _filename << endl;
  _maxFrameCount = 0;

  if (!type.isEmpty() && type != "CDF") {
    return;
  }

  _valid = initFile();
}


CdfSource::~CdfSource() {
  _maxFrameCount = 0;
}


bool CdfSource::reset() {
  // kstdDebug() << "Calling reset() for " << _filename << endl;
  _maxFrameCount = 0;
  _fieldList.clear();
  return _valid = initFile();
}


bool CdfSource::initFile() {
  CDFid id;
  CDFstatus status = CDFopen(_filename.latin1(), &id);
  if (status < CDF_OK) {
    kstdDebug() << _filename << ": failed to open in initFile()" << endl;
    return false;
  }
  // Query field list and store it in _fieldList (plus "INDEX")
  _fieldList.clear();
  //_fieldList += "INDEX"; commented out as it leads to problems in countFrames():
  // which value should the method return when not all variables have the same length ?
  long numRvars = 0, numZvars = 0, varN = 0, numDims = 0, dimSizes[CDF_MAX_DIMS], maxRec = 0;
  char varName[CDF_VAR_NAME_LEN + 1];
  status = CDFlib(SELECT_, 
                     CDF_READONLY_MODE_, READONLYon,
                  GET_,
                     CDF_NUMrVARS_, &numRvars,
                     CDF_NUMzVARS_, &numZvars,
                  NULL_);

  // We accept the following types of fields:
  // - scalars (numDims == 0)
  // - vectors (numDims == 1) of size 1 (dimSizes[0] == 1)
  // - vectors (numDims == 1) of any size, PROVIDED THAT THEY HAVE ONLY 1 RECORD
  // Note that the last case is required by Matlab-generated CDFs, where vectors are stored that way  

  // Add 0-dimensional rVariables 
  for (varN = 0; varN < numRvars; varN++) {
    status = CDFlib(SELECT_,
                       rVAR_, varN,
                    GET_,
                       rVAR_NAME_, varName,
                       rVARs_NUMDIMS_, &numDims,
                       rVARs_DIMSIZES_, dimSizes,
                       rVAR_MAXREC_, &maxRec,
                    NULL_);
    // maxRec is not exactly the number of records :-)
    maxRec += 1;
    if (status == CDF_OK && numDims < 2) {
      if (numDims == 1 && dimSizes[0] > 1 && maxRec > 1) { // Ignore that, this is not really a vector
        kstdDebug() << "Variable " << varName << " can't be handled by kst: only CDF vectors with dimensionalities 0, 1[1] or 1[n] but only one record in the latter case are supported (try cdfexport on your CDF if you have problems)" << endl;
        continue;
      }
      _fieldList += varName;
      if (numDims == 1 && dimSizes[0] > 1) {
        _frameCounts[QString(varName)] = dimSizes[0];
      if ((int) dimSizes[0] > _maxFrameCount) {
        _maxFrameCount = dimSizes[0];
      }
      } else {
        _frameCounts[QString(varName)] = maxRec;
      if ((int) maxRec > _maxFrameCount) {
        _maxFrameCount = maxRec;
      }
      }
    }
  }
  // Add 0-dimensional zVariables 
  for (varN = 0; varN < numZvars; varN++) {
    status = CDFlib(SELECT_,
                       zVAR_, varN,
                    GET_,
                       zVAR_NAME_, varName,
                       zVAR_NUMDIMS_, &numDims,
                       zVAR_DIMSIZES_, dimSizes,
                       zVAR_MAXREC_, &maxRec,
                    NULL_);
    maxRec += 1;
    if (status == CDF_OK && numDims < 2) {
      if (numDims == 1 && dimSizes[0] > 1 && maxRec > 1) { // Ignore that, this is not really a vector
        kstdDebug() << "Variable " << varName << " can't be handled by kst: only CDF vectors with dimensionalities 0, 1[1] or 1[n] but only one record in the latter case are supported (try cdfexport on your CDF if you have problems)" << endl;
        continue;
      }
      _fieldList += varName;
      if (numDims == 1 && dimSizes[0] > 1) {
        _frameCounts[QString(varName)] = dimSizes[0];
      if ((int) dimSizes[0] > _maxFrameCount) {
        _maxFrameCount = dimSizes[0];
      }
      } else {
        _frameCounts[QString(varName)] = maxRec;
      if ((int) maxRec > _maxFrameCount) {
          _maxFrameCount = maxRec;
        }
      }
    }
  }

  // Close the file :-)
  status = CDFclose(id);

  return status >= CDF_OK;
}


KstObject::UpdateType CdfSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }
  return setLastUpdateResult(KstObject::NO_CHANGE);
}


int CdfSource::readField(double *v, const QString& field, int s, int n) {
  int i;
  CDFstatus status;
  CDFid id;
  long dataType = 0, maxRec = 0, numDims = 0, dimSizes[CDF_MAX_DIMS];
  long recCount = 0, indices[1] = {0}, counts[1] = {0};
  char varName[CDF_VAR_NAME_LEN+1];
  bool isZvar = true;     /* Should be the case for recent cdf files */
  // kstdDebug() << "Entering CdfSource::readField with params: " << field << ", from " << s << " for " << n << " values" << endl;
  // Handle the special case where we query INDEX
  if (field.lower() == "index") {
    if (n < 0) {
      v[0] = double(s);
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i);
    }
    return n;
  }

  // If not INDEX, look into the CDF file...
  status = CDFopen(_filename.latin1(), &id);
  if (status < CDF_OK) {
    kstdDebug() << _filename << ": failed to open to read from field " << field << endl;
    return -1;
  }

  QString ftmp = field;
  ftmp.truncate(CDF_VAR_NAME_LEN);
  // Variable selection
  strcpy(varName, ftmp.latin1());
  status = CDFlib(SELECT_,
                     zVAR_NAME_, varName, 
                  GET_,
                     zVAR_DATATYPE_, &dataType, 
                  NULL_);
  if (status < CDF_OK) { // if not zVar, try rVar
    // kstdDebug() << ftmp << ": " << " not a zVAR (" << status <<")" << endl;
    isZvar = false;
    status = CDFlib(SELECT_, 
                       rVAR_NAME_, varName, 
                    GET_, 
                       rVAR_DATATYPE_, &dataType, 
                    NULL_);
  }

  // I suppose the returned int is the number of values read, <0 when there is a problem
  if (status < CDF_OK) {
    kstdDebug() << ftmp << ": " << " not a rVAR either -> exiting" << endl;
    CDFclose(id);
    return -1; 
  }

  // If n<0 set it to 1 as suggested by George Staikos
  // (needs to be documented better I guess !)
  // First check for the existence of more values for this field
  if (n < 0) {
    n = 1;
  }

  void *binary = 0L;
  void *pt = 0L;
 
  // Cast the iteration pointer to the right type and allocate the needed space for binary
  switch (dataType) {
    case CDF_INT2:
       binary = malloc(n*sizeof(Int16));
       pt = (Int16 *)binary;
      break;
    case CDF_INT4:
      binary = malloc(n*sizeof(Int32));
      pt = (Int32 *)binary;
      break;
    case CDF_UINT1:
      binary = malloc(n*sizeof(uChar));
      pt = (uChar *)binary;
      break;
    case CDF_UINT2:
      binary = malloc(n*sizeof(uInt16));
      pt = (uInt16 *)binary;
      break;
    case CDF_UINT4:
      binary = malloc(n*sizeof(uInt32));
      pt = (uInt32 *)binary;
      break;
    case CDF_REAL4:
    case CDF_FLOAT:
      binary = malloc(n*sizeof(float));
      pt = (float *)binary;
      break;
    case CDF_REAL8:
    case CDF_DOUBLE:
      binary = malloc(n*sizeof(double));
      pt = (double *)binary; 
      break;
    default :
      binary = malloc(n*sizeof(long double));
    break;
  }

  // Get some useful values
  status = CDFlib (GET_, 
                    BOO(isZvar, zVAR_MAXREC_, rVAR_MAXREC_), &maxRec,
                    BOO(isZvar, zVAR_NUMDIMS_, rVARs_NUMDIMS_), &numDims,
                    BOO(isZvar, zVAR_DIMSIZES_, rVARs_DIMSIZES_), dimSizes,
                 NULL_);
  maxRec += 1;

  if (numDims == 0 || (numDims == 1 && dimSizes[0] < 2)) { // Vars of dimension 0, or vectors of size 1 (pseudo scalars) with records > 1
    status = CDFlib (SELECT_,
                        BOO(isZvar, zVAR_RECNUMBER_, rVAR_SEQPOS_), (long) s,
                        BOO(isZvar, zVAR_RECCOUNT_, rVARs_RECCOUNT_), (long) n,
                     GET_,
                        BOO(isZvar, zVAR_HYPERDATA_, rVAR_HYPERDATA_), binary,
                     NULL_);
  }
  else { // Vectors of size [1: n>1] with only one sample/record a la Matlab
     indices[0] = s;
     counts[0] = n;
     recCount = 1;
     status = CDFlib (SELECT_, 
                         BOO(isZvar,zVAR_RECCOUNT_,rVARs_RECCOUNT_), recCount,
                         BOO(isZvar,zVAR_DIMINDICES_,rVARs_DIMINDICES_), indices, 
                         BOO(isZvar,zVAR_DIMCOUNTS_,rVARs_DIMCOUNTS_), counts, 
                      GET_,
                         BOO(isZvar,zVAR_HYPERDATA_,rVAR_HYPERDATA_), binary,
                      NULL_);
    maxRec = dimSizes[0];
  }

  for (i = 0; i < n && i < maxRec; i++) {
    switch (dataType) {
      case CDF_INT2:
        v[i] = (double) *((Int16 *)pt);
        pt = (Int16 *)pt + 1;
       break;
      case CDF_INT4:
        v[i] = (double) *((Int32 *)pt);
        pt = (Int32 *)pt + 1;
        break;
      case CDF_UINT1:
        v[i] = (double) *((uChar *)pt);
        pt = (uChar *)pt + 1;
        break;
      case CDF_UINT2:
        v[i] = (double) *((uInt16 *)pt);
        pt = (uInt16 *)pt + 1;
        break;
      case CDF_UINT4:
        v[i] = (double) *((uInt32 *)pt);
        pt = (uInt32 *)pt + 1;
        break;
      case CDF_REAL4:
      case CDF_FLOAT:
        v[i] = (double) *((float *)pt);
        pt = (float *)pt + 1;
        break;
      case CDF_REAL8:
      case CDF_DOUBLE: 
        v[i] = (double) *((double *)pt);
        pt = (double *)pt + 1;
       break;
    }
  }


  free(binary);

  status = CDFclose(id);

  return i;
}


bool CdfSource::isValidField(const QString& field) const {  
  return _fieldList.contains(field);
}


int CdfSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1; // For 0-dimensional vars always the case I guess, but what are frames exactly ?
}


int CdfSource::frameCount(const QString& field) const {
  // Handle the case where the argument is null (return max number of records for all vars)
  if (field.isEmpty() || field == "INDEX") {
    return _maxFrameCount;
  }
  // Other case : count queried specifically for one field
  else { 
    return _frameCounts[field];
  }
}


QString CdfSource::fileType() const {
  return "CDF";
}


void CdfSource::save(QTextStream &ts, const QString& indent) {
  // FIXME (copied from ascii.cpp !)
  KstDataSource::save(ts, indent);
}


bool CdfSource::isEmpty() const {
  return frameCount() < 1;
}


extern "C" {
KstDataSource *create_cdf(KConfig *cfg, const QString& filename, const QString& type) {
  return new CdfSource(cfg, filename, type);
}

QStringList provides_cdf() {
  QStringList rc;
  rc += "CDF";
  return rc;
}

/** understands_cdf: returns true if:
    - the file is readable (!)
    - the file has a .cdf extension (required by the cdf lib)
    - CDFopen does not complain (currently disabled) **/
int understands_cdf(KConfig*, const QString& filename) {
  QFile f(filename);
  QFileInfo fInfo(f);

  if (!f.open(IO_ReadOnly)) {
    kstdDebug() << "Unable to read file !" << endl;
    return 0;
  }

  if (fInfo.extension(false) != "cdf") {
    return 0;
  }

  return 100;
  // Commented out the following (though nice and robust) because it takes too long
  // Extension check should be enough :-) (?)
  /* CDFid id;
  CDFstatus status;
  status = CDFopen (fInfo.baseName(true).latin1(), &id);
  if (status < CDF_OK) {
    kstdDebug() << "CDFlib unable to read the file !" << endl;
    return false;
  }
  else {
    status = CDFclose (id);
    return true;
  } */

}

// FIXME: implement fieldlist accelerator static here for better performance?

}

KST_KEY_DATASOURCE_PLUGIN(cdf)

// vim: ts=2 sw=2 et
