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

#ifndef DEFAULTLABELPROPERTIESTAB_H
#define DEFAULTLABELPROPERTIESTAB_H

#include "dialogtab.h"
#include "ui_defaultlabelpropertiestab.h"

#include "kst_export.h"

namespace Kst {

class DefaultLabelPropertiesTab : public DialogTab, Ui_DefaultLabelPropertiesTab {
  Q_OBJECT
  public:
    DefaultLabelPropertiesTab(QWidget *parent = 0);
    virtual ~DefaultLabelPropertiesTab();

    qreal labelScale() const;
    void setLabelScale(const qreal scale);

    QColor labelColor() const;
    void setLabelColor(const QColor &color);

    QFont labelFont() const;
    void setLabelFont(const QFont &font);

    double referenceViewWidth() const;
    void setReferenceViewWidth(const double width);

    double referenceViewHeight() const;
    void setReferenceViewHeight(const double height);

    int minimumFontSize() const;
    void setMinimumFontSize(const int points);

    void checkSizeDefaults();

  public Q_SLOTS:
    void referenceViewSizeComboChanged(int i);
};

}

#endif

// vim: ts=2 sw=2 et
