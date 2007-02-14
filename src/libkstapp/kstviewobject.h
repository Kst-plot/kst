/***************************************************************************
                   kstviewobject.h: base class for view objects
                             -------------------
    begin                : Mar 11, 2004
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

#ifndef KSTVIEWOBJECT_H
#define KSTVIEWOBJECT_H

#include <qcolor.h>
#include <qdom.h>
#include <qguardedptr.h>

#include <kdeversion.h>
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
#include <kdemacros.h>
#endif

#include "kstobject.h"
#include "kstalignment.h"
#include "kstpainter.h"
#include "kst_export.h"

class KPopupMenu;
class KstGfxMouseHandler;
class KstViewObject;
class KstViewWidget;
class KstTopLevelView;
typedef KstSharedPtr<KstViewObject> KstViewObjectPtr;
typedef KstObjectList<KstViewObjectPtr> KstViewObjectList;
typedef KstViewObjectPtr (*KstViewObjectFactoryMethod)();
typedef KstGfxMouseHandler* (*KstHandlerFactoryMethod)();
typedef KstSharedPtr<KstTopLevelView> KstTopLevelViewPtr;

struct KstAspectRatio {
  KstAspectRatio() : x(0.0), y(0.0), w(0.0), h(0.0) {}
  double x, y, w, h;
  friend QDataStream& operator>>(QDataStream& str, KstAspectRatio& ar) {
    str >> ar.x >> ar.y >> ar.w >> ar.h;
    return str;
  }
  friend QDataStream& operator<<(QDataStream& str, KstAspectRatio& ar) {
    str << ar.x << ar.y << ar.w << ar.h;
    return str;
  }
};

class KST_EXPORT KstViewObject : public KstObject {
  Q_OBJECT
  protected:
    KstViewObject(const QString& type);
  public:
    KstViewObject(const QDomElement& e);
    KstViewObject(const KstViewObject& viewObject);
    virtual ~KstViewObject();

    virtual UpdateType update(int = -1);
    // FIXME: bring this down to 1 function
    virtual void save(QTextStream& ts, const QString& indent = QString::null);
    virtual void saveAttributes(QTextStream& ts, const QString& indent = QString::null);
    virtual void loadChildren(const QDomElement& e);

    virtual void invalidateClipRegion();
    virtual QRegion clipRegion();

    // If cols <= 0, optimal is chosen automatically
    virtual void cleanup(int cols = -1);
    virtual int columns() const;
    virtual void setColumns(int cols);
    virtual bool onGrid() const;
    virtual void setOnGrid(bool on_grid);

    virtual void resize(const QSize& size);
    // FIXME: this stuff has got to go.
    virtual void resizeForPrint(const QSize& size);
    virtual void revertForPrint();
    virtual void resizeFromAspect(double x, double y, double w, double h);
    virtual QSize size() const;
    virtual void internalAlignment(KstPainter& p, QRect& plotRegion);
    virtual QPoint position() const;
    virtual const QRect& geometry() const;
    const KstAspectRatio& aspectRatio() const;
    virtual QRect surroundingGeometry() const;
    // This is, by definition, the contents not including decoration. (borders
    // etc)
    virtual QRect contentsRect() const;
    virtual void setContentsRect(const QRect& rect);

    // with cleanup, use this factor to decide if more space should be used
    // in the row.  1.0 means that the dataRect = geometry.  >1 means dataRect < geometry.
    virtual double verticalSizeFactor();
    virtual double horizontalSizeFactor();
    
    virtual void move(const QPoint& to);

    // Draw a focus highlight
    virtual void setFocus(bool focus);
    virtual bool focused() const;
    virtual void setHasFocus(bool hasFocus);
    virtual void removeFocus(KstPainter& p);

    virtual bool maintainAspect() const;
    virtual void setMaintainAspect(bool maintain);

    virtual void appendChild(KstViewObjectPtr obj, bool keepAspect = false);
    virtual void prependChild(KstViewObjectPtr obj, bool keepAspect = false);
    virtual bool removeChild(KstViewObjectPtr obj, bool recursive = false);
    virtual void insertChildAfter(const KstViewObjectPtr after, KstViewObjectPtr obj, bool keepAspect = false);
    virtual void clearChildren();
    const KstViewObjectList& children() const;
    KstViewObjectList& children();

    bool objectDirty() const; // true if this object or a child is dirty
    bool fallThroughTransparency() const;
    bool isResizable() const;
    
    virtual QWidget *configWidget();

    // can't be const due to KstViewObjectPtr?
    // I don't like this method at all, but for now it makes things work.  Maybe
    // split it into two methods eventually.  The bool is for toplevelview so
    // that it can do resize rect on transparent objects
    KstViewObjectPtr findChild(const QPoint& pos, bool borderForTransparent = false);
    KstViewObjectPtr findDeepestChild(const QPoint& pos, bool borderForTransparent = false);
    KstViewObjectPtr findChild(const QString& name, bool recursive = true);
    bool contains(const KstViewObjectPtr child) const;
    template<class T> KstObjectList<KstSharedPtr<T> > findChildrenType(bool recursive = false);

    // Finds a container that can hold this rect.  Only searches objects that
    // are containers.
    KstViewObjectPtr findChild(const QRect& rect);
    KstViewObjectPtr findDeepestChild(const QRect& rect);

    virtual void setBackgroundColor(const QColor& color);
    virtual QColor backgroundColor() const;

    virtual void setForegroundColor(const QColor& color);
    virtual QColor foregroundColor() const;

    // One day maybe we'll even have translucency available!
    virtual void setTransparent(bool transparent);
    virtual bool transparent() const;

    virtual void setFollowsFlow(bool follow);
    virtual bool followsFlow() const;

    virtual void updateSelection(const QRect& region);
    bool isContainer() const;
    
    KstViewObjectPtr parent() const;
    KstViewObjectPtr topLevelParent() const;

    void recursively(void (KstViewObject::*)(), bool self = false);
    template<class T, class U> void recursively(void (U::*)(T), T, bool self = false);
    template<class T, class U> void forEachChild(void (U::*)(T), T, bool self = false);
    // GCC bug: compiler crashes if forEachChild is used here too
    // note also: self=false
    template<class U> void forEachChild2(void (U::*)());

    virtual bool popupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topParent);
    virtual bool layoutPopupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topParent);

    // FIXME: split into layout and non-layout?
    enum StandardActions { Delete =          0x1,
                           Copy =            0x2,
                           Cut =             0x4,
                           Paste =           0x8,
                           Raise =          0x10,
                           Lower =          0x20,
                           RaiseToTop =     0x40,
                           LowerToBottom =  0x80,
                           Properties =    0x100,
                           Rename =        0x200,
                           Edit =          0x400,
                           Zoom =          0x800,
                           Pause =        0x1000,
                           MoveTo =       0x2000,
                           CopyTo =       0x4000
                        };

    virtual void drawFocusRect(KstPainter& p);
    virtual void drawSelectRect(KstPainter& p);

    virtual void setMaximized(bool maximized);
    bool maximized() const;
    virtual bool isSelected() const;
    virtual void setSelected(bool selected);
    void selectAll();
    void unselectAll();

    void recursivelyQuery(bool (KstViewObject::*method)() const, KstViewObjectList& list, bool matchRecurse = false);

    virtual void detach(); // remove from its parent

    const QString& type() const;

    virtual bool mouseHandler() const;
    virtual void mouseMoveEvent(QWidget *view, QMouseEvent *e);
    virtual void mousePressEvent(QWidget *view, QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QWidget *view, QMouseEvent *e);
    virtual void mouseReleaseEvent(QWidget *view, QMouseEvent *e);
    virtual void keyPressEvent(QWidget *view, QKeyEvent *e);
    virtual void keyReleaseEvent(QWidget *view, QKeyEvent *e);
    virtual void dragMoveEvent(QWidget *view, QDragMoveEvent *e);
    virtual void dragEnterEvent(QWidget *view, QDragEnterEvent *e);
    virtual void dropEvent(QWidget *view, QDropEvent *e);
    virtual void wheelEvent(QWidget *view, QWheelEvent *e);

    friend QDataStream& operator<<(QDataStream& str, KstViewObjectPtr obj);
    friend QDataStream& operator>>(QDataStream& str, KstViewObjectPtr obj);

    virtual void load(const QDomElement& e);

    virtual QString menuTitle() const;

    void setMinimumSize(const QSize& sz);
    const QSize& minimumSize() const;
    
    // returns the mouse direction for this position on this view object
    enum Direction {NONE = 0, 
                    UP = 1, 
                    DOWN = 2, 
                    LEFT = 4, 
                    RIGHT = 8, 
                    CENTEREDRESIZE = 16,
                    ENDPOINT = 32};
    virtual signed int directionFor(const QPoint& pos);
    
    // returns instructions for the edit dialog of this view object
    // given a property name
    // FIXME: Don't use QMap.  We need to preserve order here.
    virtual QMap<QString, QVariant > widgetHints(const QString& propertyName) const;

    // draws a minimalistic shadow outline of this object at the given position
    virtual void drawShadow(KstPainter& p, const QPoint& pos);

    // handle custom widget, if any: is called by KstEditViewObjectDialogI
    virtual bool fillConfigWidget(QWidget *w, bool isNew) const;
    virtual bool readConfigWidget(QWidget *w);
    virtual void connectConfigWidget(QWidget *parent, QWidget *w) const;
    
    virtual bool paste(QMimeSource* source, KstViewObjectList *list = 0L);
    
    virtual KstGfxMouseHandler *createHandler();
    void setDirty(bool = true);

  public slots:
    void paint(KstPainter& p, const QRegion& bounds);
    virtual void paintSelf(KstPainter& p, const QRegion& bounds);
    virtual void updateSelf();
    virtual void paintUpdate();
    virtual void updateFromAspect();
    virtual void updateAspectSize();
    virtual void updateAspectPos();
    virtual void updateAspect();
    virtual void zoomToggle();
    virtual void copyObject();
    virtual KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
    virtual bool showDialog(KstTopLevelViewPtr invoker, bool isNew);

  protected slots:
    virtual void parentResized();
    virtual void parentResizedForPrint();
    virtual void parentRevertedForPrint();
    virtual void parentMoved(const QPoint& offset);

    /***********  Actions ************/
    virtual void deleteObject();
    virtual void raiseToTop();
    virtual void lowerToBottom();
    virtual void raise();
    virtual void lower();
    virtual void moveTo(int);
    virtual void copyTo(int);
    virtual void rename();
    virtual void pauseToggle();
    virtual void edit();

  protected:
    friend class KstTopLevelView;
    friend class KstEditViewObjectDialogI;
    void setDialogLock(bool lock);
    bool dialogLocked() const;
    virtual KstViewObjectFactoryMethod factory() const;
    virtual KstHandlerFactoryMethod handlerFactory() const;
    virtual void writeBinary(QDataStream& str);
    virtual void readBinary(QDataStream& str);
    KstObject::UpdateType updateChildren(int counter);
    
    KstViewObjectList _children;
    QRect _geom;
    QRect _geomOld;
    QColor _backgroundColor, _foregroundColor; // FIXME: use QColorGroup
    bool _hasFocus : 1;
    bool _focus : 1;
    bool _selected : 1;
    bool _onGrid : 1;
    bool _prevOnGrid : 1;
    bool _maximized : 1;
    bool _transparent : 1;
    bool _followsFlow : 1;
    bool _dialogLock : 1;
    bool _container : 1;
    bool _fallThroughTransparency : 1;
    bool _isResizable : 1;
    bool _maintainAspect : 1;
    int _columns : 10;
    QGuardedPtr<KstViewObject> _topObjectForMenu;
    QGuardedPtr<KstViewObject> _parent; // danger!!
    Q_UINT32 _standardActions, _layoutActions;
    KstAspectRatio _aspect;
    QSize _idealSize; //ideal size for object. useful when _maintainAspect==true
    KstAspectRatio _aspectOldZoomedObject;
    QString _type;
    QRegion _clipMask; // The mask of this object
    QMap<int, QString> _moveToMap;
    QMap<int, QString> _copyToMap;
    QSize _minimumSize;
};


template<class U>
void KstViewObject::forEachChild2(void (U::*method)()) {
  for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
    U *it = kst_cast<U>(*i);
    if (it) {
      (it->*method)();
    }
  }
}


template<class T, class U>
void KstViewObject::forEachChild(void (U::*method)(T), T arg, bool self) {
  if (self) {
    U *me = dynamic_cast<U*>(this);
    if (me) {
      (me->*method)(arg);
    }
  }
  for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
    U *it = kst_cast<U>(*i);
    if (it) {
      (it->*method)(arg);
    }
  }
}


template<class T, class U>
void KstViewObject::recursively(void (U::*method)(T), T arg, bool self) {
  if (self) {
    U *me = dynamic_cast<U*>(this);
    if (me) {
      (me->*method)(arg);
    }
  }
  for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
    (*i)->recursively<T>(method, arg, true);
  }
}


template<class T>
KstObjectList<KstSharedPtr<T> > KstViewObject::findChildrenType(bool recursive) {
  KstObjectList<KstSharedPtr<T> > rc;
  for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
    T *o = kst_cast<T>(*i);
    if (o) {
      rc.append(o);
    }

    if (recursive) {
      rc += (*i)->findChildrenType<T>(recursive);
    }
  }

  return rc;
}

#endif
// vim: ts=2 sw=2 et
