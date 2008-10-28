/***************************************************************************
                  healpix_tools.cpp  -  tools for healpix datasource
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

#include "healpix_tools.h"

/* FITS keys */

healpix_keys *healpix_keys_alloc()
{
  healpix_keys *keys;
  keys = (healpix_keys *) calloc(1, sizeof(healpix_keys));
  keys->nskeys = 0;
  keys->nikeys = 0;
  keys->nfkeys = 0;
  keys->skeynames = NULL;
  keys->skeyvals = NULL;
  keys->skeycoms = NULL;
  keys->ikeynames = NULL;
  keys->ikeyvals = NULL;
  keys->ikeycoms = NULL;
  keys->fkeynames = NULL;
  keys->fkeyvals = NULL;
  keys->fkeycoms = NULL;
  return keys;
}

int healpix_keys_free(healpix_keys * keys)
{
  size_t i;
  if (keys) {
    for (i = 0; i < (keys->nskeys); i++) {
      free(keys->skeynames[i]);
      free(keys->skeyvals[i]);
      free(keys->skeycoms[i]);
    }
    if (keys->nskeys != 0) {
      free(keys->skeynames);
      free(keys->skeyvals);
      free(keys->skeycoms);
    }
    for (i = 0; i < (keys->nikeys); i++) {
      free(keys->ikeynames[i]);
      free(keys->ikeycoms[i]);
    }
    if (keys->nikeys != 0) {
      free(keys->ikeynames);
      free(keys->ikeyvals);
      free(keys->ikeycoms);
    }
    for (i = 0; i < (keys->nfkeys); i++) {
      free(keys->fkeynames[i]);
      free(keys->fkeycoms[i]);
    }
    if (keys->nfkeys != 0) {
      free(keys->fkeynames);
      free(keys->fkeyvals);
      free(keys->fkeycoms);
    }
    free(keys);
  }
  return 0;
}

int healpix_keys_clear(healpix_keys * keys)
{
  size_t i;
  if (keys) {
    for (i = 0; i < (keys->nskeys); i++) {
      free(keys->skeynames[i]);
      free(keys->skeyvals[i]);
      free(keys->skeycoms[i]);
    }
    if (keys->nskeys != 0) {
      free(keys->skeynames);
      free(keys->skeyvals);
      free(keys->skeycoms);
    }
    for (i = 0; i < (keys->nikeys); i++) {
      free(keys->ikeynames[i]);
      free(keys->ikeycoms[i]);
    }
    if (keys->nikeys != 0) {
      free(keys->ikeynames);
      free(keys->ikeyvals);
      free(keys->ikeycoms);
    }
    for (i = 0; i < (keys->nfkeys); i++) {
      free(keys->fkeynames[i]);
      free(keys->fkeycoms[i]);
    }
    if (keys->nfkeys != 0) {
      free(keys->fkeynames);
      free(keys->fkeyvals);
      free(keys->fkeycoms);
    }
    keys->nskeys = 0;
    keys->nikeys = 0;
    keys->nfkeys = 0;
    keys->skeynames = NULL;
    keys->skeyvals = NULL;
    keys->skeycoms = NULL;
    keys->ikeynames = NULL;
    keys->ikeyvals = NULL;
    keys->ikeycoms = NULL;
    keys->fkeynames = NULL;
    keys->fkeyvals = NULL;
    keys->fkeycoms = NULL;
  }
  return 0;
}

/* add keys */

int healpix_keys_sadd(healpix_keys * keys, char *keyname, char *keyval, char *keycom)
{
  if (keys) {
    
    (keys->skeynames) =
        (char **)realloc(keys->skeynames,
    ((keys->nskeys) + 1) * sizeof(char *));
    (keys->skeynames[keys->nskeys]) =
        (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    (keys->skeyvals) = (char **)realloc(keys->skeyvals, ((keys->nskeys) + 1) * sizeof(char *));
    (keys->skeyvals[keys->nskeys]) = (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    (keys->skeycoms) = (char **)realloc(keys->skeycoms, ((keys->nskeys) + 1) * sizeof(char *));
    (keys->skeycoms[keys->nskeys]) = (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    strncpy(keys->skeynames[keys->nskeys], keyname, HEALPIX_STRNL);
    strncpy(keys->skeyvals[keys->nskeys], keyval, HEALPIX_STRNL);
    strncpy(keys->skeycoms[keys->nskeys], keycom, HEALPIX_STRNL);
    (keys->nskeys)++;
  }
  return 0;
}

int healpix_keys_iadd(healpix_keys * keys, char *keyname, int keyval, char *keycom)
{
  if (keys) {
    (keys->ikeynames) =
        (char **)realloc(keys->ikeynames,
    ((keys->nikeys) + 1) * sizeof(char *));
    (keys->ikeynames[keys->nikeys]) =
        (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    (keys->ikeyvals) =
        (int *)realloc(keys->ikeyvals, ((keys->nikeys) + 1) * sizeof(int));
    
    (keys->ikeycoms) =
        (char **)realloc(keys->ikeycoms, ((keys->nikeys) + 1) * sizeof(char *));
    (keys->ikeycoms[keys->nikeys]) = (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    strncpy(keys->ikeynames[keys->nikeys], keyname, HEALPIX_STRNL);
    keys->ikeyvals[keys->nikeys] = keyval;
    strncpy(keys->ikeycoms[keys->nikeys], keycom, HEALPIX_STRNL);
    (keys->nikeys)++;
  }
  return 0;
}

int healpix_keys_fadd(healpix_keys * keys, char *keyname, float keyval, char *keycom)
{
  if (keys) {
    (keys->fkeynames) =
        (char **)realloc(keys->fkeynames,
    ((keys->nfkeys) + 1) * sizeof(char *));
    (keys->fkeynames[keys->nfkeys]) =
        (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    (keys->fkeyvals) =
        (float *)realloc(keys->fkeyvals, ((keys->nfkeys) + 1) * sizeof(float));
    
    (keys->fkeycoms) =
        (char **)realloc(keys->fkeycoms, ((keys->nfkeys) + 1) * sizeof(char *));
    (keys->fkeycoms[keys->nfkeys]) = (char *)calloc(HEALPIX_STRNL, sizeof(char));
    
    strncpy(keys->fkeynames[keys->nfkeys], keyname, HEALPIX_STRNL);
    keys->fkeyvals[keys->nfkeys] = keyval;
    strncpy(keys->fkeycoms[keys->nfkeys], keycom, HEALPIX_STRNL);
    (keys->nfkeys)++;
  }
  return 0;  
}

/* read keys */

int healpix_keys_read(healpix_keys * keys, fitsfile * fp, int *ret)
{
  int nread = 0;
  int rec = 0;
  size_t nexc = 21;
  char **exclist;
  char **inclist;
  char card[HEALPIX_STRNL];
  char keyval[HEALPIX_STRNL];
  char keycom[HEALPIX_STRNL];
  char keytype;
  char keyname[HEALPIX_STRNL];
  int keylen;

  exclist = healpix_strarr_alloc(nexc);
  inclist = healpix_strarr_alloc(1);

  /*exclude required keywords */
  strcpy(exclist[0], "XTENSION");
  strcpy(exclist[1], "BITPIX");
  strcpy(exclist[2], "NAXIS*");
  strcpy(exclist[3], "PCOUNT");
  strcpy(exclist[4], "GCOUNT");
  strcpy(exclist[5], "TFIELDS");
  strcpy(exclist[6], "TTYPE*");
  strcpy(exclist[7], "TFORM*");
  strcpy(exclist[8], "TUNIT*");
  strcpy(exclist[9], "EXTNAME");
  strcpy(exclist[10], "PIXTYPE");
  strcpy(exclist[11], "ORDERING");
  strcpy(exclist[12], "NSIDE");
  strcpy(exclist[13], "COORDSYS");
  strcpy(exclist[14], "INDXSCHM");
  strcpy(exclist[15], "GRAIN");
  strcpy(exclist[16], "COMMENT");
  strcpy(exclist[17], "TBCOL*");
  strcpy(exclist[18], "SIMPLE");
  strcpy(exclist[19], "EXTEND");
  strcpy(exclist[19], "CREATOR");
  strcpy(inclist[0], "*");

  (*ret) = 0;
  if (fits_read_record(fp, rec, card, ret)) {
    return 0;
  }
  while (!fits_find_nextkey(fp, inclist, 1, exclist, (int)nexc, card, ret)) {
    fits_get_keyname(card, keyname, &keylen, ret);
    fits_parse_value(card, keyval, keycom, ret);
    fits_get_keytype(keyval, &keytype, ret);
    switch (keytype) {
      case 'I':
        healpix_keys_iadd(keys, keyname, atoi(keyval), keycom);
        break;
      case 'F':
        healpix_keys_fadd(keys, keyname, (float)atof(keyval), keycom);
        break;
      default:
        healpix_keys_sadd(keys, keyname, keyval, keycom);
        break;
    }
    nread++;
  }
  (*ret = 0);

  healpix_strarr_free(exclist, nexc);
  healpix_strarr_free(inclist, 1);

  return nread;
}

/* FITS file tests */

int healpix_fits_map_test(char *filename, size_t * nside, int *order, int *coord, int *type, size_t * nmaps)
{
  fitsfile *fp;
  int ret = 0;
  int ttype;
  int inside;
  long nrows;
  long pcount;
  int tfields;
  int grain;
  char pixtype[HEALPIX_STRNL];
  char coordstr[HEALPIX_STRNL];
  char orderstr[HEALPIX_STRNL];
  char indxstr[HEALPIX_STRNL];
  char comment[HEALPIX_STRNL];
  char extname[HEALPIX_STRNL];
  float nullval = HEALPIX_NULL;
  int nnull;
  float testval;
  long keynpix;
  long keyfirst;
  int ischunk = 0;
  int lastcol;

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    return 0;
  }

  /* make sure extension is a binary table */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (ttype != BINARY_TBL) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* determine the number of maps */
  if (fits_read_btblhdr(fp, HEALPIX_FITS_MAXCOL, &nrows, &tfields, NULL, NULL, NULL, extname, &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* make sure this is a HEALPIX file */
  if (fits_read_key(fp, TSTRING, "PIXTYPE", pixtype, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(pixtype, "HEALPIX", HEALPIX_STRNL) != 0) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* check required keywords (NSIDE, ORDERING, COORDSYS) */
  if (fits_read_key(fp, TINT, "NSIDE", &inside, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  (*nside) = (size_t) inside;
  if (healpix_nsidecheck(*nside)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (fits_read_key(fp, TSTRING, "ORDERING", orderstr, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(orderstr, "RING", HEALPIX_STRNL) == 0) {
    (*order) = HEALPIX_RING;
  } else if (strncmp(orderstr, "NESTED", HEALPIX_STRNL) == 0) {
    (*order) = HEALPIX_NEST;
  } else {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (fits_read_key(fp, TSTRING, "COORDSYS", coordstr, comment, &ret)) {
    ret = 0;
    (*coord) = HEALPIX_COORD_C;
  } else {
    if (strncmp(coordstr, "C", HEALPIX_STRNL) == 0) {
      (*coord) = HEALPIX_COORD_C;
    } else if (strncmp(coordstr, "G", HEALPIX_STRNL) == 0) {
      (*coord) = HEALPIX_COORD_G;
    } else if (strncmp(coordstr, "E", HEALPIX_STRNL) == 0) {
      (*coord) = HEALPIX_COORD_E;
    } else {
      (*coord) = HEALPIX_COORD_O;
    }
  }

  /* check for INDXSCHM and GRAIN.  if not found, assume implicit */
  
  strcpy(indxstr,"");
  if (fits_read_key(fp, TSTRING, "OBJECT", indxstr, comment, &ret)) {
    ret = 0;
    if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
      ret = 0;
      (*type) = HEALPIX_FITS_FULL;
    } else {
      if (strncmp(indxstr, "EXPLICIT", HEALPIX_STRNL) == 0) {
        (*type) = HEALPIX_FITS_CUT;
      } else {
        (*type) = HEALPIX_FITS_FULL;
      }
    }
    if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
      ret = 0;
      grain = 0;
    }
    if (((grain == 0) && ((*type) == HEALPIX_FITS_CUT)) ||
        ((grain != 0) && ((*type) == HEALPIX_FITS_FULL))) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {   
    if (strncmp(indxstr, "PARTIAL", HEALPIX_STRNL) == 0) {
      (*type) = HEALPIX_FITS_CUT;
    } else {
      if (strncmp(indxstr, "FULLSKY", HEALPIX_STRNL) == 0) {
        (*type) = HEALPIX_FITS_FULL;
      } else {
        if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
          ret = 0;
          (*type) = HEALPIX_FITS_FULL;
        } else {
          if (strncmp(indxstr, "EXPLICIT", HEALPIX_STRNL) == 0) {
            (*type) = HEALPIX_FITS_CUT;
          } else {
            (*type) = HEALPIX_FITS_FULL;
          }
        }
        if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
          ret = 0;
          grain = 0;
        }
        if (((grain == 0) && ((*type) == HEALPIX_FITS_CUT)) ||
            ((grain != 0) && ((*type) == HEALPIX_FITS_FULL))) {
          ret = 0;
          fits_close_file(fp, &ret);
          return 0;
        } 
      } 
    }
  }
  
  /* check for chunk file and truncation */
  
  if ((*type) == HEALPIX_FITS_FULL) {
    (*nmaps) = tfields;
    if ((nrows != (long)(12*inside*inside))&&(1024*nrows != (long)(12*inside*inside))) {
      /*is this a chunk file?*/
      if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
        /*must at least have FIRSTPIX key*/
        ret = 0;
        ischunk = 0;
      } else {
        if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
          ret = 0;
          /*might be using LASTPIX instead*/
          if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
            ret = 0;
            ischunk = 0;
          } else {
            keynpix = keynpix - keyfirst + 1;
            if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
              ischunk = 0;
            } else {
              ischunk = 1;
            }
          }
        } else {
          if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
            ischunk = 0;
          } else {
            ischunk = 1;
          }
        }
      }
    } else {
      /*header doesn't matter, since we have entire map*/
      if (nrows == (long)(12*inside*inside)) {
        lastcol = 1;
      } else {
        lastcol = 1024;
      }
    }
    if (ischunk) {
      if (nrows == keynpix) {
        lastcol = 1;
      } else {
        lastcol = keynpix % 1024;
        if (lastcol == 0) {
          lastcol = 1024;
        }
      }
    }
    if (fits_read_col(fp, TFLOAT, 1, nrows, lastcol, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    (*nmaps) = tfields - 3;
    if (fits_read_col(fp, TFLOAT, 2, nrows, 1, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    } 
  }
  
  fits_close_file(fp, &ret);
  return 1;
}

int healpix_fits_map_info(char *filename, size_t * nside, int *order, int *coord, int *type, size_t * nmaps, char *creator, char *extname, char **names, char **units, healpix_keys *keys)
{
  fitsfile *fp;
  int ret = 0;
  int ttype;
  int inside;
  long nrows;
  long pcount;
  int tfields;
  int grain;
  char pixtype[HEALPIX_STRNL];
  char coordstr[HEALPIX_STRNL];
  char orderstr[HEALPIX_STRNL];
  char indxstr[HEALPIX_STRNL];
  char comment[HEALPIX_STRNL];
  float nullval = HEALPIX_NULL;
  int nnull;
  float testval;
  long keynpix;
  long keyfirst;
  int ischunk = 0;
  int lastcol;

  /* open file */
  if (fits_open_file(&fp, filename, READONLY, &ret)) {
    return 0;
  }
  
  /* read header info */
  fits_read_key(fp, TSTRING, "CREATOR", creator, comment, &ret);
  ret = 0;

  /* make sure extension is a binary table */
  if (fits_movabs_hdu(fp, 2, &ttype, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (ttype != BINARY_TBL) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* determine the number of maps */
  if (fits_read_btblhdr(fp, HEALPIX_FITS_MAXCOL, &nrows, &tfields, names, NULL, units, extname, &pcount, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  
  /* make sure this is a HEALPIX file */
  if (fits_read_key(fp, TSTRING, "PIXTYPE", pixtype, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(pixtype, "HEALPIX", HEALPIX_STRNL) != 0) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }

  /* check required keywords (NSIDE, ORDERING, COORDSYS) */
  if (fits_read_key(fp, TINT, "NSIDE", &inside, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  (*nside) = (size_t) inside;
  if (healpix_nsidecheck(*nside)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (fits_read_key(fp, TSTRING, "ORDERING", orderstr, comment, &ret)) {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (strncmp(orderstr, "RING", HEALPIX_STRNL) == 0) {
    (*order) = HEALPIX_RING;
  } else if (strncmp(orderstr, "NESTED", HEALPIX_STRNL) == 0) {
    (*order) = HEALPIX_NEST;
  } else {
    ret = 0;
    fits_close_file(fp, &ret);
    return 0;
  }
  if (fits_read_key(fp, TSTRING, "COORDSYS", coordstr, comment, &ret)) {
    ret = 0;
    (*coord) = HEALPIX_COORD_C;
  } else {
    if (strncmp(coordstr, "C", HEALPIX_STRNL) == 0) {
      (*coord) = HEALPIX_COORD_C;
    } else if (strncmp(coordstr, "G", HEALPIX_STRNL) == 0) {
      (*coord) = HEALPIX_COORD_G;
    } else if (strncmp(coordstr, "E", HEALPIX_STRNL) == 0) {
      (*coord) = HEALPIX_COORD_E;
    } else {
      (*coord) = HEALPIX_COORD_O;
    }
  }

  /* check for INDXSCHM and GRAIN.  if not found, assume implicit */
  
  strcpy(indxstr,"");
  if (fits_read_key(fp, TSTRING, "OBJECT", indxstr, comment, &ret)) {
    ret = 0;
    if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
      ret = 0;
      (*type) = HEALPIX_FITS_FULL;
    } else {
      if (strncmp(indxstr, "EXPLICIT", HEALPIX_STRNL) == 0) {
        (*type) = HEALPIX_FITS_CUT;
      } else {
        (*type) = HEALPIX_FITS_FULL;
      }
    }
    if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
      ret = 0;
      grain = 0;
    }
    if (((grain == 0) && ((*type) == HEALPIX_FITS_CUT)) ||
        ((grain != 0) && ((*type) == HEALPIX_FITS_FULL))) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {   
    if (strncmp(indxstr, "PARTIAL", HEALPIX_STRNL) == 0) {
      (*type) = HEALPIX_FITS_CUT;
    } else {
      if (strncmp(indxstr, "FULLSKY", HEALPIX_STRNL) == 0) {
        (*type) = HEALPIX_FITS_FULL;
      } else {
        if (fits_read_key(fp, TSTRING, "INDXSCHM", indxstr, comment, &ret)) {
          ret = 0;
          (*type) = HEALPIX_FITS_FULL;
        } else {
          if (strncmp(indxstr, "EXPLICIT", HEALPIX_STRNL) == 0) {
            (*type) = HEALPIX_FITS_CUT;
          } else {
            (*type) = HEALPIX_FITS_FULL;
          }
        }
        if (fits_read_key(fp, TINT, "GRAIN", &grain, comment, &ret)) {
          ret = 0;
          grain = 0;
        }
        if (((grain == 0) && ((*type) == HEALPIX_FITS_CUT)) ||
            ((grain != 0) && ((*type) == HEALPIX_FITS_FULL))) {
          ret = 0;
          fits_close_file(fp, &ret);
          return 0;
        } 
      } 
    }
  }
      
  /* read extension keys */
  healpix_keys_read(keys, fp, &ret);
        
  /* check for chunk file and truncation */
  
  if ((*type) == HEALPIX_FITS_FULL) {
    (*nmaps) = tfields;
    if ((nrows != (long)(12*inside*inside))&&(1024*nrows != (long)(12*inside*inside))) {
      /*is this a chunk file?*/
      if (fits_read_key(fp, TLONG, "FIRSTPIX", &keyfirst, comment, &ret)) {
        /*must at least have FIRSTPIX key*/
        ret = 0;
        ischunk = 0;
      } else {
        if (fits_read_key(fp, TLONG, "NPIX", &keynpix, comment, &ret)) {
          ret = 0;
          /*might be using LASTPIX instead*/
          if (fits_read_key(fp, TLONG, "LASTPIX", &keynpix, comment, &ret)) {
            ret = 0;
            ischunk = 0;
          } else {
            keynpix = keynpix - keyfirst + 1;
            if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
              ischunk = 0;
            } else {
              ischunk = 1;
            }
          }
        } else {
          if ((keyfirst < 0)||(keynpix < 0)||(keynpix+keyfirst > (long)(12*inside*inside))) {
            ischunk = 0;
          } else {
            ischunk = 1;
          }
        }
      }
    } else {
      /*header doesn't matter, since we have entire map*/
      if (nrows == (long)(12*inside*inside)) {
        lastcol = 1;
      } else {
        lastcol = 1024;
      }
    }
    if (ischunk) {
      if (nrows == keynpix) {
        lastcol = 1;
      } else {
        lastcol = keynpix % 1024;
        if (lastcol == 0) {
          lastcol = 1024;
        }
      }
    }
    if (fits_read_col(fp, TFLOAT, 1, nrows, lastcol, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    }
  } else {
    (*nmaps) = tfields - 3;
    if (fits_read_col(fp, TFLOAT, 2, nrows, 1, 1, &nullval, &testval, &nnull, &ret)) {
      ret = 0;
      fits_close_file(fp, &ret);
      return 0;
    } 
  }
  
  fits_close_file(fp, &ret);
  return 1;
}

