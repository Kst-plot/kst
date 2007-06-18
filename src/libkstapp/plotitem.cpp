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

// FIXME temporary
#include "kstsvector.h"
#include "kstvcurve.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "render2dcartesian.h"

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent) {
  _backgroundColor = Qt::transparent;
  // FIXME: temporary test code
  QColor temp = Qt::black;
  _colorStack.push(temp);
  
  // FIXME: fake data for testing rendering
  KstVectorPtr xTest = new KstSVector(0.0, 100.0, 10000, KstObjectTag::fromString("X vector"));
  KstVectorPtr yTest = new KstSVector(-100.0, 100.0, 10000, KstObjectTag::fromString("Y vector"));
  KstVCurvePtr renderTest = new KstVCurve(QString("rendertest"), xTest, yTest, NULL, NULL, NULL, NULL, QColor(Qt::red));
  
  Render2DCartesian carTest(QString("cartesiantest"));
  carTest.sources.append(kst_cast<KstBaseCurve>(renderTest));
  renderers.append(carTest);
}


PlotItem::~PlotItem() {
}


void CreatePlotCommand::createItem() {
  _item = new PlotItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

void PlotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  QPainterPath path;
  setBrush(Qt::transparent);
  //FIXME:  temporary test code
  const qreal w = pen().widthF();
  path.addEllipse(rect().adjusted(w, w, -w, -w));
  painter->drawPath(path);
  QPen p = pen();
  setPen(Qt::NoPen);
  ViewItem::paint(painter, option, widget);
  setPen(p);
  
  //QFont testFont;
  //QColor fg = Qt::black;
  //QPen testPen(fg);
  //path.addText(100, 100, testFont, tr("This is a test")); 
  /*
  for (KstBaseCurveList::Iterator i = _sources.begin(); i != _sources.end(); ++i) {
    (*i)->paint(&path);
  }
  */
}

void PlotItem::paint(QPainter *painter) {
}

}

#include "plotitem.moc"

// vim: ts=2 sw=2 et
