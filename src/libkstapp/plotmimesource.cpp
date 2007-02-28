/***************************************************************************
                          plotmimesource.cpp  -  description
                             -------------------
    begin                : Mar 14th 2005
    copyright            : (C) 2005 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <qmime.h>

// include files for KDE

// application specific includes
#include "kst.h"
#include "kstviewwindow.h"
#include "plotmimesource.h"

PlotMimeSource::PlotMimeSource(QString window, QStringList plots, QWidget *dragSource) : 
  KstDrag(mimeType(), dragSource) {
  _window = window;
  _plots = plots;
}

QByteArray PlotMimeSource::encodedData(const char *mime) const {
  QByteArray  bytes;
  QDataStream data(bytes, QIODevice::WriteOnly);
  
  if (strcmp(mime, format(0)) == 0) {
    data << _window;
    data << _plots;    
  }
  
  return bytes;
}

const char* PlotMimeSource::mimeType() {
  return "application/x-kst-plot-names";
}

