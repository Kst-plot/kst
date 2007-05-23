/***************************************************************************
                              kstplotbase.h
                             ---------------
    begin                : Mar 28, 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef KSTPLOTBASE_H
#define KSTPLOTBASE_H

#include "kstmetaplot.h"

class KstPlotBase : public KstMetaPlot {
  Q_OBJECT
  public:
    KstPlotBase(const QString& type);
  public:
    KstPlotBase(const QDomElement& e);
    KstPlotBase(const KstPlotBase& plotBase);
    virtual ~KstPlotBase();

    virtual void save(QTextStream& ts, const QString& indent = QString::null);
};

typedef KstSharedPtr<KstPlotBase> KstPlotBasePtr;
typedef KstObjectList<KstPlotBasePtr> KstPlotBaseList;
typedef KstObjectMap<KstPlotBasePtr> KstPlotBaseMap;


#endif
// vim: ts=2 sw=2 et
