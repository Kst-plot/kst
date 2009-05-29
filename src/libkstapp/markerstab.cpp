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

#include "plotdefines.h"

#include <QMessageBox>

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

  connect(_curveMarkers, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_vectorMarkers, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_currentMarkersList, SIGNAL(itemSelectionChanged()), this, SLOT(update()));
  connect(_newMarker, SIGNAL(textChanged(const QString&)), this, SLOT(update()));

  connect(_markerLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_markerLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_curveMarkers, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_vectorMarkers, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_vector, SIGNAL(selectionChanged(const QString&)), this, SIGNAL(modified()));
  connect(_curve, SIGNAL(selectionChanged(const QString&)), this, SIGNAL(modified()));
  connect(_risingEdge, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_fallingEdge, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_both, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_markerLineWidth, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));

  connect(_addMarker, SIGNAL(clicked()), this, SLOT(add()));
  connect(_removeMarker, SIGNAL(clicked()), this, SLOT(remove()));
  connect(_clearMarkers, SIGNAL(clicked()), this, SLOT(clear()));

  connect(this, SIGNAL(modified()), this, SLOT(setDirty()));

  update();
}


MarkersTab::~MarkersTab() {
}


void MarkersTab::setObjectStore(ObjectStore *store) {
  _vector->setObjectStore(store);
  _curve->setObjectStore(store);
}


void MarkersTab::update() {
  _curve->setEnabled(_curveMarkers->isChecked());
  _risingEdge->setEnabled(_curveMarkers->isChecked());
  _fallingEdge->setEnabled(_curveMarkers->isChecked());
  _both->setEnabled(_curveMarkers->isChecked());
  _vector->setEnabled(_vectorMarkers->isChecked());

  _removeMarker->setEnabled(_currentMarkersList->selectedItems().count() > 0);
  _clearMarkers->setEnabled(_currentMarkersList->count() > 0);
  _addMarker->setEnabled(!_newMarker->text().isEmpty());
}


Qt::PenStyle MarkersTab::lineStyle() const {
  return Qt::PenStyle(_markerLineStyle->itemData(_markerLineStyle->currentIndex()).toInt());
}


void MarkersTab::setLineStyle(Qt::PenStyle style) {
  _markerLineStyle->setCurrentIndex(_markerLineStyle->findData(QVariant(style)));
}


QColor MarkersTab::lineColor() const {
  return _markerLineColor->color();
}


void MarkersTab::setLineColor(const QColor &color) {
  _markerLineColor->setColor(color);
}


double MarkersTab::lineWidth() const {
  return _markerLineWidth->value();
}


void MarkersTab::setLineWidth(const double width) {
  _markerLineWidth->setValue(width);
}


bool MarkersTab::isCurveSource() const {
  return _curveMarkers->isChecked();
}


void MarkersTab::setCurveSource(const bool enabled) {
  _curveMarkers->setChecked(enabled);
}


bool MarkersTab::isVectorSource() const {
  return _vectorMarkers->isChecked();
}


void MarkersTab::setVectorSource(const bool enabled) {
  _vectorMarkers->setChecked(enabled);
}


VectorPtr MarkersTab::vector() const {
  return _vector->selectedVector();
}


void MarkersTab::setVector(const VectorPtr vector) {
  _vector->setSelectedVector(vector);
}


PlotMarkers::CurveMarkerMode MarkersTab::curveMarkerMode() const {
  if (_risingEdge->isChecked()) {
    return PlotMarkers::RisingEdge;
  } else if (_fallingEdge->isChecked()) {
    return PlotMarkers::FallingEdge;
  } else {
    return PlotMarkers::BothEdges;
  }
}


void MarkersTab::setCurveMarkerMode(const PlotMarkers::CurveMarkerMode mode) {
  if (mode == PlotMarkers::RisingEdge) {
    _risingEdge->setChecked(true);
  } else if (mode == PlotMarkers::FallingEdge) {
    _fallingEdge->setChecked(true);
  } else {
    _both->setChecked(true);
  }
}


QList<double> MarkersTab::markers() const {
  QList<double> markerList;
  foreach(QListWidgetItem *item, _currentMarkersList->findItems("*", Qt::MatchWildcard)) {
    markerList << item->text().toDouble();
  }
  return markerList;
}


void MarkersTab::setMarkers(const QList<double> &markers) {
  QStringList markerList;
  foreach(double marker, markers) {
    markerList << QString::number(marker, 'g', MARKER_LABEL_PRECISION);
  }
  _currentMarkersList->clear();
  _currentMarkersList->addItems(markerList);
}


CurvePtr MarkersTab::curve() const {
  return _curve->selectedCurve();
}


void MarkersTab::setCurve(CurvePtr curve) {
  _curve->setSelectedCurve(curve);
}


void MarkersTab::add() {
  if (!_newMarker->text().isEmpty()) {
    bool ok;
    double newMarkerValue = _newMarker->text().toDouble(&ok);

    if (ok) {
      int i = 0;
      QString newMarkerString;

      newMarkerString.setNum(newMarkerValue, 'g', MARKER_LABEL_PRECISION);
      while (i < _currentMarkersList->count() && _currentMarkersList->item(i)->text().toDouble() < newMarkerValue) {
        i++;
      }
      if (i == _currentMarkersList->count()) {
        _currentMarkersList->addItem(newMarkerString);
        _newMarker->clear();
        update();
        emit modified();
      } else if (newMarkerValue != _currentMarkersList->item(i)->text().toDouble()) {
        _currentMarkersList->insertItem(i, newMarkerString);
        _newMarker->clear();
        update();
        emit modified();
      } else {
        QMessageBox::warning(this, tr("Kst"), tr("A plot marker with equal (or very close) value already exists."));
      }
    } else {
        QMessageBox::warning(this, tr("Kst"), tr("The text you have entered is not a valid number."));
    }
  }
}


void MarkersTab::remove() {
  foreach(QListWidgetItem *item, _currentMarkersList->selectedItems()) {
    _currentMarkersList->takeItem(_currentMarkersList->row(item));
  }
  emit modified();
}


void MarkersTab::clear() {
  _currentMarkersList->clear();
  emit modified();
}


PlotMarkers MarkersTab::plotMarkers() const {
  PlotMarkers plotMarker = _plotMarkers;
  plotMarker.setManualMarkers(markers());
  plotMarker.setLineStyle(lineStyle());
  plotMarker.setLineColor(lineColor());
  plotMarker.setLineWidth(lineWidth());
  if (isVectorSource()) {
    plotMarker.setVector(vector());
  } else {
    plotMarker.setVector(0);
  }
  if (isCurveSource()) {
    plotMarker.setCurve(curve());
  } else {
    plotMarker.setCurve(0);
  }
  plotMarker.setCurveMarkerMode(curveMarkerMode());
  return plotMarker;
}


void MarkersTab::setPlotMarkers(const PlotMarkers &plotMarkers) {
  _plotMarkers = plotMarkers;
  setMarkers(plotMarkers.manualMarkers());
  setLineStyle(plotMarkers.lineStyle());
  setLineColor(plotMarkers.lineColor());
  setLineWidth(plotMarkers.lineWidth());
  setCurveMarkerMode(plotMarkers.curveMarkerMode());
  if (plotMarkers.isVectorSource()) {
    setVector(plotMarkers.vector());
    setVectorSource(true);
  } else {
    setVectorSource(false);
  }
  if (plotMarkers.isCurveSource()) {
    setCurve(plotMarkers.curve());
    setCurveSource(true);
  } else {
    setCurveSource(false);
  }
  _dirty = false;
}


void MarkersTab::clearTabValues() {
  _currentMarkersList->clear();
  _hidden->setChecked(true);
  _markerLineStyle->setCurrentIndex(-1);
  _markerLineColor->clearSelection();
  _markerLineWidth->clear();

  _curve->clearSelection();
  _vector->clearSelection();

  _curveMarkers->setCheckState(Qt::PartiallyChecked);
  _vectorMarkers->setCheckState(Qt::PartiallyChecked);
}


void MarkersTab::enableSingleEditOptions(bool enabled) {
  if (enabled) {
    _curveMarkers->setTristate(false);
    _vectorMarkers->setTristate(false);
  }
}

}

// vim: ts=2 sw=2 et
