/***************************************************************************
                              kstmetaplot.cpp
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

#include "kstmetaplot.h"


KstMetaPlot::KstMetaPlot(const QString& type)
: KstBorderedViewObject(type) {
  setFollowsFlow(true);
}


KstMetaPlot::KstMetaPlot(const QDomElement& e)
: KstBorderedViewObject(e) {
  setFollowsFlow(true);
}


KstMetaPlot::KstMetaPlot(const KstMetaPlot& metaPlot)
: KstBorderedViewObject(metaPlot) {
}


KstMetaPlot::~KstMetaPlot() {
}


void KstMetaPlot::save(Q3TextStream& ts, const QString& indent) {
  KstBorderedViewObject::save(ts, indent);
}


#include "kstmetaplot.moc"
// vim: ts=2 sw=2 et
