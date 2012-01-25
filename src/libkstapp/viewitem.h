/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QXmlStreamWriter>
#include <QHash>
#include <QAction>
#include <QMimeData>

#include "namedobject.h"
#include "kst_export.h"
#include "viewcommand.h"
#include "view.h"
#include "curveplacement.h"
//#include "sharedptr.h"
#include "application.h"
#include "tabwidget.h"

namespace Kst {

class DialogPage;
class ViewGridLayout;
class ViewItem;

typedef QList<ViewItem *> ViewItemList;

class ViewItem : public QObject, public NamedObject, public QGraphicsRectItem
{
  Q_OBJECT
  public:
    friend class DialogLauncherSI;
    enum GripMode {
      Move    = 1,
      Resize  = 2,
      Scale   = 4,
      Rotate  = 8
    };
    Q_DECLARE_FLAGS(GripModes, GripMode)

    enum ActiveGrip {
      NoGrip          = 1,
      TopLeftGrip     = 2,
      TopRightGrip    = 4,
      BottomRightGrip = 8,
      BottomLeftGrip  = 16,
      TopMidGrip      = 32,
      RightMidGrip    = 64,
      BottomMidGrip   = 128,
      LeftMidGrip     = 256
    };
    Q_DECLARE_FLAGS(ActiveGrips, ActiveGrip)

    enum CreationState {
      None,
      InProgress,
      Completed
    };

    ViewItem(View *parent);
    virtual ~ViewItem();

    virtual void save(QXmlStreamWriter &xml);
    bool parse(QXmlStreamReader &xml, bool &validChildTag);
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    void setTypeName(const QString& name) { _typeName = name; }
    const QString typeName() const { return _typeName; }

    // for dialog defaults
    virtual const QString defaultsGroupName() const = 0;

    // for view item dialogs
    virtual bool hasStroke() const {return false;}
    virtual bool hasBrush() const {return false;}
    virtual bool hasFont() const {return false;}

    virtual void setFont(const QFont &f, const QColor &c) {return;}

    View* view() const;
    void setView(View*);

    ViewItem *parentViewItem() const;
    void setParentViewItem(ViewItem *parent);

    virtual void updateRelativeSize();

    void moveTo(const QPointF& pos);
    
    qreal relativeHeight() const { return _parentRelativeHeight; }
    void setRelativeHeight(const qreal height) { _parentRelativeHeight = height; }
    qreal relativeWidth() const { return _parentRelativeWidth; }
    void setRelativeWidth(const qreal width) { _parentRelativeWidth = width; }

    QPointF relativeCenter() const { return _parentRelativeCenter; }
    void setRelativeCenter(const QPointF center) { _parentRelativeCenter = center; }
    QPointF relativePosition() const { return _parentRelativePosition; }
    void setRelativePosition(const QPointF pos) { _parentRelativePosition = pos; }
    
    qreal rotationAngle() const;

    GripMode gripMode() const;
    void setGripMode(GripMode mode);

    GripModes allowedGripModes() const;
    void setAllowedGripModes(GripModes modes);
    bool isAllowed(GripMode mode) const;

    bool fixedSize() const { return _fixedSize; }
    void setFixedSize(bool fixedSize) { _fixedSize = fixedSize; }

    bool lockAspectRatio() const { return _lockAspectRatio; }
    void setLockAspectRatio(bool lockAspectRatio) { _lockAspectRatio = lockAspectRatio; }

    bool lockAspectRatioFixed() const { return _lockAspectRatioFixed; }
    void setLockAspectRatioFixed(bool enable) { _lockAspectRatioFixed = enable; }

    bool hasStaticGeometry() const { return _hasStaticGeometry; }
    void setHasStaticGeometry(bool hasStaticGeometry ) { _hasStaticGeometry = hasStaticGeometry; }

    bool lockParent() const { return _lockParent; }
    void setLockParent(bool lockParent ) { _lockParent = lockParent; }

    bool skipNextParentCheck() const { return _skipNextParentCheck; }
    void setSkipNextParentCheck(bool skipNextParentCheck) { _skipNextParentCheck = skipNextParentCheck; }

    bool allowsLayout() const { return _allowsLayout; }
    void setAllowsLayout(bool allowsLayout ) { _allowsLayout = allowsLayout; }

    bool isHighlighted() const { return _highlighted; }
    void setHighlighted(bool highlighted ) { _highlighted = highlighted; }

    //NOTE This should be used in place of QGraphicsRectItem::setRect()...
    QRectF viewRect() const;
    void setViewRect(const QRectF &viewRect, bool automaticChange = false);
    void setViewRect(qreal x, qreal y, qreal width, qreal height, bool automaticChange = false);

    qreal width() const { return viewRect().normalized().width(); }
    qreal height() const { return viewRect().normalized().height(); }

    ActiveGrip activeGrip() const;
    void setActiveGrip(ActiveGrip grip);

    ActiveGrips allowedGrips() const;
    void setAllowedGrips(ActiveGrips grips);
    bool isAllowed(ActiveGrip grip) const;

    virtual QSizeF sizeOfGrip() const;
    virtual QPainterPath grips() const;

    virtual QRectF selectBoundingRect() const;
    virtual QRectF gripBoundingRect() const;

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual QPainterPath itemShape() const { return QGraphicsRectItem::shape(); }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual void paint(QPainter *painter);

    virtual void paintChildItems(QPainter &painter);

    virtual QList<DialogPage*> dialogPages() const { return QList<DialogPage*>(); }

    virtual QPointF centerOfRotation() const { return rect().center(); }

    bool isHovering() const { return _hovering; }

    bool acceptsChildItems() const { return _acceptsChildItems; }
    void setAcceptsChildItems(bool acceptsChildItems) { _acceptsChildItems = acceptsChildItems; }

    QSizeF layoutMargins() const { return _layoutMargins; }
    void setLayoutMargins(const QSizeF margins) { _layoutMargins = margins; }

    QSizeF layoutSpacing() const { return _layoutSpacing; }
    void setLayoutSpacing(const QSizeF spacing) { _layoutSpacing = spacing; }

    //This is a workaround for context menu bug in Qt4.3 graphicsview
    bool acceptsContextMenuEvents() const
    { return _acceptsContextMenuEvents; }
    void setAcceptsContextMenuEvents(bool acceptsContextMenuEvents)
    { _acceptsContextMenuEvents = acceptsContextMenuEvents; }

    virtual bool tryShortcut(const QString &keySequence);
    virtual QPainterPath checkBox() const;
    virtual QPainterPath tiedZoomCheck() const;
    virtual QSizeF tiedZoomSize() const { return QSizeF(checkBox().controlPointRect().size() * 1.5); }

    virtual bool isTiedZoom() const { return (_isXTiedZoom || _isYTiedZoom); }
    virtual bool isXTiedZoom() const { return _isXTiedZoom; }
    virtual bool isYTiedZoom() const { return _isYTiedZoom; }
    virtual void setTiedZoom(bool tiedXZoom, bool tiedYZoom, bool checkAllTied = true);

    virtual bool supportsTiedZoom() const { return _supportsTiedZoom; }
    virtual void setSupportsTiedZoom(const bool supports);    


    View::ZoomOnlyMode zoomOnlyMode() const { return _zoomOnlyMode; }
    void setZoomOnly(View::ZoomOnlyMode val) { _zoomOnlyMode = val; }

    CreationState creationState() const { return _creationState; }

    virtual void setItemPen(const QPen & pen) { setPen(pen); }
    virtual void setItemBrush(const QBrush & brush) { setBrush(brush); }

    template<class T> static T* retrieveItem(const QString &name);

    template<class T> static QList<T *> getItems();

   // TODO: Remove, needed only for a Qt 4.3 bug workaround
    bool doSceneEvent(QGraphicsSceneContextMenuEvent *event) {
      return sceneEvent(event);
    }

    virtual bool isMaximized();
    QPointF dropHotSpot;

    void normalizePosition();

    virtual void applyDialogDefaultsFill();
    virtual void applyDialogDefaultsStroke();

  Q_SIGNALS:
    void geometryChanged();
    void creationComplete();

  /*FIXME these should be made private for only undo commands to access*/
  public Q_SLOTS:
    virtual void edit();
    virtual void raise();
    virtual void lower();
    virtual void createAutoLayout();
    virtual void createProtectedLayout();
    virtual void createCustomLayout(int columns = 0);
    virtual void sharePlots(QPainter *painter, bool creation);
    virtual void remove();
    void resizeTopLeft(const QPointF &offset);
    void resizeTopRight(const QPointF &offset);
    void resizeBottomLeft(const QPointF &offset);
    void resizeBottomRight(const QPointF &offset);
    void resizeTop(qreal offset);
    void resizeBottom(qreal offset);
    void resizeLeft(qreal offset);
    void resizeRight(qreal offset);
    void setTopLeft(const QPointF &point);
    void setTopRight(const QPointF &point);
    void setBottomLeft(const QPointF &point);
    void setBottomRight(const QPointF &point);
    void setTop(qreal y);
    void setBottom(qreal y);
    void setLeft(qreal x);
    void setRight(qreal x);

  protected:
    virtual QPainterPath topLeftGrip() const;
    virtual QPainterPath topRightGrip() const;
    virtual QPainterPath bottomRightGrip() const;
    virtual QPainterPath bottomLeftGrip() const;
    virtual QPainterPath topMidGrip() const;
    virtual QPainterPath rightMidGrip() const;
    virtual QPainterPath bottomMidGrip() const;
    virtual QPainterPath leftMidGrip() const;
    QTransform selectTransform() const;
    bool transformToRect(const QRectF &from, const QRectF &to);
    bool transformToRect(const QPolygonF &from, const QPolygonF &to);
    void rotateTowards(const QPointF &corner, const QPointF &point);
    QPointF lockOffset(const QPointF &offset, qreal ratio, bool oddCorner) const;
    virtual bool updateViewItemParent();
    GripMode nextGripMode(GripMode currentMode) const;
    void addTitle(QMenu *menu) const;
    void registerShortcut(QAction *action);
    void reRegisterShortcut();

    QString descriptionTip() const;

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);
    void creationPolygonChangedFixedAspect(View::CreationEvent even, double aspect);

  protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void addToMenuForContextEvent(QMenu &menu);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);


    QAction *_editAction;
    QAction *_deleteAction;
    QAction *_raiseAction;
    QAction *_lowerAction;
    QAction *_autoLayoutAction;
    QAction *_protectedLayoutAction;
    QAction *_customLayoutAction;

    bool _isXTiedZoom;
    bool _isYTiedZoom;
    bool _plotMaximized;

  private Q_SLOTS:
    void viewMouseModeChanged(View::MouseMode oldMode);
    void updateView();

  protected:
  public:
    virtual void updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect);
  protected:
    virtual QString _automaticDescriptiveName() const;
    virtual void _initializeShortName();
    QPointF dragStartPosition;
    void startDragging(QWidget *widget, const QPointF& hotspot);
    ActiveGrip _activeGrip;

  private:
    GripMode _gripMode;
    GripModes _allowedGripModes;
    CreationState _creationState;
    QString _typeName;
    View::ZoomOnlyMode _zoomOnlyMode;
    bool _supportsTiedZoom;    
    bool _fixedSize;
    bool _lockAspectRatio;
    bool _lockAspectRatioFixed;
    bool _hasStaticGeometry;
    bool _lockParent;
    bool _skipNextParentCheck;
    bool _allowsLayout;
    bool _hovering;
    bool _acceptsChildItems;
    bool _acceptsContextMenuEvents;
    bool _updatingLayout;
    bool _highlighted;
    QPointF _originalPosition;
    QPointF _parentRelativeCenter;
    QPointF _parentRelativePosition;
    QRectF _originalRect;
    QTransform _originalTransform;
    QLineF _normalLine;
    QLineF _rotationLine;
    ActiveGrips _allowedGrips;
    QTransform _rotationTransform;
    QHash<QString, QAction*> _shortcutMap;
    qreal _parentRelativeHeight, _parentRelativeWidth;

    QSizeF _layoutMargins, _layoutSpacing;    

    // use view() / setView(View*)
    QObject* parent() const;
    void setParent(QObject*);

    // use setParentViewItem(ViewItem*)
    void setParentItem(QGraphicsItem*);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ViewItem::GripModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(ViewItem::ActiveGrips)

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, ViewItem*);
#endif

class ViewItemCommand : public QUndoCommand
{
  public:
    ViewItemCommand(ViewItem *item, const QString &text, bool addToStack = true, QUndoCommand *parent = 0);
    virtual ~ViewItemCommand();

  protected:
    QPointer<ViewItem> _item;
};

class CreateCommand : public QObject, public ViewCommand
{
  Q_OBJECT
  public:
    explicit CreateCommand(const QString &text, QUndoCommand *parent = 0);
    CreateCommand(View *view, const QString &text, QUndoCommand *parent = 0);
    virtual ~CreateCommand();

    virtual void undo();
    virtual void redo();
    virtual void createItem();

    ViewItem *item() const { return _item; }

  public Q_SLOTS:
    virtual void creationComplete();

  protected:
    QPointer<ViewItem> _item;
};

class LayoutCommand : public ViewItemCommand
{
  public:
    LayoutCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Create layout"), false) {}

    virtual ~LayoutCommand() {}

    virtual void undo();
    virtual void redo();
    void createLayout(bool preserve = true, int columns = 0);
  private:
    QPointer<ViewGridLayout> _layout;
};

class AppendLayoutCommand : public ViewItemCommand
{
  public:
    AppendLayoutCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Append Item to Layout"), false) {}

    virtual ~AppendLayoutCommand() {}

    virtual void undo();
    virtual void redo();
    void appendLayout(CurvePlacement::Layout layout, ViewItem* item, int columns = 0);

  private:
    QPointer<ViewGridLayout> _layout;
};

class MoveCommand : public ViewItemCommand
{
  public:
    MoveCommand(ViewItem *item, const QPointF &originalPos, const QPointF &newPos)
        : ViewItemCommand(item, QObject::tr("Move")),
          _originalPos(originalPos),
          _newPos(newPos) {}

    virtual ~MoveCommand() {}

    virtual void undo();
    virtual void redo();

  private:
    QPointF _originalPos;
    QPointF _newPos;
};

class ResizeCommand : public ViewItemCommand
{
  public:
    ResizeCommand(ViewItem *item, const QRectF &originalRect, const QRectF &newRect)
        : ViewItemCommand(item, QObject::tr("Resize")),
          _originalRect(originalRect),
          _newRect(newRect) {}

    virtual ~ResizeCommand() {}

    virtual void undo();
    virtual void redo();

  private:
    QRectF _originalRect;
    QRectF _newRect;
};

class RemoveCommand : public ViewItemCommand
{
  public:
    RemoveCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Remove")) {}

    virtual ~RemoveCommand() {}

    virtual void undo();
    virtual void redo();
};

class RaiseCommand : public ViewItemCommand
{
  public:
    RaiseCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Raise")) {}

    virtual ~RaiseCommand() {}

    virtual void undo();
    virtual void redo();
};

class LowerCommand : public ViewItemCommand
{
  public:
    LowerCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Lower")) {}

    virtual ~LowerCommand() {}

    virtual void undo();
    virtual void redo();
};

class TransformCommand : public ViewItemCommand
{
  public:
    TransformCommand(ViewItem *item, const QTransform &originalTransform,
                  const QTransform &newTransform, const QString &text)
        : ViewItemCommand(item, text),
          _originalTransform(originalTransform), _newTransform(newTransform) {}

    virtual ~TransformCommand() {}

    virtual void undo();
    virtual void redo();

  private:
    QTransform _originalTransform;
    QTransform _newTransform;
};

class ScaleCommand : public TransformCommand
{
  public:
    ScaleCommand(ViewItem *item, const QTransform &originalTransform, const QTransform &newTransform)
        : TransformCommand(item, originalTransform, newTransform, QObject::tr("Scale")) {}

    virtual ~ScaleCommand() {}
};

class RotateCommand : public TransformCommand
{
  public:
    RotateCommand(ViewItem *item, const QTransform &originalTransform, const QTransform &newTransform)
        : TransformCommand(item, originalTransform, newTransform, QObject::tr("Rotate")) {}

    virtual ~RotateCommand() {}
};


template<class T>
QList<T *> ViewItem::getItems() {
  QList<T *> tItems;
  ViewItem *viewItem;
  T* tItem;

  QList<View*> views = kstApp->mainWindow()->tabWidget()->views();

  for (int i_view = 0; i_view<views.count(); i_view++) {
    QList<QGraphicsItem*> items = views.at(i_view)->scene()->items();
    for (int i_item = 0; i_item<items.count(); i_item++) {
      viewItem = qgraphicsitem_cast<ViewItem *>(items[i_item]);
      tItem = dynamic_cast<T*>(viewItem);
      if (tItem) {
        tItems.append(tItem);
      }
    }
  }
  return tItems;
}

template<class T>
T* ViewItem::retrieveItem(const QString &name) {
  QList<T *> tItems = getItems<T>();

  int match = -1;

  if (name.isEmpty()) {
    return NULL;
  }

  QString shortName;
  QRegExp rx("(\\(|^)([A-Z]\\d+)(\\)$|$)");
  rx.indexIn(name);
  shortName = rx.cap(2);

  // 1) search for short names
  int size = tItems.size();
  for (int i = 0; i < size; ++i) {
    if (tItems.at(i)->shortName()==shortName)
      return tItems.at(i);
  }
  // 3) search for descriptive names: must be unique
  for (int i = 0; i < size; ++i) {
    if (tItems.at(i)->descriptiveName() == name) {
      if (match != -1)
        return NULL; // not unique, so... no match
      match = i;
    }
  }

  if (match >-1)
    return tItems.at(match);

  return NULL;
}


class MimeDataViewItem : public QMimeData
{
  Q_OBJECT
public:
  MimeDataViewItem();

  ViewItem* item;
  QPointF hotSpot;

  static const MimeDataViewItem* downcast(const QMimeData*);
};

}

#endif

// vim: ts=2 sw=2 et
