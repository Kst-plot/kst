/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "viewitem.h"
#include "applicationsettings.h"
#include "viewitemdialog.h"
#include "viewgridlayout.h"
#include "plotitem.h"
#include "plotitemmanager.h"
#include "document.h"
#include "datacollection.h"

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
#include <QDrag>
#include <QMimeData>

static const double ONE_PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * ONE_PI;
static double RAD2DEG = 180.0 / ONE_PI;
static const int DRAWING_ZORDER = 500;

#define DEBUG_GEOMETRY 0
#define DEBUG_REPARENT 0
#define DEBUG_CHILD_GEOMETRY 0


// enable drag & drop
#define KST_ENABLE_DD

namespace Kst {

ViewItem::ViewItem(View *parentView) :
    QObject(parentView),
    NamedObject(),
    _isXTiedZoom(false),
    _isYTiedZoom(false),
    _plotMaximized(false),
    _gripMode(Move),
    _allowedGripModes(Move | Resize | Rotate /*| Scale*/),
    _creationState(None),
    _typeName("View Item"),
    _zoomOnlyMode(View::ZoomOnlyDisabled),
    _supportsTiedZoom(false),
    _fixedSize(false),
    _lockAspectRatio(false),
    _lockAspectRatioFixed(false),
    _hasStaticGeometry(false),
    _lockParent(false),
    _skipNextParentCheck(false),
    _allowsLayout(true),
    _hovering(false),
    _acceptsChildItems(true),
    _acceptsContextMenuEvents(true),
    _updatingLayout(false),
    _highlighted(false),
    _activeGrip(NoGrip),
    _allowedGrips(TopLeftGrip | TopRightGrip | BottomRightGrip | BottomLeftGrip |
                  TopMidGrip | RightMidGrip | BottomMidGrip | LeftMidGrip),
    _parentRelativeHeight(0),
    _parentRelativeWidth(0)
{
  _initializeShortName();
  setZValue(DRAWING_ZORDER);
  setAcceptsHoverEvents(true);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  connect(parentView, SIGNAL(mouseModeChanged(View::MouseMode)),
          this, SLOT(viewMouseModeChanged(View::MouseMode)));
  connect(parentView, SIGNAL(viewModeChanged(View::ViewMode)),
          this, SLOT(updateView()));

  connect(this, SIGNAL(geometryChanged()), parentView, SLOT(viewChanged()));

  setLayoutMargins(ApplicationSettings::self()->layoutMargins());
  setLayoutSpacing(ApplicationSettings::self()->layoutSpacing());

  // Add actions common to all view objects
  // create them here in the constructor so we
  // can register shortcuts.
  _editAction = new QAction(tr("Edit"), this);
  _editAction->setShortcut(Qt::Key_E);
  registerShortcut(_editAction);
  connect(_editAction, SIGNAL(triggered()), this, SLOT(edit()));

  _deleteAction = new QAction(tr("Delete"), this);
  _deleteAction->setShortcut(Qt::Key_Delete);
  registerShortcut(_deleteAction);
  connect(_deleteAction, SIGNAL(triggered()), this, SLOT(remove()));

  _raiseAction = new QAction(tr("Raise"), this);
  connect(_raiseAction, SIGNAL(triggered()), this, SLOT(raise()));

  _lowerAction = new QAction(tr("Lower"), this);
  connect(_lowerAction, SIGNAL(triggered()), this, SLOT(lower()));

  _autoLayoutAction = new QAction(tr("Automatic"), this);
  connect(_autoLayoutAction, SIGNAL(triggered()), this, SLOT(createAutoLayout()));

  _customLayoutAction = new QAction(tr("Custom"), this);
  connect(_customLayoutAction, SIGNAL(triggered()), this, SLOT(createCustomLayout()));

  // only drop plots onto TabBar
  setAcceptDrops(false);
}


ViewItem::~ViewItem() {
}

void ViewItem::_initializeShortName() {
  _shortName = 'D'+QString::number(_dnum);
  if (_dnum>max_dnum)
    max_dnum = _dnum;
  _dnum++;

}


void ViewItem::save(QXmlStreamWriter &xml) {
  xml.writeAttribute("name", typeName());
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
  xml.writeStartElement("relativesize");
  xml.writeAttribute("width", QVariant(_parentRelativeWidth).toString());
  xml.writeAttribute("height", QVariant(_parentRelativeHeight).toString());
  xml.writeAttribute("centerx", QVariant(_parentRelativeCenter.x()).toString());
  xml.writeAttribute("centery", QVariant(_parentRelativeCenter.y()).toString());
  xml.writeAttribute("posx", QVariant(_parentRelativePosition.x()).toString());
  xml.writeAttribute("posy", QVariant(_parentRelativePosition.y()).toString());
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
      stopList += ',';
      stopList += color.name();
      stopList += ',';
    }
    xml.writeAttribute("gradient", stopList);
  }
  xml.writeEndElement();

  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    viewItem->save(xml);
  }
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
        setTypeName(av.toString());
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
    } else if (xml.name().toString() == "relativesize") {
      knownTag = true;
      double width = 0, height = 0, centerx = 0, centery = 0, posx = 0, posy = 0;
      av = attrs.value("width");
      if (!av.isNull()) {
        width = av.toString().toDouble();
      }
      av = attrs.value("height");
      if (!av.isNull()) {
        height = av.toString().toDouble();
      }
      av = attrs.value("centerx");
      if (!av.isNull()) {
        centerx = av.toString().toDouble();
      }
      av = attrs.value("centery");
      if (!av.isNull()) {
        centery = av.toString().toDouble();
      }
      av = attrs.value("posx");
      if (!av.isNull()) {
        posx = av.toString().toDouble();
      }
      av = attrs.value("posy");
      if (!av.isNull()) {
        posy = av.toString().toDouble();
      }
      setRelativeWidth(width);
      setRelativeHeight(height);
      setRelativeCenter(QPointF(centerx, centery));
      setRelativePosition(QPointF(posx, posy));
    } else if (xml.name().toString() == "transform") {
      knownTag = true;
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

  if (knownTag) {
    xml.readNext();
    if (xml.isEndElement()) {
      if ((xml.name().toString() == expectedTag) ) {
      validChildTag = true;
      }
    }
  }
  return knownTag;
}

View *ViewItem::view() const {
  return qobject_cast<View*>(QObject::parent());
}

void ViewItem::setView(View* parentView) {
  QObject::setParent(parentView);
  reRegisterShortcut();
}


ViewItem *ViewItem::parentViewItem() const {
  return qgraphicsitem_cast<ViewItem*>(parentItem());
}


void ViewItem::setParentViewItem(ViewItem* parent) {
  QGraphicsItem::setParentItem(parent);
  updateRelativeSize();
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

    viewItem->setSkipNextParentCheck(true);
    viewItem->updateChildGeometry(oldViewRect, viewRect);
  }
}


void ViewItem::setViewRect(qreal x, qreal y, qreal width, qreal height, bool automaticChange) {
  setViewRect(QRectF(x, y, width, height), automaticChange);
}


QSizeF ViewItem::sizeOfGrip() const {
  if (!view())
    return QSizeF();

  int base = 9;
  return view()->mapToScene(QRect(0, 0, base, base)).boundingRect().size();
}


QPainterPath ViewItem::topLeftGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

  return path;
}


QPainterPath ViewItem::topRightGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

  return path;
}


QPainterPath ViewItem::bottomRightGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomRight() - QPointF(sizeOfGrip().width(), sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

  return path;
}


QPainterPath ViewItem::bottomLeftGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPointF(0, sizeOfGrip().height()), sizeOfGrip());
  QPainterPath path;
  if (_gripMode == Resize || _gripMode == Scale || _gripMode == Move)
    path.addRect(grip);
  else
    path.addEllipse(grip);

  return path;
}


QPainterPath ViewItem::topMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);

  return path;
}


QPainterPath ViewItem::rightMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);

  return path;
}


QPainterPath ViewItem::bottomMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.bottomLeft() - QPointF(0, sizeOfGrip().height()), sizeOfGrip());
  grip.moveCenter(QPointF(bound.center().x(), grip.center().y()));

  QPainterPath path;
  path.addRect(grip);

  return path;
}


QPainterPath ViewItem::leftMidGrip() const {
  if (_gripMode == Move || _gripMode == Rotate || _lockAspectRatio)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));

  QPainterPath path;
  path.addRect(grip);

  return path;
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
  return rect();
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
  if (view()) /* false when exiting */
    inCreation = view()->mouseMode() == View::Create;
  if ((!isSelected() && !isHovering()) || inCreation)
    return QGraphicsRectItem::boundingRect();

  QPolygonF gripBound = gripBoundingRect();
  return QRectF(gripBound[0], gripBound[2]);
}


QPainterPath ViewItem::shape() const {
  if ((!isSelected() && !isHovering()) || (view()->mouseMode() == View::Create))
    return itemShape();

  QPainterPath selectPath;
  selectPath.setFillRule(Qt::WindingFill);

  selectPath.addPolygon(rect());

  selectPath.addPath(grips());
  return selectPath;
}

bool ViewItem::isMaximized() {
  if (_plotMaximized) {
    return true;
  } else if (parentViewItem()) {
    return parentViewItem()->isMaximized();
  } else {
    return false;
  }
}

void ViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);
  if ((!isMaximized()) && view()->childMaximized()) {
    return;
  }
  painter->setPen(pen());
  painter->setBrush(brush());
  paint(painter); //this is the overload that subclasses should use...

  if (!view()->isPrinting() && !view()->childMaximized()) {
    painter->save();
    painter->setPen(Qt::DotLine);
    painter->setBrush(Qt::NoBrush);
    if ((isSelected() || isHovering())
        && view()->mouseMode() != View::Create
        && view()->viewMode() != View::Data) {
      painter->drawPath(shape());
      if (_gripMode == Resize)
        painter->fillPath(grips(), Qt::blue);
      else if (_gripMode == Scale)
        painter->fillPath(grips(), Qt::black);
      else if (_gripMode == Rotate)
        painter->fillPath(grips(), Qt::red);
      else if (_gripMode == Move)
        painter->fillPath(grips(), Qt::transparent);
    } else if (isHighlighted()) {
      QColor highlightColor(QColor(255, 255, 0, 120));
      painter->fillPath(shape(), highlightColor);
    }
    if (supportsTiedZoom()) {
      painter->save();
      painter->setPen(Qt::black);
      painter->setRenderHint(QPainter::Antialiasing, true);
      painter->fillPath(checkBox(), Qt::white);
      if (isHovering()) {
        QRectF check = checkBox().controlPointRect();
        check.setSize(QSizeF(check.width() / 1.8, check.height() / 1.8));
        check.moveCenter(checkBox().controlPointRect().center());
        QPainterPath p;
        p.addEllipse(check);
        painter->fillPath(p, Qt::black);
      }
      if (isTiedZoom()) {
        painter->save();
        QColor c = Qt::black;
        c.setAlphaF(c.alphaF() * 0.5);
        painter->fillPath(tiedZoomCheck(), c);
        painter->restore();
      }
      painter->setBrush(Qt::transparent);
      painter->drawPath(checkBox());
      painter->restore();
    }
#if DEBUG_GEOMETRY
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
  }
}


void ViewItem::paint(QPainter *painter) {
  Q_UNUSED(painter);
}


void ViewItem::edit() {
  ViewItemDialog *editDialog = new ViewItemDialog(this, kstApp->mainWindow());
  editDialog->show();
}


void ViewItem::sharePlots(QPainter *painter, bool creation) {
  if (!_updatingLayout) {
    _updatingLayout = true;
    ViewGridLayout::sharePlots(this, painter, creation);
    _updatingLayout = false;
  }
}


void ViewItem::createAutoLayout() {
  if (parentViewItem()) {
    LayoutCommand *layout = new LayoutCommand(parentViewItem());
    layout->createLayout();
  } else if (view()) {
    view()->createLayout();
  }
}


void ViewItem::createCustomLayout() {
  bool ok;
  int default_cols = qMax(1,int(sqrt(Data::self()->plotList().count())));
  int columns = QInputDialog::getInteger(view(), tr("Kst"),
                                      tr("Select Number of Columns"),default_cols, 0,
                                      10, 1, &ok);

  if (ok) {
    if (parentViewItem()) {
      LayoutCommand *layout = new LayoutCommand(parentViewItem());
      layout->createLayout(columns);
    } else if (view()) {
      view()->createLayout(columns);
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
  if (event == View::EscapeEvent) {
    deleteLater();
    kstApp->mainWindow()->clearDrawingMarker();
    return;
  }

  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(0.0, 0.0, 0.0, 0.0);
    view()->scene()->addItem(this);
    _creationState = InProgress;
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseMove));
    QRectF newRect(rect().x(), rect().y(),
                   poly.last().x() - rect().x(),
                   poly.last().y() - rect().y());
    setViewRect(newRect);
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseRelease));
    QRectF newRect(rect().x(), rect().y(),
                   poly.last().x() - rect().x(),
                   poly.last().y() - rect().y());

    if (!newRect.isValid()) {
      newRect = newRect.normalized();
      setPos(pos() + newRect.topLeft());

      newRect.moveTopLeft(QPointF(0, 0));
      setViewRect(newRect);

      view()->setPlotBordersDirty(true);
    } else {
      setViewRect(newRect.normalized());
    }

    view()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    view()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    view()->setMouseMode(View::Default);

    updateViewItemParent();
    _creationState = Completed;
    setZValue(1);
    emit creationComplete();
    return;
  }
}


void ViewItem::addTitle(QMenu *menu) const {
  QWidgetAction *action = new QWidgetAction(menu);
  action->setEnabled(false);

  QLabel *label = new QLabel(typeName() + tr(" Menu"), menu);
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

  menu.addAction(_editAction);

  QMenu layoutMenu;
  if (!(lockParent() || (parentViewItem() && parentViewItem()->lockParent()))) {
    menu.addAction(_raiseAction);
    menu.addAction(_lowerAction);

    layoutMenu.setTitle(tr("Cleanup Layout"));
    layoutMenu.addAction(_autoLayoutAction);
    layoutMenu.addAction(_customLayoutAction);
    menu.addMenu(&layoutMenu);

    menu.addAction(_deleteAction);
  }


  addToMenuForContextEvent(menu);

  menu.exec(event->screenPos());
}


void ViewItem::addToMenuForContextEvent(QMenu &menu) {
  Q_UNUSED(menu);
}

void ViewItem::startDragging(QWidget *widget, const QPointF& hotspot) {
  // UNDO tied zoom settings done in PlotItem::mousePressEvent
  setTiedZoom(false, false);

  QDrag *drag = new QDrag(widget);
  MimeDataViewItem* mimeData = new MimeDataViewItem;
  mimeData->item = this;
  mimeData->hotSpot = hotspot;
  drag->setMimeData(mimeData);

  QPixmap pixmap(sceneBoundingRect().size().toSize());
  pixmap.fill(Qt::white);
  // Qt::transparent is maybe too expensive, and when 
  // not moving a plot it also has no transparent background
  //pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.translate(-rect().left(), -rect().top());
  paint(&painter); // TODO also paint annotations
  QList<QGraphicsItem*> children = childItems();
  foreach(QGraphicsItem* child, children) {
    ViewItem* item = qgraphicsitem_cast<ViewItem*>(child);
    if (item) {
      item->paint(&painter);
    }
  }
  painter.end();

  drag->setPixmap(pixmap);
  drag->setHotSpot(hotspot.toPoint()- rect().topLeft().toPoint());

  hide();
  Qt::DropActions dact = Qt::MoveAction;
  Qt::DropAction dropAction = drag->exec(dact);
  if (dropAction != Qt::MoveAction) {
    show();
  }
}

void ViewItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (view()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

#ifdef KST_ENABLE_DD
  if (!dragStartPosition.isNull() && event->buttons() & Qt::LeftButton) {
    if (view()->mouseMode() == View::Move) {
      startDragging(event->widget(), dragStartPosition.toPoint());
      return;
    }
  }
#endif

  if (view()->mouseMode() == View::Default) {
    if (gripMode() == ViewItem::Move || activeGrip() == NoGrip) {
      view()->setMouseMode(View::Move);
      view()->undoStack()->beginMacro(tr("Move"));
    } else if (gripMode() == ViewItem::Resize) {
      view()->setMouseMode(View::Resize);
      view()->undoStack()->beginMacro(tr("Resize"));
    } else if (gripMode() == ViewItem::Scale) {
      view()->setMouseMode(View::Scale);
      view()->undoStack()->beginMacro(tr("Scale"));
    } else if (gripMode() == ViewItem::Rotate) {
      view()->setMouseMode(View::Rotate);
      view()->undoStack()->beginMacro(tr("Rotate"));
    }
  }

  if (activeGrip() == NoGrip)
    return QGraphicsRectItem::mouseMoveEvent(event);

  QPointF p = event->pos();
  QPointF s = event->scenePos();

  if (gripMode() == ViewItem::Rotate) {
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
  } else if (gripMode() == ViewItem::Resize) {

    switch(_activeGrip) {
    case TopLeftGrip:
        resizeTopLeft(p - topLeftGrip().controlPointRect().center()); break;
    case TopRightGrip:
        resizeTopRight(p - topRightGrip().controlPointRect().center()); break;
    case BottomRightGrip:
        resizeBottomRight(p - bottomRightGrip().controlPointRect().center()); break;
    case BottomLeftGrip:
        resizeBottomLeft(p - bottomLeftGrip().controlPointRect().center()); break;
    case TopMidGrip:
        resizeTop(p.y() - topMidGrip().controlPointRect().center().y()); break;
    case RightMidGrip:
        resizeRight(p.x() - rightMidGrip().controlPointRect().center().x()); break;
    case BottomMidGrip:
        resizeBottom(p.y() - bottomMidGrip().controlPointRect().center().y()); break;
    case LeftMidGrip:
        resizeLeft(p.x() - leftMidGrip().controlPointRect().center().x()); break;
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
              "lockAspect error", QString(QString::number(newAspect) + "!=" + QString::number(oldAspect)).toLatin1().constData());
  Q_UNUSED(newAspect);
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
              "lockAspect error", QString(QString::number(newAspect) + "!=" + QString::number(oldAspect)).toLatin1().constData());
  Q_UNUSED(newAspect);
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
              "lockAspect error", QString(QString::number(newAspect) + "!=" + QString::number(oldAspect)).toLatin1().constData());
  Q_UNUSED(newAspect);
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
              "lockAspect error", QString(QString::number(newAspect) + "!=" + QString::number(oldAspect)).toLatin1().constData());
  Q_UNUSED(newAspect);
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
  QPointF anchor = selectTransform().map(rect().bottomLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setTop(y);
  from.moveBottomLeft(anchor);
  to.moveBottomLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setBottom(qreal y) {

  QPointF anchor = selectTransform().map(rect().topLeft());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setBottom(y);
  from.moveTopLeft(anchor);
  to.moveTopLeft(anchor);
  transformToRect(from, to);
}


void ViewItem::setLeft(qreal x) {

  QPointF anchor = selectTransform().map(rect().topRight());

  QRectF from = selectBoundingRect();
  QRectF to = from;

  to.setLeft(x);
  from.moveTopRight(anchor);
  to.moveTopRight(anchor);
  transformToRect(from, to);
}


void ViewItem::setRight(qreal x) {

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
     or the inverse by using selectTransform().inverted().
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

  return o;
}


bool ViewItem::updateViewItemParent() {
  if (lockParent() || skipNextParentCheck()) {
    setSkipNextParentCheck(false);
    return false;
  }
  //First get a list of all items that collide with this one
  QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemShape);

  bool topLevel = !parentItem();
  QPointF origin = mapToScene(QPointF(0,0));

#if DEBUG_REPARENT
  qDebug() << "updateViewItemParent" << this
           << "topLevel:" << (topLevel ? "true" : "false")
           << "origin:" << origin
           << "rect:" << rect()
           << "collision count:" << collisions.count()
           << endl;
#endif

  //Doesn't collide then reparent to top-level
  if (collisions.isEmpty() && !topLevel) {
#if DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#if DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParentViewItem(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#if DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  //Look for collisions that completely contain us
  foreach (QGraphicsItem *item, collisions) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);

    if (!viewItem || !viewItem->acceptsChildItems() || isAncestorOf(viewItem) || !collidesWithItem(viewItem, Qt::ContainsItemBoundingRect)) {
#if DEBUG_REPARENT
     qDebug() << "rejecting collision" << viewItem << !viewItem->acceptsChildItems() << isAncestorOf(viewItem) << !collidesWithItem(viewItem, Qt::ContainsItemBoundingRect); 
#endif
      continue;
    }

    if (parentItem() == viewItem) { /*already done*/
#if DEBUG_REPARENT
      qDebug() << "already in containing parent" << endl;
#endif
      return false;
    }

#if DEBUG_REPARENT
    qDebug() << "reparent to" << viewItem << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    if (!topLevel) { /*bring the old parent's transform with us*/
      setTransform(parentItem()->transform(), true);
    }

    /*cancel out the new parent's initial transform*/
    setTransform(viewItem->transform().inverted(), true);

#if DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParentViewItem(viewItem);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#if DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }

  //No suitable collisions then reparent to top-level
  if (!topLevel) {
#if DEBUG_REPARENT
    qDebug() << "reparent to topLevel" << endl;

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#if DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    setParentViewItem(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#if DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0))
             << endl;
#endif

    return true;
  }
  return false;
}


void ViewItem::updateRelativeSize() {
  if (parentViewItem()) {
    _parentRelativeHeight = (height() / parentViewItem()->height());
    _parentRelativeWidth = (width() / parentViewItem()->width());
    _parentRelativeCenter =  mapToParent(rect().center()) - parentViewItem()->rect().topLeft();
    _parentRelativeCenter =  QPointF(_parentRelativeCenter.x() / parentViewItem()->width(), _parentRelativeCenter.y() / parentViewItem()->height());
    _parentRelativePosition =  mapToParent(rect().topLeft()) - parentViewItem()->rect().topLeft();
    _parentRelativePosition =  QPointF(_parentRelativePosition.x() / parentViewItem()->width(), _parentRelativePosition.y() / parentViewItem()->height());
   } else if (view()) {
    _parentRelativeHeight = (height() / view()->height());
    _parentRelativeWidth = (width() / view()->width());
    _parentRelativeCenter =  mapToParent(rect().center()) - view()->rect().topLeft();
    _parentRelativeCenter =  QPointF(_parentRelativeCenter.x() / view()->width(), _parentRelativeCenter.y() / view()->height());
    _parentRelativePosition =  mapToParent(rect().topLeft()) - view()->rect().topLeft();
    _parentRelativePosition =  QPointF(_parentRelativePosition.x() / view()->width(), _parentRelativePosition.y() / view()->height());
  } else {
    _parentRelativeHeight = 0;
    _parentRelativeWidth = 0;
    _parentRelativeCenter = QPointF(0, 0);
    _parentRelativePosition = QPointF(0, 0);
  }
}


void ViewItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
#if DEBUG_CHILD_GEOMETRY
  qDebug() << "ViewItem::updateChildGeometry" << this << oldParentRect << newParentRect << endl;
#else
  Q_UNUSED(oldParentRect);
#endif

  QRectF itemRect = rect();

  //Lock aspect ratio for rotating objects or children with a lockedAspectRatio
  //FIXME is the child rotated with respect to the parent is the real question...
  if (transform().isRotating() || lockAspectRatio()) {

#if DEBUG_CHILD_GEOMETRY
    qDebug() << "ViewItem::updateChildGeometry Fixed Ratio" << mapToParent(rect().center()) << _parentRelativeCenter;
#endif

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
    qreal newHeight = relativeHeight() * newParentRect.height();
    qreal newWidth = relativeWidth() * newParentRect.width();

#if DEBUG_CHILD_GEOMETRY
    qDebug() << "ViewItem::updateChildGeometry non-Fixed Ratio" << "relativeHeight = " << relativeHeight() << "relative Width" << relativeWidth();
#endif

    QPointF newTopLeft = newParentRect.topLeft() - itemRect.topLeft() +
                         QPointF(newParentRect.width() * _parentRelativePosition.x(),
                                 newParentRect.height() * _parentRelativePosition.y());

    itemRect.setWidth(newWidth);
    itemRect.setHeight(newHeight);
    setPos(newTopLeft);
  }

#if DEBUG_CHILD_GEOMETRY
  qDebug() << "resize"
            << "\nbefore:" << rect()
            << "\nafter:" << itemRect
            << endl;
#endif

  setViewRect(itemRect, true);
}


void ViewItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  if (view()->viewMode() == View::Data) {
    event->ignore();
    return;
  }
}


void ViewItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (view()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  const QPointF p = event->pos();

  dragStartPosition = p;

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
  if (view()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  dragStartPosition = QPointF(0, 0);

  if (view()->mouseMode() != View::Default) {
    view()->setMouseMode(View::Default);
    view()->undoStack()->endMacro();
  }

  QGraphicsRectItem::mouseReleaseEvent(event);
}


void ViewItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  if (view()->viewMode() == View::Data) {
    return;
  }
  if (isSelected()) {
    QPointF p = event->pos();
    if ((isAllowed(TopLeftGrip) && topLeftGrip().contains(p)) || (isAllowed(BottomRightGrip) && bottomRightGrip().contains(p))) {
      if (gripMode() == ViewItem::Rotate) {
        view()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        view()->setCursor(Qt::SizeFDiagCursor);
      }
    } else if ((isAllowed(TopRightGrip) && topRightGrip().contains(p)) || (isAllowed(BottomLeftGrip) && bottomLeftGrip().contains(p))) {
      if (gripMode() == ViewItem::Rotate) {
        view()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        view()->setCursor(Qt::SizeBDiagCursor);
      }
    } else if ((isAllowed(TopMidGrip) && topMidGrip().contains(p)) || (isAllowed(BottomMidGrip) && bottomMidGrip().contains(p))) {
      if (gripMode() == ViewItem::Rotate) {
        view()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        view()->setCursor(Qt::SizeVerCursor);
      }
    } else if ((isAllowed(RightMidGrip) && rightMidGrip().contains(p)) || (isAllowed(LeftMidGrip) && leftMidGrip().contains(p))) {
      if (gripMode() == ViewItem::Rotate) {
        view()->setCursor(Qt::CrossCursor);
      } else if (gripMode() == ViewItem::Resize) {
        view()->setCursor(Qt::SizeHorCursor);
      }
    } else {
      view()->setCursor(Qt::SizeAllCursor);
    }
  } else {
    view()->setCursor(Qt::SizeAllCursor);
  }
}


void ViewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  _hovering = true;
  update();
}


void ViewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  view()->setCursor(Qt::ArrowCursor);

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

void ViewItem::moveTo(const QPointF& pos)
{

  QPointF newpos = view()->snapPoint(pos);

  if (parentViewItem()) {
    newpos -= parentViewItem()->scenePos();
  }

  setPos(newpos);
  new MoveCommand(this, _originalPosition, pos);
  updateViewItemParent();
  updateRelativeSize();
}

void ViewItem::viewMouseModeChanged(View::MouseMode oldMode) {
  if (view()->mouseMode() == View::Move) {
    _originalPosition = pos();
  } else if (view()->mouseMode() == View::Resize ||
             view()->mouseMode() == View::Scale ||
             view()->mouseMode() == View::Rotate) {
    _originalRect = rect();
    _originalTransform = transform();
  } else if (oldMode == View::Move && _originalPosition != pos()) {
#ifndef KST_ENABLE_DD
    moveTo(pos());
#endif
  } else if (oldMode == View::Resize && _originalRect != rect()) {
    new ResizeCommand(this, _originalRect, rect());
    updateViewItemParent();
  } else if (oldMode == View::Scale && _originalTransform != transform()) {
    new ScaleCommand(this, _originalTransform, transform());

    updateViewItemParent();
  } else if (oldMode == View::Rotate && _originalTransform != transform()) {
    new RotateCommand(this, _originalTransform, transform());

    updateViewItemParent();
  }
}


void ViewItem::registerShortcut(QAction *action) {
  Q_ASSERT(action->parent() == this);
  view()->grabShortcut(action->shortcut());
  _shortcutMap.insert(action->shortcut(), action);
}


void ViewItem::reRegisterShortcut() {
  QHashIterator<QString, QAction*> it(_shortcutMap);
  while (it.hasNext()) {
    it.next();
    view()->grabShortcut(it.key());
  }
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


QPainterPath ViewItem::checkBox() const {
  QRectF bound = selectBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width() * 1.25, sizeOfGrip().height() * -.25), sizeOfGrip());
  QPainterPath path;
  path.addEllipse(grip);
  return path;
}


QPainterPath ViewItem::tiedZoomCheck() const {
  QRectF bound = selectBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width() * 1.25, sizeOfGrip().height() * -.25), sizeOfGrip());
  QPainterPath path;
  if (isXTiedZoom() && isYTiedZoom()) {
    path.addEllipse(grip);
  } else if (isXTiedZoom()) {
    path.moveTo(grip.center());
    path.arcTo(grip, 225, 180);
  } else if (isYTiedZoom()) {
    path.moveTo(grip.center());
    path.arcTo(grip, 45, 180);
  }
  return path;
}


void ViewItem::updateView() {
  update();
}


qreal ViewItem::rotationAngle() const {
  return 180.0/ONE_PI * atan2(transform().m12(), transform().m11());
}


void ViewItem::setSupportsTiedZoom(const bool supports) {
  if (supports != _supportsTiedZoom) {

    _supportsTiedZoom = supports;

    if (_supportsTiedZoom && ((layoutMargins().width() < tiedZoomSize().width()) || (layoutMargins().height() < tiedZoomSize().height()))) {
      setLayoutMargins(layoutMargins().expandedTo(tiedZoomSize()));
    }

    if (_supportsTiedZoom) {
      PlotItemManager::self()->addViewItem(this);
    } else {
      setTiedZoom(false, false, false);
      PlotItemManager::self()->removeViewItem(this);
    }
  }
}


void ViewItem::setTiedZoom(bool tiedXZoom, bool tiedYZoom, bool checkAllTied) {
  Q_UNUSED(checkAllTied)
  if ((_isXTiedZoom == tiedXZoom) && (_isYTiedZoom == tiedYZoom))
    return;

  bool wasTiedZoom = isTiedZoom();

  _isXTiedZoom = tiedXZoom;
  _isYTiedZoom = tiedYZoom;

  if (isTiedZoom() && !wasTiedZoom) {
    PlotItemManager::self()->addTiedZoomViewItem(this);
  } else if (!isTiedZoom() && wasTiedZoom) {
    PlotItemManager::self()->removeTiedZoomViewItem(this);
  }

  //FIXME ugh, this is expensive, but need to redraw the checkboxes...
  update();
}

QString ViewItem::_automaticDescriptiveName() const {
  return typeName();
}

QString ViewItem::descriptionTip() const {
  return typeName();
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, ViewItem *viewItem) {
    dbg.nospace() << viewItem->typeName();
    return dbg.space();
}
#endif


ViewItemCommand::ViewItemCommand(ViewItem *item, const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(item) {
  if (addToStack)
    _item->view()->undoStack()->push(this);
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
  kstApp->mainWindow()->clearDrawingMarker();
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
  Q_ASSERT(_item->view());

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
  delete grid;

  if (qobject_cast<LayoutBoxItem*>(_item)) {
    QObject::connect(_layout, SIGNAL(enabledChanged(bool)),
                     _item, SLOT(setEnabled(bool)));
  }

  _layout->apply();
  _item->view()->undoStack()->push(this);
}


void AppendLayoutCommand::undo() {
  if (_layout) {
    _layout->reset();
  }
}


void AppendLayoutCommand::redo() {
  Q_ASSERT(_layout);
  _layout->apply();
}


void AppendLayoutCommand::appendLayout(CurvePlacement::Layout layout, ViewItem* item, int columns) {
  Q_ASSERT(_item);
  Q_ASSERT(_item->view());
  Q_ASSERT(item);

  _layout = new ViewGridLayout(_item);

  QPointF center = _item->view()->sceneRect().center();
  center -= QPointF(100.0, 100.0);

  item->setPos(center);
  item->setViewRect(0.0, 0.0, 200.0, 200.0);
  _item->view()->scene()->addItem(item);

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
    if (layout == CurvePlacement::Auto) {
      columns = (int)sqrt((double)viewItems.count()+1);
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
    delete grid;

    if (qobject_cast<LayoutBoxItem*>(_item)) {
      QObject::connect(_layout, SIGNAL(enabledChanged(bool)),
                      _item, SLOT(setEnabled(bool)));
    }

    _layout->apply();
  }
  _item->view()->undoStack()->push(this);
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
  _item->setViewRect(_originalRect, true);
}


void ResizeCommand::redo() {
  Q_ASSERT(_item);
  _item->setViewRect(_newRect, true);
}


void RemoveCommand::undo() {
  Q_ASSERT(_item);
  _item->show();
}


void RemoveCommand::redo() {
  Q_ASSERT(_item);
  _item->hide();
  // hmmm... view items aren't really deleted!!  if we delete them,
  // then we run into trouble with the undo stack.  If we don't, then
  // they keep holding onto the curves, preventing purge.
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




MimeDataViewItem::MimeDataViewItem() : QMimeData() {
}

const MimeDataViewItem* MimeDataViewItem::downcast(const QMimeData* m) {
  return qobject_cast<const MimeDataViewItem*>(m);
}

}

// vim: ts=2 sw=2 et
