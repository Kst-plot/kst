/***************************************************************************
                  fitstools.h  -  general tools for FITS files
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

#ifndef FITSTOOLS_H
#define FITSTOOLS_H

#include <math.h>
#include <fitsio.h>
#include <qpair.h>
#include <qstring.h>
#include <q3valuelist.h>
#include <qstringlist.h>
#include <qmap.h>

// convenience functions

char **fitsSarrayAlloc( size_t nstring );

int fitsSarrayFree( char **array, size_t nstring );

// determine properties of the file

int fitsNHDU( fitsfile *fp );

QMap<QString, QString> fitsKeys( fitsfile *fp, int HDU );
    
int fitsNamesUnits( fitsfile *fp, int HDU, QStringList *names, QStringList *units );

QStringList fitsFields( fitsfile *fp, int HDU );
    
QStringList fitsMatrices( fitsfile *fp, int HDU );

Q3ValueList<int> fitsDim( fitsfile *fp, int HDU );

// read data from file





#endif
// vim: ts=2 sw=2 et

