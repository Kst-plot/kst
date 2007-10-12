/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
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
#include <QXmlStreamWriter>
#include <QHash>
#include <QAction>

#include "kst_export.h"
#include "viewcommand.h"
#include "view.h" //forward declare, but enums??

namespace Kst {

class DialogPage;
class ViewGridLayout;

class KST_EXPORT ViewItem : public QObject, public QGraphicsRectItem
{
  Q_OBJECT
  public:
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

    ViewItem(View *parent);
    virtual ~ViewItem();

    virtual void save(QXmlStreamWriter &xml);
    bool parse(QXmlStreamReader &xml, bool &validChildTag);
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    View *parentView() const;
    ViewItem *parentViewItem() const;
    bool itemInLayout() const;

    GripMode gripMode() const;
    void setGripMode(GripMode mode);

    GripModes allowedGripModes() const;
    void setAllowedGripModes(GripModes modes);
    bool isAllowed(GripMode mode) const;

    bool lockAspectRatio() const { return _lockAspectRatio; }
    void setLockAspectRatio(bool lockAspectRatio) { _lockAspectRatio = lockAspectRatio; }

    bool hasStaticGeometry() { return _hasStaticGeometry; }
    void setHasStaticGeometry(bool hasStaticGeometry ) { _hasStaticGeometry = hasStaticGeometry; }

    //NOTE We can change this to a generic abstract class once we have
    //more layouts besides grid layout...
    ViewGridLayout *layout() const;
    void setLayout(ViewGridLayout *layout);

    //NOTE This should be used in place of QGraphicsRectItem::setRect()...
    QRectF viewRect() const;
    void setViewRect(const QRectF &viewRect);
    void setViewRect(qreal x, qreal y, qreal width, qreal height);

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

    virtual QList<DialogPage*> dialogPages() const { return QList<DialogPage*>(); }

    virtual QPointF centerOfRotation() const { return rect().center(); }

    bool isHovering() const { return _hovering; }

  Q_SIGNALS:
    void geometryChanged();
    void creationComplete();

  /*FIXME these should be made private for only undo commands to access*/
  public Q_SLOTS:
    void edit();
    void raise();
    void lower();
    void createLayout();
    void breakLayout();
    void remove();
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
    virtual bool maybeReparent();
    GripMode nextGripMode(GripMode currentMode) const;
    void addTitle(QMenu *menu) const;
    void registerShortcut(QAction *action);

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);

  protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  private Q_SLOTS:
    void viewMouseModeChanged(View::MouseMode oldMode);

  private:
    bool tryShortcut(const QString &keySequence);
    static void updateChildGeometry(ViewItem *child, const QRectF &oldParentRect,
                                                    const QRectF &newParentRect);

  private:
    GripMode _gripMode;
    GripModes _allowedGripModes;
    bool _lockAspectRatio;
    bool _hasStaticGeometry;
    bool _hovering;
    ViewGridLayout *_layout;
    QPointF _originalPosition;
    QRectF _originalRect;
    QTransform _originalTransform;
    QLineF _normalLine;
    QLineF _rotationLine;
    ActiveGrip _activeGrip;
    ActiveGrips _allowedGrips;
    QTransform _rotationTransform;
    QHash<QString, QAction*> _shortcutMap;

    friend class View;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ViewItem::GripModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(ViewItem::ActiveGrips)

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, ViewItem*);
#endif

class KST_EXPORT ViewItemCommand : public QUndoCommand
{
  public:
    ViewItemCommand(ViewItem *item, const QString &text, bool addToStack = true, QUndoCommand *parent = 0);
    virtual ~ViewItemCommand();

  protected:
    QPointer<ViewItem> _item;
};

// TODO: inherit from ViewItemCommand?
class KST_EXPORT CreateCommand : public QObject, public ViewCommand
{
  Q_OBJECT
  public:
    CreateCommand(const QString &text, QUndoCommand *parent = 0);
    CreateCommand(View *view, const QString &text, QUndoCommand *parent = 0);
    virtual ~CreateCommand();

    virtual void undo();
    virtual void redo();
    virtual void createItem();

    ViewItem *item() const { return _item; }

  public Q_SLOTS:
    void creationComplete();

  protected:
    QPointer<ViewItem> _item;
};

class KST_EXPORT LayoutCommand : public ViewItemCommand
{
  public:
    LayoutCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Create layout"), false) {}

    virtual ~LayoutCommand() {}

    virtual void undo();
    virtual void redo();
    void createLayout();

  private:
    QPointer<ViewGridLayout> _layout;
};

class KST_EXPORT BreakLayoutCommand : public ViewItemCommand
{
  public:
    BreakLayoutCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Break layout")) {}

    virtual ~BreakLayoutCommand() {}

    virtual void undo();
    virtual void redo();

  private:
    QPointer<ViewGridLayout> _layout;
};

class KST_EXPORT MoveCommand : public ViewItemCommand
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

class KST_EXPORT ResizeCommand : public ViewItemCommand
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

class KST_EXPORT RemoveCommand : public ViewItemCommand
{
  public:
    RemoveCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Remove")) {}

    virtual ~RemoveCommand() {}

    virtual void undo();
    virtual void redo();
};

class KST_EXPORT RaiseCommand : public ViewItemCommand
{
  public:
    RaiseCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Raise")) {}

    virtual ~RaiseCommand() {}

    virtual void undo();
    virtual void redo();
};

class KST_EXPORT LowerCommand : public ViewItemCommand
{
  public:
    LowerCommand(ViewItem *item)
        : ViewItemCommand(item, QObject::tr("Lower")) {}

    virtual ~LowerCommand() {}

    virtual void undo();
    virtual void redo();
};

class KST_EXPORT TransformCommand : public ViewItemCommand
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

class KST_EXPORT ScaleCommand : public TransformCommand
{
  public:
    ScaleCommand(ViewItem *item, const QTransform &originalTransform, const QTransform &newTransform)
        : TransformCommand(item, originalTransform, newTransform, QObject::tr("Scale")) {}

    virtual ~ScaleCommand() {}
};

class KST_EXPORT RotateCommand : public TransformCommand
{
  public:
    RotateCommand(ViewItem *item, const QTransform &originalTransform, const QTransform &newTransform)
        : TransformCommand(item, originalTransform, newTransform, QObject::tr("Rotate")) {}

    virtual ~RotateCommand() {}
};

}

#endif

// vim: ts=2 sw=2 et
