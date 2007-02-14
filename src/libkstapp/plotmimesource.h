/***************************************************************************
                          plotmimesource.h  -  description
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

#ifndef PLOTMIMESOURCE_H
#define PLOTMIMESOURCE_H

#include "kstdrag.h"

class PlotMimeSource : public KstDrag {
  public:
    PlotMimeSource(QString window, QStringList plots, QWidget *dragSource = 0L);

    virtual QByteArray  encodedData(const char*) const;
    static const char*  mimeType();

  private:
    QString _window;
    QStringList _plots;
};

#endif
// vim: ts=2 sw=2 et
