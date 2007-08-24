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

static qreal MARGIN_WIDTH = 20.0;
static qreal MARGIN_HEIGHT = 20.0;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent), _marginWidth(0), _marginHeight(0) {

  // FIXME fake data for testing rendering
  KstVectorPtr xTest = new KstSVector(0.0, 100.0, 10000, KstObjectTag::fromString("X vector"));
  xTest->setLabel("a nice bottom label");
  KstVectorPtr yTest = new KstSVector(0.0, 100.0, 10000, KstObjectTag::fromString("Y vector"));
  yTest->setLabel("a nice left label");

  KstVectorPtr yTest2 = new KstSVector(-100.0, 100.0, 10000, KstObjectTag::fromString("Y vector 2"));
  yTest2->setLabel("another nice left label");

  KstVectorPtr errorX = new KstSVector(0.0, 0.0, 0, KstObjectTag::fromString("X error"));
  KstVectorPtr errorY = new KstSVector(0.0, 0.0, 0, KstObjectTag::fromString("y error"));

  KstVCurvePtr renderTest = new KstVCurve(QString("rendertest"), xTest, yTest, errorX, errorY, errorX, errorY, QColor(Qt::red));
  renderTest->writeLock();
  renderTest->update(0);
  renderTest->unlock();

  KstVCurvePtr renderTest2 = new KstVCurve(QString("rendertest2"), xTest, yTest2, errorX, errorY, errorX, errorY, QColor(Qt::blue));
  renderTest2->writeLock();
  renderTest2->update(0);
  renderTest2->unlock();

  KstRelationList relationList;
  relationList.append(kst_cast<KstRelation>(renderTest));
  relationList.append(kst_cast<KstRelation>(renderTest2));

  VectorCurveRenderItem *test = new VectorCurveRenderItem("cartesiantest", this);
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

  painter->save();
  painter->translate(QPointF(rect().x(), rect().y()));

  //Calculate and adjust the margins based on the bounds...
  calculateLeftLabelBound(painter);
//   calculateBottomLabelBound(painter);
//   calculateRightLabelBound(painter);
//   calculateTopLabelBound(painter);

//   qDebug() << "=============> leftLabel:" << leftLabel() << endl;
//   qDebug() << "=============> bottomLabel:" << bottomLabel() << endl;
//   qDebug() << "=============> rightLabel:" << rightLabel() << endl;
//   qDebug() << "=============> topLabel:" << topLabel() << endl;

  //paint the left label
  {
  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  QRectF leftLabelRect = verticalLabelRect();
  leftLabelRect.moveTopLeft(QPointF(0.0, marginHeight()));

  qDebug() << "leftLabelRect" << leftLabelRect << endl;

  painter->drawText(t.mapRect(leftLabelRect), Qt::TextWordWrap | Qt::AlignCenter, leftLabel());
  painter->restore();
  }

  //paint the right label
  {
  painter->save();
  painter->translate(width() - marginWidth(), 0.0);
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);

  //same as left but painter is translated
  QRectF rightLabelRect = verticalLabelRect();
  rightLabelRect.moveTopLeft(QPointF(0.0, marginHeight()));

  painter->drawText(t.mapRect(rightLabelRect), Qt::TextWordWrap | Qt::AlignCenter, rightLabel());
  painter->restore();
  }

  painter->restore();
}


qreal PlotItem::marginWidth() const {
  qreal m = qMax(MARGIN_WIDTH, _marginWidth);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


void PlotItem::setMarginWidth(qreal marginWidth) {
  _marginWidth = marginWidth;
  emit geometryChanged();
}


qreal PlotItem::marginHeight() const {
  qreal m = qMax(MARGIN_HEIGHT, _marginHeight);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


void PlotItem::setMarginHeight(qreal marginHeight) {
  _marginHeight = marginHeight;
  emit geometryChanged();
}


QString PlotItem::leftLabel() const {
  foreach (PlotRenderItem *renderer, _renderers) {
    if (!renderer->leftLabel().isEmpty())
      return renderer->leftLabel();
  }
  return QString();
}


QString PlotItem::bottomLabel() const {
  foreach (PlotRenderItem *renderer, _renderers) {
    if (!renderer->bottomLabel().isEmpty())
      return renderer->bottomLabel();
  }
  return QString();
}


QString PlotItem::rightLabel() const {
  foreach (PlotRenderItem *renderer, _renderers) {
    if (!renderer->rightLabel().isEmpty())
      return renderer->rightLabel();
  }
  return QString();
}


QString PlotItem::topLabel() const {
  foreach (PlotRenderItem *renderer, _renderers) {
    if (!renderer->topLabel().isEmpty())
      return renderer->topLabel();
  }
  return QString();
}


QRectF PlotItem::horizontalLabelRect() const {
  return QRectF(0.0, 0.0, width() - 2.0 * marginWidth(), marginHeight());
}


QRectF PlotItem::verticalLabelRect() const {
  return QRectF(0.0, 0.0, marginWidth(), height() - 2.0 * marginHeight());
}


void PlotItem::calculateLeftLabelBound(QPainter *painter) {

//   painter->save();
//   QTransform t;
//   t.rotate(90.0);
//   painter->rotate(-90.0);
// 
//   QRectF leftLabelRect = verticalLabelRect();
//   leftLabelRect.moveTopLeft(QPointF(0.0, marginHeight()));
// 
//   painter->drawText(t.mapRect(leftLabelRect), Qt::AlignCenter, leftLabel());
//   painter->restore();


  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  QRectF leftLabelRect = verticalLabelRect();
  leftLabelRect.moveTopLeft(QPointF(0.0, marginHeight()));

  QRectF leftLabelBound = painter->boundingRect(t.mapRect(leftLabelRect),
                                                Qt::TextWordWrap | Qt::AlignCenter, leftLabel());
  painter->restore();

  qDebug() << leftLabelBound << endl;

  if (leftLabelBound.height() > marginWidth()) {
    qDebug() << "here1" << endl;
    setMarginWidth(leftLabelBound.width());
  }
  if (leftLabelBound.width() > height() - 2 * marginHeight() && height() > 2 * marginHeight()) {
    qDebug() << "here2" << leftLabelBound.width() << height() << (2 * marginHeight()) << endl;
    setMarginHeight(height() - leftLabelBound.height() / 2);
  }
}


void PlotItem::calculateBottomLabelBound(QPainter *painter) {
  QRectF bottomLabelBound = painter->boundingRect(horizontalLabelRect(), Qt::AlignCenter, bottomLabel());
  if (bottomLabelBound.height() > marginHeight())
    setMarginHeight(bottomLabelBound.height());
  if (bottomLabelBound.width() > width() - 2 * marginWidth())
    setMarginWidth(width() - bottomLabelBound.width() / 2);
}


void PlotItem::calculateRightLabelBound(QPainter *painter) {
  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);
  QRectF rightLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect()),
                                                 Qt::AlignCenter, rightLabel());
  painter->restore();

  if (rightLabelBound.width() > marginWidth())
    setMarginWidth(rightLabelBound.width());
  if (rightLabelBound.height() > height() - 2 * marginHeight())
    setMarginHeight(height() - rightLabelBound.height() / 2);
}


void PlotItem::calculateTopLabelBound(QPainter *painter) {
  QRectF topLabelBound = painter->boundingRect(horizontalLabelRect(), Qt::AlignCenter, topLabel());
  if (topLabelBound.height() > marginHeight())
    setMarginHeight(topLabelBound.height());
  if (topLabelBound.width() > width() - 2 * marginWidth())
    setMarginWidth(width() - topLabelBound.width() / 2);
}

}

// vim: ts=2 sw=2 et
