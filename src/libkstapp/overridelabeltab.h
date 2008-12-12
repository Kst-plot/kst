/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OVERRIDETAB_H
#define OVERRIDETAB_H

#include "dialogtab.h"
#include "ui_overridelabeltab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT OverrideLabelTab : public DialogTab, Ui::OverrideLabelTab {
  Q_OBJECT
  public:
    OverrideLabelTab(QString title, QWidget *parent = 0);
    virtual ~OverrideLabelTab();

    QFont labelFont() const;
    void setLabelFont(const QFont &font);

    qreal labelFontScale() const;
    void setLabelFontScale(const qreal scale);

    QColor labelColor() const;
    void setLabelColor(const QColor &color);
};

}

#endif

// vim: ts=2 sw=2 et
