/***************************************************************************
                  fitstools.h  -  general tools for FITS files
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

#ifndef FITSTOOLS_H
#define FITSTOOLS_H

#include <math.h>
#include <fitsio.h>
#include <qpair.h>
#include <qstring.h>
#include <q3valuelist.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qregexp.h>

// convenience functions

char **fitsSarrayAlloc( size_t nstring );

int fitsSarrayFree( char **array, size_t nstring );

void fitsGetHC( QString name, int *hdu, int *col );

// determine file properties

int fitsNHDU( fitsfile *fp );

QMap<QString, QString> fitsKeys( fitsfile *fp);
    
// create field and matrix lists
    
int fitsFieldNamesUnits( fitsfile *fp, QStringList *names, QStringList *units );

int fitsMatrixNamesUnits( fitsfile *fp, QStringList *names, QStringList *units );

QStringList fitsFields( fitsfile *fp );
    
QStringList fitsMatrices( fitsfile *fp );

// determine dimensions of fields and matrices

long fitsFieldDims( fitsfile *fp, QString field, long *nX, long *nY );

long fitsMatrixDims( fitsfile *fp, QString matrix, long *nX, long *nY );

// read data from file

long fitsReadField( fitsfile *fp, QString field, double *data, long start, long skip, long n );

long fitsReadMatrix( fitsfile *fp, QString matrix, double *data, long xStart, long xSkip, long nX, long yStart, long ySkip, long nY );


#endif
// vim: ts=2 sw=2 et

