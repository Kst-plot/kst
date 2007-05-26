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

#ifndef KSTPLOTCOMMANDS_H
#define KSTPLOTCOMMANDS_H

#include <QObject>
#include <QPointF>
#include <QPointer>
#include <QUndoCommand>

#include "kst_export.h"

class KstPlotView;
class KstPlotItem;

class KST_EXPORT KstPlotViewCommand : public QUndoCommand
{
public:
  KstPlotViewCommand(const QString &text,
                     bool addToStack = true, QUndoCommand *parent = 0);
  KstPlotViewCommand(KstPlotView *view, const QString &text,
                     bool addToStack = true, QUndoCommand *parent = 0);
  virtual ~KstPlotViewCommand();

protected:
  QPointer<KstPlotView> _view;
};

class KST_EXPORT KstPlotItemCommand : public QUndoCommand
{
public:
  KstPlotItemCommand(const QString &text,
                     bool addToStack = true, QUndoCommand *parent = 0);
  KstPlotItemCommand(KstPlotItem *item, const QString &text,
                     bool addToStack = true, QUndoCommand *parent = 0);
  virtual ~KstPlotItemCommand();

protected:
  QPointer<KstPlotItem> _item;
};

class KST_EXPORT CreateCommand : public QObject, public KstPlotViewCommand
{
  Q_OBJECT
public:
  CreateCommand(const QString &text, QUndoCommand *parent = 0);
  CreateCommand(KstPlotView *view, const QString &text, QUndoCommand *parent = 0);
  virtual ~CreateCommand();

  virtual void undo();
  virtual void redo();
  virtual void createItem() = 0;

public Q_SLOTS:
  void creationComplete();

protected:
  QPointer<KstPlotItem> _item;
};

class KST_EXPORT CreateLabelCommand : public CreateCommand
{
public:
  CreateLabelCommand() : CreateCommand(QObject::tr("Create Label")) {}
  CreateLabelCommand(KstPlotView *view): CreateCommand(view, QObject::tr("Create Label")) {}
  virtual ~CreateLabelCommand() {}
  virtual void createItem();
};

class KST_EXPORT CreateLineCommand : public CreateCommand
{
public:
  CreateLineCommand() : CreateCommand(QObject::tr("Create Line")) {}
  CreateLineCommand(KstPlotView *view) : CreateCommand(view, QObject::tr("Create Line")) {}
  virtual ~CreateLineCommand() {}
  virtual void createItem();
};

class KST_EXPORT MoveCommand : public KstPlotItemCommand
{
public:
  MoveCommand(QPointF originalPos, QPointF newPos)
      : KstPlotItemCommand(QObject::tr("Move Object")),
        _originalPos(originalPos),
        _newPos(newPos) {}
  MoveCommand(KstPlotItem *item, QPointF originalPos, QPointF newPos)
      : KstPlotItemCommand(item, QObject::tr("Move Object")),
        _originalPos(originalPos),
        _newPos(newPos) {}

  virtual ~MoveCommand() {}

  virtual void undo();
  virtual void redo();

private:
  QPointF _originalPos;
  QPointF _newPos;
};

/*
  LABEL
  BOX
  ELLIPSE
  LINE
  ARROW
  PICTURE
  PLOT
*/

#endif

// vim: ts=2 sw=2 et
