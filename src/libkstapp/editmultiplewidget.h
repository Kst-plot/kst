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

#ifndef EDITMULTIPLEWIDGET_H
#define EDITMULTIPLEWIDGET_H

#include <QWidget>
#include "ui_editmultiplewidget.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT EditMultipleWidget : public QWidget, public Ui::EditMultipleWidget {
  Q_OBJECT
  public:
    EditMultipleWidget(QWidget *parent = 0);
    virtual ~EditMultipleWidget();

  public slots:
    void selectAllObjects();
    void applyFilter(const QString &filter);
};

}

#endif

// vim: ts=2 sw=2 et
