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

#ifndef ARROWPROPERTIESTAB_H
#define ARROWPROPERTIESTAB_H

#include "dialogtab.h"
#include "ui_arrowpropertiestab.h"

#include "kst_export.h"

namespace Kst {

class ArrowPropertiesTab : public DialogTab, Ui::ArrowPropertiesTab {
  Q_OBJECT
  public:
    ArrowPropertiesTab(QWidget *parent = 0);
    virtual ~ArrowPropertiesTab();

    bool startArrowHead();
    void setStartArrowHead(const bool enabled);

    bool endArrowHead();
    void setEndArrowHead(const bool enabled);

    qreal startArrowScale();
    void setStartArrowScale(const qreal scale);

    qreal endArrowScale();
    void setEndArrowScale(const qreal scale);

  public Q_SLOTS:
    void update();
};

}

#endif

// vim: ts=2 sw=2 et
