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

#ifndef LABELTAB_H
#define LABELTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "ui_labeltab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT LabelTab : public DialogTab, Ui::LabelTab {
  Q_OBJECT
  public:
    LabelTab(PlotItem* plotItem, QWidget *parent = 0);
    virtual ~LabelTab();

    QString leftLabel() const;
    void setLeftLabel(const QString &label);

    QString bottomLabel() const;
    void setBottomLabel(const QString &label);

    QString topLabel() const;
    void setTopLabel(const QString &label);

    QString rightLabel() const;
    void setRightLabel(const QString &label);

    bool showLegend() const;
    void setShowLegend(const bool show);

    QFont globalLabelFont() const;
    qreal globalLabelFontScale() const;
    QColor globalLabelColor() const;

  Q_SIGNALS:
    void test();
    void globalFontUpdate();

  public Q_SLOTS:
    void update();
    void autoLabel();
    void applyGlobals();

  private Q_SLOTS:
    void labelUpdate(const QString&);
    void labelSelected();

  private:
    void setGlobalFont(const QFont &font);

    PlotItem *_plotItem;
    QLineEdit* _activeLineEdit;

};

}

#endif

// vim: ts=2 sw=2 et
