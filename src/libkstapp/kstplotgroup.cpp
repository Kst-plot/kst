/***************************************************************************
                              kstplotgroup.cpp
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

#include <assert.h>

// include files for Qt
#include <qmetaobject.h>
#include <qpainter.h>

// include files for KDE
#include "ksdebug.h"
#include <klocale.h>

// application specific includes
#include "kst.h"
#include "kstdoc.h"
#include "kstplotgroup.h"
#include "ksttimers.h"
#include "kstviewobjectfactory.h"
#include "kstdatacollection.h"

static int pgcount = 0;

KstPlotGroup::KstPlotGroup()
: KstMetaPlot("PlotGroup") {
  _standardActions |= Delete | Raise | Zoom | Lower | RaiseToTop | LowerToBottom;
  _layoutActions |= Delete | Copy | Raise | Lower | RaiseToTop | LowerToBottom | Rename | MoveTo;
  setTagName(KstObjectTag(i18n("Plot Group %1").arg(++pgcount), KstObjectTag::globalTagContext));  // FIXME: tag context?
  _type = "PlotGroup";
  setBorderColor(Qt::blue);
  _container = false; // plot group is a container that doesn't behave like one
  setTransparent(true);
  setBorderWidth(0);
}


KstPlotGroup::KstPlotGroup(const QDomElement& e)
: KstMetaPlot(e) {
  setBorderWidth(0);
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement();
    if (!el.isNull()) {
      if (metaObject()->findProperty(el.tagName().latin1(), true) > -1) {
        setProperty(el.tagName().latin1(), QVariant(el.text()));
      }
    }
    n = n.nextSibling();
  }

  // always have these values
  _standardActions |= Delete | Raise | Zoom | Lower | RaiseToTop | LowerToBottom;
  _layoutActions |= Delete | Copy | Raise | Lower | RaiseToTop | LowerToBottom | Rename | MoveTo;
  setTagName(KstObjectTag(i18n("Plot Group %1").arg(++pgcount), KstObjectTag::globalTagContext));  // FIXME: tag context?
  _type = "PlotGroup";
  _container = false; // plot group is a container that doesn't behave like one
  setBorderColor(Qt::blue);
}


KstPlotGroup::KstPlotGroup(const KstPlotGroup& plotGroup)
: KstMetaPlot(plotGroup) {
  _type = "PlotGroup";
  _container = false; // plot group is a container that doesn't behave like one

  setTagName(KstObjectTag(i18n("Plot Group %1").arg(++pgcount), KstObjectTag::globalTagContext));  // FIXME: tag context?
}


KstPlotGroup::~KstPlotGroup() {
}


void KstPlotGroup::copyObject() {
  if (_parent) {
    KstApp::inst()->document()->setModified();
    _parent->appendChild(new KstPlotGroup(*this), true);
    QTimer::singleShot(0, KstApp::inst(), SLOT(updateDialogs()));
  }
}


KstViewObject* KstPlotGroup::copyObjectQuietly(KstViewObject& parent, const QString& name) const {
  Q_UNUSED(name)

  KstPlotGroup* plotGroup = new KstPlotGroup(*this);
  parent.appendChild(plotGroup, true);
  
  return plotGroup;
}


bool KstPlotGroup::removeChild(KstViewObjectPtr obj, bool recursive) {
  if (KstViewObject::removeChild(obj, recursive)) {
    if (_children.count() > 1) {
      QRect gg; // = _children.first()->geometry();
      for (KstViewObjectList::ConstIterator it = _children.begin(); it != _children.end(); ++it) {
        gg |= (*it)->geometry();
      }

      _geom = gg;
      updateAspect();
      for (KstViewObjectList::ConstIterator i = _children.begin(); i != _children.end(); ++i) {
        updateAspect();
      }
    } else if (_parent) { // can be false if we are being deleted already
      flatten();
    }

    return true;
  }

  return false;
}


void KstPlotGroup::flatten() {
  assert(_parent);
  for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
    (*i)->setSelected(false);
    (*i)->setFocus(false);
    _parent->insertChildAfter(KstViewObjectPtr(this), *i);
  }
  _parent->removeChild(this);
  KstApp::inst()->document()->setModified();
}


void KstPlotGroup::save(QTextStream& ts, const QString& indent) {
  ts << indent << "<" << type() << ">" << endl;
  KstMetaPlot::save(ts, indent + "  ");
  ts << indent << "</" << type() << ">" << endl;
}


void KstPlotGroup::paintSelf(KstPainter& p, const QRegion& bounds) {
  if (!transparent()) {
    p.save();
    // fill non-children areas with color
    QRegion clipRegion(contentsRect());
    QBrush brush(_backgroundColor);
    for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
      clipRegion -= (*i)->clipRegion();
    }
    p.setClipRegion(clipRegion);
    p.fillRect(contentsRect(), brush);
    p.restore();
  }

  KstMetaPlot::paintSelf(p, bounds);
}


bool KstPlotGroup::popupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topParent) {
  KstMetaPlot::popupMenu(menu, pos, topParent);
  KstViewObjectPtr c = findChild(pos + position());
  if (c) {
    KPopupMenu *s = new KPopupMenu(menu);
    if (c->popupMenu(s, pos - c->position(), topParent)) {
      menu->insertItem(c->tagName(), s);
    } else {
      delete s;
    }
  }
  return true;
}


bool KstPlotGroup::layoutPopupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topParent) {
  KstMetaPlot::layoutPopupMenu(menu, pos, topParent);
  menu->insertItem(i18n("&Ungroup"), this, SLOT(flatten()));
  KstViewObjectPtr c = findChild(pos + position());
  if (c) {
    KPopupMenu *s = new KPopupMenu(menu);
    if (c->layoutPopupMenu(s, pos - c->position(), topParent)) {
      menu->insertItem(c->tagName(), s);
    } else {
      delete s;
    }
  }
  return true;
}


KstViewObjectPtr create_KstPlotGroup() {
  return KstViewObjectPtr(new KstPlotGroup);
}


KstViewObjectFactoryMethod KstPlotGroup::factory() const {
  return &create_KstPlotGroup;
}


QMap<QString, QVariant> KstPlotGroup::widgetHints(const QString& propertyName) const {
  // don't ask for borderedviewobject hints because we don't want to set border color
  QMap<QString, QVariant> map = KstViewObject::widgetHints(propertyName);
  if (!map.empty()) {
    return map;
  }

  if (propertyName == "transparent") {
    map.insert(QString("_kst_widgetType"), QString("QCheckBox"));
    map.insert(QString("_kst_label"), QString::null);
    map.insert(QString("text"), i18n("Transparent background"));
  } else if (propertyName == "backColor") {
    map.insert(QString("_kst_widgetType"), QString("KColorButton"));
    map.insert(QString("_kst_label"), i18n("Background color"));
  }
  return map;
}


QRegion KstPlotGroup::clipRegion() {
  if (transparent()) {
    // make the clipregion just the children
    QRegion region;
    for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
      region += (*i)->clipRegion();
    }
    return region;
  }
  return KstViewObject::clipRegion();
}


void KstPlotGroup::setTransparent(bool transparent) {
  KstMetaPlot::setTransparent(transparent);
}


bool KstPlotGroup::transparent() const {
  return KstMetaPlot::transparent();
}


namespace {
KstViewObject *create_KstPlotGroup() {
  return new KstPlotGroup;
}


KstGfxMouseHandler *handler_KstPlotGroup() {
  return 0L;
}

KST_REGISTER_VIEW_OBJECT(PlotGroup, create_KstPlotGroup, handler_KstPlotGroup)
}


#include "kstplotgroup.moc"
// vim: ts=2 sw=2 et
