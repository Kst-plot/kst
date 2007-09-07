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

#include "dialog.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class ViewItem;
class FillTab;
class StrokeTab;
class LayoutTab;

class KST_EXPORT ViewItemDialog : public Dialog
{
  Q_OBJECT
  public:
    ViewItemDialog(ViewItem *item, QWidget *parent = 0);
    virtual ~ViewItemDialog();

  private Q_SLOTS:
    void fillChanged();
    void strokeChanged();
    void layoutChanged();

  private:
    void setupFill();
    void setupStroke();
    void setupLayout();

  private:
    QPointer<ViewItem> _item;
    FillTab *_fillTab;
    StrokeTab *_strokeTab;
    LayoutTab *_layoutTab;
};

}

#endif

// vim: ts=2 sw=2 et
