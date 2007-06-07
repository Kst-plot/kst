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

#ifndef VIEWCOMMAND_H
#define VIEWCOMMAND_H

#include <QPointer>
#include <QUndoCommand>
#include "kst_export.h"

namespace Kst {
class View;

class KST_EXPORT ViewCommand : public QUndoCommand
{
public:
  ViewCommand(const QString &text, bool addToStack = true, QUndoCommand *parent = 0);
  ViewCommand(View *view, const QString &text, bool addToStack = true, QUndoCommand *parent = 0);
  virtual ~ViewCommand();

protected:
  QPointer<View> _view;
};

}

#endif

// vim: ts=2 sw=2 et
