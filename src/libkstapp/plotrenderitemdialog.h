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

#ifndef PLOTRENDERITEMDIALOG_H
#define PLOTRENDERITEMDIALOG_H

#include <QDialog>

#include "plotrenderitem.h"

#include "ui_plotrenderitemdialog.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT PlotRenderItemDialog : public QDialog, Ui::PlotRenderItemDialog
{
  Q_OBJECT
  public:
    PlotRenderItemDialog(PlotRenderItem *item, QWidget *parent = 0);
    virtual ~PlotRenderItemDialog();

    void exec();

  private:
    QPointer<PlotRenderItem> _item;
};

}

#endif

// vim: ts=2 sw=2 et
