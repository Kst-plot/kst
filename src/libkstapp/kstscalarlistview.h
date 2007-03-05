/***************************************************************************
                          kstscalarlistview.h  -  description
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

#ifndef KSTSCALARLISTVIEW_H
#define KSTSCALARLISTVIEW_H

#include <k3listview.h>

#include "kstobject.h"
#include "kstobjectcollection.h"

class KstScalarListView : public K3ListView
{
  public:
    KstScalarListView(QWidget *parent = 0, KstObjectCollection<KstScalar> *coll = NULL);

    void update();

  private:
    KstObjectCollection<KstScalar> *_coll;
};


class KstScalarListViewItem : public K3ListViewItem
{
  public:
    KstScalarListViewItem(KstScalarListView *parent, KstObjectTreeNode<KstScalar> *node);
    KstScalarListViewItem(KstScalarListViewItem *parent, KstObjectTreeNode<KstScalar> *node);

    QString text(int column) const;

    KstObjectTreeNode<KstScalar> *node() const { return _node; }

  private:
    KstObjectTreeNode<KstScalar> *_node;
};

#endif
// vim: ts=2 sw=2 et
