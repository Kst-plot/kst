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

    QFont leftLabelFont() const;
    void setLeftLabelFont(const QFont &font);

    QFont rightLabelFont() const;
    void setRightLabelFont(const QFont &font);

    QFont topLabelFont() const;
    void setTopLabelFont(const QFont &font);

    QFont bottomLabelFont() const;
    void setBottomLabelFont(const QFont &font);

    qreal bottomLabelFontScale() const;
    void setBottomLabelFontScale(const qreal scale);

    qreal leftLabelFontScale() const;
    void setLeftLabelFontScale(const qreal scale);

    qreal topLabelFontScale() const;
    void setTopLabelFontScale(const qreal scale);

    qreal rightLabelFontScale() const;
    void setRightLabelFontScale(const qreal scale);

  public Q_SLOTS:
    void update();
    void autoLabel();
    void applyGlobals();

  private:
    void setGlobalFont(const QFont &font);

    PlotItem *_plotItem;

};

}

#endif

// vim: ts=2 sw=2 et
