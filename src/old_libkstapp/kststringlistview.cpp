/***************************************************************************
                          kststringlistview.cpp
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
#include "kststring.h"

#include "kststringlistview.h"
//Added by qt3to4:
#include <Q3ValueList>

KstStringListViewItem::KstStringListViewItem(KstStringListView *parent, KstObjectTreeNode<KstString> *node) : K3ListViewItem(parent), _node(node) {
}

KstStringListViewItem::KstStringListViewItem(KstStringListViewItem *parent, KstObjectTreeNode<KstString> *node) : K3ListViewItem(parent), _node(node) {
}

QString KstStringListViewItem::text(int column) const {
  if (!_node) {
    return QString::null;
  }

  switch (column) {
    case 0:
      return _node->nodeTag();
    case 1:
      {
        KstString *s = dynamic_cast<KstString*>(_node->object());
        if (s) {
          return s->value();
        } else {
          return QString::null;
        }
      }
    default:
      return QString::null;
  }
}


KstStringListView::KstStringListView(QWidget *parent, KstObjectCollection<KstString> *tree) : K3ListView(parent), _coll(tree) {
  addColumn(i18n("String"));
  addColumn(i18n("Value"));

  setRootIsDecorated(true);
  setAllColumnsShowFocus(true);

  update();
}


static void addChildItems(KstStringListViewItem *parentItem, KstObjectTreeNode<KstString> *parentNode) {
  if (!parentItem || !parentNode) {
    return;
  }

  Q3ValueList<KstObjectTreeNode<KstString>*> children = parentNode->children().values();
  for (Q3ValueList<KstObjectTreeNode<KstString>*>::ConstIterator i = children.begin(); i != children.end(); ++i) {
    KstStringListViewItem *item = new KstStringListViewItem(parentItem, *i);
    addChildItems(item, *i);
  }
}

void KstStringListView::update() {
  clear();

  if (_coll) {
    KstReadLocker(&_coll->lock());
    Q3ValueList<KstObjectTreeNode<KstString>*> rootItems = _coll->nameTreeRoot()->children().values();
    for (Q3ValueList<KstObjectTreeNode<KstString>*>::ConstIterator i = rootItems.begin(); i != rootItems.end(); ++i) {
      KstStringListViewItem *item = new KstStringListViewItem(this, *i);
      addChildItems(item, *i);
    }
  }

/*
  qDebug() << "Updated KstStringListView: now " << childCount() << " root-child items" << endl;
  Q3ListViewItemIterator it(this);
  while (it.current()) {
    KstStringListViewItem *item = dynamic_cast<KstStringListViewItem*>(it.current());
    if (item) {
      qDebug() << "  " << item->node()->fullTag().join(KstObjectTag::tagSeparator) << ": " << item->text(0) << ", " << item->text(1) << endl;
    }
    ++it;
  }
*/

  K3ListView::update();
}

// vim: ts=2 sw=2 et
