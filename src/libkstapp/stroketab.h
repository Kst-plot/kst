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

#ifndef STROKETAB_H
#define STROKETAB_H

#include "dialogtab.h"
#include "ui_stroketab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT StrokeTab : public DialogTab, Ui::StrokeTab {
  Q_OBJECT
public:
  StrokeTab(QWidget *parent = 0);
  virtual ~StrokeTab();

  Qt::PenStyle style() const;
  void setStyle(Qt::PenStyle style);

  qreal width() const;
  void setWidth(qreal width);

  QColor brushColor() const;
  void setBrushColor(const QColor &color);

  Qt::BrushStyle brushStyle() const;
  void setBrushStyle(Qt::BrushStyle style);

  Qt::PenJoinStyle joinStyle() const;
  void setJoinStyle(Qt::PenJoinStyle style);

  Qt::PenCapStyle capStyle() const;
  void setCapStyle(Qt::PenCapStyle style);
};

}

#endif

// vim: ts=2 sw=2 et
