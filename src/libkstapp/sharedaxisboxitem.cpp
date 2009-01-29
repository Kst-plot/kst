/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sharedaxisboxitem.h"

#include "viewitemzorder.h"
#include "plotitem.h"

#include "gridlayouthelper.h"
#include "viewgridlayout.h"

#include "application.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QMenu>

namespace Kst {

SharedAxisBoxItem::SharedAxisBoxItem(View *parent)
    : ViewItem(parent), _layout(0), _loaded(false), _creationStarted(false) {
  setName("Shared Axis Box");
  setZValue(SHAREDAXISBOX_ZVALUE);
  setBrush(Qt::transparent);

  _breakAction = new QAction(tr("Break Shared Axis Box"), this);
  _breakAction->setShortcut(Qt::Key_B);
  registerShortcut(_breakAction);
  connect(_breakAction, SIGNAL(triggered()), this, SLOT(breakShare()));

  connect(this, SIGNAL(breakShareSignal()), this, SLOT(breakShare()));
}


SharedAxisBoxItem::~SharedAxisBoxItem() {
}


void SharedAxisBoxItem::paint(QPainter *painter) {
  painter->drawRect(rect());
}


void SharedAxisBoxItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("sharedaxisbox");
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


bool SharedAxisBoxItem::acceptItems() {
  bool bReturn = false;

  if (_loaded) {
    return true;
  } else {
    _loaded = true;
  }

  QRectF maxSize(mapToParent(viewRect().topLeft()), mapToParent(viewRect().bottomRight()));
  ViewItem* child = 0;
  if (parentView()) {
    QList<QGraphicsItem*> list = parentView()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem || !viewItem->isVisible() || viewItem == this ||  viewItem == parentItem() || !collidesWithItem(viewItem, Qt::IntersectsItemBoundingRect)) {
        continue;
      }

      if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
        if (plotItem->parentItem()) {
          ViewItem *parent = static_cast<ViewItem*>(plotItem->parentItem());
          SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(parent);
          if (shareBox) {
            shareBox->breakShare();
          } else if (parent != parentItem()) {
            continue;
          }
        } else if (parentItem()) {
          continue;
        }
        plotItem->setSharedAxisBox(this);
        child = plotItem;
        if (!maxSize.contains(plotItem->mapToParent(plotItem->viewRect().topLeft()))) {
          maxSize.setTop(qMin(plotItem->mapToParent(plotItem->viewRect().topLeft()).y(), maxSize.top()));
          maxSize.setLeft(qMin(plotItem->mapToParent(plotItem->viewRect().topLeft()).x(), maxSize.left()));
        }
        if (!maxSize.contains(plotItem->mapToParent(plotItem->viewRect().bottomRight()))) {
          maxSize.setBottom(qMax(plotItem->mapToParent(plotItem->viewRect().bottomRight()).y(), maxSize.bottom()));
          maxSize.setRight(qMax(plotItem->mapToParent(plotItem->viewRect().bottomRight()).x(), maxSize.right()));
        }
      }
    }
    if (child) {
      setPen(QPen(Qt::white));
      setBrush(Qt::white);
      ViewGridLayout::updateProjections(this);
      sharePlots();
      bReturn =  true;
    }
  }
  if (maxSize != viewRect()) {
    setPos(maxSize.topLeft());
    setViewRect(QRectF(mapFromParent(maxSize.topLeft()), mapFromParent(maxSize.bottomRight())));
  }
  return bReturn;
}


void SharedAxisBoxItem::breakShare() {
  _loaded = false;
  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
      plotItem->setPos(mapToParent(plotItem->pos()));
      plotItem->setSharedAxisBox(0);
      plotItem->setLabelsVisible(true);
      plotItem->update();
    }
  }
  if (_layout) {
    _layout->reset();
  }
  hide();
}


void SharedAxisBoxItem::lockItems() {
  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
      plotItem->setAllowedGripModes(0);
      plotItem->setFlags(0);
      plotItem->setTiedZoom(true);
    }
  }
  if (!list.isEmpty()) {
    setPen(QPen(Qt::white));
    setBrush(Qt::white);
  }
}


void SharedAxisBoxItem::addToMenuForContextEvent(QMenu &menu) {
  menu.addAction(_breakAction);
}


void SharedAxisBoxItem::triggerContextEvent(QGraphicsSceneContextMenuEvent *event) {
  contextMenuEvent(event);
}


void SharedAxisBoxItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    ViewItem::creationPolygonChanged(event);
    _creationStarted = true;
    return;
  }

  if (event == View::MouseMove) {
    ViewItem::creationPolygonChanged(event);
    if (!_creationStarted) {
      return;
    }

    QList<PlotItem*> plots;
    if (parentView()) {
      QList<QGraphicsItem*> list = parentView()->items();
      foreach (QGraphicsItem *item, list) {
        ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
        if (!viewItem || !viewItem->isVisible() || viewItem == this ||  viewItem == parentItem() || !collidesWithItem(viewItem, Qt::IntersectsItemBoundingRect)) {
          continue;
        }
        if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
          plots.append(plotItem);
        }
      }
      highlightPlots(plots);
    }
    return;
  }

  if (event == View::EscapeEvent || event == View::MouseRelease) {
    ViewItem::creationPolygonChanged(event);
    highlightPlots(QList<PlotItem*>());
    return;
  }
}


void SharedAxisBoxItem::highlightPlots(QList<PlotItem*> plots) {
  QList<PlotItem*> currentlyHighlighted = _highlightedPlots;
  _highlightedPlots.clear();

  foreach(PlotItem *plotItem, plots) {
    _highlightedPlots.append(plotItem);
    if (!currentlyHighlighted.contains(plotItem)) {
      plotItem->setHighlighted(true);
      plotItem->update();
    }
  }

  foreach(PlotItem* plotItem, currentlyHighlighted) {
    if (!_highlightedPlots.contains(plotItem)) {
      plotItem->setHighlighted(false);
      plotItem->update();
    }
  }
}


void CreateSharedAxisBoxCommand::createItem() {
  _item = new SharedAxisBoxItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


void CreateSharedAxisBoxCommand::undo() {
  Q_ASSERT(_item);
  _item->hide();
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(_item);
  if (shareBox) {
    shareBox->breakShare();
  }
}


void CreateSharedAxisBoxCommand::redo() {
  Q_ASSERT(_item);
  _item->show();
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(_item);
  if (shareBox) {
    if (!shareBox->acceptItems()) {
      _item->hide();
    }
  }
}


void CreateSharedAxisBoxCommand::creationComplete() {
  Q_ASSERT(_item);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(_item);
  if (shareBox) {
    if (shareBox->acceptItems()) {
      CreateCommand::creationComplete();
    } else {
      delete _item;
      deleteLater();
      kstApp->mainWindow()->clearDrawingMarker();
    }
  }
}


SharedAxisBoxItemFactory::SharedAxisBoxItemFactory()
: GraphicsFactory() {
  registerFactory("sharedaxisbox", this);
}


SharedAxisBoxItemFactory::~SharedAxisBoxItemFactory() {
}


ViewItem* SharedAxisBoxItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  SharedAxisBoxItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "sharedaxisbox") {
        Q_ASSERT(!rc);
        rc = new SharedAxisBoxItem(view);
        if (parent) {
          rc->setParent(parent);
        }
        // Add any new specialized SharedAxisBoxItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "sharedaxisbox") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating sharedaxisbox object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  rc->lockItems();
  return rc;
}

}

// vim: ts=2 sw=2 et
