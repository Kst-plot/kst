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

#ifndef CURVEPLACEMENTWIDGET_H
#define CURVEPLACEMENTWIDGET_H

#include <QWidget>
#include "ui_curveplacementwidget.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT CurvePlacementWidget : public QWidget, public Ui::CurvePlacementWidget {
  Q_OBJECT
  public:
    CurvePlacementWidget(QWidget *parent = 0);
    virtual ~CurvePlacementWidget();
};

}

#endif

// vim: ts=2 sw=2 et
