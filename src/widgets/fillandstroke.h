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

#ifndef FILLANDSTROKE_H
#define FILLANDSTROKE_H

#include <QWidget>
#include "ui_fillandstroke.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT FillAndStroke : public QWidget, Ui::FillAndStroke {
  Q_OBJECT
public:
  FillAndStroke(QWidget *parent = 0);
  virtual ~FillAndStroke();

  QColor fillColor() const;
  void setFillColor(const QColor &color);

  Qt::BrushStyle fillStyle() const;
  void setFillStyle(Qt::BrushStyle style);

  QGradient fillGradient() const;
  void setFillGradient(const QGradient &gradient);

  Qt::PenStyle strokeStyle() const;
  void setStrokeStyle(Qt::PenStyle style);

  qreal strokeWidth() const;
  void setStrokeWidth(qreal width);

  QColor brushColor() const;
  void setBrushColor(const QColor &color);

  Qt::BrushStyle brushStyle() const;
  void setBrushStyle(Qt::BrushStyle style);

  Qt::PenJoinStyle joinStyle() const;
  void setJoinStyle(Qt::PenJoinStyle style);

  Qt::PenCapStyle capStyle() const;
  void setCapStyle(Qt::PenCapStyle style);

Q_SIGNALS:
  void fillChanged();
  void strokeChanged();
};

}

#endif

// vim: ts=2 sw=2 et
