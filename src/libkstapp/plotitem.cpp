/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDebug>
#include <QPainterPath>

#include "plotitem.h"

#include "kstsvector.h"
#include "kstvcurve.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "render2dcartesian.h"

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent) {

  // FIXME fake data for testing rendering
  KstVectorPtr xTest = new KstSVector(0.0, 100.0, 10000, KstObjectTag::fromString("X vector"));
  KstVectorPtr yTest = new KstSVector(-100.0, 100.0, 10000, KstObjectTag::fromString("Y vector"));
  KstVCurvePtr renderTest = new KstVCurve(QString("rendertest"), xTest, yTest, NULL, NULL, NULL, NULL, QColor(Qt::red));

  KstRelationList relationList;
  relationList.append(kst_cast<KstRelation>(renderTest));

  Render2DCartesian *test = new Render2DCartesian("cartesiantest");
  test->setRelationList(relationList);

  _renderers.append(test);
}


PlotItem::~PlotItem() {
}


void CreatePlotCommand::createItem() {
  _item = new PlotItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


void PlotItem::paint(QPainter *painter) {
  ViewItem::paint(painter);

  foreach (PlotRenderer2D *renderer, _renderers) {
    QList<QPainterPath> paths = renderer->projectedPaths();
    foreach (QPainterPath path, paths) {
      painter->drawPath(path);
    }
  }
}

}

// vim: ts=2 sw=2 et
