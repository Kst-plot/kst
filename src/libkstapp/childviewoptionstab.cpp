/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "childviewoptionstab.h"

namespace Kst {

ChildViewOptionsTab::ChildViewOptionsTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Child View Options"));

  connect(_shareAxis, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
}


ChildViewOptionsTab::~ChildViewOptionsTab() {
}


bool ChildViewOptionsTab::shareAxis() const {
  return _shareAxis->isChecked();
}


void ChildViewOptionsTab::setShareAxis(bool shareAxis) {
  _shareAxis->setChecked(shareAxis);
}

}

// vim: ts=2 sw=2 et
