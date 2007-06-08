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

#include "kst_export.h"
#include "viewcommand.h"
#include "view.h" //forward declare, but enums??

// #define DEBUG_GEOMETRY 1

namespace Kst {

class KST_EXPORT ViewItem : public QObject, public QGraphicsRectItem
{
  Q_OBJECT
public:
  enum MouseMode { Default, Move, Resize, Rotate };
  ViewItem(View *parent);
  virtual ~ViewItem();

  View *parentView() const;

  MouseMode mouseMode() const;
  void setMouseMode(MouseMode mode);

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

Q_SIGNALS:
  void creationComplete();

/*FIXME these should be made private for only undo commands to access*/
public Q_SLOTS:
  void remove();
  void raise();
  void lower();
  bool transformToRect(const QRectF &newRect, bool combine = false);

protected Q_SLOTS:
  virtual void creationPolygonChanged(View::CreationEvent event);

protected:
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private Q_SLOTS:
  void viewMouseModeChanged(View::MouseMode oldMode);

private:
  MouseMode _mouseMode;
  QPointF _originalPosition;
  QTransform _originalTransform;
};

class KST_EXPORT ViewItemCommand : public QUndoCommand
{
public:
  ViewItemCommand(const QString &text, bool addToStack = true, QUndoCommand *parent = 0);
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

public Q_SLOTS:
  void creationComplete();

protected:
  QPointer<ViewItem> _item;
};

class KST_EXPORT MoveCommand : public ViewItemCommand
{
public:
  MoveCommand(QPointF originalPos, QPointF newPos)
      : ViewItemCommand(QObject::tr("Move")),
        _originalPos(originalPos),
        _newPos(newPos) {}
  MoveCommand(ViewItem *item, QPointF originalPos, QPointF newPos)
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

class KST_EXPORT RemoveCommand : public ViewItemCommand
{
public:
  RemoveCommand()
      : ViewItemCommand(QObject::tr("Remove")) {}
  RemoveCommand(ViewItem *item)
      : ViewItemCommand(item, QObject::tr("Remove")) {}

  virtual ~RemoveCommand() {}

  virtual void undo();
  virtual void redo();
};

class KST_EXPORT RaiseCommand : public ViewItemCommand
{
public:
  RaiseCommand()
      : ViewItemCommand(QObject::tr("Raise")) {}
  RaiseCommand(ViewItem *item)
      : ViewItemCommand(item, QObject::tr("Raise")) {}

  virtual ~RaiseCommand() {}

  virtual void undo();
  virtual void redo();
};

class KST_EXPORT LowerCommand : public ViewItemCommand
{
public:
  LowerCommand()
      : ViewItemCommand(QObject::tr("Lower")) {}
  LowerCommand(ViewItem *item)
      : ViewItemCommand(item, QObject::tr("Lower")) {}

  virtual ~LowerCommand() {}

  virtual void undo();
  virtual void redo();
};

class KST_EXPORT ResizeCommand : public ViewItemCommand
{
public:
  ResizeCommand(const QTransform &originalTransform, const QTransform &newTransform)
      : ViewItemCommand(QObject::tr("Resize")),
        _originalTransform(originalTransform), _newTransform(newTransform) {}

  ResizeCommand(ViewItem *item, const QTransform &originalTransform, const QTransform &newTransform)
      : ViewItemCommand(item, QObject::tr("Resize")),
        _originalTransform(originalTransform), _newTransform(newTransform) {}

  virtual ~ResizeCommand() {}

  virtual void undo();
  virtual void redo();

private:
  QTransform _originalTransform;
  QTransform _newTransform;
};

}

#endif

// vim: ts=2 sw=2 et
