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

#include "labelcreator.h"

#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

LabelCreator::LabelCreator(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  _labelText->setObjectStore(kstApp->mainWindow()->document()->objectStore());
}


LabelCreator::~LabelCreator() {
}


QString LabelCreator::labelText() {
  return _labelText->labelText();
}

}

// vim: ts=2 sw=2 et
