/***************************************************************************
                          kststringlistview.h  -  description
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

#ifndef KSTSTRINGLISTVIEW_H
#define KSTSTRINGLISTVIEW_H

#include <klistview.h>

#include "kstobject.h"
#include "kstobjectcollection.h"

class KstStringListView : public KListView
{
  public:
    KstStringListView(QWidget *parent = 0, KstObjectCollection<KstString> *coll = NULL);

    void update();

  private:
    KstObjectCollection<KstString> *_coll;
};


class KstStringListViewItem : public KListViewItem
{
  public:
    KstStringListViewItem(KstStringListView *parent, KstObjectTreeNode<KstString> *node);
    KstStringListViewItem(KstStringListViewItem *parent, KstObjectTreeNode<KstString> *node);

    QString text(int column) const;

    KstObjectTreeNode<KstString> *node() const { return _node; }

  private:
    KstObjectTreeNode<KstString> *_node;
};

#endif
// vim: ts=2 sw=2 et
