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

#include "markerstab.h"

namespace Kst {

MarkersTab::MarkersTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Markers"));

  _markerLineStyle->addItem("SolidLine", Qt::SolidLine);
  _markerLineStyle->addItem("DashLine", Qt::DashLine);
  _markerLineStyle->addItem("DotLine", Qt::DotLine);
  _markerLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _markerLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _markerLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);

  connect(_dataObjectMarkers, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_vectorMarkers, SIGNAL(stateChanged(int)), this, SLOT(update()));

  connect(_markerLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_markerLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_dataObjectMarkers, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_vectorMarkers, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_vector, SIGNAL(selectionChanged(const QString&)), this, SIGNAL(modified()));
  connect(_relation, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_risingEdge, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_fallingEdge, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_both, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_markerLineWidth, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));

  update();
}


MarkersTab::~MarkersTab() {
}


void MarkersTab::setObjectStore(ObjectStore *store) {
  _vector->setObjectStore(store);
}


void MarkersTab::setObjects(QStringList allObjects) {
  _relation->clear();
  _relation->addItems(allObjects);
}


void MarkersTab::update() {
  _relation->setEnabled(_dataObjectMarkers->isChecked());
  _risingEdge->setEnabled(_dataObjectMarkers->isChecked());
  _fallingEdge->setEnabled(_dataObjectMarkers->isChecked());
  _both->setEnabled(_dataObjectMarkers->isChecked());
  _vector->setEnabled(_vectorMarkers->isChecked());
}

}

// vim: ts=2 sw=2 et
