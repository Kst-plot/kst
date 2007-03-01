/***************************************************************************
                   kstviewwidget.h: widget for the toplevel view
                             -------------------
    begin                : Mar 27, 2004
    copyright            : (C) 2003 The University of Toronto
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

#ifndef KSTVIEWWIDGET_H
#define KSTVIEWWIDGET_H

#include <qpointer.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QWheelEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QFocusEvent>
#include <QDragLeaveEvent>

#include <kmenu.h>
#include "ksttoplevelview.h"

class Q3DragObject;

class KstViewWidget : public QWidget {
  Q_OBJECT
  public:
    KstViewWidget(KstTopLevelViewPtr view, QWidget *parent = 0L, const char *name = 0L, Qt::WFlags w = 0);
    virtual ~KstViewWidget();

    void setDropEnabled(bool);
    void setDragEnabled(bool);

    Q3DragObject *dragObject();
    KstTopLevelViewPtr viewObject() const;

    void paint();
    void paint(const QRegion& region);

    KstViewObjectPtr findChildFor(const QPoint& pos);
    void dropped(QDropEvent *e);
    
  protected:
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dragMoveEvent(QDragMoveEvent *e);
    virtual void dragLeaveEvent(QDragLeaveEvent *e);
    virtual void dropEvent(QDropEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

    friend class KstTopLevelView;
    void release() { _view = 0L; }

  private:
    KstTopLevelViewPtr _view;
    KstTopLevelView::ViewMode _lastViewMode;
    QPointer<KstViewObject> _vo_datamode;
    bool _dropEnabled : 1;
    bool _dragEnabled : 1;
    QPointer<KPopupMenu> _menu;
    Q3DragObject *_drag;
};

#endif
// vim: ts=2 sw=2 et
