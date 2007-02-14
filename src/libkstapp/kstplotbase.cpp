/***************************************************************************
                              kstplotbase.cpp
                             -----------------
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

#include "kstplotbase.h"


KstPlotBase::KstPlotBase(const QString& type)
: KstMetaPlot(type) {
}


KstPlotBase::KstPlotBase(const QDomElement& e)
: KstMetaPlot(e) {
}


KstPlotBase::KstPlotBase(const KstPlotBase& plotBase)
: KstMetaPlot(plotBase) {
}


KstPlotBase::~KstPlotBase() {
}


void KstPlotBase::save(QTextStream& ts, const QString& indent) {
  KstMetaPlot::save(ts, indent);
}


#include "kstplotbase.moc"
// vim: ts=2 sw=2 et
