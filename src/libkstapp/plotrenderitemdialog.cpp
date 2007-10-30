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

#include "settings.h"
#include "plotdefines.h"
#include "linestyle.h"
#include "curve.h"
#include "scalar.h"
#include "dataobjectcollection.h"
#include "datacollection.h"

namespace Kst {

struct MajorTickSpacing {
  const char *label;
  int majorTickDensity;
};

const MajorTickSpacing MajorTickSpacings[] = {
      { "Coarse", 2 },
      { "Default", 5 },
      { "Fine", 10 },
      { "Very fine", 15 }
    };

const unsigned int numMajorTickSpacings = sizeof( MajorTickSpacings ) / sizeof( MajorTickSpacing );

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

  connect(_xMajorGrid, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));
  connect(_yMajorGrid, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));
  connect(_xMinorGrid, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));
  connect(_yMinorGrid, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));

  connect(_xMinorTicksAuto, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));
  connect(_yMinorTicksAuto, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));
  connect(_checkBoxDefaultMajorGridColor, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));
  connect(_checkBoxDefaultMinorGridColor, SIGNAL(clicked()), this, SLOT(updateAxesButtons()));


  for (unsigned i = 0; i < numMajorTickSpacings; i++) {
    _xMajorTickSpacing->addItem(i18n(MajorTickSpacings[i].label));
    _yMajorTickSpacing->addItem(i18n(MajorTickSpacings[i].label));
  }

  _xMajorTickSpacing->setCurrentIndex(1);
  _yMajorTickSpacing->setCurrentIndex(1);


  for (unsigned i = 0; i < numAxisInterpretations; i++) {
    _comboBoxXInterpret->addItem(i18n(AxisInterpretations[i].label));
    _comboBoxYInterpret->addItem(i18n(AxisInterpretations[i].label));
  }
  for (unsigned i = 0; i < numAxisDisplays; i++) {
    _comboBoxXDisplay->addItem(i18n(AxisDisplays[i].label));
    _comboBoxYDisplay->addItem(i18n(AxisDisplays[i].label));
  }

  QColor defaultColor((Settings::globalSettings()->foregroundColor.red() + Settings::globalSettings()->backgroundColor.red())/2,
                      (Settings::globalSettings()->foregroundColor.green() + Settings::globalSettings()->backgroundColor.green())/2,
                      (Settings::globalSettings()->foregroundColor.blue() + Settings::globalSettings()->backgroundColor.blue())/2);
  _majorGridColor->setColor(defaultColor);
  _minorGridColor->setColor(defaultColor);

  _plotColors->setColor(Settings::globalSettings()->backgroundColor);

  _comboBoxTopLabelJustify->addItem("Left");
  _comboBoxTopLabelJustify->addItem("Right");
  _comboBoxTopLabelJustify->addItem("Center");

  _colorMarker->setColor(QColor("black"));

  fillMarkerLineCombo();

  update();
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


void PlotRenderItemDialog::updateAxesButtons() {
  bool major = _xMajorGrid->isChecked() || _yMajorGrid->isChecked();
  bool minor = _xMinorGrid->isChecked() || _yMinorGrid->isChecked();

  _checkBoxDefaultMajorGridColor->setEnabled(major);
  _checkBoxDefaultMinorGridColor->setEnabled(minor);
  _majorGridColor->setEnabled(major && !_checkBoxDefaultMajorGridColor->isChecked());
  _minorGridColor->setEnabled(minor && !_checkBoxDefaultMinorGridColor->isChecked());
  _majorPenWidth->setEnabled(major);
  _minorPenWidth->setEnabled(minor);
  _majorPenWidthLabel->setEnabled(major);
  _minorPenWidthLabel->setEnabled(minor);
  _xMinorTicks->setEnabled(!_xMinorTicksAuto->isChecked());
  _yMinorTicks->setEnabled(!_yMinorTicksAuto->isChecked());
}


void PlotRenderItemDialog::update() {
  _vectorForMarkers->update();
  _scalarSelectorX1->update();
  _scalarSelectorY1->update();
  _scalarSelectorX2->update();
  _scalarSelectorY2->update();

  CurveList curves; //FIXME // = ObjectSubList<DataObject, Curve>(dataObjectList);

  _displayedCurveList->clear();
  _availableCurveList->clear();

  //TODO  Populate the curve list.

  updateScalarCombo();

  // TODO Update with plot/view details.
}



void PlotRenderItemDialog::updateScalarCombo() {
  _scalarList->clear();
  _scalarSelectorX1->clear();
  _scalarSelectorX2->clear();
  _scalarSelectorY1->clear();
  _scalarSelectorY2->clear();

  ScalarList sl; //FIXME = scalarList.list();

  qSort(sl);
  for (ScalarList::ConstIterator i = sl.begin(); i != sl.end(); ++i) {
    (*i)->readLock();
    QString n = (*i)->tag().displayString();
    (*i)->unlock();
    _scalarList->addItem(n);
    _scalarSelectorX1->addItem(n);
    _scalarSelectorX2->addItem(n);
    _scalarSelectorY1->addItem(n);
    _scalarSelectorY2->addItem(n);
  }
}


void PlotRenderItemDialog::fillMarkerLineCombo() {
  QStyleOptionComboBox option;
  option.initFrom(_comboMarkerLineStyle);
  option.currentIcon = _comboMarkerLineStyle->itemIcon(_comboMarkerLineStyle->currentIndex());
  option.currentText = _comboMarkerLineStyle->itemText(_comboMarkerLineStyle->currentIndex());
  option.editable = _comboMarkerLineStyle->isEditable();
  option.frame = _comboMarkerLineStyle->hasFrame();
  option.iconSize = _comboMarkerLineStyle->iconSize();

  QRect rect = _comboMarkerLineStyle->style()->subControlRect(
                 QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxEditField, _comboMarkerLineStyle);
  rect.setLeft(rect.left() + 2);
  rect.setRight(rect.right() - 2);
  rect.setTop(rect.top() + 2);
  rect.setBottom(rect.bottom() - 2);

  // fill the point type dialog with point types
  QPixmap ppix(rect.width(), rect.height());
  QPainter pp(&ppix);
  QPen pen(QColor("black"), 0);

  int currentIndex = _comboMarkerLineStyle->currentIndex();
  _comboMarkerLineStyle->clear();

  for (int style = 0; style < (int)LINESTYLE_MAXTYPE; style++) {
    pen.setStyle(LineStyle[style]);
    pp.setPen(pen);
    pp.fillRect( pp.window(), QColor("white"));
    pp.drawLine(1,ppix.height()/2,ppix.width()-1, ppix.height()/2);
    _comboMarkerLineStyle->addItem(QIcon(ppix), QString::null);
  }

  _comboMarkerLineStyle->setCurrentIndex(currentIndex);
}

}
// vim: ts=2 sw=2 et
