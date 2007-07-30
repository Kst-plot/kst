/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 Theodore Kisner <tsk@humanityforward.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "render2dcartesian.h"

#include <QDebug>

namespace Kst {

Render2DCartesian::Render2DCartesian(const QString &name)
  : PlotRenderer2D(name) {
  setType(Cartesian);
}

Render2DCartesian::~Render2DCartesian() {
}

QList<QPainterPath> Render2DCartesian::projectedPaths() {
    qDebug() << "FIXME!! PUBLISH THE PLOT PATHS!" << endl;
    return QList<QPainterPath>();
}


QPointF Render2DCartesian::mapToProjection(const QPointF &point) {
    qDebug() << "FIXME!! DO SOMETHING WITH" << point << endl;
    return QPointF();
}


QPointF Render2DCartesian::mapFromProjection(const QPointF &point) {
    qDebug() << "FIXME!! DO SOMETHING WITH" << point << endl;
    return QPointF();
}

}

// vim: ts=2 sw=2 et
