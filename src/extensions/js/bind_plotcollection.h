/***************************************************************************
                            bind_plotcollection.h
                             -------------------
    begin                : Mar 31 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef BIND_PLOTCOLLECTION_H
#define BIND_PLOTCOLLECTION_H

#include "bind_collection.h"

#include <kst2dplot.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class PlotCollection
   @inherits Collection
   @description An array of Plots.
*/
class KstBindPlotCollection : public KstBindCollection {
  public:
    KstBindPlotCollection(KJS::ExecState *exec, KstViewWindow *window);
    KstBindPlotCollection(KJS::ExecState *exec, Kst2DPlotList plots);
    ~KstBindPlotCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;

  protected:
    QStringList _plots;
    QString _window;
    bool _isWindow;
};


#endif

// vim: ts=2 sw=2 et
