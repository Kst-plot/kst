/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "labelpropertiestab.h"

#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

LabelPropertiesTab::LabelPropertiesTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Label Properties"));

  _labelText->setObjectStore(kstApp->mainWindow()->document()->objectStore());

  _labelText->setWhatsThis(i18n("<qt>The syntax for labels is a derivative of a subset of LaTeX.  "
                                "Supported syntax is: <b>\\[greeklettername]</b> and <b>\\[Greeklettername]</b>, "
                                "<b>\\approx</b>, <b>\\cdot</b>, <b>\\ge</b>, <b>\\geq</b>, <b>\\inf</b> ,"
                                "<b>\\int</b>, <b>\\le</b>, <b>\\leq</b>, <b>\\ne</b>, <b>\\n</b>, "
                                "<b>\\partial</b>, <b>\\prod</b>, <b>\\pm</b>, "
                                "<b>\\textcolor{color name}{colored text}</b>, <b>\\textbf{bold text}</b>, "
                                "<b>\\textit{italicized text}</b>, <b>\\t</b>, <b>\\sum</b>, <b>\\sqrt</b>, "
                                "<b>\\underline{underlined text}</b>, <b>x^y</b>, <b>x_y</b>.  "
                                "Scalars, equations, and vector elements can be embedded.  "
                                "Scalar: <i>[V1/Mean]</i>.  Vector Element: <i>[V1[4]]</i>.  "
                                "Equation: <i>[=[V1/Mean]^2]</i>.  A [ character can be inserted as <i>\\[</i>."));

  _bold->setFixedWidth(32);
  _bold->setFixedHeight(32);
  _bold->setIcon(QPixmap(":kst_bold.png"));
  _italic->setFixedWidth(32);
  _italic->setFixedHeight(32);
  _italic->setIcon(QPixmap(":kst_italic.png"));
  _labelColor->setFixedWidth(32);
  _labelColor->setFixedHeight(32);

  connect(_labelText, SIGNAL(labelChanged()), this, SIGNAL(modified()));
  connect(_labelFontScale, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_labelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_bold, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_italic, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
}


LabelPropertiesTab::~LabelPropertiesTab() {
}


QString LabelPropertiesTab::labelText() const { 
  return _labelText->labelText(); 
}


void LabelPropertiesTab::setLabelText(const QString &text) {
  _labelText->setLabelText(text);
}


qreal LabelPropertiesTab::labelScale() const { 
  return _labelFontScale->value(); 
}


void LabelPropertiesTab::setLabelScale(const qreal scale) { 
  _labelFontScale->setValue(scale);
}


QColor LabelPropertiesTab::labelColor() const { 
  return _labelColor->color();
}


void LabelPropertiesTab::setLabelColor(const QColor &color) {
  _labelColor->setColor(color);
}


QFont LabelPropertiesTab::labelFont() const {
  QFont font(_family->currentFont());
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  return font;
}


void LabelPropertiesTab::setLabelFont(const QFont &font) {
  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _italic->setChecked(font.italic());
}



}
// vim: ts=2 sw=2 et
