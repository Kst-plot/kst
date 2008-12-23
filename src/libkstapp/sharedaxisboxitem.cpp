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

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QMenu>

namespace Kst {

SharedAxisBoxItem::SharedAxisBoxItem(View *parent)
    : ViewItem(parent), _layout(0) {
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
  xml.writeStartElement("sharedaxisbox");
  ViewItem::save(xml);
  xml.writeEndElement();
}


void SharedAxisBoxItem::acceptItems() {
  ViewItem* child = 0;
  if (parentView()) {
    QList<QGraphicsItem*> list = parentView()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem || !viewItem->isVisible() || viewItem == this ||  !collidesWithItem(viewItem, Qt::IntersectsItemBoundingRect)) {
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
        }
        plotItem->setParent(this);
        plotItem->setAllowedGripModes(0);
        plotItem->setFlags(0);
        child = plotItem;
      }
    }
    if (child) {
      setBrush(Qt::white);

      QList<ViewItem*> viewItems;
      QList<QGraphicsItem*> list = QGraphicsItem::children();
      if (list.isEmpty())
        return; //not added to undostack

      foreach (QGraphicsItem *item, list) {
        ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
        if (!viewItem || viewItem->hasStaticGeometry() || !viewItem->allowsLayout() || viewItem->parentItem() != this)
          continue;
        viewItems.append(viewItem);
      }

      if (viewItems.isEmpty())
        return; //not added to undostack

      Grid *grid = Grid::buildGrid(viewItems, 0);
      Q_ASSERT(grid);

      _layout = new ViewGridLayout(this);

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

      _layout->apply();
      _layout->apply();
    } else {
      delete this;
    }
  }
}


void SharedAxisBoxItem::breakShare() {
  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    viewItem->setParent(0);
    viewItem->setAllowedGripModes(Move | Resize | Rotate);
    viewItem->setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  }
  if (_layout) {
    _layout->reset();
  }
  hide();
}


void SharedAxisBoxItem::addToMenuForContextEvent(QMenu &menu) {
  menu.addAction(_breakAction);
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
    shareBox->acceptItems();
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
      if (xml.name().toString() == "sharedaxisbox") {
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
  return rc;
}

}

// vim: ts=2 sw=2 et
