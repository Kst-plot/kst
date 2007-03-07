/***************************************************************************
                   viewlabelwidget.cpp
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "viewlabelwidget.h"

#include <kst_export.h>

ViewLabelWidget::ViewLabelWidget(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);
}


ViewLabelWidget::~ViewLabelWidget() {}


void ViewLabelWidget::init() {
  connect(_scalars, SIGNAL(selectionChanged(const QString &)),
          this, SLOT(insertScalarInText(const QString &)));
  _horizontal->addItem(i18n("Left"));
  _horizontal->addItem(i18n("Right"));
  _horizontal->addItem(i18n("Center"));

  connect(_strings, SIGNAL(selectionChanged(const QString &)),
          this, SLOT(insertStringInText(const QString &)));

  Q3WhatsThis::add
    (_text, i18n("<qt>The syntax for labels is a derivative of a subset of LaTeX.  Supported syntax is: <b>\\[greeklettername]</b> and <b>\\[Greeklettername]</b>, <b>\\approx</b>, <b>\\cdot</b>, <b>\\ge</b>, <b>\\geq</b>, <b>\\inf</b>, <b>\\int</b>, <b>\\le</b>, <b>\\leq</b>, <b>\\ne</b>, <b>\\n</b>, <b>\\partial</b>, <b>\\prod</b>, <b>\\pm</b>, <b>\\textcolor{color name}{colored text}</b>, <b>\\textbf{bold text}</b>, <b>\\textit{italicized text}</b>, <b>\\t</b>, <b>\\sum</b>, <b>\\sqrt</b>, <b>\\underline{underlined text}</b>, <b>x^y</b>, <b>x_y</b>.  Data references and embedded expressions should be enclosed in square brackets.  Example: <i>[=[MyScalar]/2]</i>"));
}


void ViewLabelWidget::insertScalarInText(const QString &S) {
  _text->insert("["+S+"]");
}


void ViewLabelWidget::insertStringInText(const QString &S) {
  _text->insert("["+S+"]");
}

#include "viewlabelwidget.moc"

// vim: ts=2 sw=2 et
