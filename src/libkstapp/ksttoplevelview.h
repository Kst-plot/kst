/***************************************************************************
                   ksttoplevelview.h: class for the toplevel view
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

#ifndef KSTTOPLEVELVIEW_H
#define KSTTOPLEVELVIEW_H

#include <qcursor.h>
#include <qpointer.h>
//Added by qt3to4:
#include <Q3MemArray>

#include "kstviewobject.h"

class KPopupMenu;
class KstGfxMouseHandler;
class KstViewWidget;

class KstTopLevelView : public KstViewObject {
  friend class KstViewWidget;
  Q_OBJECT
  public:
    KstTopLevelView(QWidget *parent = 0L, const char *name = 0L, Qt::WFlags w = 0);
    KstTopLevelView(const QDomElement& e, QWidget *parent = 0L, const char *name = 0L, Qt::WFlags w = 0);
    virtual ~KstTopLevelView();

    virtual void save(QTextStream& ts, const QString& indent = QString::null);

    void release(); // Release this from it's window/view.  When you call this,
                    // you'd better be deleting this object in the next line.
    KstViewWidget *widget() const;

    virtual void resize(const QSize& size);
    virtual void paint(KstPainter& p, const QRegion& bounds);

    void updateAlignment(KstPainter& p);
    void paint(KstPainter::PaintType type);
    void paint(KstPainter::PaintType type, const QRegion& boundry);

    void clearFocus();
    enum ViewMode { LayoutMode = 0, DisplayMode, CreateMode, LabelMode, Unknown = 15 };
    ViewMode viewMode() const { return _mode; }
    void setViewMode(ViewMode v, const QString& createType = QString::null);

    // Are we tracking (rubber band / move / resize)
    bool tracking() const;
    bool trackingIsMove() const;

    KstViewObjectList& selectionList() { return _selectionList; }
    KstViewObjectPtr pressTarget() const { return _pressTarget; }
    void clearPressTarget() { _pressTarget = 0L; }
    
    template<class T> KstSharedPtr<T> createObject(const QString& name, bool doCleanup = true);

    bool mouseGrabbed() const { return _mouseGrabbed; }
    KstViewObjectPtr mouseGrabber() const { return _mouseGrabber; }
    bool grabMouse(KstViewObjectPtr me);
    void releaseMouse(KstViewObjectPtr me);
    
    // save defaults for mouse mode from a given object
    void saveDefaults(KstViewObjectPtr object);
  
    bool tiedZoomPrev(const QString& plotName);
    bool tiedZoomMode(int zoom, bool flag, double center, int mode, int modeExtra, const QString& plotName);
    bool tiedZoom(bool x, double xmin, double xmax, bool y, double ymin, double ymax, const QString& plotName);

  public slots:
    void cleanupDefault();
    void cleanupCustom();      
  
  private slots:
    void menuClosed();
    void condenseXAxis();
    void condenseYAxis();
    void makeSameWidth();
    void makeSameHeight();
    void makeSameSize();
    void alignLeft();
    void alignRight();
    void alignTop();
    void alignBottom();
    void packVertically();
    void packHorizontally();
    void groupSelection();

  protected:
    void resized(const QSize& size);
    void cancelMouseOperations();
    void deleteSelectedObjects();
    void updateFocus(const QPoint& pos);
    bool handlePress(const QPoint& pos, bool shift = false);
    bool handleDoubleClick(const QPoint& pos, bool shift = false);
    
    // press move handlers
    void pressMove(const QPoint& pos, bool shift = false);
    void pressMoveLayoutMode(const QPoint& pos, bool shift = false);
    // helpers for pressMoveLayoutMode
    void pressMoveLayoutModeMove(const QPoint& pos, bool shift = false);
    void pressMoveLayoutModeResize(const QPoint& pos, bool maintainAspect = false);
    void pressMoveLayoutModeSelect(const QPoint& pos, bool shift = false);
    void pressMoveLayoutModeEndPoint(const QPoint& pos, bool maintainAspect = false);
    void pressMoveLayoutModeCenteredResize(const QPoint& pos, bool maintainAspect = false);
    
    // release press handlers 
    void releasePress(const QPoint& pos, bool shift = false);
    void releasePressLayoutMode(const QPoint& pos, bool shift = false);
    // helpers for releasePressLayoutMode
    void releasePressLayoutModeMove(const QPoint& pos, bool shift = false);
    void releasePressLayoutModeResize(const QPoint& pos, bool shift = false);
    void releasePressLayoutModeSelect(const QPoint& pos, bool shift = false);
    void releasePressLayoutModeEndPoint(const QPoint& pos, bool shift = false);
    void releasePressLayoutModeCenteredResize(const QPoint& pos, bool shift = false);

    void setCursorFor(const QPoint& pos, KstViewObjectPtr p);
    bool popupMenu(KPopupMenu *menu, const QPoint& pos);
    void correctPosition(KstViewObjectPtr pObject, QPoint point);
    QRect newSize(const QRect& originalSize, const QRect& bounds, int direction, const QPoint& pos, bool maintainAspect = false);
    QRect newSizeCentered(const QRect& oldSize, const QRect& bounds, const QPoint& pos, bool maintainAspect);
    QRect correctWidthForRatio(const QRect& oldRect, double ratio, int direction);
    QRect correctHeightForRatio(const QRect& oldRect, double ratio, int direction, int origRight, int origLeft);
    void moveSnapToBorders(int *xMin, int *yMin, const KstViewObjectPtr &obj, const QRect &r) const;
    void resizeSnapToBorders(int *xMin, int *yMin, const KstViewObjectPtr& obj, const QRect &r, int direction) const;
    QRect resizeSnapToObjects(const QRect& r, int direction);
    // Called as a response to drag re-entering widget()
    void restartMove();
    
    KstGfxMouseHandler *handlerForObject(const QString& objType);

  private:
    void commonConstructor();
    // If no children, returns (0,0)
    QSize averageChildSize() const;

    QPointer<KstViewWidget> _w;
    bool _focusOn : 1;
    bool _mouseGrabbed : 1;
    bool _mouseMoved : 1; // true even if mouse moved back to original position
    ViewMode _mode : 9;
    signed int _pressDirection : 7;
    QCursor _cursor;
    QCursor _endpointCursor; // for storage of the custom hotpoint cursor
    QPoint _moveOffset;
    QPoint _moveOffsetSticky;
    KstViewObjectPtr _pressTarget, _hoverFocus;
    QRect _prevBand;
    KstViewObjectList _selectionList;
    KstViewObjectPtr _mouseGrabber;
    KstGfxMouseHandler *_activeHandler;
    QMap<QString,KstGfxMouseHandler*> _handlers;
};

typedef KstSharedPtr<KstTopLevelView> KstTopLevelViewPtr;
typedef KstObjectList<KstTopLevelViewPtr> KstTopLevelViewList;

template<class T>
KstSharedPtr<T> KstTopLevelView::createObject(const QString& name, bool doCleanup) {
  T *plot = new T(name);
  if (_onGrid) {
    // FIXME: make this more powerful, preserve columns
    appendChild(plot);
    if (doCleanup) {
      this->cleanup(-1); // GCC 2.95/ppc bug.  Don't touch!!!
    }
  } else {
    QSize sz = averageChildSize();
    if (sz != QSize(0, 0)) {
      plot->resize(sz);
    } else {
      plot->resize(size());
    }
    // First look at the overall clip mask.  If there are gaps, take the
    // biggest one and use that location.
    QRegion r = clipRegion();
    Q3MemArray<QRect> rects = r.rects();
    if (!rects.isEmpty()) {
      QRect maxRect(0, 0, 0, 0);
      for (Q3MemArray<QRect>::ConstIterator i = rects.begin(); i != rects.end(); ++i) {
        if ((*i).width() * (*i).height() > maxRect.width() * maxRect.height()) {
          maxRect = *i;
        }
      }
      if (maxRect.left() + plot->geometry().width() > geometry().width()) {
        maxRect.moveLeft(geometry().width() - plot->geometry().width());
      }
      if (maxRect.top() + plot->geometry().height() > geometry().height()) {
        maxRect.moveTop(geometry().height() - plot->geometry().height());
      }
      plot->move(maxRect.topLeft());
    } else {
      // If no gaps, then look at the top object and place relative to it.  It
      // would probably be better to iterate back->front with complete masks
      // but that's more complicated and not worth the effort at this time.
      r = QRegion(geometry());
      r -= QRegion(_children.last()->geometry());
      rects = r.rects();
      if (rects.isEmpty()) {
        plot->move(QPoint(0, 0));
      } else {
        QRect maxRect(0, 0, 0, 0);
        for (Q3MemArray<QRect>::ConstIterator i = rects.begin(); i != rects.end(); ++i) {
          if ((*i).width() * (*i).height() > maxRect.width() * maxRect.height()) {
            maxRect = *i;
          }
        }
        if (maxRect.left() + plot->geometry().width() > geometry().width()) {
          maxRect.moveLeft(geometry().width() - plot->geometry().width());
        }
        if (maxRect.top() + plot->geometry().height() > geometry().height()) {
          maxRect.moveTop(geometry().height() - plot->geometry().height());
        }
        plot->move(maxRect.topLeft());
      }
    }
    appendChild(plot);
  }
  return plot;
}


#endif
// vim: ts=2 sw=2 et
