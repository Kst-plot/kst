/***************************************************************************
                               kstplotgroup.h
                             -------------------
    begin                : Mar 21, 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef KSTPLOTGROUP_H
#define KSTPLOTGROUP_H

#include <kstmetaplot.h>

class KstPlotGroup : public KstMetaPlot {
  Q_OBJECT
  Q_PROPERTY(bool transparent READ transparent WRITE setTransparent)
  public:
    KstPlotGroup();
    KstPlotGroup(const QDomElement& e);
    KstPlotGroup(const KstPlotGroup& plotGroup);
    ~KstPlotGroup();

    void save(QTextStream& ts, const QString& indent = QString::null);

    bool removeChild(KstViewObjectPtr obj, bool recursive = false);
    bool popupMenu(KMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent);
    bool layoutPopupMenu(KMenu *menu, const QPoint& pos, KstViewObjectPtr topParent);

    void setTransparent(bool transparent);
    bool transparent() const;

    QRegion clipRegion();

    QMap<QString, QVariant> widgetHints(const QString& propertyName) const;

    void paintSelf(KstPainter& p, const QRegion& bounds);
  public slots:
    void copyObject();
    KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
    void flatten();

  protected:
    KstViewObjectFactoryMethod factory() const;
};

typedef KstSharedPtr<KstPlotGroup> KstPlotGroupPtr;
typedef KstObjectList<KstPlotGroupPtr> KstPlotGroupList;


#endif
// vim: ts=2 sw=2 et
