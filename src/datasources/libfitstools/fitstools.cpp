/***************************************************************************
                  fitstools.cpp  -  general tools for FITS files
                             -------------------
    begin                : Tue May 30 2006
    copyright            : (C) 2006 Ted Kisner
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

#include "fitstools.h"


char **fitsSarrayAlloc(size_t nstring)
{
  size_t i,j;
  char **array = NULL;
  if (nstring == 0) {
    return NULL;
  }
  array = (char **)calloc(nstring, sizeof(char *));
  if (!array) {
    return NULL;
  }
  for (i = 0; i < nstring; i++) {
    array[i] = NULL;
    array[i] = (char *)calloc(FLEN_CARD, sizeof(char));
    if (!array[i]) {
      for (j = 0; j < i; j++) {
        free(array[j]);
      }
      free(array);
      return NULL;
    }
  }
  return array;
}


int fitsSarrayFree(char **array, size_t nstring)
{
  size_t i;
  if ((array == NULL)||(nstring == 0)) {
    return 0;
  }
  for (i = 0; i < nstring; i++) {
    free(array[i]);
  }
  free(array);
  return 0;
}


int fitsNHDU ( fitsfile *fp ) {
  int nhdu;
  int status = 0;
  
  // the internal function of cfitsio does the "right thing" and simply
  // scans through the file to see how many HDU's there are
  
  fits_get_num_hdus(fp, &nhdu, &status);
  
  if (status) {
    nhdu = 0;
  }
  
  return nhdu;
}


QMap<QString, QString> fitsKeys( fitsfile *fp, int HDU ) {
  
  QMap<QString, QString> keyMap;
  
  int ret = 0;
  int nHDU;
  int hduType;
  // try to seek to the desired HDU
  if (fits_get_num_hdus(fp, &nHDU, &ret)) {
    return keyMap;
  }
  if (HDU > nHDU) {
    return keyMap;
  } else {
    if (fits_movabs_hdu(fp, HDU, &hduType, &ret)) {
      return keyMap;
    }
  }
  
  // reset keyword pointer to beginning of HDU
  int rec = 0;
  char card[FLEN_CARD];
  if (fits_read_record(fp, rec, card, &ret)) {
    return keyMap;
  } 
  
  // go through all keys and add to the QMap
  QString theKey;
  QString theVal;
  char keyval[FLEN_VALUE];
  char keycom[FLEN_COMMENT];
  char keyname[FLEN_KEYWORD];
  int keylen;
  while (!fits_find_nextkey(fp, NULL, 1, NULL, 0, card, &ret)) {
    fits_get_keyname(card, keyname, &keylen, &ret);
    fits_parse_value(card, keyval, keycom, &ret);
    theKey = keyname;
    theVal = keyval;
    keyMap.insert(theKey, theVal);
  }
  
  return keyMap;
}


int fitsNamesUnits( fitsfile *fp, int HDU, QStringList *names, QStringList *units ) {
  
  int nString = 0;
  
  // clear the lists
  names->clear();
  units->clear();
  
  // move to desired HDU
  int ret = 0;
  int hduType;
  if (fits_movabs_hdu(fp, HDU, &hduType, &ret)) {
    return nString;
  }
  
  // for each type of HDU, find the column names or assign
  // a generic image name (since images don't have names)
  
  QString curName;
  QString curUnits;
  int maxDim;
  long rowLen;
  long nRows;
  int nCols;
  long tbCol;
  char **fitsNames;
  char **fitsUnits;
  long *dimAxes = NULL;
  int nAxis;
  switch (hduType) {
    case IMAGE_HDU:
      // find the size of the image and include it in the 
      // generic name.
      maxDim = 100; // no images should have > 100 dimensions...
      dimAxes = (long*)calloc(maxDim, sizeof(long));
      
      if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
        return nString;
      }
      curName = "IMAGE (";
      for (int i = 0; i < nAxis; i++) {
        curUnits.sprintf("%ld", dimAxes[i]);
        curName.append(curUnits);
        if (i != nAxis-1) {
          curName.append("x");
        }
      }
      curName.append(")");
      names->append(curName);
      units->append(QString());
      nString++;
      free(dimAxes);
      break;
      
    case ASCII_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of columns
      if (fits_read_atblhdr(fp, maxDim, &rowLen, &nRows, &nCols, NULL, &tbCol, NULL, NULL, NULL, &ret)) {
        return nString;
      }
      
      // allocate names and units
      fitsNames = fitsSarrayAlloc(nCols);
      fitsUnits = fitsSarrayAlloc(nCols);
      
      // read names and units
      if (fits_read_atblhdr(fp, maxDim, &rowLen, &nRows, &nCols, fitsNames, &tbCol, NULL, fitsUnits, NULL, &ret)) {
        return nString;
      }
      for (int i = 0; i < nCols; i++) {
        curName = fitsNames[i];
        curUnits = fitsUnits[i];
        names->append(curName);
        units->append(curUnits);
        nString++;
      }
      fitsSarrayFree(fitsNames, nCols);
      fitsSarrayFree(fitsUnits, nCols);
      break;
      
    case BINARY_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of columns
      if (fits_read_btblhdr(fp, maxDim, &nRows, &nCols, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return nString;
      }
      
      // allocate names and units
      fitsNames = fitsSarrayAlloc(nCols);
      fitsUnits = fitsSarrayAlloc(nCols);
      
      // read names and units
      if (fits_read_btblhdr(fp, maxDim, &nRows, &nCols, fitsNames, NULL, fitsUnits, NULL, NULL, &ret)) {
        return nString;
      }
      for (int i = 0; i < nCols; i++) {
        curName = fitsNames[i];
        curUnits = fitsUnits[i];
        names->append(curName);
        units->append(curUnits);
        nString++;
      }
      fitsSarrayFree(fitsNames, nCols);
      fitsSarrayFree(fitsUnits, nCols);
      break;
      
    default:
      return nString;
      break;
  }
  
  return nString;
}


QStringList fitsFields( fitsfile *fp, int HDU ) {
  
  QStringList fields;
  QStringList names;
  QStringList units;
  
  int nString = fitsNamesUnits(fp, HDU, &names, &units);

  if (nString <= 0) {
    return fields;
  }
  
  QString cur;
  fields.append("INDEX");
  for (int i = 0; i < nString; i++) {
    if (names[i].contains("IMAGE")) {
      cur.sprintf("%d - %s Data", i, names[i].latin1());
    } else {
      cur.sprintf("%d - %s", i, names[i].latin1());
    }
    if (units[i].isEmpty()) {
      cur.sprintf("%s (Unknown Units)", cur.latin1());
    } else {
      cur.sprintf("%s (%s)", cur.latin1(), units[i].latin1());
    }
    fields.append(cur);
  }
  
  return fields;
}


QStringList fitsMatrices( fitsfile *fp, int HDU ) {
  
  QStringList matrices;
  QStringList names;
  QStringList units;
  
  int nString = fitsNamesUnits(fp, HDU, &names, &units);

  if (nString <= 0) {
    return matrices;
  }
  
  QString cur;
  matrices.append("INDEX");
  
  if (names[0].contains("IMAGE")) {
    cur.sprintf("1 - %s", names[0].latin1());
  } else {    
    cur.sprintf("1 - TABLE");
  }
  matrices.append(cur);
    
  return matrices;
}


QValueList<int> fitsDim( fitsfile *fp, int HDU ) {
  
  QValueList<int> dims;
  
  // move to desired HDU
  int ret = 0;
  int hduType;
  if (fits_movabs_hdu(fp, HDU, &hduType, &ret)) {
    return dims;
  }
  
  // for each type of HDU, find the dimensions
  
  int maxDim;
  long rowLen;
  long nRows;
  int nCols;
  long tbCol;
  long *dimAxes = NULL;
  int nAxis;
  switch (hduType) {
    case IMAGE_HDU:
      // find the size of the image and include it in the 
      // generic name.
      maxDim = 100; // no images should have > 100 dimensions...
      dimAxes = (long*)calloc(maxDim, sizeof(long));
      
      if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
        return dims;
      }
      for (int i = 0; i < nAxis; i++) {
        dims.append((int)dimAxes[i]);
      }
      free(dimAxes);
      break;
      
    case ASCII_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of columns
      if (fits_read_atblhdr(fp, maxDim, &rowLen, &nRows, &nCols, NULL, &tbCol, NULL, NULL, NULL, &ret)) {
        return dims;
      }
      
      dims.append((int)nCols);
      dims.append((int)nRows);
      
      break;
      
    case BINARY_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of columns
      if (fits_read_btblhdr(fp, maxDim, &nRows, &nCols, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return dims;
      }
      
      dims.append((int)nCols);
      dims.append((int)nRows);
      
      break;
      
    default:
      return dims;
      break;
  }
  
  return dims;
}


// vim: ts=2 sw=2 et

