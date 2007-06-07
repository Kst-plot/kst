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

#include "lineitem.h"
#include "view.h"
#include <kstdebug.h>

#include <QDebug>
#include <QGraphicsScene>

namespace Kst {

LineItem::LineItem(View *parent)
  : ViewItem(parent) {
}


LineItem::~LineItem() {
}


void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  painter->drawLine(_line);
  QPen p = pen();
  setPen(Qt::NoPen);
  QGraphicsRectItem::paint(painter, option, widget);
  setPen(p);
}

QLineF LineItem::line() const {
  return _line;
}

void LineItem::setLine(const QLineF &line) {
  _line = line;
  setRect(QRectF(_line.p1(), _line.p2()));
}


void LineItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setLine(QLineF(poly[0], poly[0])); //start and end
    parentView()->scene()->addItem(this);
    setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    setLine(QLineF(line().p1(), poly.last())); //start and end
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    setLine(QLineF(line().p1(), poly.last())); //start and end

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    parentView()->setMouseMode(View::Default);
    emit creationComplete();
    return;
  }
}

void CreateLineCommand::createItem() {
  _item = new LineItem(_view);
  _view->setMouseMode(View::Create);
  _view->setCursor(Qt::CrossCursor);
  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(_view, SIGNAL(mouseModeChanged()), _item, SLOT(deleteLater()));
  connect(_view, SIGNAL(creationPolygonChanged(View::CreationEvent)),
          _item, SLOT(creationPolygonChanged(View::CreationEvent)));
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));
  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}

LineItemFactory::LineItemFactory()
: GraphicsFactory() {
  registerFactory("line", this);
}


LineItemFactory::~LineItemFactory() {
}


ViewItem* LineItemFactory::generateGraphics(QXmlStreamReader& xml, View *view, ViewItem *parent) {
  LineItem *rc = 0;
  double x1 = 0., y1 = 0., x2 = 10., y2 = 10.;
  while (!xml.atEnd()) {
    if (xml.isStartElement()) {
      if (xml.name().toString() == "line") {
        Q_ASSERT(!rc);
        rc = new LineItem(view);
        if (parent) {
          rc->setParentItem(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("thickness");
        if (!av.isNull()) {
          QPen p = rc->pen();
          p.setWidthF(av.toString().toDouble());
          rc->setPen(p);
        }
        av = attrs.value("color");
        if (!av.isNull()) {
          QPen p = rc->pen();
          p.setColor(QColor(av.toString()));
          rc->setPen(p);
        }
        av = attrs.value("x1");
        if (!av.isNull()) {
          x1 = av.toString().toDouble();
        }
        av = attrs.value("y1");
        if (!av.isNull()) {
          y1 = av.toString().toDouble();
        }
        av = attrs.value("x2");
        if (!av.isNull()) {
          x2 = av.toString().toDouble();
        }
        av = attrs.value("y2");
        if (!av.isNull()) {
          y2 = av.toString().toDouble();
        }
      } else {
        Q_ASSERT(rc);
        ViewItem *i = GraphicsFactory::parse(xml, view, rc);
        if (!i) {
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "line") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating line object from Kst file."), KstDebug::Warning);
        delete rc;
        return 0;
      }
    }
    xml.readNext();
  }

  rc->setLine(QLineF(QPointF(x1, y1), QPointF(x2, y2)));
  return rc;
}

}

#include "lineitem.moc"

// vim: ts=2 sw=2 et
