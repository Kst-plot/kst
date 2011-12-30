/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "viewitemmanager.h"

#include "view.h"
#include "viewitem.h"
#include "plotitem.h"
#include "layoutboxitem.h"

#include <QDebug>
#include <QCoreApplication>


namespace Kst {

static ViewItemManager *_self = 0;

void ViewItemManager::cleanup() {
  delete _self;
  _self = 0;
}

ViewItemManager *ViewItemManager::self() {
  if (!_self) {
    _self = new ViewItemManager;
    qAddPostRoutine(cleanup);
  }
  return _self;
}


ViewItemManager::ViewItemManager() {
}


ViewItemManager::~ViewItemManager() {
}


QList<ViewItem*> ViewItemManager::viewItemsForView(View *view) {
  if (ViewItemManager::self()->_viewItemLists.contains(view)) {
    return ViewItemManager::self()->_viewItemLists.value(view);
  }
  return QList<ViewItem*>();
}


QList<ViewItem*> ViewItemManager::layoutableViewItemsForView(View *view) {
  QList<ViewItem*> view_items = viewItemsForView(view);
  QList<ViewItem*> layoutable_view_items;
  foreach (ViewItem *item, view_items) {
    if (item && (!item->hasStaticGeometry()) &&
        item->allowsLayout() &&
        ((dynamic_cast<LayoutBoxItem*>(item->parentViewItem())!=0) || (!item->parentViewItem()))) {
      layoutable_view_items.append(item);
    }
  }
  return layoutable_view_items;
}


void ViewItemManager::addViewItem(ViewItem *viewItem) {
  if (!_viewItemLists.contains(viewItem->view())) {
    _viewItemLists.insert(viewItem->view(), QList<ViewItem*>() << viewItem);
  } else {
    QList<ViewItem*> list = _viewItemLists.value(viewItem->view());
    list << viewItem;
    _viewItemLists.insert(viewItem->view(), list); // insert replaces in a qhash
  }
}

void ViewItemManager::removeViewItem(ViewItem *viewItem) {
  if (!_viewItemLists.contains(viewItem->view()))
    return;

  QList<ViewItem*> list = _viewItemLists.value(viewItem->view());
  list.removeAll(viewItem);
  _viewItemLists.insert(viewItem->view(), list);
}


void ViewItemManager::clearViewItemsForView(View *view) {
  if (ViewItemManager::self()->_viewItemLists.contains(view)) {
    ViewItemManager::self()->_viewItemLists.remove(view);
  }
}

}
