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
#include "sharedaxisboxitem.h"
#include "plotitemmanager.h"
#include "document.h"
#include "datacollection.h"
#include "formatgridhelper.h"
#include "dialogdefaults.h"
#include "cartesianrenderitem.h"
#include "viewitemscriptinterface.h"

#include "layoutboxitem.h"

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
#include <QtAlgorithms>

static const qreal ONE_PI = 3.14159265358979323846264338327950288419717;
static qreal TWO_PI = 2.0 * ONE_PI;
static qreal RAD2DEG = 180.0 / ONE_PI;
static const int DRAWING_ZORDER = 500;

#define DEBUG_GEOMETRY 0
#define DEBUG_REPARENT 0

// enable drag & drop
#define KST_ENABLE_DD

namespace Kst {

ViewItem::ViewItem(View *parentView) :
    QObject(parentView),
    NamedObject(),
    _isXTiedZoom(false),
    _isYTiedZoom(false),
    _plotMaximized(false),
    _activeGrip(NoGrip),
    _parentRelativeHeight(0),
    _parentRelativeWidth(0),
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
    _allowedGrips(TopLeftGrip | TopRightGrip | BottomRightGrip | BottomLeftGrip |
                TopMidGrip | RightMidGrip | BottomMidGrip | LeftMidGrip),
    _lockPosToData(false),
    _editDialog(0),
    _interface(0),
    _dpi(71.0)
{
  _initializeShortName();
  setZValue(DRAWING_ZORDER);
  setAcceptHoverEvents(true);
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
  _autoLayoutAction->setShortcut(Qt::Key_U);
  registerShortcut(_autoLayoutAction);
  connect(_autoLayoutAction, SIGNAL(triggered()), this, SLOT(createAutoLayout()));

  _protectedLayoutAction = new QAction(tr("Protect Layout"), this);
  connect(_protectedLayoutAction, SIGNAL(triggered()), this, SLOT(createProtectedLayout()));

  _customLayoutAction = new QAction(tr("Columns"), this);
  connect(_customLayoutAction, SIGNAL(triggered()), this, SLOT(createCustomLayout()));

  _oneColumnLayoutAction = new QAction(tr("1 Column"), this);
  _oneColumnLayoutAction->setShortcut(Qt::Key_1);
  registerShortcut(_oneColumnLayoutAction);
  connect(_oneColumnLayoutAction, SIGNAL(triggered()), this, SLOT(createOneColLayout()));

  _twoColumnLayoutAction = new QAction(tr("2 Columns"), this);
  _twoColumnLayoutAction->setShortcut(Qt::Key_2);
  registerShortcut(_twoColumnLayoutAction);
  connect(_twoColumnLayoutAction, SIGNAL(triggered()), this, SLOT(createTwoColLayout()));

  _threeColumnLayoutAction = new QAction(tr("3 Columns"), this);
  _threeColumnLayoutAction->setShortcut(Qt::Key_3);
  registerShortcut(_threeColumnLayoutAction);
  connect(_threeColumnLayoutAction, SIGNAL(triggered()), this, SLOT(createThreeColLayout()));

  _lockPosToDataAction = new QAction(tr("&Lock Position to Data"),this);
  _lockPosToDataAction->setCheckable(true);
  connect(_lockPosToDataAction, SIGNAL(toggled(bool)), this, SLOT(setLockPosToData(bool)));

  // only drop plots onto TabBar
  setAcceptDrops(false);

}


ViewItem::~ViewItem() {
}

void ViewItem::_initializeShortName() {
  _shortName = 'D'+QString::number(_viewitemnum);
  if (_viewitemnum>max_viewitemnum)
    max_viewitemnum = _viewitemnum;
  _viewitemnum++;

}


void ViewItem::save(QXmlStreamWriter &xml) {

  // when saving, check to see if your parent is a cartesianrenderitem.
  // if so, we'll need to adjust to take into account that when loaded,
  // the object will be created in a cartesian render item with no plot
  // borders, and then resized.
  // this keeps line end points in the right place.
  CartesianRenderItem *cri = dynamic_cast<CartesianRenderItem *>(parentItem());
  QTransform tr;
  qreal w = _parentRelativeWidth;
  if (cri) {
    QRectF cri_rect = cri->rect();
    QRectF plot_rect = cri->parentRect();
    qreal oldL = relativeWidth()*cri_rect.width();

    qreal r0 = rotationAngleRadians();
    qreal dy = oldL*sin(r0)*plot_rect.height()/cri_rect.height();
    qreal dx = oldL*cos(r0)*plot_rect.width()/cri_rect.width();
    qreal r1 = atan2(dy, dx);

    w = sqrt(dy*dy + dx*dx)/plot_rect.width();

    tr.rotateRadians(r1);
  } else {
    tr = transform();
  }

  xml.writeAttribute("name", typeName());
  xml.writeStartElement("position");
  xml.writeAttribute("x", QVariant(pos().x()).toString());
  xml.writeAttribute("y", QVariant(pos().y()).toString());
  xml.writeAttribute("z", QVariant(zValue()).toString());
  xml.writeEndElement();
  xml.writeStartElement("rect");
  xml.writeAttribute("x", QVariant(viewRect().x()).toString());
  xml.writeAttribute("y", QVariant(viewRect().y()).toString());
  xml.writeAttribute("width", QVariant(viewRect().width()).toString());
  xml.writeAttribute("height", QVariant(viewRect().height()).toString());
  xml.writeEndElement();
  xml.writeStartElement("relativesize");
  xml.writeAttribute("width", QVariant(w).toString());
  xml.writeAttribute("height", QVariant(_parentRelativeHeight).toString());
  xml.writeAttribute("centerx", QVariant(_parentRelativeCenter.x()).toString());
  xml.writeAttribute("centery", QVariant(_parentRelativeCenter.y()).toString());
  xml.writeAttribute("posx", QVariant(_parentRelativePosition.x()).toString());
  xml.writeAttribute("posy", QVariant(_parentRelativePosition.y()).toString());
  xml.writeAttribute("leftx", QVariant(_parentRelativeLeft.x()).toString());
  xml.writeAttribute("lefty", QVariant(_parentRelativeLeft.y()).toString());
  xml.writeAttribute("rightx", QVariant(_parentRelativeRight.x()).toString());
  xml.writeAttribute("righty", QVariant(_parentRelativeRight.y()).toString());
  xml.writeAttribute("fixaspect", QVariant(_lockAspectRatio).toString());
  xml.writeAttribute("lockpostodata", QVariant(_lockPosToData).toString());
  if (_lockPosToData) { // meaningless if not locked: why pollute the file?
    xml.writeAttribute("datarect_x", QVariant(_dataRelativeRect.x()).toString());
    xml.writeAttribute("datarect_y", QVariant(_dataRelativeRect.y()).toString());
    xml.writeAttribute("datarect_width", QVariant(_dataRelativeRect.width()).toString());
    xml.writeAttribute("datarect_height", QVariant(_dataRelativeRect.height()).toString());
  }
  xml.writeEndElement();

  xml.writeStartElement("transform");
  xml.writeAttribute("m11", QVariant(tr.m11()).toString());
  xml.writeAttribute("m12", QVariant(tr.m12()).toString());
  xml.writeAttribute("m13", QVariant(tr.m13()).toString());
  xml.writeAttribute("m21", QVariant(tr.m21()).toString());
  xml.writeAttribute("m22", QVariant(tr.m22()).toString());
  xml.writeAttribute("m23", QVariant(tr.m23()).toString());
  xml.writeAttribute("m31", QVariant(tr.m31()).toString());
  xml.writeAttribute("m32", QVariant(tr.m32()).toString());
  xml.writeAttribute("m33", QVariant(tr.m33()).toString());
  xml.writeEndElement();
  xml.writeStartElement("pen");
  xml.writeAttribute("style", QVariant((int)pen().style()).toString());
  xml.writeAttribute("width", QVariant(storedPen().widthF()).toString());
  xml.writeAttribute("miterlimit", QVariant(pen().miterLimit()).toString());
  xml.writeAttribute("cap", QVariant(pen().capStyle()).toString());
  xml.writeAttribute("joinStyle", QVariant(pen().joinStyle()).toString());
  xml.writeStartElement("brush");
  xml.writeAttribute("color", pen().brush().color().name());
  xml.writeAttribute("alpha", QString::number(pen().brush().color().alphaF()));
  xml.writeAttribute("style", QVariant((int)pen().brush().style()).toString());
  xml.writeEndElement();
  xml.writeEndElement();
  xml.writeStartElement("brush");
  xml.writeAttribute("color", brush().color().name());
  xml.writeAttribute("alpha", QString::number(brush().color().alphaF()));
  xml.writeAttribute("style", QVariant((int)brush().style()).toString());
  if (brush().gradient()) {
    QString stopList;
    foreach(const QGradientStop &stop, brush().gradient()->stops()) {
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

  QList<QGraphicsItem*> list = QGraphicsItem::childItems();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    viewItem->save(xml);
  }
}

void ViewItem::applyDialogDefaultsFill(bool default_no_fill) {
  if (hasBrush()) {
    //set the brush
    QBrush brush = dialogDefaultsBrush(defaultsGroupName(), default_no_fill);
    setBrush(brush);
  }
}

void ViewItem::applyDialogDefaultsStroke(bool default_no_pen) {
  if (hasStroke()) {
    // set the pen
    QPen pen = dialogDefaultsPen(defaultsGroupName(), default_no_pen);
    storePen(pen);
  }
}

void ViewItem::applyDialogDefaultsLockPosToData() {
  setLockPosToData(dialogDefaultsLockPosToData(defaultsGroupName()));
}

void ViewItem::setLockPosToData(bool lockPosToData) {
  _lockPosToData = lockPosToData;
  _lockPosToDataAction->setChecked(lockPosToData);
  emit(relativeSizeUpdated());
}


QRectF ViewItem::parentRect() const {
  if (parentViewItem()) {
    return parentViewItem()->rect().normalized();
  } else if (view()) {
    return view()->rect();
  } else {
    Q_ASSERT_X(false,"parent test", "item has no parentview item");
  }
  return QRectF(0,0,1,1); // shouldn't get here
}


void ViewItem::applyDataLockedDimensions() {
  PlotRenderItem *render_item = dynamic_cast<PlotRenderItem *>(parentViewItem());
  if (render_item) {
    qreal parentWidth = render_item->width();
    qreal parentHeight = render_item->height();
    qreal parentX = render_item->rect().x();
    qreal parentY = render_item->rect().y();

    qreal aspectRatio;
    if (rect().width() > 0) {
      aspectRatio = qreal(rect().height()) / qreal(rect().width());
    } else {
      aspectRatio = 10000.0;
    }

    qreal relativeWidth = _dataRelativeRect.width()/(render_item->plotItem()->xMax() - render_item->plotItem()->xMin());
    qreal relativeHeight = _dataRelativeRect.height()/(render_item->plotItem()->yMax() - render_item->plotItem()->yMin());
    qreal relativeX = (_dataRelativeRect.center().x() - render_item->plotItem()->xMin())/
        (render_item->plotItem()->xMax() - render_item->plotItem()->xMin());
    qreal relativeY = (_dataRelativeRect.center().y() - render_item->plotItem()->yMin())/
        (render_item->plotItem()->yMax() - render_item->plotItem()->yMin());

    qreal width = relativeWidth * parentWidth;
    qreal height;
    if (lockAspectRatio()) {
      height = width * aspectRatio;
    } else {
      height = relativeHeight * parentHeight;
    }
    setPos(parentX + relativeX*parentWidth, parentY + (1.0-relativeY)*parentHeight);
    setViewRect(-width/2, -height/2, width, height);

    updateRelativeSize();

  } else {
    qDebug() << "apply data locked dimensions called without a render item (!)";
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
      qreal x = 0, y = 0, z = DRAWING_ZORDER;
      av = attrs.value("x");
      if (!av.isNull()) {
        x = av.toString().toDouble();
      }
      av = attrs.value("y");
      if (!av.isNull()) {
        y = av.toString().toDouble();
     }
     setPos(x, y);

     av = attrs.value("z");
     if (!av.isNull()) {
       z = av.toString().toDouble();
     }
     setZValue(z);

    } else if (xml.name().toString() == "brush") {
      knownTag = true;
      QBrush brush;
      av = attrs.value("gradient");
      if (!av.isNull()) {
        QStringList stopInfo = av.toString().split(',', Qt::SkipEmptyParts);
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
        av = attrs.value("alpha");
        if (!av.isNull()) {
          qreal alpha = av.toString().toDouble();
          QColor c = brush.color();
          c.setAlphaF(alpha);
          brush.setColor(c);
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
        av = attrs.value("alpha");
        if (!av.isNull()) {
          qreal alpha = av.toString().toDouble();
          QColor c = penBrush.color();
          c.setAlphaF(alpha);
          penBrush.setColor(c);
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
      storePen(pen);
    } else if (xml.name().toString() == "rect") {
      knownTag = true;
      qreal x = 0, y = 0, w = 10, h = 10;
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
      qreal width = 0, height = 0, centerx = 0, centery = 0, posx = 0, posy = 0;
      qreal leftx = -1.0, lefty = -1.0, rightx = -1.0, righty = -1.0;
      bool lock_aspect_ratio = false;
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
      av = attrs.value("leftx");
      if (!av.isNull()) {
        leftx = av.toString().toDouble();
      }
      av = attrs.value("lefty");
      if (!av.isNull()) {
        lefty = av.toString().toDouble();
      }
      av = attrs.value("rightx");
      if (!av.isNull()) {
        rightx = av.toString().toDouble();
      }
      av = attrs.value("righty");
      if (!av.isNull()) {
        righty = av.toString().toDouble();
      }
      av = attrs.value("fixaspect");
      if (!av.isNull()) {
        lock_aspect_ratio = QVariant(av.toString()).toBool();
      }
      if (rightx <-0.99) { // old kst file: generate from center
        rightx = centerx + width/2.0;
        leftx  = centerx - width/2.0;
        righty = centery + height/2.0;
        lefty  = centery + height/2.0;
      }
      setRelativeWidth(width);
      setRelativeHeight(height);
      setRelativeCenter(QPointF(centerx, centery));
      setRelativePosition(QPointF(posx, posy));
      setRelativeLeft(QPointF(leftx, lefty));
      setRelativeRight(QPointF(rightx, righty));
      setLockAspectRatio(lock_aspect_ratio);

      av = attrs.value("lockpostodata");
      if (!av.isNull()) {
        bool lock_pos_to_data = QVariant(av.toString()).toBool();
        setLockPosToData(lock_pos_to_data);
        if (lock_pos_to_data) {
          qreal x=0, y=0, w=0.1, h=0.1;
          av = attrs.value("datarect_x");
          if (!av.isNull()) {
            x = av.toString().toDouble();
          }
          av = attrs.value("datarect_y");
          if (!av.isNull()) {
            y = av.toString().toDouble();
          }
          av = attrs.value("datarect_width");
          if (!av.isNull()) {
            w = av.toString().toDouble();
          }
          av = attrs.value("datarect_height");
          if (!av.isNull()) {
            h = av.toString().toDouble();
          }
          _dataRelativeRect = QRectF(x,y,w,h);
        }
      } else {
        setLockPosToData(false);
      }
    } else if (xml.name().toString() == "transform") {
      knownTag = true;
      qreal m11 = 1.0, m12 = 0, m13 = 0, m21 = 0, m22 = 1.0, m23 = 0, m31 = 0, m32= 0, m33 = 1.0;
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

  QList<QGraphicsItem*> list = QGraphicsItem::childItems();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    if (viewItem) {
      viewItem->setView(parentView);
    }
  }
}


ViewItem *ViewItem::parentViewItem() const {
  return dynamic_cast<ViewItem*>(parentItem());
}


void ViewItem::setParentViewItem(ViewItem* parent) {
  QGraphicsItem::setParentItem(parent);
  updateRelativeSize(true);
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

  foreach (QGraphicsItem *item, QGraphicsItem::childItems()) {
    if (item->parentItem() != this)
      continue;

    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);

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

  int base = _dpi*11.0/96.0 ;

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

  _dpi = painter->device()->logicalDpiX();

  QPen rescaled_pen(_storedPen);
  rescaled_pen.setWidthF(Curve::lineDim(painter->window(),rescaled_pen.widthF()));
  setPen(rescaled_pen);

  painter->save();
  painter->setPen(pen());
  painter->setBrush(brush());
  if (_lockPosToData) {
    PlotRenderItem *render_item = dynamic_cast<PlotRenderItem *>(parentViewItem());
    if (render_item) {
      QPolygonF PF = mapFromParent(render_item->rect());
      QPainterPath path;
      path.addPolygon(PF);
      painter->setClipPath(path);
    }
  }
  paint(painter); //this is the overload that subclasses should use...
  if (!view()->isPrinting() && !view()->childMaximized()) {
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
        check.setSize(QSizeF(check.width() / 3.5, check.height() / 3.5));
        check.moveCenter(checkBox().controlPointRect().center());
        QPainterPath p;
        p.addEllipse(check);
        painter->fillPath(p, Qt::black);
      }
      if (isTiedZoom()) {
        painter->save();
        QColor c = Qt::black;
        c.setAlphaF(c.alphaF() * 0.6);
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
  }
  painter->restore();
}


void ViewItem::paint(QPainter *painter) {
  Q_UNUSED(painter);
}


void ViewItem::edit() {
  if (!_editDialog) {
    _editDialog = new ViewItemDialog(this, kstApp->mainWindow());
  }
  _editDialog->show();
  _editDialog->raise();
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
    layout->createLayout(false);
  } else if (view()) {
    view()->createLayout(false);
  }
}

void ViewItem::createProtectedLayout() {
  if (parentViewItem()) {
    LayoutCommand *layout = new LayoutCommand(parentViewItem());
    layout->createLayout(true);
  } else if (view()) {
    view()->createLayout(true);
  }
}


void ViewItem::createCustomLayout(int columns) {
  bool ok = true;
  int default_cols = qMax(1,int(sqrt((qreal)Data::self()->plotList().count())));

  if (columns<1) {
    columns = QInputDialog::getInt(view(), tr("Kst: Column Layout"),
                                       tr("Layout in columns in order of creation.\nSelect number of columns:"),default_cols, 1,
                                       15, 1, &ok);
  }

  if (ok) {
    if (parentViewItem() && false) {
      LayoutCommand *layout = new LayoutCommand(parentViewItem());
      layout->createLayout(false, columns);
    } else if (view()) {
      view()->createLayout(false, columns);
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
    qreal x0 = qMin(qreal(0.0), poly.last().x());
    qreal y0 = qMin(qreal(0.0), poly.last().y());
    QRectF newRect(x0, y0, fabs(poly.last().x()), fabs(poly.last().y()));
    setViewRect(newRect);
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseRelease));
    qreal x0 = qMin(qreal(0.0), poly.last().x());
    qreal y0 = qMin(qreal(0.0), poly.last().y());
    QRectF newRect(x0, y0, fabs(poly.last().x()), fabs(poly.last().y()));

    if (!newRect.isValid()) {
      newRect = newRect.normalized();
      newRect.setWidth(qMax(qreal(3.0), newRect.width()));
      newRect.setHeight(qMax(qreal(3.0), newRect.height()));
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
    setZValue(DRAWING_ZORDER);
    emit creationComplete();
    return;
  }
}



void ViewItem::creationPolygonChangedFixedAspect(View::CreationEvent event, qreal aspect) {

  if (event == View::EscapeEvent) {
    ViewItem::creationPolygonChanged(event);
    return;
  }

  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));
    setRect(0,0,4,4);
    view()->scene()->addItem(this);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseMove));

    QPointF offset = lockOffset(poly.last(), aspect, false);

    if (offset.x()<5.0) {
      offset.setX(5.0);
      offset.setY(5.0/aspect);
    }

    setViewRect(0,0,offset.x(), offset.y());
    return;
  }

  if (event == View::MouseRelease) {
    view()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    view()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    view()->setMouseMode(View::Default);

    updateViewItemParent();
    _creationState = Completed;
    setZValue(DRAWING_ZORDER);
    emit creationComplete();
    return;
  }
}


void ViewItem::addTitle(QMenu *menu) const {
  QWidgetAction *action = new QWidgetAction(menu);
  action->setEnabled(false);

  QLabel *label = new QLabel(tr("%1 Menu", "title of menu for object type arg1").arg(typeName()), menu);
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


  //if (!(lockParent() || (parentViewItem() && parentViewItem()->lockParent()))) {
  QMenu *layoutMenu = menu.addMenu(tr("Cleanup Layout"));
  layoutMenu->setTitle(tr("Cleanup Layout"));
  layoutMenu->addAction(_autoLayoutAction);
  //layoutMenu->addAction(_protectedLayoutAction);
  layoutMenu->addAction(_customLayoutAction);
  layoutMenu->addAction(_oneColumnLayoutAction);
  layoutMenu->addAction(_twoColumnLayoutAction);
  layoutMenu->addAction(_threeColumnLayoutAction);
  //}

  menu.addAction(_editAction);

  addToMenuForContextEvent(menu);

  menu.addSeparator();

  if (!(lockParent() || (parentViewItem() && parentViewItem()->lockParent()))) {
    menu.addAction(_raiseAction);
    menu.addAction(_lowerAction);
    menu.addAction(_deleteAction);
  }

  if (dataPosLockable()) {
    menu.addSeparator();
    menu.addAction(_lockPosToDataAction);
  }


  menu.exec(event->screenPos());
}

bool ViewItem::dataPosLockable() const {
  return bool(dynamic_cast<PlotRenderItem *>(parentViewItem()));
}

void ViewItem::addToMenuForContextEvent(QMenu &menu) {
  Q_UNUSED(menu);
}

void ViewItem::startDragging(QWidget *widget, const QPointF& hotspot) {

  QPointF old_topleft = rect().topLeft();
  normalizePosition();
  QPointF new_hotspot = hotspot.toPoint() + rect().topLeft() - old_topleft;

  // UNDO tied zoom settings done in PlotItem::mousePressEvent
  setTiedZoom(false, false);

  QDrag *drag = new QDrag(widget);
  MimeDataViewItem* mimeData = new MimeDataViewItem;
  mimeData->item = this;
  mimeData->hotSpot = new_hotspot;

  qreal theta = rotationAngle()*ONE_PI/180.0;
  qreal w = fabs(rect().width()*cos(theta)) + fabs(rect().height()*sin(theta));
  qreal h = fabs(rect().width()*sin(theta)) + fabs(rect().height()*cos(theta));

  int device_pixel_ratio = view()->devicePixelRatio();

  QPixmap pixmap(device_pixel_ratio*(w+2), device_pixel_ratio*(h+2));

  pixmap.setDevicePixelRatio(device_pixel_ratio);

  if (ApplicationSettings::self()->transparentDrag()) {
    pixmap.fill(Qt::transparent);
  } else {
    //pixmap.fill(brush().color());
    pixmap.fill(view()->backgroundBrush().color());
  }
  QPainter painter(&pixmap);

  qreal x1 = -rect().height()*sin(theta);
  qreal x3 = rect().width()*cos(theta);
  qreal x2 = x1+x3;
  qreal dx;

  dx = qMin(qreal(0.0), x1);
  dx = qMin(x2,dx);
  dx = qMin(x3,dx);

  qreal y1 = rect().height()*cos(theta);
  qreal y3 = rect().width()*sin(theta);
  qreal y2 = y1+y3;
  qreal dy;

  dy = qMin(qreal(0.0), y1);
  dy = qMin(y2,dy);
  dy = qMin(y3,dy);

  painter.translate(-dx,-dy);
  painter.rotate(rotationAngle());
  painter.translate(-rect().left(), -rect().top());

  painter.setPen(pen());
  //painter.setBrush(brush());

  QBrush brush_hold = brush();
  setBrush(Qt::NoBrush);
  paint(&painter);
  setBrush(brush_hold);

  // TODO also paint annotations
  paintChildItems(painter);
  painter.end();

  drag->setPixmap(pixmap);
  mimeData->setImageData(pixmap.toImage());
  drag->setMimeData(mimeData);

  qreal hx = new_hotspot.toPoint().x()-rect().left();
  qreal hy = new_hotspot.toPoint().y()-rect().top();
  qreal hx_r = hx * cos(theta) - hy * sin(theta);
  qreal hy_r = hy * cos(theta) + hx * sin(theta);
  drag->setHotSpot(QPoint(hx_r-dx,hy_r-dy));

  dropHotSpot = QPoint(hx_r-dx-w/2-1,hy_r-dy-h/2-1);

  hide();
  Qt::DropActions dact = Qt::MoveAction;
  Qt::DropAction dropAction = drag->exec(dact);
  if (dropAction != Qt::MoveAction) {
    show();
  }
  kstApp->mainWindow()->document()->setChanged(true);

}

void ViewItem::paintChildItems(QPainter &painter) {
  QList<QGraphicsItem*> children = childItems();
  foreach(QGraphicsItem* child, children) {
    ViewItem* item = dynamic_cast<ViewItem*>(child);
    if (item) {
      painter.save();
      painter.translate(item->pos().x(),
                        item->pos().y());
      painter.rotate(item->rotationAngle());
      item->paint(&painter);
      item->paintChildItems(painter);
      painter.restore();
    }
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
  updateRelativeSize(true);
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

void ViewItem::normalizePosition() {

  qreal parentWidth = parentRect().width();
  qreal parentHeight = parentRect().height();
  qreal parentX = parentRect().x();
  qreal parentY = parentRect().y();

  qreal w = relativeWidth() * parentWidth;
  qreal h = relativeHeight() * parentHeight;

  setPos(parentX + relativeCenter().x()*parentWidth,
         parentY + relativeCenter().y()*parentHeight);


  setViewRect(-w/2, -h/2, w, h);

  QTransform transform;
  transform.rotate(rotationAngle());

  setTransform(transform);
  //updateRelativeSize();
  //updateViewItemParent();

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


bool ViewItem::updateViewItemParent(bool force_toplevel) {
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
           << "collision count:" << collisions.count();
#endif

  //Doesn't collide then reparent to top-level
  if (collisions.isEmpty() && !topLevel) {
#if DEBUG_REPARENT
    qDebug() << "reparent to topLevel";

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0));
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#if DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0));
#endif

    setParentViewItem(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#if DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0));
#endif

    return true;
  }

  if (!force_toplevel) {
    //Look for collisions that completely contain us
    foreach (QGraphicsItem *item, collisions) {
      ViewItem *viewItem = dynamic_cast<ViewItem*>(item);

      if (!viewItem || !viewItem->acceptsChildItems() || isAncestorOf(viewItem) || !collidesWithItem(viewItem, Qt::ContainsItemBoundingRect)) {
#if DEBUG_REPARENT
        qDebug() << "rejecting collision" << viewItem << !viewItem->acceptsChildItems() <<
                    isAncestorOf(viewItem) << !collidesWithItem(viewItem, Qt::ContainsItemBoundingRect);
#endif
        continue;
      }

      if (parentItem() == viewItem) { /*already done*/
#if DEBUG_REPARENT
        qDebug() << "already in containing parent";
#endif
        return false;
      }

#if DEBUG_REPARENT
      qDebug() << "reparent to" << viewItem;

      qDebug() << "before transform"
               << "origin:" << mapToScene(QPointF(0,0));
#endif

      if (!topLevel) { /*bring the old parent's transform with us*/
        setTransform(parentItem()->transform(), true);
      }

      /*cancel out the new parent's initial transform*/
      setTransform(viewItem->transform().inverted(), true);

#if DEBUG_REPARENT
      qDebug() << "after transform"
               << "origin:" << mapToScene(QPointF(0,0));
#endif

      setParentViewItem(viewItem);
      setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
      updateRelativeSize(true);

#if DEBUG_REPARENT
      qDebug() << "after new parent"
               << "origin:" << mapToScene(QPointF(0,0));
#endif

      return true;
    }
  }
  //No suitable collisions then reparent to top-level
  if (!topLevel) {
#if DEBUG_REPARENT
    qDebug() << "reparent to topLevel";

    qDebug() << "before transform"
             << "origin:" << mapToScene(QPointF(0,0));
#endif

    /*bring the old parent's transform with us*/
    setTransform(parentItem()->transform(), true);

#if DEBUG_REPARENT
    qDebug() << "after transform"
             << "origin:" << mapToScene(QPointF(0,0));
#endif

    setParentViewItem(0);
    setPos(mapToParent(mapFromScene(origin)) + pos() - mapToParent(QPointF(0,0)));
    updateRelativeSize();

#if DEBUG_REPARENT
    qDebug() << "after new parent"
             << "origin:" << mapToScene(QPointF(0,0));
#endif

    return true;
  }
  return false;
}


void ViewItem::updateDataRelativeRect(bool force) {
  CartesianRenderItem* plot = dynamic_cast<CartesianRenderItem*>(parentViewItem());
  if (plot) {
    if ((!lockPosToData()) || force) {
      qreal rotation = rotationAngle();
      QTransform transform;
      setTransform(transform);
      QPointF top_left = mapToParent(rect().topLeft());
      QPointF bottom_right = mapToParent(rect().bottomRight());
      QRectF localRect(top_left, bottom_right);
      _dataRelativeRect = plot->plotItem()->mapToProjection(localRect);
      transform.rotate(rotation);
      setTransform(transform);
    }
  }
}

void ViewItem::updateRelativeSize(bool force_data) {
  if (parentViewItem()) {
    QPointF P;
    qreal parentHeight = parentViewItem()->height() == 0 ? 1 : parentViewItem()->height();
    qreal parentWidth  = parentViewItem()->width() == 0 ? 1 : parentViewItem()->width();
    _parentRelativeHeight = (height() / parentHeight);
    _parentRelativeWidth = (width() / parentWidth);

    P =  mapToParent(rect().center()) - parentViewItem()->rect().topLeft();
    _parentRelativeCenter =  QPointF(P.x() / parentWidth,
                                     P.y() / parentHeight);
    P =  mapToParent(rect().topLeft()) - parentViewItem()->rect().topLeft();
    _parentRelativePosition =  QPointF(P.x() / parentWidth,
                                       P.y() / parentHeight);
    P =  mapToParent(rect().bottomLeft()) - parentViewItem()->rect().topLeft();
    _parentRelativeLeft = QPointF(P.x() / parentWidth,
                                      P.y() / parentHeight);
    P =  mapToParent(rect().bottomRight()) - parentViewItem()->rect().topLeft();
    _parentRelativeRight =  QPointF(P.x() / parentWidth,
                                    P.y() / parentHeight);
    updateDataRelativeRect(force_data);
   } else if (view()) {
    QPointF P;
    _parentRelativeHeight = (height() / view()->height());
    _parentRelativeWidth = (width() / view()->width());
    P =  mapToParent(rect().center()) - view()->rect().topLeft();
    _parentRelativeCenter =  QPointF(P.x() / view()->width(),
                                     P.y() / view()->height());
    P =  mapToParent(rect().topLeft()) - view()->rect().topLeft();
    _parentRelativePosition =  QPointF(P.x() / view()->width(),
                                       P.y() / view()->height());
    P =  mapToParent(rect().bottomLeft()) - view()->rect().topLeft();
    _parentRelativeLeft =  QPointF(P.x() / view()->width(),
                                   P.y() / view()->height());
    P =  mapToParent(rect().bottomRight()) - view()->rect().topLeft();
    _parentRelativeRight =  QPointF(P.x() / view()->width(),
                                   P.y() / view()->height());
  } else {
    _parentRelativeHeight = 0;
    _parentRelativeWidth = 0;
    _parentRelativeCenter = QPointF(0, 0);
    _parentRelativePosition = QPointF(0, 0);
    _parentRelativeLeft = QPointF(0, 0);
    _parentRelativeRight = QPointF(0, 0);
  }

  emit relativeSizeUpdated();
}


void ViewItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
  Q_UNUSED(oldParentRect);

  QRectF itemRect = rect();
  //Lock aspect ratio for rotating objects or children with a lockedAspectRatio
  //FIXME is the child rotated with respect to the parent is the real question...
  if (transform().isRotating() || lockAspectRatio()) {
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

    QPointF newTopLeft = newParentRect.topLeft() - itemRect.topLeft() +
                         QPointF(newParentRect.width() * _parentRelativePosition.x(),
                                 newParentRect.height() * _parentRelativePosition.y());

    itemRect.setWidth(newWidth);
    itemRect.setHeight(newHeight);
    setPos(newTopLeft);
  }

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
  kstApp->mainWindow()->document()->setChanged(true);

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
    //view()->setCursor(Qt::SizeAllCursor);
  }
}


void ViewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  _hovering = true;
  update();
}


void ViewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  //view()->setCursor(Qt::ArrowCursor);

  _hovering = false;
  update();
}


QVariant ViewItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
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
  updateRelativeSize(true);
}

void ViewItem::setItemPos(qreal x, qreal y)
{
  if (_lockPosToData) {
    QRectF dr = dataRelativeRect();
    dr.moveCenter(QPointF(x,y));
    setDataRelativeRect(dr);
    applyDataLockedDimensions();
  } else {
    QRectF parent_rect = parentRect();
    qreal parentWidth = parent_rect.width();
    qreal parentHeight = parent_rect.height();
    qreal parentX = parent_rect.x();
    qreal parentY = parent_rect.y();

    x = x*parentWidth + parentX;
    y = y*parentHeight + parentY;

    setPos(x,y);
  }
}

void ViewItem::setItemSize(qreal w, qreal h)
{
  if (_lockPosToData) {
    QRectF dr = dataRelativeRect();
    QPointF center = dr.center();
    dr.setWidth(w);
    if (h>0) {
      dr.setHeight(h);
    }
    dr.moveCenter(center);
    setDataRelativeRect(dr);
    applyDataLockedDimensions();
  } else {
    QRectF parent_rect = parentRect();
    qreal parentWidth = parent_rect.width();
    qreal parentHeight = parent_rect.height();

    qreal width = w * parentWidth;
    qreal height;

    if (lockAspectRatio()) {
      qreal aspectRatio;
      if (rect().width() > 0) {
        aspectRatio = qreal(rect().height()) / qreal(rect().width());
      } else {
        aspectRatio = 10000.0;
      }
      height = width * aspectRatio;
    } else  if (h < 0.0) {
      height = rect().height();
    } else {
      height = h * parentHeight;
    }

    setViewRect(-width/2, -height/2, width, height);
  }
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
  _shortcutMap.insert(action->shortcut().toString(), action);
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


qreal ViewItem::rotationAngleRadians() const {
  return atan2(transform().m12(), transform().m11());
}


void ViewItem::setSupportsTiedZoom(const bool supports) {
  if (supports != _supportsTiedZoom) {

    _supportsTiedZoom = supports;

    if (_supportsTiedZoom && ((layoutMargins().width() < tiedZoomSize().width()) || (layoutMargins().height() < tiedZoomSize().height()))) {
      setLayoutMargins(layoutMargins().expandedTo(tiedZoomSize()));
    }

    if (!_supportsTiedZoom) {
      setTiedZoom(false, false, false);
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

ScriptInterface* ViewItem::createScriptInterface() {
  return new ViewItemSI(this);
}

ScriptInterface* ViewItem::scriptInterface() {
  if (!_interface) {
    _interface = createScriptInterface();
  }
  return _interface;
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
  kstApp->mainWindow()->document()->setChanged(true);
}


void LayoutCommand::undo() {
  Q_ASSERT(_layout);
  _layout->reset();
}


void LayoutCommand::redo() {
  Q_ASSERT(_layout);
  _layout->apply();
}


void LayoutCommand::createLayout(bool preserve, int columns) {
  Q_ASSERT(_item);
  Q_ASSERT(_item->view());

  QList<ViewItem*> viewItems;
  QList<QGraphicsItem*> list = _item->QGraphicsItem::childItems();
  if (list.isEmpty()) {
    return; //not added to undostack
  }

  viewItems = _item->view()->layoutableViewItems();

  if (viewItems.isEmpty()) {
    return; //not added to undostack
  }

  _layout = new ViewGridLayout(_item);

  FormatGridHelper grid(viewItems, preserve);

  if (columns == 0) {
    int n_view_items = viewItems.size();
    for (int i_view_item = 0; i_view_item<n_view_items; i_view_item++) {
      ViewItem *v = viewItems.at(i_view_item);
      struct AutoFormatRC rc = grid.rcList.at(i_view_item);
      _layout->addViewItem(v, rc.row, rc.col, rc.row_span, rc.col_span);
    }

  } else {
    int row = 0;
    int col = 0;
    int n_view_items = viewItems.size();

    for (int i_view_item = 0; i_view_item<n_view_items; i_view_item++) {
      ViewItem *v = viewItems.at(i_view_item);
      _layout->addViewItem(v, row, col, 1, 1);
      col++;
      if (col>=columns) {
        col = 0;
        row++;
      }
    }
  }
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

  if (layout == CurvePlacement::Auto) {
    columns = 0;
  }

  if (layout == CurvePlacement::Custom) layout = CurvePlacement::Protect;
  if (layout == CurvePlacement::Protect) {
    _layout = new ViewGridLayout(_item);

    QPointF center = _item->view()->sceneRect().center();
    center -= QPointF(100.0, 100.0);

    item->setPos(center);
    item->setViewRect(0.0, 0.0, 200.0, 200.0);
    _item->view()->scene()->addItem(item);
    //_item->view()->undoStack()->push(this);
    return;
  }


  QList<ViewItem*> viewItems;
  viewItems = _item->view()->layoutableViewItems();

  _layout = new ViewGridLayout(_item);

  FormatGridHelper grid(viewItems);

  if (grid.n_cols == columns) {
    if (grid.numHoles()<columns) {
      columns = 0; // already in correct columns - just line stuff up
    }
  }

  if (columns == 0) {
    int row = -1;
    int col = -1;
    for (int i_col = 0; i_col<grid.n_cols; i_col++) {
      for (int i_row = 0; i_row<grid.n_rows; i_row++) {
        if (grid.a[i_row][i_col]==0) {
          row = i_row;
          col = i_col;
          break;
        }
        if (row>=0) {
          break;
        }
      }
    }
    if (row<0) { // no empty slots
      if (grid.n_rows>grid.n_cols) { // add a column
        row = 0;
        col = grid.n_cols;
      } else { // add a row
        row = grid.n_rows;
        col = 0;
      }
    }

    int n_views = viewItems.size();
    for (int i_view = 0; i_view<n_views; i_view++) {
      ViewItem *v = viewItems.at(i_view);
      struct AutoFormatRC rc = grid.rcList.at(i_view);
      _layout->addViewItem(v, rc.row, rc.col, rc.row_span, rc.col_span);
    }
    _item->view()->scene()->addItem(item);
    _layout->addViewItem(item, row, col, 1,1);
  } else {
    int row = 0;
    int col = 0;
    int n_views = viewItems.size();

    for (int i_view = 0; i_view<n_views; i_view++) {
      ViewItem *v = viewItems.at(i_view);
      _layout->addViewItem(v, row, col, 1, 1);
      col++;
      if (col>=columns) {
        col = 0;
        row++;
      }
    }
    _item->view()->scene()->addItem(item);
    _layout->addViewItem(item, row, col, 1,1);
    _layout->setColumnCount(columns);
  }

  if (qobject_cast<LayoutBoxItem*>(_item)) {
    QObject::connect(_layout, SIGNAL(enabledChanged(bool)),
                    _item, SLOT(setEnabled(bool)));
  }

  _layout->apply();
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
