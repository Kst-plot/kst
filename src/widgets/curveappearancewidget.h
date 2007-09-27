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

#ifndef CURVEAPPEARANCEWIDGET_H
#define CURVEAPPEARANCEWIDGET_H

#include <QWidget>
#include "ui_curveappearancewidget.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT CurveAppearanceWidget : public QWidget, public Ui::CurveAppearanceWidget {
  Q_OBJECT
  public:
    CurveAppearanceWidget(QWidget *parent = 0);
    virtual ~CurveAppearanceWidget();
};

}

#endif

// vim: ts=2 sw=2 et
