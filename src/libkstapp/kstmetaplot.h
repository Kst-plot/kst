/***************************************************************************
                               kstmetaplot.h
                             -------------------
    begin                : Mar 11, 2004
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

#ifndef KSTMETAPLOT_H
#define KSTMETAPLOT_H

#include "kstborderedviewobject.h"

class KstMetaPlot : public KstBorderedViewObject {
  Q_OBJECT
  protected:
    KstMetaPlot(const QString& type);
  public:
    KstMetaPlot(const QDomElement& e);
    KstMetaPlot(const KstMetaPlot& metaPlot);
    virtual ~KstMetaPlot();

    virtual void save(QTextStream& ts, const QString& indent = QString::null);
};

typedef KstSharedPtr<KstMetaPlot> KstMetaPlotPtr;
typedef KstObjectList<KstMetaPlotPtr> KstMetaPlotList;


#endif
// vim: ts=2 sw=2 et
