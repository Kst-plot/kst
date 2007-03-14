/***************************************************************************
                   editmultiplewidget.cpp
                             -------------------
    begin                : 02/27/07
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

#include "editmultiplewidget.h"

#include <QRegExp>

EditMultipleWidget::EditMultipleWidget(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(_selectAllBut, SIGNAL(clicked()), this, SLOT(selectAllObjects()));

  connect(_selectNoneBut, SIGNAL(clicked()), _objectList, SLOT(clearSelection()));

  connect(_filterEdit, SIGNAL(textChanged(const QString&)), this, SLOT(applyFilter(const QString&)));
}


EditMultipleWidget::~EditMultipleWidget() {}


void EditMultipleWidget::selectAllObjects() {
  _objectList->clearSelection();
  _objectList->invertSelection();
}


void EditMultipleWidget::applyFilter(const QString& filter) {
  _objectList->clearSelection();

  // case insensitive and wildcards
  QRegExp re(filter, Qt::CaseInsensitive, QRegExp::Wildcard);

  uint c = _objectList->count();
  for (uint i = 0; i < c; ++i) {
    if (re.exactMatch(_objectList->text(i))) {
      _objectList->setSelected(i, true);
    }
  }
}

#include "editmultiplewidget.moc"

// vim: ts=2 sw=2 et
