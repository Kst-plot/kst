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

#ifndef LABELTAB_H
#define LABELTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "ui_labeltab.h"

#include "kst_export.h"

namespace Kst {

class LabelTab : public DialogTab, Ui::LabelTab {
  Q_OBJECT
  public:
    explicit LabelTab(PlotItem* plotItem, QWidget *parent = 0);
    virtual ~LabelTab();

    QString leftLabel() const;
    bool leftLabelDirty() const;
    void setLeftLabel(const QString &label);

    QString bottomLabel() const;
    bool bottomLabelDirty() const;
    void setBottomLabel(const QString &label);

    QString topLabel() const;
    bool topLabelDirty() const;
    void setTopLabel(const QString &label);

    QString rightLabel() const;
    bool rightLabelDirty() const;
    void setRightLabel(const QString &label);

    bool leftLabelAuto() const;
    bool leftLabelAutoDirty() const;
    void setLeftLabelAuto(bool a);

    bool rightLabelAuto() const;
    bool rightLabelAutoDirty() const;
    void setRightLabelAuto(bool a);

    bool topLabelAuto() const;
    bool topLabelAutoDirty() const;
    void setTopLabelAuto(bool a);

    bool bottomLabelAuto() const;
    bool bottomLabelAutoDirty() const;
    void setBottomLabelAuto(bool a);

    bool showLegend() const;
    bool showLegendDirty() const;
    void setShowLegend(const bool show);

    QFont globalLabelFont(const QFont ref_font) const;
    bool globalLabelFontDirty() const;

    qreal globalLabelFontScale() const;
    bool globalLabelFontScaleDirty() const;

    QColor globalLabelColor() const;
    bool globalLabelColorDirty() const;

    bool autoScaleNumbers() const;
    bool autoScaleNumbersDirty() const;
    void setAutoScaleNumbers(const bool scale);

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  Q_SIGNALS:
    void test();
    void globalFontUpdate();

  public Q_SLOTS:
    void update();

  private Q_SLOTS:
    void labelUpdate(const QString&);
    void labelSelected();
    void activateFields();
    void buttonUpdate();


private:
    void setGlobalFont(const QFont &font);

    PlotItem *_plotItem;
    QLineEdit* _activeLineEdit;
    bool _fontDirty;


};

}

#endif

// vim: ts=2 sw=2 et
