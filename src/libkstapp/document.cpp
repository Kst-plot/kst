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

#include "document.h"
#include "sessionmodel.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

namespace Kst {

Document::Document()
: _session(new SessionModel), _dirty(false), _isOpen(false) {
}


Document::~Document() {
  delete _session;
  _session = 0;
}


SessionModel* Document::session() const {
  return _session;
}


bool Document::save(const QString& to) {
  setChanged(false);
  return false;
}


bool Document::open(const QString& file) {
  _isOpen = false;
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    // QMessageBox::critical
    return false;
  }

  QXmlStreamReader xml;
  xml.setDevice(&f);

  while (!xml.atEnd()) {
    if (xml.isStartElement()) {
      qDebug() << "Got a node: " << xml.name().toString();
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    // QMessageBox::critical
    return false;
  }

  return _isOpen = true;
}


QString Document::lastError() const {
  return QString::null;
}


bool Document::isChanged() const {
  return _dirty;
}


bool Document::isOpen() const {
  return _isOpen;
}


void Document::setChanged(bool dirty) {
  _dirty = dirty;
}


}

// vim: ts=2 sw=2 et
