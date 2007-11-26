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

#include "curveappearance.h"

#include "curvepointsymbol.h"
#include "linestyle.h"
#include <QPainter>

namespace Kst {

CurveAppearance::CurveAppearance(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);
  drawSampleLine();
  populatePointSymbolCombo();
  populateLineStyleCombo();

  connect(_showPoints, SIGNAL(toggled(bool)), this, SLOT(enableSettings()));
  connect(_showLines, SIGNAL(toggled(bool)), this, SLOT(enableSettings()));
  connect(_showBars, SIGNAL(toggled(bool)), this, SLOT(enableSettings()));

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
    _comboPointSymbol->addItem(QIcon(ppix), QString::null);
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
  return _showLines->isChecked();

}


void CurveAppearance::setShowLines(const bool showLines) {
  _showLines->setChecked(showLines);
  enableSettings();
  drawSampleLine();
}


bool CurveAppearance::showPoints() const {
  return _showPoints->isChecked();
}


void CurveAppearance::setShowPoints(const bool showPoints) {
  _showPoints->setChecked(showPoints);
  enableSettings();
  drawSampleLine();
}


bool CurveAppearance::showBars() const {
  return _showBars->isChecked();
}


void CurveAppearance::setShowBars(const bool showBars) {
  _showBars->setChecked(showBars);
  enableSettings();
  drawSampleLine();
}

QColor CurveAppearance::color() const {
  return _color->color();
}


void CurveAppearance::setColor(const QColor & c) {
  _color->setColor(c);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::pointType() const {
  return _comboPointSymbol->currentIndex();
}


void CurveAppearance::setPointType(const int pointType) {
  _comboPointSymbol->setCurrentIndex(pointType);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::lineStyle() const {
  return _comboLineStyle->currentIndex();
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


void CurveAppearance::setBarStyle(const int barStyle) {
  _barStyle->setCurrentIndex(barStyle);
  enableSettings();
  drawSampleLine();
}


int CurveAppearance::pointDensity() const {
  return _comboPointDensity->currentIndex();
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


void CurveAppearance::setLineWidth(const int lineWidth) {
  _spinBoxLineWidth->setValue(lineWidth);
  enableSettings();
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
  QPen pen(color(), 0);

  int currentItem = _comboLineStyle->currentIndex();
  _comboLineStyle->clear();

  for (int style = 0; style < (int)LINESTYLE_MAXTYPE; style++) {
    pen.setStyle(LineStyle[style]);
    pp.setPen(pen);
    pp.fillRect( pp.window(), QColor("white"));
    pp.drawLine(1,ppix.height()/2,ppix.width()-1, ppix.height()/2);
    _comboLineStyle->addItem(QIcon(ppix), QString::null);
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

  p.setPen(pen);
  if (_showLines->isChecked()) {
    p.drawLine(1, pix.height()/2, pix.width()-1, pix.height()/2);
  }

  if (_showPoints->isChecked()) {
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);
    CurvePointSymbol::draw(pointType(), &p, pix.width()/2, pix.height()/2, lineWidth(), 600);
  }

  _label->setPixmap(pix);
}


}
// vim: ts=2 sw=2 et
