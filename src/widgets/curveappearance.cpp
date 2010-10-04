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

#include "curveappearance.h"

#include "curvepointsymbol.h"
#include "linestyle.h"
#include <QPainter>
#include "dialogdefaults.h"
#include "colorsequence.h"

namespace Kst {

CurveAppearance::CurveAppearance(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);  
  populatePointSymbolCombo();
  populateLineStyleCombo();

  drawSampleLine();

  connect(_showPoints, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));
  connect(_showLines, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));
  connect(_showBars, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));

  connect(_color, SIGNAL(changed(const QColor&)), this, SLOT(populatePointSymbolCombo()));
  connect(_color, SIGNAL(changed(const QColor&)), this, SLOT(populateLineStyleCombo()));

  connect(_color, SIGNAL(changed(const QColor&)), this, SLOT(drawSampleLine()));
  connect(_showLines, SIGNAL(clicked()), this, SLOT(drawSampleLine()));
  connect(_showPoints, SIGNAL(clicked()), this, SLOT(drawSampleLine()));
  connect(_comboPointSymbol, SIGNAL(activated(int)), this, SLOT(drawSampleLine()));
  connect(_comboLineStyle, SIGNAL(activated(int)), this, SLOT(drawSampleLine()));
  connect(_spinBoxLineWidth, SIGNAL(valueChanged(int)), this, SLOT(drawSampleLine()));
  connect(_barStyle, SIGNAL(activated(int)), this, SLOT(drawSampleLine()));
  connect(_showBars, SIGNAL(clicked()), this, SLOT(drawSampleLine()));

  connect(_color, SIGNAL(changed(const QColor&)), this, SIGNAL(modified()));
  connect(_showLines, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_showPoints, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_comboPointDensity, SIGNAL(activated(int)), this, SIGNAL(modified()));
  connect(_comboPointSymbol, SIGNAL(activated(int)), this, SIGNAL(modified()));
  connect(_comboLineStyle, SIGNAL(activated(int)), this, SIGNAL(modified()));
  connect(_spinBoxLineWidth, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_barStyle, SIGNAL(activated(int)), this, SIGNAL(modified()));
  connect(_showBars, SIGNAL(clicked()), this, SIGNAL(modified()));
}


CurveAppearance::~CurveAppearance() {
}


void CurveAppearance::populatePointSymbolCombo() {
  QStyleOptionComboBox option;
  option.initFrom(_comboPointSymbol);
  option.currentIcon = _comboPointSymbol->itemIcon(_comboPointSymbol->currentIndex());
  option.currentText = _comboPointSymbol->itemText(_comboPointSymbol->currentIndex());
  option.editable = _comboPointSymbol->isEditable();
  option.frame = _comboPointSymbol->hasFrame();
  option.iconSize = _comboPointSymbol->iconSize();

  QRect rect = _comboPointSymbol->style()->subControlRect(
                 QStyle::CC_ComboBox,
                 &option,
                 QStyle::SC_ComboBoxEditField,
                 _comboPointSymbol );
  rect.setLeft( rect.left() + 2 );
  rect.setRight( rect.right() - 2 );
  rect.setTop( rect.top() + 2 );
  rect.setBottom( rect.bottom() - 2 );

_comboPointSymbol->setIconSize(QSize(rect.width(), rect.height()));

  // fill the point type dialog with point types
  QPixmap ppix( rect.width(), rect.height() );
  QPainter pp( &ppix );

  int currentItem = _comboPointSymbol->currentIndex();
  _comboPointSymbol->clear();
  pp.setPen(color());

  for (int ptype = 0; ptype < KSTPOINT_MAXTYPE; ptype++) {
    pp.fillRect(pp.window(), QColor("white"));
    CurvePointSymbol::draw(ptype, &pp, ppix.width()/2, ppix.height()/2, 0, 600);
    _comboPointSymbol->addItem(QIcon(ppix), QString());
  }

  if (currentItem > 0) {
    _comboPointSymbol->setCurrentIndex( currentItem );
  }
}


void CurveAppearance::enableSettings() {
  bool enable;

  enable = showLines() || showBars();
  _comboLineStyle->setEnabled(enable);
  _textLabelLineStyle->setEnabled(enable);

  enable = enable || showPoints();
  _textLabelWeight->setEnabled(enable);
  _spinBoxLineWidth->setEnabled(enable);

  enable = showBars();
  _textLabelBarStyle->setEnabled(enable);
  _barStyle->setEnabled(enable);

  enable = showPoints();
  _textLabelPointStyle->setEnabled(enable);
  _comboPointSymbol->setEnabled(enable);

  enable = enable && showLines();
  _textLabelPointDensity->setEnabled(enable);
  _comboPointDensity->setEnabled(enable);
}


bool CurveAppearance::showLines() const {
  return _showLines->checkState() == Qt::Checked;

}


bool CurveAppearance::showLinesDirty() const {
  return _showLines->checkState() != Qt::PartiallyChecked;
}


void CurveAppearance::setShowLines(const bool showLines) {
  _showLines->setChecked(showLines);
  enableSettings();
  drawSampleLine();
}


bool CurveAppearance::showPoints() const {
  return _showPoints->checkState() == Qt::Checked;
}


bool CurveAppearance::showPointsDirty() const {
  return _showPoints->checkState() != Qt::PartiallyChecked;
}


void CurveAppearance::setShowPoints(const bool showPoints) {
  _showPoints->setChecked(showPoints);
  enableSettings();
  drawSampleLine();
}


bool CurveAppearance::showBars() const {
  return _showBars->checkState() == Qt::Checked;
}


bool CurveAppearance::showBarsDirty() const {
  return _showBars->checkState() != Qt::PartiallyChecked;
}


void CurveAppearance::setShowBars(const bool showBars) {
  _showBars->setChecked(showBars);
  enableSettings();
  drawSampleLine();
}


QColor CurveAppearance::color() const {
  return _color->color();
}


bool CurveAppearance::colorDirty() const {
  return _color->colorDirty();
}


void CurveAppearance::setColor(const QColor & c) {
  _color->setColor(c);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::pointType() const {
  return _comboPointSymbol->currentIndex();
}


bool CurveAppearance::pointTypeDirty() const {
  return _comboPointSymbol->currentIndex() != -1;
}


void CurveAppearance::setPointType(const int pointType) {
  _comboPointSymbol->setCurrentIndex(pointType);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::lineStyle() const {
  return _comboLineStyle->currentIndex();
}


bool CurveAppearance::lineStyleDirty() const {
  return _comboLineStyle->currentIndex() != -1;
}


void CurveAppearance::setLineStyle(int lineStyle) {
  if (lineStyle < 0 || lineStyle >= (int)LINESTYLE_MAXTYPE) {
    lineStyle = 0;
  }
  _comboLineStyle->setCurrentIndex(lineStyle);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::barStyle() const {
 return _barStyle->currentIndex();
}


bool CurveAppearance::barStyleDirty() const {
  return _barStyle->currentIndex() != -1;
}


void CurveAppearance::setBarStyle(const int barStyle) {
  _barStyle->setCurrentIndex(barStyle);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::pointDensity() const {
  return _comboPointDensity->currentIndex();
}


bool CurveAppearance::pointDensityDirty() const {
  return _comboPointDensity->currentIndex() != -1;
}


void CurveAppearance::setPointDensity(int pointDensity) {
  if (pointDensity < 0 || pointDensity >= POINTDENSITY_MAXTYPE) {
    pointDensity = 0;
  }
  _comboPointDensity->setCurrentIndex(pointDensity);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::lineWidth() const {
  if (_spinBoxLineWidth->text() == " ") {
    return 0;
  } else {
    return _spinBoxLineWidth->value();
  }
}


bool CurveAppearance::lineWidthDirty() const {
  return !_spinBoxLineWidth->text().isEmpty();
}


void CurveAppearance::setLineWidth(const int lineWidth) {
  _spinBoxLineWidth->setValue(lineWidth);
  enableSettings();
  drawSampleLine();
}


void CurveAppearance::clearValues() {
  _color->clearSelection();
  _spinBoxLineWidth->clear();
  _comboPointSymbol->setCurrentIndex(-1);
  _comboPointDensity->setCurrentIndex(-1);
  _comboLineStyle->setCurrentIndex(-1);
  _barStyle->setCurrentIndex(-1);
  _showPoints->setCheckState(Qt::PartiallyChecked);
  _showLines->setCheckState(Qt::PartiallyChecked);
  _showBars->setCheckState(Qt::PartiallyChecked);
  drawSampleLine();
}


void CurveAppearance::populateLineStyleCombo() {

  QStyleOptionComboBox option;
  option.initFrom(_comboLineStyle);
  option.currentIcon = _comboLineStyle->itemIcon(_comboLineStyle->currentIndex());
  option.currentText = _comboLineStyle->itemText(_comboLineStyle->currentIndex());
  option.editable = _comboLineStyle->isEditable();
  option.frame = _comboLineStyle->hasFrame();
  option.iconSize = _comboLineStyle->iconSize();

  QRect rect = _comboLineStyle->style()->subControlRect(
                 QStyle::CC_ComboBox,
                 &option,
                 QStyle::SC_ComboBoxEditField,
                 _comboLineStyle );
  rect.setLeft(rect.left() + 2);
  rect.setRight(rect.right() - 2);
  rect.setTop(rect.top() + 2);
  rect.setBottom(rect.bottom() - 2);

  _comboLineStyle->setIconSize(QSize(rect.width(), rect.height()));

  // fill the point type dialog with point types
  QPixmap ppix(rect.width(), rect.height());
  QPainter pp(&ppix);
  QColor lineColor(color());
  if (lineColor == Qt::transparent) {
    lineColor = Qt::black;
  }
  QPen pen(lineColor, 1);

  int currentItem = _comboLineStyle->currentIndex();
  _comboLineStyle->clear();

  for (int style = 0; style < (int)LINESTYLE_MAXTYPE; style++) {
    pen.setStyle(LineStyle[style]);
    pp.setPen(pen);
    pp.fillRect( pp.window(), QColor("white"));
    pp.drawLine(1,ppix.height()/2,ppix.width()-1, ppix.height()/2);
    _comboLineStyle->addItem(QIcon(ppix), QString());
  }

  if (currentItem > 0) {
    _comboLineStyle->setCurrentIndex( currentItem );
  }
}


void CurveAppearance::drawSampleLine() {
  QPixmap pix(_label->contentsRect().height()*7, _label->contentsRect().height());
  QPainter p(&pix);
  QPen pen(color(),lineWidth(),LineStyle[lineStyle()]);

  p.fillRect(p.window(), QColor("white"));

  if (showBars()) {
    QRect rectBar((pix.width()-pix.height())/2,
                  pix.height()/2,
                  pix.height(),
                  (pix.height()/2)+1);

    if (barStyle() == 1) {
      p.fillRect(rectBar,QBrush(QColor(color())));
      p.setPen(QPen(QColor("black"),lineWidth(), LineStyle[lineStyle()]));
    } else {
      p.setPen(pen);
    }
    p.drawRect(rectBar);
  }

#ifdef Q_WS_WIN32
  // Note:  This modification was made in response to an apparent bug in Qt 4.4.0 in which any
  // painting done with a cosmetic pen resulted in a crash.

  // When this is no longer required similar code should also be removed in ViewItemDialog.cpp and
  // curve.cpp - setLineWidth().
  if (pen.isCosmetic()) {
    pen.setWidth(1);
  }
#endif
  p.setPen(pen);
  if (showLines()) {
    p.drawLine(1, pix.height()/2, pix.width()-1, pix.height()/2);
  }

  if (showPoints()) {
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);
    CurvePointSymbol::draw(pointType(), &p, pix.width()/2, pix.height()/2, lineWidth(), 600);
  }

  _label->setPixmap(pix);
}

// store the current state of the widget as the default 
void CurveAppearance::setWidgetDefaults(bool nextColor) {
  if (nextColor) {
    ColorSequence::next();
  }
  _dialogDefaults->setValue("curves/showPoints",showPoints());
  _dialogDefaults->setValue("curves/showLines", showLines());
  _dialogDefaults->setValue("curves/showBars",showBars());
  _dialogDefaults->setValue("curves/lineWidth",lineWidth());
  _dialogDefaults->setValue("curves/lineStyle",lineStyle());
  _dialogDefaults->setValue("curves/pointType", pointType());
  _dialogDefaults->setValue("curves/pointDensity", pointDensity());
  _dialogDefaults->setValue("curves/barStyle", barStyle());
}

// set the widget to the stored default values
void CurveAppearance::loadWidgetDefaults() {
  setColor(ColorSequence::current());
  setShowPoints(_dialogDefaults->value("curves/showPoints",false).toBool());
  setShowLines(_dialogDefaults->value("curves/showLines",true).toBool());
  setShowBars(_dialogDefaults->value("curves/showBars",false).toBool());
  setLineWidth(_dialogDefaults->value("curves/lineWidth",0).toInt());
  setLineStyle(_dialogDefaults->value("curves/lineStyle",0).toInt());
  setPointType(_dialogDefaults->value("curves/pointType",0).toInt());
  setPointDensity(_dialogDefaults->value("curves/pointDensity",0).toInt());
  setBarStyle(_dialogDefaults->value("curves/barStyle",0).toInt());
}

}
// vim: ts=2 sw=2 et
