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

#ifndef VIEWITEMMANAGER_H
#define VIEWITEMMANAGER_H

#include <QHash>
#include <QList>

namespace Kst {

class View;
class ViewItem;

class ViewItemManager : public QObject
{
  Q_OBJECT
public:
  static ViewItemManager *self();

  static QList<ViewItem*> viewItemsForView(View *view);
  static QList<ViewItem*> layoutableViewItemsForView(View *view);

  static void clearViewItemsForView(View *view);

private:
  static void cleanup();
  friend class ViewItem;

  ViewItemManager();
  ~ViewItemManager();
  void addViewItem(ViewItem *viewItem);
  void removeViewItem(ViewItem *viewItem);

  QHash< View*, QList<ViewItem*> > _viewItemLists;

signals:
  
public slots:
  
};

}
#endif // VIEWITEMMANAGER_H
