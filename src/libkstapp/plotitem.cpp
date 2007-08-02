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

#include "plotrenderitem.h"

#include "kstsvector.h"
#include "kstvcurve.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "vectorcurverenderitem.h"

#include <QDebug>

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent) {

  // FIXME fake data for testing rendering
  KstVectorPtr xTest = new KstSVector(0.0, 100.0, 10000, KstObjectTag::fromString("X vector"));
  KstVectorPtr yTest = new KstSVector(-100.0, 100.0, 10000, KstObjectTag::fromString("Y vector"));

  KstVCurvePtr renderTest = new KstVCurve(QString("rendertest"), xTest, yTest, xTest, yTest, xTest, yTest, QColor(Qt::red));
  renderTest->writeLock();
  renderTest->update(0);
  renderTest->unlock();

  KstRelationList relationList;
  relationList.append(kst_cast<KstRelation>(renderTest));

  VectorCurveRenderItem *test = new VectorCurveRenderItem("cartesiantest");
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

  painter->translate(rect().x(), rect().y() + rect().height());

  QRectF range = boundingRect();
  range = painter->transform().mapRect(range);
  range.moveTopLeft(QPoint(0,0));

  foreach (PlotRenderItem *renderer, _renderers) {

    renderer->setRange(range); //FIXME no idea if this is the idea...
    renderer->paint(painter);
  }
}

}

// vim: ts=2 sw=2 et
