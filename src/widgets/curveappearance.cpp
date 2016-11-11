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
  populateSymbolCombos();
  populateLineStyleCombo();

  drawSampleLine();

  connect(_showPoints, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));
  connect(_showLines, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));
  connect(_showBars, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));
  connect(_showHead, SIGNAL(stateChanged(int)), this, SLOT(enableSettings()));

  connect(_color, SIGNAL(changed(QColor)), this, SLOT(populateSymbolCombos()));
  connect(_color, SIGNAL(changed(QColor)), this, SLOT(populateLineStyleCombo()));
  connect(_headColor, SIGNAL(changed(QColor)), this, SLOT(populateSymbolCombos()));
  connect(_headColor, SIGNAL(changed(QColor)), this, SLOT(populateLineStyleCombo()));

  connect(_color, SIGNAL(changed(QColor)), this, SLOT(drawSampleLine()));
  connect(_headColor, SIGNAL(changed(QColor)), this, SLOT(drawSampleLine()));
  connect(_barFillColor, SIGNAL(changed(QColor)), this, SLOT(drawSampleLine()));
  connect(_showLines, SIGNAL(clicked()), this, SLOT(drawSampleLine()));
  connect(_showPoints, SIGNAL(clicked()), this, SLOT(drawSampleLine()));
  connect(_showHead, SIGNAL(clicked()), this, SLOT(drawSampleLine()));
  connect(_comboPointSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(drawSampleLine()));
  connect(_comboLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(drawSampleLine()));
  connect(_comboHeadSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(drawSampleLine()));
  connect(_spinBoxLineWidth, SIGNAL(valueChanged(int)), this, SLOT(drawSampleLine()));
  connect(_spinBoxPointSize, SIGNAL(valueChanged(double)), this, SLOT(drawSampleLine()));
  connect(_showBars, SIGNAL(clicked()), this, SLOT(drawSampleLine()));

  connect(_color, SIGNAL(changed(QColor)), this, SIGNAL(modified()));
  connect(_headColor, SIGNAL(changed(QColor)), this, SIGNAL(modified()));
  connect(_barFillColor, SIGNAL(changed(QColor)), this, SIGNAL(modified()));
  connect(_showLines, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_showPoints, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_showHead, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_comboPointDensity, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_comboPointSymbol, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_comboHeadSymbol, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_comboLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_spinBoxLineWidth, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_spinBoxPointSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_showBars, SIGNAL(clicked()), this, SIGNAL(modified()));
}


CurveAppearance::~CurveAppearance() {
}


void CurveAppearance::populateSymbolCombos() {
  populateSymbolCombo(_comboPointSymbol, color());
  populateSymbolCombo(_comboHeadSymbol, headColor());
}


void CurveAppearance::populateSymbolCombo(QComboBox *combo, QColor symbolColor) {
#ifdef QT5
  int pixel_ratio = combo->devicePixelRatio();
#else
  int pixel_ratio = 1;
#endif

  if (symbolColor == Qt::transparent) {
    symbolColor = Qt::black;
  }
  int h = fontMetrics().lineSpacing();

  combo->setIconSize(QSize(4*h, h));

  // fill the point type dialog with point types
  QPixmap ppix( 4*h*pixel_ratio, h*pixel_ratio );

#ifdef QT5
  ppix.setDevicePixelRatio(pixel_ratio);
#endif

  int pix_w = ppix.width()/pixel_ratio;
  int pix_h = ppix.height()/pixel_ratio;


  QPainter pp( &ppix );

  int currentItem = combo->currentIndex();
  combo->clear();
  QPen pen(symbolColor);
  pen.setWidthF(h/18.0);
  pp.setPen(pen);

  for (int ptype = 0; ptype < KSTPOINT_MAXTYPE; ptype++) {
    pp.fillRect(pp.window(), QColor("white"));
    CurvePointSymbol::draw(ptype, &pp, pix_w/2, pix_h/2, h/4);
    combo->addItem(QIcon(ppix), QString());
  }

  if (currentItem > 0) {
    combo->setCurrentIndex( currentItem );
  }
}


void CurveAppearance::enableSettings() {
  bool enable;

  // Use an intermediate boolean to avoid having to use setEnabled(true) and setEnabled(false)
  enable = showLines() || showBars();

  // Line appearance options common to lines and bars
  _comboLineStyle->setEnabled(enable);

  // Option specific to bars
  enable = showBars();
  _barFillColor->setEnabled(enable);

  // Now point options
  enable = showPoints();
  _comboPointSymbol->setEnabled(enable);
  _spinBoxPointSize->setEnabled(enable);
  _textLabelPointSize->setEnabled(enable);

  // and disable widget if not using lines, as using only points and not plotting all of them sounds weird
  enable = enable && showLines();
  _comboPointDensity->setEnabled(enable);
  _textLabelPointDensity->setEnabled(enable);

  // Heads
  enable = showHead();
  _comboHeadSymbol->setEnabled(enable);
  _headColor->setEnabled(enable);

}


bool CurveAppearance::showLines() const {
  return _showLines->checkState() == Qt::Checked;

}


bool CurveAppearance::showLinesDirty() const {
  return _showLines->checkState() != Qt::PartiallyChecked;
}


void CurveAppearance::setShowLines(const bool showLines) {
  _showLines->setChecked(showLines);
  _showLines->setTristate(false);
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
  _showPoints->setTristate(false);
  enableSettings();
  drawSampleLine();
}


bool CurveAppearance::showHead() const {
  return _showHead->checkState() == Qt::Checked;
}


bool CurveAppearance::showHeadDirty() const {
  return _showHead->checkState() != Qt::PartiallyChecked;
}


void CurveAppearance::setShowHead(const bool showHead) {
  _showHead->setChecked(showHead);
  _showHead->setTristate(false);
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
  _showBars->setTristate(false);
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

QColor CurveAppearance::headColor() const {
  return _headColor->color();
}


bool CurveAppearance::headColorDirty() const {
  return _headColor->colorDirty();
}


void CurveAppearance::setHeadColor(const QColor & c) {
  _headColor->setColor(c);
  enableSettings();
  drawSampleLine();
}


QColor CurveAppearance::barFillColor() const {
  return _barFillColor->color();
}


bool CurveAppearance::barFillColorDirty() const {
  return _barFillColor->colorDirty();
}


void CurveAppearance::setBarFillColor(const QColor & c) {
  _barFillColor->setColor(c);
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

int CurveAppearance::headType() const {
  return _comboHeadSymbol->currentIndex();
}


bool CurveAppearance::headTypeDirty() const {
  return _comboHeadSymbol->currentIndex() != -1;
}


void CurveAppearance::setHeadType(const int pointType) {
  _comboHeadSymbol->setCurrentIndex(pointType);
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


double CurveAppearance::pointSize() const {
  if (_spinBoxPointSize->text() == " ") {
    return CURVE_DEFAULT_POINT_SIZE;
  } else {
    return _spinBoxPointSize->value();
  }
}


bool CurveAppearance::pointSizeDirty() const {
  return !_spinBoxPointSize->text().isEmpty();
}


void CurveAppearance::setPointSize(double pointSize) {
  if (pointSize<0.1) {
    pointSize = CURVE_DEFAULT_POINT_SIZE;
  }
  _spinBoxPointSize->setValue(pointSize);
  enableSettings();
  drawSampleLine();
}


void CurveAppearance::clearValues() {
  _color->clearSelection();
  _headColor->clearSelection();
  _barFillColor->clearSelection();
  _spinBoxLineWidth->clear();
  _spinBoxPointSize->clear();
  _comboHeadSymbol->setCurrentIndex(-1);
  _comboPointSymbol->setCurrentIndex(-1);
  _comboPointDensity->setCurrentIndex(-1);
  _comboLineStyle->setCurrentIndex(-1);
  _showPoints->setCheckState(Qt::PartiallyChecked);
  _showLines->setCheckState(Qt::PartiallyChecked);
  _showBars->setCheckState(Qt::PartiallyChecked);
  drawSampleLine();
}


void CurveAppearance::populateLineStyleCombo() {
  int h = fontMetrics().lineSpacing();

  _comboLineStyle->setIconSize(QSize(4*h, h));

  // fill the point type dialog with point types
  QPixmap ppix(4*h, h);
  QPainter pp(&ppix);

  QColor lineColor(color());
  if (lineColor == Qt::transparent) {
    lineColor = Qt::black;
  }


  QPen pen(lineColor, 1);
  pen.setWidthF(h/18.0);

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
  // In some OSs, HiDPI is handled by setting the device pixel ratio so that
  // logical pixels are not physical pixels.  However, not all Qt functions
  // seem to play well in this universe, requiring some... entertainment.

#ifdef QT5
  int pixel_ratio = _label->devicePixelRatio();
#else
  int pixel_ratio = 1;
#endif

  int h = fontMetrics().lineSpacing()*3/2;
  _label->resize(h*5, h);
  QPixmap pix(_label->contentsRect().width()*pixel_ratio,
              _label->contentsRect().height()*pixel_ratio);

#ifdef QT5
  pix.setDevicePixelRatio(pixel_ratio);
#endif

  int pix_w = pix.width()/pixel_ratio;
  int pix_h = pix.height()/pixel_ratio;


  QPainter p(&pix);

  int line_width = lineWidth()*fontMetrics().lineSpacing()/18;

  QPen pen(color(),line_width,LineStyle[lineStyle()]);

  p.fillRect(p.window(), QColor("white"));


  if (showBars()) {
    QRect rectBar((pix_w-pix_h)/2,
                  pix_h/2,
                  pix_h,
                  (pix_h/2)+1);


    p.fillRect(rectBar,QBrush(QColor(barFillColor())));
    p.setPen(QPen(QColor(color()),line_width, LineStyle[lineStyle()]));
    p.drawRect(rectBar);
  }

#ifdef Q_OS_WIN32
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
    if (showHead()) {
      p.drawLine(1, pix_h/2, pix_w-10, pix_h/2);
    } else {
      p.drawLine(1, pix_h/2, pix_w-1, pix_h/2);
    }
  }

  h = fontMetrics().lineSpacing() * pointSize()*12/font().pointSizeF()/56;

  if (showPoints()) {
    pen.setStyle(Qt::SolidLine);
    p.setPen(pen);
    CurvePointSymbol::draw(pointType(), &p, pix_w/2, pix_h/2, h);
  }

  if (showHead()) {
    pen.setStyle(Qt::SolidLine);
    pen.setColor(headColor());
    p.setPen(pen);
    CurvePointSymbol::draw(headType(), &p, pix_w-10, pix_h/2, h);
  }
  _label->setPixmap(pix);
}

// store the current state of the widget as the default 
void CurveAppearance::setWidgetDefaults(bool nextColor) {
  if (nextColor) {
    ColorSequence::self().next();
  }
  dialogDefaults().setValue("curves/showPoints",showPoints());
  dialogDefaults().setValue("curves/showLines", showLines());
  dialogDefaults().setValue("curves/showBars",showBars());
  dialogDefaults().setValue("curves/lineWidth",lineWidth());
  dialogDefaults().setValue("curves/pointSize",pointSize());
  dialogDefaults().setValue("curves/lineStyle",lineStyle());
  dialogDefaults().setValue("curves/pointType", pointType());
  dialogDefaults().setValue("curves/headType", headType());
  dialogDefaults().setValue("curves/pointDensity", pointDensity());
  dialogDefaults().setValue("curves/showHead", showHead());
}

// set the widget to the stored default values
void CurveAppearance::loadWidgetDefaults() {
  setColor(ColorSequence::self().current());
  ColorSequence::self().next();
  setHeadColor(ColorSequence::self().current());
  ColorSequence::self().next();
  setBarFillColor(ColorSequence::self().current());

  setShowPoints(dialogDefaults().value("curves/showPoints",false).toBool());
  setShowLines(dialogDefaults().value("curves/showLines",true).toBool());
  setShowBars(dialogDefaults().value("curves/showBars",false).toBool());
  setShowHead(dialogDefaults().value("curves/showHead",false).toBool());
  setLineWidth(dialogDefaults().value("curves/lineWidth",0).toInt());
  setPointSize(dialogDefaults().value("curves/pointSize",0).toInt());
  setLineStyle(dialogDefaults().value("curves/lineStyle",0).toInt());
  setPointType(dialogDefaults().value("curves/pointType",0).toInt());
  setHeadType(dialogDefaults().value("curves/headType",0).toInt());
  setPointDensity(dialogDefaults().value("curves/pointDensity",0).toInt());
}

}
// vim: ts=2 sw=2 et
