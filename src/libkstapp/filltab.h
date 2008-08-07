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

#ifndef FILLTAB_H
#define FILLTAB_H

#include "dialogtab.h"
#include "ui_filltab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT FillTab : public DialogTab, Ui::FillTab {
  Q_OBJECT
  public:
    FillTab(bool resetFullMono = true, QWidget *parent = 0);
    virtual ~FillTab();

    QColor color() const;
    void setColor(const QColor &color);

    Qt::BrushStyle style() const;
    void setStyle(Qt::BrushStyle style);

    QGradient gradient() const;
    void setGradient(const QGradient &gradient);

  public Q_SLOTS:
    void updateButtons();
};

}

#endif

// vim: ts=2 sw=2 et
