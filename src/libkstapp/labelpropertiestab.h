/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LABELPROPERTIESTAB_H
#define LABELPROPERTIESTAB_H

#include "dialogtab.h"
#include "ui_labelpropertiestab.h"

#include "kst_export.h"

namespace Kst {

class LabelPropertiesTab : public DialogTab, Ui::LabelPropertiesTab {
  Q_OBJECT
  public:
    LabelPropertiesTab(QWidget *parent = 0);
    virtual ~LabelPropertiesTab();

    QString labelText() const;
    void setLabelText(const QString &text);

    qreal labelScale() const;
    void setLabelScale(const qreal scale);

    QColor labelColor() const;
    void setLabelColor(const QColor &color);

    QFont labelFont() const;
    void setLabelFont(const QFont &font);
};

}

#endif

// vim: ts=2 sw=2 et
