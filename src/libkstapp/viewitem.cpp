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

#include "viewitem.h"
#include "application.h"
#include "tabwidget.h"
#include "viewitemdialog.h"
#include "viewgridlayout.h"

#include "layoutboxitem.h"

#include "gridlayouthelper.h"

#include <math.h>

#include <QMenu>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidgetAction>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>

static const double ONE_PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * ONE_PI;
static double RAD2DEG = 180.0 / ONE_PI;

// #define DEBUG_GEOMETRY
// #define DEBUG_REPARENT
#define INKSCAPE_MODE 0

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent),
    _gripMode(Move),
    _allowedGripModes(Move | Resize | Rotate /*| Scale*/),
    _fixedSize(false),
    _lockAspectRatio(false),
    _lockAspectRatioFixed(false),
    _hasStaticGeometry(false),
    _allowsLayout(true),
    _hovering(false),
    _acceptsChildItems(true),
    _acceptsContextMenuEvents(true),
    _activeGrip(NoGrip),
    _allowedGrips(TopLeftGrip | TopRightGrip | BottomRightGrip | BottomLeftGrip |
                  TopMidGrip | RightMidGrip | BottomMidGrip | LeftMidGrip),
    _parentRelativeHeight(0),
    _parentRelativeWidth(0)
 {

  setZValue(1);
  setName("View Item");
  setAcceptsHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  connect(parent, SIGNAL(mouseModeChanged(View::MouseMode)),
          this, SLOT(viewMouseModeChanged(View::MouseMode)));
  connect(parent, SIGNAL(viewModeChanged(View::ViewMode)),
          this, SLOT(updateView()));
}


ViewItem::~ViewItem() {
}


void ViewItem::save(QXmlStreamWriter &xml) {
  xml.writeAttribute("name", name());
  xml.writeStartElement("position");
  xml.writeAttribute("x", QVariant(pos().x()).toString());
  xml.writeAttribute("y", QVariant(pos().y()).toString());
  xml.writeEndElement();
  xml.writeStartElement("rect");
  xml.writeAttribute("x", QVariant(viewRect().x()).toString());
  xml.writeAttribute("y", QVariant(viewRect().y()).toString());
  xml.writeAttribute("width", QVariant(viewRect().width()).toString());
  xml.writeAttribute("height", QVariant(viewRect().height()).toString());
  xml.writeEndElement();
  xml.writeStartElement("transform");
  xml.writeAttribute("m11", QVariant(transform().m11()).toString());
  xml.writeAttribute("m12", QVariant(transform().m12()).toString());
  xml.writeAttribute("m13", QVariant(transform().m13()).toString());
  xml.writeAttribute("m21", QVariant(transform().m21()).toString());
  xml.writeAttribute("m22", QVariant(transform().m22()).toString());
  xml.writeAttribute("m23", QVariant(transform().m23()).toString());
  xml.writeAttribute("m31", QVariant(transform().m31()).toString());
  xml.writeAttribute("m32", QVariant(transform().m32()).toString());
  xml.writeAttribute("m33", QVariant(transform().m33()).toString());
  xml.writeEndElement();
  xml.writeStartElement("pen");
  xml.writeAttribute("style", QVariant(pen().style()).toString());
  xml.writeAttribute("width", QVariant(pen().widthF()).toString());
  xml.writeAttribute("miterlimit", QVariant(pen().miterLimit()).toString());
  xml.writeAttribute("cap", QVariant(pen().capStyle()).toString());
  xml.writeAttribute("joinStyle", QVariant(pen().joinStyle()).toString());
  xml.writeStartElement("brush");
  xml.writeAttribute("color", pen().brush().color().name());
  xml.writeAttribute("style", QVariant(pen().brush().style()).toString());
  xml.writeEndElement();
  xml.writeEndElement();
  xml.writeStartElement("brush");
  xml.writeAttribute("color", brush().color().name());
  xml.writeAttribute("style", QVariant(brush().style()).toString());
  if (brush().gradient()) {
    QString stopList;
    foreach(QGradientStop stop, brush().gradient()->stops()) {
      qreal point = (qreal)stop.first;
      QColor color = (QColor)stop.second;

      stopList += QString::number(point);
      stopList += ",";
      stopList += color.name();
      stopList += ",";
    }
    xml.writeAttribute("gradient", stopList);
  }
  xml.writeEndElement();
}


bool ViewItem::parse(QXmlStreamReader &xml, bool &validChildTag) {
  bool knownTag = false;
  QString expectedTag;
  if (xml.isStartElement()) {
    expectedTag = xml.name().toString();
    QXmlStreamAttributes attrs = xml.attributes();
    QStringRef av;
    if (xml.name().toString() == "name") {
      knownTag = true;
      av = attrs.value("name");
      if (!av.isNull()) {
        setName(av.toString());
     }
    } else if (xml.name().toString() == "position") {
      knownTag = true;
      double x = 0, y = 0;
      av = attrs.value("x");
      if (!av.isNull()) {
        x = av.toString().toDouble();
      }
      av = attrs.value("y");
      if (!av.isNull()) {
        y = av.toString().toDouble();
     }
     setPos(x, y);
    } else if (xml.name().toString() == "brush") {
      knownTag = true;
      QBrush brush;
      av = attrs.value("gradient");
      if (!av.isNull()) {
        QStringList stopInfo = av.toString().split(',', QString::SkipEmptyParts);
        QLinearGradient gradient(1,0,0,0);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        for (int i = 0; i < stopInfo.size(); i+=2) {
          gradient.setColorAt(stopInfo.at(i).toDouble(), QColor(stopInfo.at(i+1)));
        }
        brush = QBrush(gradient);
      } else {
        av = attrs.value("color");
        if (!av.isNull()) {
            brush.setColor(QColor(av.toString()));
        }
        av = attrs.value("style");
        if (!av.isNull()) {
          brush.setStyle((Qt::BrushStyle)av.toString().toInt());
        }
      }
      setBrush(brush);
    } else if (xml.name().toString() == "pen") {
      knownTag = true;
      QPen pen;
      av = attrs.value("style");
      if (!av.isNull()) {
        pen.setStyle((Qt::PenStyle)av.toString().toInt());
      }
      av = attrs.value("width");
      if (!av.isNull()) {
        pen.setWidthF(av.toString().toDouble());
      }
      av = attrs.value("miterlimit");
      if (!av.isNull()) {
        pen.setMiterLimit(av.toString().toDouble());
      }
      av = attrs.value("cap");
      if (!av.isNull()) {
        pen.setCapStyle((Qt::PenCapStyle)av.toString().toInt());
      }
      av = attrs.value("joinstyle");
      if (!av.isNull()) {
        pen.setJoinStyle((Qt::PenJoinStyle)av.toString().toInt());
      }
      xml.readNext();
      xml.readNext();
      if (xml.isStartElement() && (xml.name().toString() == "brush")) {
        QBrush penBrush;
        attrs = xml.attributes();
        av = attrs.value("color");
        if (!av.isNull()) {
            penBrush.setColor(QColor(av.toString()));
        }
        av = attrs.value("style");
        if (!av.isNull()) {
          penBrush.setStyle((Qt::BrushStyle)av.toString().toInt());
        }
        pen.setBrush(penBrush);
        xml.readNext();
        if (!xml.isEndElement() || (xml.name().toString() != "brush")) {
          expectedTag = "InvalidTag";
        }
        xml.readNext();
      }
      setPen(pen);
    } else if (xml.name().toString() == "rect") {
      knownTag = true;
      double x = 0, y = 0, w = 10, h = 10;
      av = attrs.value("width");
      if (!av.isNull()) {
        w = av.toString().toDouble();
      }
      av = attrs.value("height");
      if (!av.isNull()) {
        h = av.toString().toDouble();
      }
      av = attrs.value("x");
      if (!av.isNull()) {
         x = av.toString().toDouble();
      }
      av = attrs.value("y");
      if (!av.isNull()) {
        y = av.toString().toDouble();
      }
     setViewRect(QRectF(QPointF(x, y), QSizeF(w, h)));
    } else if (xml.name().toString() == "transform") {
      double m11 = 1.0, m12 = 0, m13 = 0, m21 = 0, m22 = 1.0, m23 = 0, m31 = 0, m32= 0, m33 = 1.0;
      av = attrs.value("m11");
      if (!av.isNull()) {
        m11 = av.toString().toDouble();
      }
      av = attrs.value("m12");
      if (!av.isNull()) {
        m12 = av.toString().toDouble();
      }
      av = attrs.value("m13");
      if (!av.isNull()) {
        m13 = av.toString().toDouble();
      }
      av = attrs.value("m21");
      if (!av.isNull()) {
        m21 = av.toString().toDouble();
      }
      av = attrs.value("m22");
      if (!av.isNull()) {
        m22 = av.toString().toDouble();
      }
      av = attrs.value("m23");
      if (!av.isNull()) {
        m23 = av.toString().toDouble();
      }
      av = attrs.value("m31");
      if (!av.isNull()) {
        m31 = av.toString().toDouble();
      }
      av = attrs.value("m32");
      if (!av.isNull()) {
        m32 = av.toString().toDouble();
      }
      av = attrs.value("m33");
      if (!av.isNull()) {
        m33 = av.toString().toDouble();
      }
      setTransform(QTransform(m11, m12, m13, m21, m22, m23, m31, m32, m33));
    }
  }

  xml.readNext();
  if (xml.isEndElement()) {
    if ((xml.name().toString() == expectedTag) ) {
    validChildTag = true;
    }
  }
  return knownTag;
}

View *ViewItem::parentView() const {
  return qobject_cast<View*>(parent());
}


ViewItem *ViewItem::parentViewItem() const {
  return qgraphicsitem_cast<ViewItem*>(parentItem());
}


ViewItem::GripMode ViewItem::gripMode() const {
  return _gripMode;
}


void ViewItem::setGripMode(GripMode mode) {
  _gripMode = mode;
  update();
}


ViewItem::GripModes ViewItem::allowedGripModes() const {
  return _allowedGripModes;
}


void ViewItem::setAllowedGripModes(GripModes modes) {
  _allowedGripModes = modes;
}


bool ViewItem::isAllowed(GripMode mode) const {
  return _allowedGripModes & mode;
}


QRectF ViewItem::viewRect() const {
  return rect();
}


void ViewItem::setViewRect(const QRectF &viewRect, bool automaticChange) {
  QRectF oldViewRect = rect();

  if (oldViewRect == viewRect)
    return;

  setRect(viewRect);
  emit geometryChanged();

  if (!automaticChange) {
    updateRelativeSize();
  }

  foreach (QGraphicsItem *item, QGraphicsItem::children()) {
    if (item->parentItem() != this)
      continue;

    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);

    if (!viewItem)
      continue;

    if (viewItem->hasStaticGeometry())
      continue;


    viewItem->updateChildGeometry(oldViewRect, viewRect);
  }
}


void ViewItem::setViewRect(qreal x, qreal y, qreal width, qreal height, bool automaticChange) {
  setViewRect(QRectF(x, y, width, height), automaticChange);
}


QSizeF ViewItem::sizeOfGrip() const {
  if (!parentView())
    return QSizeF();

  int base = 15;
#if INKSCAPE_MODE
  return mapFromScene(parentView()->mapToScene(QRect(0, 0, base, base)).boundingRect()).boundingRect().size();
#else
  return parentView()->mapToScene(QRect(0, 0, base, base)).boundingRect().size();
#endif
}


QPainterPath ViewItem::topLeftGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::topRightGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::bottomRightGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomRight() - QPointF(sizeOfGrip().width(), sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::bottomLeftGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPointF(0, sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::topMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::rightMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::bottomMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPointF(0, sizeOfGrip().height()), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::leftMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);

#if INKSCAPE_MODE
    return mapFromScene(path);
#else
    return path;
#endif
}


QPainterPath ViewItem::grips() const {
  QPainterPath grips;
  grips.addPath(topLeftGrip());
  grips.addPath(topRightGrip());
  grips.addPath(bottomRightGrip());
  grips.addPath(bottomLeftGrip());
  grips.addPath(topMidGrip());
  grips.addPath(rightMidGrip());
  grips.addPath(bottomMidGrip());
  grips.addPath(leftMidGrip());
  return grips;
}


ViewItem::ActiveGrip ViewItem::activeGrip() const {
  return _activeGrip;
}


void ViewItem::setActiveGrip(ActiveGrip grip) {
  _activeGrip = grip;
}


ViewItem::ActiveGrips ViewItem::allowedGrips() const {
  return _allowedGrips;
}


void ViewItem::setAllowedGrips(ActiveGrips grips) {
  _allowedGrips = grips;
}


bool ViewItem::isAllowed(ActiveGrip grip) const {
  return _allowedGrips & grip;
}


QRectF ViewItem::selectBoundingRect() const {
#if INKSCAPE_MODE
  return mapToScene(itemShape()).controlPointRect();
#else
  return rect();
#endif
}


QRectF ViewItem::gripBoundingRect() const {
  QRectF bound = selectBoundingRect();
  bound.setTopLeft(bound.topLeft() - QPointF(sizeOfGrip().width() / 2.0, sizeOfGrip().height() / 2.0));
  bound.setWidth(bound.width() + sizeOfGrip().width() / 2.0);
  bound.setHeight(bound.height() + sizeOfGrip().height() / 2.0);
  return bound;
}


QRectF ViewItem::boundingRect() const {
  bool inCreation = false;
  if (parentView()) /* false when exiting */
    inCreation = parentView()->mouseMode() == View::Create;
  if (!isSelected() && !isHovering() || inCreation)
    return QGraphicsRectItem::boundingRect();

#if INKSCAPE_MODE
  QPolygonF gripBound = mapFromScene(gripBoundingRect());
#else
  QPolygonF gripBound = gripBoundingRect();
#endif
  return QRectF(gripBound[0], gripBound[2]);
}


QPainterPath ViewItem::shape() const {
  if (!isSelected() && !isHovering() || parentView()->mouseMode() == View::Create)
    return itemShape();

  QPainterPath selectPath;
  selectPath.setFillRule(Qt::WindingFill);

#if INKSCAPE_MODE
    selectPath.addPolygon(mapFromScene(selectBoundingRect()));
#else
    selectPath.addPolygon(rect());
#endif

  selectPath.addPath(grips());
  return selectPath;
}


void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setPen(pen());
  painter->setBrush(brush());
  paint(painter); //this is the overload that subclasses should use...

  painter->save();
  painter->setPen(Qt::DotLine);
  painter->setBrush(Qt::NoBrush);
  if ((isSelected() || isHovering())
      && parentView()->mouseMode() != View::Create
      && parentView()->viewMode() != View::Data) {
    painter->drawPath(shape());
    if (_gripMode == Resize)
      painter->fillPath(grips(), Qt::blue);
    else if (_gripMode == Scale)
      painter->fillPath(grips(), Qt::black);
    else if (_gripMode == Rotate)
      painter->fillPath(grips(), Qt::red);
    else if (_gripMode == Move)
      painter->fillPath(grips(), Qt::transparent);
  }

#ifdef DEBUG_GEOMETRY
//  painter->fillRect(selectBoundingRect(), Qt::blue);
  QColor semiRed(QColor(255, 0, 0, 50));
  painter->fillPath(shape(), semiRed);

  QPen p = painter->pen();

  painter->setPen(Qt::white);
  painter->drawLine(_normalLine);

  painter->setPen(Qt::red);
  painter->drawLine(_rotationLine);
  painter->setPen(p);

  painter->drawText(rect().topLeft(), "TL");
  painter->drawText(rect().topRight(), "TR");
  painter->drawText(rect().bottomLeft(), "BL");
  painter->drawText(rect().bottomRight(), "BR");
#endif
  painter->restore();

//   QGraphicsRectItem::paint(painter, option, widget);
}


void ViewItem::paint(QPainter *painter) {
  Q_UNUSED(painter);
}


void ViewItem::edit() {
  ViewItemDialog editDialog(this);
  editDialog.exec();
}


void ViewItem::createAutoLayout() {
  if (parentViewItem()) {
    LayoutCommand *layout = new LayoutCommand(parentViewItem());
    layout->createLayout();
  } else if (parentView()) {
    parentView()->createLayout();
  }
}


void ViewItem::createCustomLayout() {
  bool ok;
  int columns = QInputDialog::getInteger(tr("Kst"),
                                      tr("Select Number of Columns"),1, 0,
                                      10, 1, &ok);
  if (ok) {
    if (parentViewItem()) {
      LayoutCommand *layout = new LayoutCommand(parentViewItem());
      layout->createLayout(columns);
    } else if (parentView()) {
      parentView()->createLayout(columns);
    }
  }
}


void ViewItem::raise() {
  RaiseCommand *up = new RaiseCommand(this);
  up->redo();
}


void ViewItem::lower() {
  LowerCommand *down = new LowerCommand(this);
  down->redo();
}


void ViewItem::remove() {
  RemoveCommand *remove = new RemoveCommand(this);
  remove->redo();
}


void ViewItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(0.0, 0.0, 0.0, 0.0);
    parentView()->scene()->addItem(this);
    //setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    QRectF newRect(rect().x(), rect().y(),
                   poly.last().x() - rect().x(),
                   poly.last().y() - rect().y());
    setViewRect(newRect);
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    QRectF newRect(rect().x(), rect().y(),
                   poly.last().x() - rect().x(),
                   poly.last().y() - rect().y());

    if (!newRect.isValid()) {
      // Special case for labels that don't need to have a size for creation to ensure proper parenting.
      newRect.setSize(QSize(1, 1));
    }

    setViewRect(newRect.normalized());

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    parentView()->setMouseMode(View::Default);
    maybeReparent();
    emit creationComplete();
    return;
  }
}


void ViewItem::addTitle(QMenu *menu) const {
  QWidgetAction *action = new QWidgetAction(menu);
  action->setEnabled(false);

  QLabel *label = new QLabel(name() + tr(" Menu"), menu);
  label->setAlignment(Qt::AlignCenter);
  label->setStyleSheet("QLabel {"
                       "border-bottom: 2px solid lightGray;"
                       "font: bold large;"
                       "padding: 3px;"
                       "margin: 1px;"
                       "}");
  action->setDefaultWidget(label);
  menu->addAction(action);
}


void ViewItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;

  addTitle(&menu);

  QAction *editAction = menu.addAction(tr("Edit"));
  connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

  QAction *raiseAction = menu.addAction(tr("Raise"));
  connect(raiseAction, SIGNAL(triggered()), this, SLOT(raise()));

  QAction *lowerAction = menu.addAction(tr("Lower"));
  connect(lowerAction, SIGNAL(triggered()), this, SLOT(lower()));

  QMenu layoutMenu;
  layoutMenu.setTitle(tr("Cleanup Layout"));

  QAction *autoLayoutAction = layoutMenu.addAction(tr("Automatic"));
  connect(autoLayoutAction, SIGNAL(triggered()), this, SLOT(createAutoLayout()));

  QAction *customLayoutAction = layoutMenu.addAction(tr("Custom"));
  connect(customLayoutAction, SIGNAL(triggered()), this, SLOT(createCustomLayout()));

  menu.addMenu(&layoutMenu);

  QAction *removeAction = menu.addAction(tr("Remove"));
  connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

  addToMenuForContextEvent(menu);

  menu.exec(event->screenPos());
}


void ViewItem::addToMenuForContextEvent(QMenu &menu) {
  Q_UNUSED(menu);
}


void ViewItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  if (parentView()->mouseMode() == View::Default) {
    if (gripMode() == ViewItem::Move || activeGrip() == NoGrip) {
      parentView()->setMouseMode(View::Move);
      parentView()->undoStack()->beginMacro(tr("Move"));
    } else if (gripMode() == ViewItem::Resize) {
      parentView()->setMouseMode(View::Resize);
      parentView()->undoStack()->beginMacro(tr("Resize"));
    } else if (gripMode() == ViewItem::Scale) {
      parentView()->setMouseMode(View::Scale);
      parentView()->undoStack()->beginMacro(tr("Scale"));
    } else if (gripMode() == ViewItem::Rotate) {
      parentView()->setMouseMode(View::Rotate);
      parentView()->undoStack()->beginMacro(tr("Rotate"));
    }
  }

  if (activeGrip() == NoGrip)
    return QGraphicsRectItem::mouseMoveEvent(event);

  QPointF p = event->pos();
  QPointF l = event->lastPos();
  QPointF s = event->scenePos();

  if (gripMode() == ViewItem::Rotate) {

#if INKSCAPE_MODE
    rotateTowards(l, p);
#else
    switch(_activeGrip) {
    case TopLeftGrip:
        rotateTowards(topLeftGrip().controlPointRect().center(), p); break;
    case TopRightGrip:
        rotateTowards(topRightGrip().controlPointRect().center(), p); break;
    case BottomRightGrip:
        rotateTowards(bottomRightGrip().controlPointRect().center(), p); break;
    case BottomLeftGrip:
        rotateTowards(bottomLeftGrip().controlPointRect().center(), p); break;
    case TopMidGrip:
        rotateTowards(topMidGrip().controlPointRect().center(), p); break;
    case RightMidGrip:
        rotateTowards(rightMidGrip().controlPointRect().center(), p); break;
    case BottomMidGrip:
        rotateTowards(bottomMidGrip().controlPointRect().center(), p); break;
    case LeftMidGrip:
        rotateTowards(leftMidGrip().controlPointRect().center(), p); break;
    case NoGrip:
      break;
    }
#endif

  } else if (gripMode() == ViewItem::Resize) {

    switch(_activeGrip) {
    case TopLeftGrip:
        resizeTopLeft(p - l); break;
    case TopRightGrip:
        resizeTopRight(p - l); break;
    case BottomRightGrip:
        resizeBottomRight(p - l); break;
    case BottomLeftGrip:
        resizeBottomLeft(p - l); break;
    case TopMidGrip:
        resizeTop(p.y() - l.y()); break;
    case RightMidGrip:
        resizeRight(p.x() - l.x()); break;
    case BottomMidGrip:
        resizeBottom(p.y() - l.y()); break;
    case LeftMidGrip:
        resizeLeft(p.x() - l.x()); break;
    case NoGrip:
      break;
    }

  } else if (gripMode() == ViewItem::Scale) {

    switch(_activeGrip) {
    case TopLeftGrip:
        setTopLeft(s); break;
    case TopRightGrip:
        setTopRight(s); break;
    case BottomRightGrip:
        setBottomRight(s); break;
    case BottomLeftGrip:
        setBottomLeft(s); break;
    case TopMidGrip:
        setTop(s.y()); break;
    case RightMidGrip:
        setRight(s.x()); break;
    case BottomMidGrip:
        setBottom(s.y()); break;
    case LeftMidGrip:
        setLeft(s.x()); break;
    case NoGrip:
      break;
    }
  }
  updateRelativeSize();
}


void ViewItem::resizeTopLeft(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, false) : offset;
  r.setTopLeft(r.topLeft() + o);
  if (!r.isValid()) return;

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeTopRight(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, true) : offset;
  r.setTopRight(r.topRight() + o);
  if (!r.isValid()) return;

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeBottomLeft(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, true) : offset;
  r.setBottomLeft(r.bottomLeft() + o);
  if (!r.isValid()) return;

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeBottomRight(const QPointF &offset) {
  const qreal oldAspect = rect().width() / rect().height();

  QRectF r = rect();
  QPointF o = _lockAspectRatio ? lockOffset(offset, oldAspect, false) : offset;
  r.setBottomRight(r.bottomRight() + o);
  if (!r.isValid()) return;

  const qreal newAspect = r.width() / r.height();
  Q_ASSERT_X(_lockAspectRatio ? qFuzzyCompare(newAspect, oldAspect) : true,
              "lockAspect error", QString::number(newAspect) + "!=" + QString::number(oldAspect));
  setViewRect(r);
}


void ViewItem::resizeTop(qreal offset) {
  QRectF r = rect();
  r.setTop(r.top() + offset);
  if (!r.isValid()) return;
  setViewRect(r);
}


void ViewItem::resizeBottom(qreal offset) {
  QRectF r = rect();
  r.setBottom(r.bottom() + offset);
  if (!r.isValid()) return;
  setViewRect(r);
}


void ViewItem::resizeLeft(qreal offset) {
  QRectF r = rect();
  r.setLeft(r.left() + offset);
  if (!r.isValid()) return;
  setViewRect(r);
}


void ViewItem::resizeRight(qreal offset) {
  QRectF r = rect();
  r.setRight(r.right() + offset);
  if (!r.isValid()) return;
  setViewRect(r);
}


void ViewItem::setTopLeft(const QPointF &point) {
//   qDebug() << "setTopLeft" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().bottomRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTopLeft(p);
  from.moveBottomRight(anchor);
  to.moveBottomRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setTopRight(const QPointF &point) {
//   qDebug() << "setTopRight" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTopRight(p);
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottomLeft(const QPointF &point) {
//   qDebug() << "setBottomLeft" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottomLeft(p);
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottomRight(const QPointF &point) {
//   qDebug() << "setBottomRight" << point << endl;
  QPointF p = point;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottomRight(p);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setTop(qreal y) {
//   qDebug() << "setTop" << x << endl;

  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTop(y);
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottom(qreal y) {
//   qDebug() << "setBottom" << x << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottom(y);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setLeft(qreal x) {
//   qDebug() << "setLeft" << x << endl;

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setLeft(x);
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setRight(qreal x) {
//   qDebug() << "setRight" << x << endl;

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setRight(x);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


QTransform ViewItem::selectTransform() const {

  /* Converts a point on the rect() to a point on the selectBoundingRect()
     or the inverse by using selectTransform().inverted()...
  */

  QRectF from = rect();
  QRectF to = selectBoundingRect();
  QTransform rt = _rotationTransform.inverted(); //inverse rotation so far

  QPolygonF from_ = QPolygonF(rt.map(from));
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(mapFromScene(to));
  to_.pop_back(); //get rid of last closed point

  QTransform select;
  QTransform::quadToQuad(from_, to_, select);

  return _rotationTransform.inverted() * select * transform();
}


bool ViewItem::transformToRect(const QRectF &from, const QRectF &to) {
  //Not sure how to handle yet
  if (!to.isValid()) {
    return false;
  }

//   qDebug() << "Mapping from " << from << "to" << to << endl;

  QPolygonF from_(from);
  from_.pop_back(); //get rid of last closed point
  QPolygonF to_(to);
  to_.pop_back(); //get rid of last closed point
  return transformToRect(from_, to_);
}


bool ViewItem::transformToRect(const QPolygonF &from, const QPolygonF &to) {

  QTransform t;
  bool success = QTransform::quadToQuad(from, to, t);

  t = transform() * t;

  if (success) setTransform(t, false);
  return success;
}


void ViewItem::rotateTowards(const QPointF &corner, const QPointF &point) {

  QPointF origin = centerOfRotation();
  if (origin == corner || origin == point)
    return;

  _normalLine = QLineF(origin, corner);
  _rotationLine = QLineF(origin, point);

  qreal angle1 = ::acos(_normalLine.dx() / _normalLine.length());
  if (_normalLine.dy() >= 0)
      angle1 = TWO_PI - angle1;

  qreal angle2 = ::acos(_rotationLine.dx() / _rotationLine.length());
  if (_rotationLine.dy() >= 0)
      angle2 = TWO_PI - angle2;

  qreal angle = RAD2DEG * (angle1 - angle2);

  QTransform t;
  t.translate(origin.x(), origin.y());
  t.rotate(angle);
  t.translate(-origin.x(), -origin.y());

  _rotationTransform = t * _rotationTransform;

  setTransform(t, true);
}


QPointF ViewItem::lockOffset(const QPointF &offset, qreal ratio, bool oddCorner) const {
  qreal x;
  qreal y;
  bool xKey;

  if (offset.x() < 0 && offset.y() > 0) {
    xKey = true;
    x = offset.x();
    y = x == 0 ? 0 : (1 / ratio) * x;
  } else if (offset.y() < 0 && offset.x() > 0) {
    xKey = false;
    y = offset.y();
    x = y == 0 ? 0 : ratio * y;
  } else if (qAbs(offset.x()) < qAbs(offset.y())) {
    xKey = true;
    x = offset.x();
    y = x == 0 ? 0 : (1 / ratio) * x;
  } else {
    xKey = false;
    y = offset.y();
    x = y == 0 ? 0 : ratio * y;
  }

  QPointF o = offset;
  if (oddCorner) {
    o = QPointF(!xKey ? -x : x,
                xKey ? -y : y);
  } else {
    o = QPointF(x, y);
  }

//   qDebug() << "lockOffset"
//             << "ratio:" << ratio
//             << "offset:" << offset
//             << "o:" << o
//             << endl;

  return o;
}


bool ViewItem::maybeReparent() {
  //First get a list of all items that collide with this one
  QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemShape);

  bool topLevel = !parentItem();
  QPointF origin = mapToScene(QPointF(0,0));

#ifdef DEBUG_REPARENT
  qDebug() << "maybeReparent" << this
           << "topLevel:" << (topLevel ? "true" : "false")
           << "origin:" << origin
           << "rect:" << rect()
           << "collision count:" << collisions.count()
           << endl;
#endif

  //Doesn't collide then reparent to top-level
  if (collisions.isEmpty() && !topLevel) {
#ifdef DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#ifdef DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParent(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#ifdef DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  //Look for collisions that completely contain us
  foreach (QGraphicsItem *item, collisions) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);

    if (!viewItem || !viewItem->acceptsChildItems()) /*bah*/
      continue;

    if (!viewItem->collidesWithItem(this, Qt::ContainsItemShape)) /*doesn't contain*/
      continue;

    if (parentItem() == viewItem) { /*already done*/
#ifdef DEBUG_REPARENT
      qDebug() << "already in containing parent" << endl;
#endif
      return false;
    }

#ifdef DEBUG_REPARENT
    qDebug() << "reparent to" << viewItem << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    if (!topLevel) /*bring the old parent's transform with us*/
      setTransform(parentItem()->transform(), true);

    /*cancel out the new parent's initial transform*/
    setTransform(viewItem->transform().inverted(), true);

#ifdef DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParent(viewItem);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#ifdef DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  //No suitable collisions then reparent to top-level
  if (!topLevel) {
#ifdef DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#ifdef DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParent(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#ifdef DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }
  return false;
}


void ViewItem::setParent(ViewItem* parent) {
  setParentItem(parent);
}


void ViewItem::updateRelativeSize() {
  if (parentViewItem()) {
    _parentRelativeHeight = (height() / parentViewItem()->height());
    _parentRelativeWidth = (width() / parentViewItem()->width());
    _parentRelativeCenter =  mapToParent(rect().center()) - parentViewItem()->rect().topLeft();
    _parentRelativeCenter =  QPointF(_parentRelativeCenter.x() / parentViewItem()->width(), _parentRelativeCenter.y() / parentViewItem()->height());
  } else if (parentView()) {
    _parentRelativeHeight = (height() / parentView()->height());
    _parentRelativeWidth = (width() / parentView()->width());
    _parentRelativeCenter =  mapToParent(rect().center()) - parentView()->rect().topLeft();
    _parentRelativeCenter =  QPointF(_parentRelativeCenter.x() / parentView()->width(), _parentRelativeCenter.y() / parentView()->height());
  } else {
    _parentRelativeHeight = 0;
    _parentRelativeWidth = 0;
    _parentRelativeCenter = QPointF(0, 0);
  }
}


void ViewItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
//   qDebug() << "ViewItem::updateChildGeometry" << oldParentRect << newParentRect << endl;

  QRectF itemRect = rect();

  //Lock aspect ratio for rotating objects or children with a lockedAspectRatio
  //FIXME is the child rotated with respect to the parent is the real question...
  if (transform().isRotating() || lockAspectRatio()) {

//     qDebug() << "ViewItem::updateChildGeometry" << mapToParent(rect().center()) << _parentRelativeCenter;

    if (!_fixedSize) {
      qreal newHeight = relativeHeight() * newParentRect.height();
      qreal newWidth = relativeWidth() * newParentRect.width();

      qreal aspectRatio = rect().width() / rect().height();
      if ((newWidth / newHeight) > aspectRatio) {
        // newWidth is too large.  Use newHeight as key.
        newWidth = newHeight * aspectRatio;
      } else {
        // newHeight is either too large, or perfect.  use newWidth as key.
        newHeight = newWidth / aspectRatio;
      }
      itemRect.setBottom(itemRect.top() + newHeight);
      itemRect.setRight(itemRect.left() + newWidth);
    }
    QPointF newCenter = newParentRect.topLeft() + QPointF(newParentRect.width() * _parentRelativeCenter.x(), newParentRect.height() * _parentRelativeCenter.y());

    QRectF r = itemRect;
    r.moveCenter(mapFromParent(newCenter));

    QPointF centerOffset = mapToParent(r.topLeft()) - mapToParent(itemRect.topLeft());
    setPos(pos() + centerOffset);
  } else {

    qreal dx = oldParentRect.width() ? newParentRect.width() / oldParentRect.width() : 0.0;
    qreal dy = oldParentRect.height() ? newParentRect.height() / oldParentRect.height() : 0.0;

    bool topChanged = oldParentRect.top() != newParentRect.top();
    bool leftChanged = oldParentRect.left() != newParentRect.left();
    bool bottomChanged = oldParentRect.bottom() != newParentRect.bottom();
    bool rightChanged = oldParentRect.right() != newParentRect.right();

    qreal width = itemRect.width() * dx;
    qreal height = itemRect.height() * dy;

    itemRect.setBottom(itemRect.top() + height);
    itemRect.setRight(itemRect.left() + width);

    if (topChanged) {
      QPointF offset = oldParentRect.bottomRight() - mapToParent(rect().bottomRight());

      qreal xOff = offset.x() * dx;
      qreal yOff = offset.y() * dy;

      QPointF newBottomRight = oldParentRect.bottomRight() - QPointF(xOff, yOff);

      QPointF o = pos() - mapToParent(itemRect.topLeft());

      QRectF r = itemRect;
      r.moveBottom(mapFromParent(newBottomRight).y());

      setPos(mapToParent(r.topLeft()) + o);
    }

    if (leftChanged) {
      QPointF offset = oldParentRect.bottomRight() - mapToParent(rect().bottomRight());

      qreal xOff = offset.x() * dx;
      qreal yOff = offset.y() * dy;

      QPointF newBottomRight = oldParentRect.bottomRight() - QPointF(xOff, yOff);

      QPointF o = pos() - mapToParent(itemRect.topLeft());

      QRectF r = itemRect;
      r.moveRight(mapFromParent(newBottomRight).x());

      setPos(mapToParent(r.topLeft()) + o);
    }

    if (bottomChanged) {

      QPointF offset = mapToParent(rect().topLeft()) - oldParentRect.topLeft();

      qreal xOff = offset.x() * dx;
      qreal yOff = offset.y() * dy;

      QPointF newTopLeft = oldParentRect.topLeft() + QPointF(xOff, yOff);

      QPointF o = pos() - mapToParent(itemRect.topLeft());

      QRectF r = itemRect;
      r.moveTop(mapFromParent(newTopLeft).y());

      setPos(mapToParent(r.topLeft()) + o);
    }

    if (rightChanged) {

      QPointF offset = mapToParent(rect().topLeft()) - oldParentRect.topLeft();

      qreal xOff = offset.x() * dx;
      qreal yOff = offset.y() * dy;

      QPointF newTopLeft = oldParentRect.topLeft() + QPointF(xOff, yOff);

      QPointF o = pos() - mapToParent(itemRect.topLeft());

      QRectF r = itemRect;
      r.moveLeft(mapFromParent(newTopLeft).x());

      setPos(mapToParent(r.topLeft()) + o);
    }
  }

//   qDebug() << "resize"
//             << "\nbefore:" << rect()
//             << "\nafter:" << rect
//             << "\nwidth:" << width
//             << "\nheight:" << height
//             << endl;
  setViewRect(itemRect, true);
}


void ViewItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  if (parentView()->viewMode() == View::Data) {
    event->ignore();
    return;
  }
}


void ViewItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  QPointF p = event->pos();
  if (isAllowed(TopLeftGrip) && topLeftGrip().contains(p)) {
    setActiveGrip(TopLeftGrip);
  } else if (isAllowed(TopRightGrip) && topRightGrip().contains(p)) {
    setActiveGrip(TopRightGrip);
  } else if (isAllowed(BottomRightGrip) && bottomRightGrip().contains(p)) {
    setActiveGrip(BottomRightGrip);
  } else if (isAllowed(BottomLeftGrip) && bottomLeftGrip().contains(p)) {
    setActiveGrip(BottomLeftGrip);
  } else if (isAllowed(TopMidGrip) && topMidGrip().contains(p)) {
    setActiveGrip(TopMidGrip);
  } else if (isAllowed(RightMidGrip) && rightMidGrip().contains(p)) {
    setActiveGrip(RightMidGrip);
  } else if (isAllowed(BottomMidGrip) && bottomMidGrip().contains(p)) {
    setActiveGrip(BottomMidGrip);
  } else if (isAllowed(LeftMidGrip) && leftMidGrip().contains(p)) {
    setActiveGrip(LeftMidGrip);
  } else {
    setActiveGrip(NoGrip);
  }

  if (!grips().contains(event->pos()) && event->button() & Qt::LeftButton) {
    setGripMode(nextGripMode(_gripMode));
  }

  QGraphicsRectItem::mousePressEvent(event);
}


ViewItem::GripMode ViewItem::nextGripMode(GripMode currentMode) const {
  if (!(_allowedGripModes & (Resize | Rotate | Scale)))
    return currentMode;

  switch (currentMode) {
  case Move:
    if (isAllowed(Resize))
      return Resize;
    else
      return nextGripMode(Resize);
    break;
  case Resize:
    if (isAllowed(Scale))
      return Scale;
    else
      return nextGripMode(Scale);
    break;
  case Scale:
    if (isAllowed(Rotate))
      return Rotate;
    else
      return nextGripMode(Rotate);
    break;
  case Rotate:
    if (isAllowed(Resize))
      return Resize;
    else
      return nextGripMode(Resize);
    break;
  default:
    break;
  }

  return currentMode;
}


void ViewItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  if (parentView()->mouseMode() != View::Default) {
    parentView()->setMouseMode(View::Default);
    parentView()->undoStack()->endMacro();
  }

  QGraphicsRectItem::mouseReleaseEvent(event);
}


void ViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  if (isSelected()) {
    QPointF p = event->pos();
    if (isAllowed(TopLeftGrip) && topLeftGrip().contains(p) || isAllowed(BottomRightGrip) && bottomRightGrip().contains(p)) {
      if (gripMode() == ViewItem::Rotate) {
        parentView()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        parentView()->setCursor(Qt::SizeFDiagCursor);
      }
    } else if (isAllowed(TopRightGrip) && topRightGrip().contains(p) || isAllowed(BottomLeftGrip) && bottomLeftGrip().contains(p)) {
      if (gripMode() == ViewItem::Rotate) {
        parentView()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        parentView()->setCursor(Qt::SizeBDiagCursor);
      }
    } else if (isAllowed(TopMidGrip) && topMidGrip().contains(p) || isAllowed(BottomMidGrip) && bottomMidGrip().contains(p)) {
      if (gripMode() == ViewItem::Rotate) {
        parentView()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        parentView()->setCursor(Qt::SizeVerCursor);
      }
    } else if (isAllowed(RightMidGrip) && rightMidGrip().contains(p) || isAllowed(LeftMidGrip) && leftMidGrip().contains(p)) {
      if (gripMode() == ViewItem::Rotate) {
        parentView()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        parentView()->setCursor(Qt::SizeHorCursor);
      }
    } else {
      parentView()->setCursor(Qt::SizeAllCursor);
    }
  } else {
    parentView()->setCursor(Qt::SizeAllCursor);
  }
}


void ViewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  _hovering = true;
  update();
}


void ViewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  parentView()->setCursor(Qt::ArrowCursor);

  _hovering = false;
  update();
}


QVariant ViewItem::itemChange(GraphicsItemChange change, const QVariant &value) {

if (change == ItemSelectedChange) {
    bool selected = value.toBool();
    if (!selected) {
      setGripMode(ViewItem::Move);
      update();
    }
  }

  return QGraphicsItem::itemChange(change, value);
}


void ViewItem::viewMouseModeChanged(View::MouseMode oldMode) {
  if (parentView()->mouseMode() == View::Move) {
    _originalPosition = pos();
  } else if (parentView()->mouseMode() == View::Resize ||
             parentView()->mouseMode() == View::Scale ||
             parentView()->mouseMode() == View::Rotate) {
    _originalRect = rect();
    _originalTransform = transform();
  } else if (oldMode == View::Move && _originalPosition != pos()) {
    setPos(parentView()->snapPoint(pos()));
    new MoveCommand(this, _originalPosition, pos());

    maybeReparent();
    ViewGridLayout::resetSharedPlots(this);
  } else if (oldMode == View::Resize && _originalRect != rect()) {
    new ResizeCommand(this, _originalRect, rect());

    maybeReparent();
    ViewGridLayout::resetSharedPlots(this);
  } else if (oldMode == View::Scale && _originalTransform != transform()) {
    new ScaleCommand(this, _originalTransform, transform());

    maybeReparent();
    ViewGridLayout::resetSharedPlots(this);
  } else if (oldMode == View::Rotate && _originalTransform != transform()) {
    new RotateCommand(this, _originalTransform, transform());

    maybeReparent();
    ViewGridLayout::resetSharedPlots(this);
  }
}


void ViewItem::registerShortcut(QAction *action) {
  Q_ASSERT(action->parent() == this);
  parentView()->grabShortcut(action->shortcut());
  _shortcutMap.insert(action->shortcut(), action);
}


bool ViewItem::tryShortcut(const QString &shortcut) {
  if (!_shortcutMap.contains(shortcut))
    return false;

  QAction *action = _shortcutMap.value(shortcut);
  if (!action->isEnabled())
    return false;

  action->trigger();
  return true;
}


void ViewItem::updateView() {
  update();
}


#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, ViewItem *viewItem) {
    dbg.nospace() << viewItem->name();
    return dbg.space();
}
#endif


ViewItemCommand::ViewItemCommand(ViewItem *item, const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(item) {
  if (addToStack)
    _item->parentView()->undoStack()->push(this);
}


ViewItemCommand::~ViewItemCommand() {
}


CreateCommand::CreateCommand(const QString &text, QUndoCommand *parent)
    : ViewCommand(text, false, parent) {
}


CreateCommand::CreateCommand(View *view, const QString &text, QUndoCommand *parent)
    : ViewCommand(view, text, false, parent) {
}


CreateCommand::~CreateCommand() {
}


void CreateCommand::undo() {
  Q_ASSERT(_item);
  _item->hide();
}


void CreateCommand::redo() {
  Q_ASSERT(_item);
  _item->show();
}


void CreateCommand::createItem() {
  Q_ASSERT(_item);
  Q_ASSERT(_view);

  _view->setMouseMode(View::Create);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(_view, SIGNAL(mouseModeChanged(View::MouseMode)), _item, SLOT(deleteLater()));
  connect(_view, SIGNAL(creationPolygonChanged(View::CreationEvent)),
          _item, SLOT(creationPolygonChanged(View::CreationEvent)));
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));
  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}


void CreateCommand::creationComplete() {
  _view->undoStack()->push(this);
}


void LayoutCommand::undo() {
  Q_ASSERT(_layout);
  _layout->reset();
}


void LayoutCommand::redo() {
  Q_ASSERT(_layout);
  _layout->apply();
}


void LayoutCommand::createLayout(int columns) {
  Q_ASSERT(_item);
  Q_ASSERT(_item->parentView());

  QList<ViewItem*> viewItems;
  QList<QGraphicsItem*> list = _item->QGraphicsItem::children();
  if (list.isEmpty())
    return; //not added to undostack

  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem || viewItem->hasStaticGeometry() || !viewItem->allowsLayout() || viewItem->parentItem() != _item)
      continue;
    viewItems.append(viewItem);
  }

  if (viewItems.isEmpty())
    return; //not added to undostack

  Grid *grid = Grid::buildGrid(viewItems, columns);
  Q_ASSERT(grid);

  _layout = new ViewGridLayout(_item);

  foreach (ViewItem *v, viewItems) {
    int r = 0, c = 0, rs = 0, cs = 0;
    if (grid->locateWidget(v, r, c, rs, cs)) {
      _layout->addViewItem(v, r, c, rs, cs);
    } else {
      grid->appendItem(v);
      if (grid->locateWidget(v, r, c, rs, cs)) {
        _layout->addViewItem(v, r, c, rs, cs);
      } else {
        qDebug() << "ooops, viewItem does not fit in layout" << endl;
      }
    }
  }

  if (qobject_cast<LayoutBoxItem*>(_item)) {
    _layout->setMargin((_item->sizeOfGrip() / 2.0));
    _layout->setSpacing((_item->sizeOfGrip() / 2.0));
    QObject::connect(_layout, SIGNAL(enabledChanged(bool)),
                     _item, SLOT(setEnabled(bool)));
  }

  _layout->apply();
  _item->parentView()->undoStack()->push(this);
}


void AppendLayoutCommand::undo() {
  Q_ASSERT(_layout);
  _layout->reset();
}


void AppendLayoutCommand::redo() {
  Q_ASSERT(_layout);
  _layout->apply();
}


void AppendLayoutCommand::appendLayout(CurvePlacement::Layout layout, ViewItem* item, int columns) {
  Q_ASSERT(_item);
  Q_ASSERT(_item->parentView());
  Q_ASSERT(item);

  _layout = new ViewGridLayout(_item);

  QPointF center = _item->parentView()->sceneRect().center();
  center -= QPointF(100.0, 100.0);

  item->setPos(center);
  item->setViewRect(0.0, 0.0, 200.0, 200.0);
  //_item->setZValue(1);
  _item->parentView()->scene()->addItem(item);

  if (layout == CurvePlacement::Auto) {
    columns = 0;
  }

  if (layout != CurvePlacement::Protect) {
    QList<ViewItem*> viewItems;
    QList<QGraphicsItem*> list = _item->QGraphicsItem::children();

    foreach (QGraphicsItem *graphicsItem, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(graphicsItem);
      if (!viewItem || viewItem->hasStaticGeometry() || !viewItem->allowsLayout() || viewItem->parentItem() != _item || viewItem == item)
        continue;
      viewItems.append(viewItem);
    }

    bool appendRequired = true;
    if (viewItems.isEmpty()) {
      viewItems.append(item);
      appendRequired = false;
    }

    Grid *grid = Grid::buildGrid(viewItems, columns);
    Q_ASSERT(grid);
    grid->appendItem(item);

    if (appendRequired) {
      viewItems.append(item);
    }

    _layout = new ViewGridLayout(_item);

    foreach (ViewItem *v, viewItems) {
      int r = 0, c = 0, rs = 0, cs = 0;
      if (grid->locateWidget(v, r, c, rs, cs)) {
        if (rs * cs == 1) {
          _layout->addViewItem(v, r, c, 1, 1);
        } else {
          _layout->addViewItem(v, r, c, rs, cs);
        }
      } else {
        qDebug() << "ooops, viewItem does not fit in layout" << endl;
      }
    }

    if (qobject_cast<LayoutBoxItem*>(_item)) {
      _layout->setMargin((_item->sizeOfGrip() / 2.0));
      _layout->setSpacing((_item->sizeOfGrip() / 2.0));
      QObject::connect(_layout, SIGNAL(enabledChanged(bool)),
                      _item, SLOT(setEnabled(bool)));
    }

    _layout->apply();
  }
  _item->parentView()->undoStack()->push(this);
}


void BreakLayoutCommand::undo() {
  Q_ASSERT(_layout);
  _layout->apply();
}


void BreakLayoutCommand::redo() {
  Q_ASSERT(_layout);
  _layout->resetSharedAxis();
}


void MoveCommand::undo() {
  Q_ASSERT(_item);
  _item->setPos(_originalPos);
}


void MoveCommand::redo() {
  Q_ASSERT(_item);
  _item->setPos(_newPos);
}


void ResizeCommand::undo() {
  Q_ASSERT(_item);
  _item->setViewRect(_originalRect);
}


void ResizeCommand::redo() {
  Q_ASSERT(_item);
  _item->setViewRect(_newRect);
}


void RemoveCommand::undo() {
  Q_ASSERT(_item);
  _item->show();
}


void RemoveCommand::redo() {
  Q_ASSERT(_item);
  _item->hide();
}


void RaiseCommand::undo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() - 1);
}


void RaiseCommand::redo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() + 1);
}


void LowerCommand::undo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() + 1);
}


void LowerCommand::redo() {
  Q_ASSERT(_item);
  _item->setZValue(_item->zValue() - 1);
}


void TransformCommand::undo() {
  Q_ASSERT(_item);
  _item->setTransform(_originalTransform);
}


void TransformCommand::redo() {
  Q_ASSERT(_item);
  _item->setTransform(_newTransform);
}

}

// vim: ts=2 sw=2 et
