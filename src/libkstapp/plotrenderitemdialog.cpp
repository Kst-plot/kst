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

#include "plotrenderitemdialog.h"

namespace Kst {

PlotRenderItemDialog::PlotRenderItemDialog(PlotRenderItem *item, QWidget *parent)
  : QDialog(parent), _item(item) {
   setupUi(this);

  //TODO Need icons.
  _add->setText("Add");
  _remove->setText("Remove");
  _up->setText("Up");
  _down->setText("Down");

  connect(_checkBoxXInterpret, SIGNAL(toggled(bool)), this, SLOT(xAxisInterpretToggled(bool)));
  connect(_checkBoxYInterpret, SIGNAL(toggled(bool)), this, SLOT(yAxisInterpretToggled(bool)));
  connect(_xTransformTop, SIGNAL(toggled(bool)), this, SLOT(xTransformTopToggled(bool)));
  connect(_yTransformRight, SIGNAL(toggled(bool)), this, SLOT(yTransformRightToggled(bool)));
  connect(_xMinorTicksAuto, SIGNAL(toggled(bool)), this, SLOT(xMinorTicksAutoToggled(bool)));
  connect(_yMinorTicksAuto, SIGNAL(toggled(bool)), this, SLOT(yMinorTicksAutoToggled(bool)));

  connect(_xMeanCentered, SIGNAL(toggled(bool)), this, SLOT(xMeanCenteredToggled(bool)));
  connect(_yMeanCentered, SIGNAL(toggled(bool)), this, SLOT(yMeanCenteredToggled(bool)));
  connect(_xExpression, SIGNAL(toggled(bool)), this, SLOT(xExpressionToggled(bool)));
  connect(_yExpression, SIGNAL(toggled(bool)), this, SLOT(yExpressionToggled(bool)));

  connect(_useCurveCheckBox, SIGNAL(toggled(bool)), this, SLOT(useCurveToggled(bool)));
  connect(_useVectorCheckBox, SIGNAL(toggled(bool)), this, SLOT(useVectorToggled(bool)));

  connect(_cancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(_OK, SIGNAL(clicked()), this, SLOT(accept()));
  connect(_apply, SIGNAL(clicked()), this, SLOT(accept()));
}


PlotRenderItemDialog::~PlotRenderItemDialog() {
}


void PlotRenderItemDialog::exec() {
  QDialog::exec();
}


void PlotRenderItemDialog::xAxisInterpretToggled(bool checked) {
  _comboBoxXInterpret->setEnabled(checked);
  _comboBoxXDisplay->setEnabled(checked);
  _textLabelXDisplayAs->setEnabled(checked);
}


void PlotRenderItemDialog::yAxisInterpretToggled(bool checked) {
  _comboBoxYInterpret->setEnabled(checked);
  _comboBoxYDisplay->setEnabled(checked);
  _textLabelYDisplayAs->setEnabled(checked);
}


void PlotRenderItemDialog::xTransformTopToggled(bool checked) {
  _xTransformTopExp->setEnabled(checked);
}


void PlotRenderItemDialog::yTransformRightToggled(bool checked) {
  _yTransformRightExp->setEnabled(checked);
}


void PlotRenderItemDialog::xMinorTicksAutoToggled(bool checked) {
  _xMinorTicks->setEnabled(!checked);
}


void PlotRenderItemDialog::yMinorTicksAutoToggled(bool checked) {
  _yMinorTicks->setEnabled(!checked);
}


void PlotRenderItemDialog::xMeanCenteredToggled(bool checked) {
  _xRange->setEnabled(checked);
}


void PlotRenderItemDialog::yMeanCenteredToggled(bool checked) {
  _yRange->setEnabled(checked);
}


void PlotRenderItemDialog::xExpressionToggled(bool checked) {
  _xExpressionMin->setEnabled(checked);
  _xExpressionMax->setEnabled(checked);
  _scalarSelectorX1->setEnabled(checked);
  _scalarSelectorX2->setEnabled(checked);
}


void PlotRenderItemDialog::yExpressionToggled(bool checked) {
  _yExpressionMin->setEnabled(checked);
  _yExpressionMax->setEnabled(checked);
  _scalarSelectorY1->setEnabled(checked);
  _scalarSelectorY2->setEnabled(checked);
}


void PlotRenderItemDialog::useCurveToggled(bool checked) {
  _curveCombo->setEnabled(checked);
  _textLabelCreateMarkersOn->setEnabled(checked);
  _markersRisingEdge->setEnabled(checked);
  _markersFallingEdge->setEnabled(checked);
  _markersBoth->setEnabled(checked);
}


void PlotRenderItemDialog::useVectorToggled(bool checked) {
  _vectorForMarkers->setEnabled(checked);
}

}
// vim: ts=2 sw=2 et
