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

#include "gradienteditor.h"

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

#include <math.h>

namespace Kst {

GradientEditor::GradientEditor(QWidget *parent)
  : QWidget(parent), _gradient(0), _movingStop(-1) {
  setMouseTracking(true);
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  _gradient = new QLinearGradient(1,0,0,0);
  _gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
}


GradientEditor::~GradientEditor() {
  delete _gradient;
}


QSize GradientEditor::sizeHint() const {
  return QSize(120,120);
}


QGradient GradientEditor::gradient() const {
  _gradient->setStops(gradientStops());
  return *_gradient;
}


void GradientEditor::setGradient(const QGradient &gradient) {
  _gradient->setStops(gradient.stops());
}


void GradientEditor::mousePressEvent(QMouseEvent *event) {
  QWidget::mousePressEvent(event);

  QList<Stop> stops = _stopHash.values();
  foreach (Stop stop, stops) {
    if (stop.path.contains(event->pos())) {
      _movingStop = stop.pos;
      return;
    }
  }
}


void GradientEditor::mouseReleaseEvent(QMouseEvent *event) {
  QWidget::mouseReleaseEvent(event);

  if (_movingStop != -1) {
    _movingStop = -1;
    return;
  }

  bool ok;
  QRgb color = QColorDialog::getRgba(Qt::white, &ok, parentWidget());
  if (ok) {
    int position = event->pos().x();
    Stop stop;
    stop.pos = position;
    stop.color = color;
    stop.path = marker(position);
    _stopHash.insert(position, stop);
    emit changed(gradient());
    update();
  }
}


void GradientEditor::mouseMoveEvent(QMouseEvent *event) {
  QWidget::mouseMoveEvent(event);

  if (_movingStop != -1) {
    int position = event->pos().x();
    Stop stop = _stopHash.take(_movingStop);
    stop.pos = position;
    stop.path = marker(position);
    _stopHash.insert(position, stop);
    _movingStop = position;
    emit changed(gradient());
  }

  update();
}


void GradientEditor::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

//   if (!isEnabled()) {
//     return;
//   }

  painter.fillRect(rect(), QBrush(gradient()));

  QPoint cursor = QWidget::mapFromGlobal(QCursor::pos());
  if (rect().contains(cursor)) {
    painter.setPen(Qt::black);
    QLine line(QPoint(cursor.x(), rect().y()), QPoint(cursor.x(), rect().bottom()));
    painter.drawLine(line);
  }

  QList<Stop> stops = _stopHash.values();
  foreach (Stop stop, stops) {
    if (stop.path.contains(cursor)) {
      painter.setPen(Qt::white);
      painter.setBrush(Qt::black);
      painter.drawPath(stop.path);
    } else {
      painter.setPen(Qt::black);
      painter.setBrush(Qt::white);
      painter.drawPath(stop.path);
    }
  }
}


bool GradientEditor::isStopAt(const QPoint &point) const {
  return _stopHash.contains(point.x());
}


QPainterPath GradientEditor::marker(int x) const {
  qreal length = 15.;
  qreal altitude = length * 0.8660254037844386468;

  QLine line(QPoint(x, rect().y()), QPoint(x, rect().bottom()));

  QPainterPath path;

  //top triangle
  QPolygonF top;
  top << line.p1() - QPointF(length/2., 0.)
      << line.p1() + QPointF(length/2., 0.)
      << line.p1() + QPointF(0., altitude);
  path.addPolygon(top);

  path.closeSubpath();

  //bottom triangle
  QPolygonF bot;
  bot << line.p2() - QPointF(length/2., 0.)
      << line.p2() + QPointF(length/2., 0.)
      << line.p2() + QPointF(0., -altitude);
  path.addPolygon(bot);
  path.closeSubpath();

  return path;
}


QGradientStops GradientEditor::gradientStops() const {
  QGradientStops realStops;

  QList<Stop> stops = _stopHash.values();
  foreach (Stop stop, stops) {
    qreal realStop = 1.0 - qreal(stop.pos) / qreal(rect().width());
    realStops << qMakePair(realStop, stop.color);
  }

  return realStops;
}


void GradientEditor::setGradientStops(const QGradientStops &stops) {
  _stopHash.clear();

  foreach (QGradientStop gradientStop, stops) {
    int position = int(-(gradientStop.first - 1.0 / qreal(rect().width())));
    Stop stop;
    stop.pos = position;
    stop.color = gradientStop.second;
    stop.path = marker(position);
    _stopHash.insert(position, stop);
  }
}

}

// vim: ts=2 sw=2 et
