/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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

class FillTab : public DialogTab, Ui::FillTab {
  Q_OBJECT
  public:
    FillTab(QWidget *parent = 0);
    virtual ~FillTab();

    void initialize(QBrush *b);

    QBrush brush(QBrush b = QBrush()) const;

    QColor color() const;
    bool colorDirty() const;
    void setColor(const QColor &color);

    Qt::BrushStyle style() const;
    bool styleDirty() const;
    void setStyle(Qt::BrushStyle style);

    QGradient gradient() const;
    bool gradientDirty() const;
    void setGradient(const QGradient &gradient);

    bool useGradient() const;
    bool useGradientDirty() const;
    void setUseGradient(const bool useGradient);

    GradientEditor *gradientEditor() { return _gradientEditor; }

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  public Q_SLOTS:
    void updateButtons();

  private:
    bool _multiEdit;
};

}

#endif

// vim: ts=2 sw=2 et
