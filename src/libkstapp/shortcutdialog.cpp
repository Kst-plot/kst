/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2009 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>
#include "shortcutdialog.h"

#include <QDesktopServices>
#include <QDebug>
#include <QTextEdit>
#include <QFile>
#include <QStyle>

#include <QStringList>

namespace Kst {

ShortcutDialog::ShortcutDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

   //int charWidth = fontMetrics().averageCharWidth();

   QFile htmlfile(":html/bindings.html");
   htmlfile.open(QIODevice::ReadOnly | QIODevice::Text);

   QString msg(htmlfile.readAll());

  text->setText(msg);
  text->setReadOnly(true);

}


ShortcutDialog::~ShortcutDialog() {
}

void ShortcutDialog::resetWidth() {


  int w = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);


  text->setFixedWidth(text->document()->idealWidth() +
                      text->contentsMargins().left() +
                      text->contentsMargins().right() +
                      3*w/2);
  resize(width(), 3*text->document()->idealWidth()/2);
  setFixedWidth(width());
}

}
// vim: ts=2 sw=2 et
