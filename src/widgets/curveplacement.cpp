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

#include "curveplacement.h"

namespace Kst {

CurvePlacement::CurvePlacement(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);
}


CurvePlacement::~CurvePlacement() {
}


CurvePlacement::Place CurvePlacement::place() const {
  return NewPlot;
}


void CurvePlacement::setPlace(CurvePlacement::Place place) {
  Q_UNUSED(place);
}


QString CurvePlacement::existingPlot() const {
  return QString();
}


void CurvePlacement::setExistingPlots(const QStringList &existingPlots) {
  Q_UNUSED(existingPlots);
}


bool CurvePlacement::relayout() const {
  return false;
}


void CurvePlacement::setRelayout(bool relayout) {
  Q_UNUSED(relayout);
}


int CurvePlacement::numberOfColumns() const {
  return -1;
}


void CurvePlacement::setNumberOfColumns(int numberOfColumns) {
  Q_UNUSED(numberOfColumns);
}

}

// vim: ts=2 sw=2 et
