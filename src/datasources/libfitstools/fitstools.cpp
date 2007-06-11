/***************************************************************************
                  fitstools.cpp  -  general tools for FITS files
                             -------------------
    begin                : Tue May 30 2006
    copyright            : (C) 2007 Ted Kisner
    email                : tsk@humanityforward.org
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

// NOTE:  FITS tables with variable-length rows are not yet supported.

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


void fitsGetHC( QString name, int *hdu, int *col ) {
  // for each field, we use regular expression matching to 
  // determine the HDU and (if the HDU is a table), the
  // column number in the table.
  
  QRegExp hduReg("HDU(\\d+)");
  QRegExp colReg("HDU\\d+:(\\d+)");
  
  // get hdu
  hduReg.search(name);
  QStringList matches = hduReg.capturedTexts();
  if (matches.count() > 1) {
    (*hdu) = matches[1].toInt();
  } else {
    (*hdu) = 0;
    (*col) = 0;
    return;
  }
  
  // see if we have a column number
  colReg.search(name);
  matches = colReg.capturedTexts();
  if (matches.count() > 1) {
    (*col) = matches[1].toInt();
  } else {
    (*col) = 0;
  }
  
  return;
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


QMap<QString, QString> fitsKeys( fitsfile *fp ) {
  
  QMap<QString, QString> keyMap;
  
  int ret = 0;
  int nHDU;
  int hduType;
  // find the number of HDU's
  nHDU = fitsNHDU(fp);
  if (nHDU < 1) {
    return keyMap;
  }
  
  // go through each HDU and find all keys.  prepend the
  // hdu number to the key name to allow for duplicate
  // keynames from different HDU's.
  
  int rec;
  char card[FLEN_CARD];
  QString theKey;
  QString theVal;
  char keyval[FLEN_VALUE];
  char keycom[FLEN_COMMENT];
  char keyname[FLEN_KEYWORD];
  int keylen;
  
  for (int i = 0; i < nHDU; i++) {
    if (fits_movabs_hdu(fp, i, &hduType, &ret)) {
      return keyMap;
    }
  
    // reset keyword pointer to beginning of HDU
    rec = 0;
    if (fits_read_record(fp, rec, card, &ret)) {
      return keyMap;
    } 
  
    //FIXME: need a 1-element include list
    // go through all keys and add to the QMap
    while (!fits_find_nextkey(fp, NULL, 1, NULL, 0, card, &ret)) {
      fits_get_keyname(card, keyname, &keylen, &ret);
      fits_parse_value(card, keyval, keycom, &ret);
      theKey.sprintf("HDU%d - %s", i+1, keyname);
      theVal = keyval;
      keyMap.insert(theKey, theVal);
    }
  }
  
  return keyMap;
}


int fitsFieldNamesUnits( fitsfile *fp, QStringList *names, QStringList *units ) {
  
  int nString = 0;
  
  // clear the lists
  names->clear();
  units->clear();
  
  // find the number of HDU's
  int nHDU = fitsNHDU(fp);
  if (nHDU < 1) {
    return nString;
  }
  
  // go through all HDU's and find the names and units of all
  // table columns and images.
  
  int ret = 0;
  int hduType;
  QString curName;
  QString curUnits;
  QString tempStr;
  int maxDim;
  long rowLen;
  long nRows;
  int nCols;
  long tbCol;
  char **fitsNames;
  char **fitsUnits;
  long *dimAxes = NULL;
  int nAxis;
  
  for (int i = 0; i < nHDU; i++) {
  
    // move to desired HDU
    if (fits_movabs_hdu(fp, i, &hduType, &ret)) {
      return nString;
    }
  
    // for each type of HDU, find the column names or assign
    // a generic image name (since images don't have names)
  
    switch (hduType) {
      case IMAGE_HDU:
        // find the size of the image and use it for the 
        // units
        maxDim = 100; // no images should have > 100 dimensions...
        dimAxes = (long*)calloc(maxDim, sizeof(long));
      
        if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
          return nString;
        }
        curName.sprintf("HDU%d - IMAGE Data", i);
        for (int j = 0; j < nAxis; j++) {
          tempStr.sprintf("%ld", dimAxes[j]);
          curUnits.append(tempStr);
          if (j != nAxis-1) {
            curUnits.append("x");
          }
        }
        names->append(curName);
        units->append(curUnits);
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
        for (int j = 0; j < nCols; j++) {
          curName.sprintf("HDU%d:%d - %s", i, j, fitsNames[j]);
          curUnits = fitsUnits[j];
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
        for (int j = 0; j < nCols; j++) {
          curName.sprintf("HDU%d:%d - %s", i, j, fitsNames[j]);
          curUnits = fitsUnits[j];
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
  }
    
  return nString;
}


int fitsMatrixNamesUnits( fitsfile *fp, QStringList *names, QStringList *units ) {
  
  int nString = 0;
  
  // clear the lists
  names->clear();
  units->clear();
  
  // find the number of HDU's
  int nHDU = fitsNHDU(fp);
  if (nHDU < 1) {
    return nString;
  }
  
  // go through all HDU's and find the dimensions of all
  // tables and images.
  
  int ret = 0;
  int hduType;
  QString curName;
  QString curUnits;
  QString tempStr;
  int maxDim;
  long rowLen;
  long nRows;
  int nCols;
  long tbCol;
  long *dimAxes = NULL;
  int nAxis;
  int nElem = 0;
  int typeCode;
  long repeat;
  long width;
  
  for (int i = 0; i < nHDU; i++) {
  
    // move to desired HDU
    if (fits_movabs_hdu(fp, i, &hduType, &ret)) {
      return nString;
    }
  
    // for each type of HDU, find the column names or assign
    // a generic image name (since images don't have names)
  
    switch (hduType) {
      case IMAGE_HDU:
        // find the size of the image and use it for the 
        // units
        maxDim = 100; // no images should have > 100 dimensions...
        dimAxes = (long*)calloc(maxDim, sizeof(long));
      
        if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
          return nString;
        }
        curName.sprintf("HDU%d - IMAGE", i);
        for (int j = 0; j < nAxis; j++) {
          tempStr.sprintf("%ld", dimAxes[j]);
          curUnits.append(tempStr);
          if (j != nAxis-1) {
            curUnits.append("x");
          }
        }
        names->append(curName);
        units->append(curUnits);
        nString++;
        free(dimAxes);
        break;
      
      case ASCII_TBL:
        maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
        // find number of columns and rows
        if (fits_read_atblhdr(fp, maxDim, &rowLen, &nRows, &nCols, NULL, &tbCol, NULL, NULL, NULL, &ret)) {
          return nString;
        }
      
        curName.sprintf("HDU%d - TABLE", i);
        curUnits.sprintf("%dx%ld", nCols, nRows);
        names->append(curName);
        units->append(curUnits);
        nString++;
        break;
      
      case BINARY_TBL:
        maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
        // find number of columns and rows
        if (fits_read_btblhdr(fp, maxDim, &nRows, &nCols, NULL, NULL, NULL, NULL, NULL, &ret)) {
          return nString;
        }
      
        // binary tables may contain "wide" columns where each row
        // entry contains a vector of values.  we need to account for
        // this case in the table dimensions.
        
        for (int j = 0; j < nCols; j++) {
          fits_get_coltype(fp, j, &typeCode, &repeat, &width, &ret);
          nElem += repeat;
        }
        curName.sprintf("HDU%d - TABLE", i);
        curUnits.sprintf("%dx%ld", nElem, nRows);
        names->append(curName);
        units->append(curUnits);
        nString++;
        break;
      
      default:
        return nString;
        break;
    }
  }
    
  return nString;
}


QStringList fitsFields( fitsfile *fp ) {
  
  QStringList fields;
  QStringList names;
  QStringList units;
  
  int nString = fitsFieldNamesUnits(fp, &names, &units);

  if (nString <= 0) {
    return fields;
  }
  
  QString cur;
  fields.append("INDEX");
  for (int i = 0; i < nString; i++) {
    cur.sprintf("%d - %s (%s)", i, names[i].latin1(), units[i].latin1());
    fields.append(cur);
  }
  
  return fields;
}


QStringList fitsMatrices( fitsfile *fp ) {
  
  QStringList matrices;
  QStringList names;
  QStringList units;
  
  int nString = fitsMatrixNamesUnits(fp, &names, &units);

  if (nString <= 0) {
    return matrices;
  }
  
  QString cur;
  matrices.append("INDEX");
  for (int i = 0; i < nString; i++) {
    cur.sprintf("%d - %s (%s)", i, names[i].latin1(), units[i].latin1());
    matrices.append(cur);
  }
    
  return matrices;
}

long fitsFieldLength( fitsfile *fp, QString field ) {
  
  long length = 0;
  int hdu;
  int col;
  
  fitsGetHC(field, &hdu, &col);
  
  // move to desired HDU and determine type
  int hduType;
  int ret = 0;
  int maxDim;
  long nRows;
  long *dimAxes = NULL;
  int nAxis;
  int typeCode;
  long repeat;
  long width;
      
  if (fits_movabs_hdu(fp, hdu, &hduType, &ret)) {
    return length;
  }
  
  switch (hduType) {
    case IMAGE_HDU:
      // find the size of the image and compute total elements
      maxDim = 100; // no images should have > 100 dimensions...
      dimAxes = (long*)calloc(maxDim, sizeof(long));      
      if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
        return length;
      }
      for (int j = 0; j < nAxis; j++) {
        length += dimAxes[j];
      }
      free(dimAxes);
      break;
      
    case ASCII_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // ASCII tables always have repeat == 1, so just find 
      // number of rows.
      if (fits_read_atblhdr(fp, maxDim, NULL, &nRows, NULL, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return length;
      }
      length = nRows;
      
      break;
      
    case BINARY_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of table rows
      if (fits_read_btblhdr(fp, maxDim, &nRows, NULL, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return length;
      }
      
      // binary tables may contain "wide" columns where each row
      // entry contains a vector of values.  we need to account for
      // this case when computing the number of elements in the column.
      
      colReg.search(field);
      matches = colReg.capturedTexts();
      col = matches[2].toInt();
        
      if (fits_get_coltype(fp, col, &typeCode, &repeat, &width, &ret)) {
        return length;
      }
      length = nRows * (long)repeat;
      break;
      
    default:
      return length;
      break;
  }
  
  return length;
}


long fitsMatrixDimX( fitsfile *fp, QString matrix ) {
  long xDim = 0;
  
  // for each matrix, we use regular expression matching to 
  // determine the HDU.
  
  QRegExp hduReg("HDU(\\d+)");
  
  // get hdu
  hduReg.search(matrix);
  QStringList matches = hduReg.capturedTexts();
  int hdu = matches[1].toInt();
  
  // move to desired HDU and determine type
  int hduType;
  int ret = 0;
  int maxDim;
  int nCols;
  long *dimAxes = NULL;
  int nAxis;
  int typeCode;
  long repeat;
  long width;
      
  if (fits_movabs_hdu(fp, hdu, &hduType, &ret)) {
    return xDim;
  }
  
  switch (hduType) {
    case IMAGE_HDU:
      // find the size of the image and use the first 2 axes
      maxDim = 100; // no images should have > 100 dimensions...
      dimAxes = (long*)calloc(maxDim, sizeof(long));      
      if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
        return xDim;
      }
      // FIXME:  eventually we want a way of returning a 2D "slice"
      // of a many-dimensional image.
      if (nAxis < 1) {
        xDim = 0;
      } else {
        xDim = dimAxes[0];
      }      
      free(dimAxes);
      break;
      
    case ASCII_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // ASCII tables always have repeat == 1, so just find 
      // number of columns.
      if (fits_read_atblhdr(fp, maxDim, NULL, NULL, &nCols, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return xDim;
      }
      xDim = (long)nCols;
      
      break;
      
    case BINARY_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of table columns
      if (fits_read_btblhdr(fp, maxDim, NULL, &nCols, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return xDim;
      }
      
      // binary tables may contain "wide" columns where each row
      // entry contains a vector of values.  we need to account for
      // this case when computing the width of the table.
      
      for (int j = 0; j < nCols; j++) {
        if (fits_get_coltype(fp, j, &typeCode, &repeat, &width, &ret)) {
          return xDim;
        }
        xDim += (long)repeat;
      }
      break;
      
    default:
      return xDim;
      break;
  }
  
  return xDim;
}


long fitsMatrixDimY( fitsfile *fp, QString matrix ) {
  long yDim = 0;
  
  // for each matrix, we use regular expression matching to 
  // determine the HDU.
  
  QRegExp hduReg("HDU(\\d+)");
  
  // get hdu
  hduReg.search(matrix);
  QStringList matches = hduReg.capturedTexts();
  int hdu = matches[1].toInt();
  
  // move to desired HDU and determine type
  int hduType;
  int ret = 0;
  int maxDim;
  long nRows;
  long *dimAxes = NULL;
  int nAxis;
      
  if (fits_movabs_hdu(fp, hdu, &hduType, &ret)) {
    return yDim;
  }
  
  switch (hduType) {
    case IMAGE_HDU:
      // find the size of the image and use the first 2 axes
      maxDim = 100; // no images should have > 100 dimensions...
      dimAxes = (long*)calloc(maxDim, sizeof(long));      
      if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
        return yDim;
      }
      // FIXME:  eventually we want a way of returning a 2D "slice"
      // of a many-dimensional image.
      if (nAxis < 2) {
        yDim = 0;
      } else {
        yDim = dimAxes[1];
      }      
      free(dimAxes);
      break;
      
    case ASCII_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // ASCII tables always have repeat == 1, so just find 
      // number of columns.
      if (fits_read_atblhdr(fp, maxDim, NULL, &nRows, NULL, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return yDim;
      }
      yDim = (long)nRows;
      
      break;
      
    case BINARY_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of table columns
      if (fits_read_btblhdr(fp, maxDim, &nRows, NULL, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return yDim;
      }
      yDim = (long)nRows;
      break;
      
    default:
      return yDim;
      break;
  }

  return yDim;
}


long fitsReadField( fitsfile *fp, QString field, double *data, long start, long skip, long n ) {
  long nRead = 0;
  
  // for each field, we use regular expression matching to 
  // determine the HDU and (if the HDU is a table), the
  // column number in the table.
  
  QRegExp hduReg("HDU(\\d+)");
  QRegExp colReg("HDU(\\d+:\\d+)");
  
  // get hdu
  hduReg.search(field);
  QStringList matches = hduReg.capturedTexts();
  int hdu = matches[1].toInt();
  
  // move to desired HDU and determine type
  int hduType;
  int ret = 0;
  int maxDim;
  long nRows;
  int col;
  long *dimAxes = NULL;
  int nAxis;
  int typeCode;
  long repeat;
  long width;
      
  if (fits_movabs_hdu(fp, hdu, &hduType, &ret)) {
    return length;
  }
  
  switch (hduType) {
    case IMAGE_HDU:
      // find the size of the image and compute total elements
      maxDim = 100; // no images should have > 100 dimensions...
      dimAxes = (long*)calloc(maxDim, sizeof(long));      
      if (fits_read_imghdr(fp, maxDim, NULL, NULL, &nAxis, dimAxes, NULL, NULL, NULL, &ret)) {
        return length;
      }
      for (int j = 0; j < nAxis; j++) {
        length += dimAxes[j];
      }
      free(dimAxes);
      break;
      
    case ASCII_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // ASCII tables always have repeat == 1, so just find 
      // number of rows.
      if (fits_read_atblhdr(fp, maxDim, NULL, &nRows, NULL, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return length;
      }
      length = nRows;
      
      break;
      
    case BINARY_TBL:
      maxDim = 2147483646; // 2^32/2 - 2 (i.e. a big positive integer)
      
      // find number of table rows
      if (fits_read_btblhdr(fp, maxDim, &nRows, NULL, NULL, NULL, NULL, NULL, NULL, &ret)) {
        return length;
      }
      
      // binary tables may contain "wide" columns where each row
      // entry contains a vector of values.  we need to account for
      // this case when computing the number of elements in the column.
      
      pos = colReg.search(field);
      matches = colReg.capturedTexts();
      col = matches[2].toInt();
        
      if (fits_get_coltype(fp, col, &typeCode, &repeat, &width, &ret)) {
        return length;
      }
      length = nRows * (long)repeat;
      break;
      
    default:
      return length;
      break;
  }
  
  return nRead;
}


long fitsReadMatrix( fitsfile *fp, QString matrix, double *data, long xStart, long xSkip, long nX, long yStart, long ySkip, long nY ) {
  long xRead = 0;
  long yRead = 0;

  
  
  
  return xRead*yRead;
}


// vim: ts=2 sw=2 et

