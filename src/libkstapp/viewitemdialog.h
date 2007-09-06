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

#ifndef VIEWITEMDIALOG_H
#define VIEWITEMDIALOG_H

#include <QDialog>
#include <QPointer>

#include "kst_export.h"

namespace Kst {

class ViewItem;
class FillAndStroke;

class KST_EXPORT ViewItemDialog : public QDialog
{
  Q_OBJECT
public:
  static ViewItemDialog *self();

  void show(ViewItem *item);

private:
  ViewItemDialog(QWidget *parent = 0);
  virtual ~ViewItemDialog();
  void setupFill();
  void setupStroke();
  static void cleanup();

private Q_SLOTS:
  void fillChanged();
  void strokeChanged();

protected:
  void setVisible(bool visible);

private:
  QPointer<ViewItem> _item;
  FillAndStroke *_fillAndStroke;
};

}

#endif

// vim: ts=2 sw=2 et
