/***************************************************************************
                          kstscalarlistview.cpp
                             -------------------
    begin                : Tue Nov 21 2006
    copyright            : (C) 2006 University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "klocale.h"

#include <qdebug.h>
#include "kstscalar.h"

#include "kstscalarlistview.h"
//Added by qt3to4:
#include <Q3ValueList>

KstScalarListViewItem::KstScalarListViewItem(KstScalarListView *parent, KstObjectTreeNode<KstScalar> *node) : KListViewItem(parent), _node(node) {
}

KstScalarListViewItem::KstScalarListViewItem(KstScalarListViewItem *parent, KstObjectTreeNode<KstScalar> *node) : KListViewItem(parent), _node(node) {
}

QString KstScalarListViewItem::text(int column) const {
  if (!_node) {
    return QString::null;
  }

  switch (column) {
    case 0:
      return _node->nodeTag();
    case 1:
      {
        KstScalar *s = dynamic_cast<KstScalar*>(_node->object());
        if (s) {
          return s->label();
        } else {
          return QString::null;
        }
      }
    default:
      return QString::null;
  }
}


KstScalarListView::KstScalarListView(QWidget *parent, KstObjectCollection<KstScalar> *coll) : KListView(parent), _coll(coll) {
  addColumn(i18n("Scalar"));
  addColumn(i18n("Value"));

  setRootIsDecorated(true);
  setAllColumnsShowFocus(true);

  update();
}


static void addChildItems(KstScalarListViewItem *parentItem, KstObjectTreeNode<KstScalar> *parentNode) {
  if (!parentItem || !parentNode) {
    return;
  }

  Q3ValueList<KstObjectTreeNode<KstScalar>*> children = parentNode->children().values();
  for (Q3ValueList<KstObjectTreeNode<KstScalar>*>::ConstIterator i = children.begin(); i != children.end(); ++i) {
    KstScalarListViewItem *item = new KstScalarListViewItem(parentItem, *i);
    addChildItems(item, *i);
  }
}

void KstScalarListView::update() {
  clear();

  if (_coll) {
    KstReadLocker(&_coll->lock());
    Q3ValueList<KstObjectTreeNode<KstScalar>*> rootItems = _coll->nameTreeRoot()->children().values();
    for (Q3ValueList<KstObjectTreeNode<KstScalar>*>::ConstIterator i = rootItems.begin(); i != rootItems.end(); ++i) {
      KstScalarListViewItem *item = new KstScalarListViewItem(this, *i);
      addChildItems(item, *i);
    }
  }

/*
  qDebug() << "Updated KstScalarListView: now " << childCount() << " root-child items" << endl;
  QListViewItemIterator it(this);
  while (it.current()) {
    KstScalarListViewItem *item = dynamic_cast<KstScalarListViewItem*>(it.current());
    if (item) {
      qDebug() << "  " << item->node()->fullTag().join(KstObjectTag::tagSeparator) << ": " << item->text(0) << ", " << item->text(1) << endl;
    }
    ++it;
  }
*/

  KListView::update();
}

// vim: ts=2 sw=2 et
