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
  ViewItem(View *parent);
  virtual ~ViewItem();

  View *parentView() const;

Q_SIGNALS:
  void creationComplete();

private Q_SLOTS:
  void mouseModeChanged();

private:
  QPointF _originalPosition;
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
  virtual void createItem() = 0;

public Q_SLOTS:
  void creationComplete();

protected:
  QPointer<ViewItem> _item;
};

class KST_EXPORT MoveCommand : public ViewItemCommand
{
public:
  MoveCommand(QPointF originalPos, QPointF newPos)
      : ViewItemCommand(QObject::tr("Move Object")),
        _originalPos(originalPos),
        _newPos(newPos) {}
  MoveCommand(ViewItem *item, QPointF originalPos, QPointF newPos)
      : ViewItemCommand(item, QObject::tr("Move Object")),
        _originalPos(originalPos),
        _newPos(newPos) {}

  virtual ~MoveCommand() {}

  virtual void undo();
  virtual void redo();

private:
  QPointF _originalPos;
  QPointF _newPos;
};

}

#endif

// vim: ts=2 sw=2 et
