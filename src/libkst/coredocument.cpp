/***************************************************************************
                              coredocument.cpp
                             -------------------
    begin                : October 3, 2007
    copyright            : (C) 2007 by The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "coredocument.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

#include "objectstore.h"
#include "../libkstapp/scriptserver.h"

namespace Kst {

CoreDocument::CoreDocument()
: _objectStore(new ObjectStore()), _dirty(false), _isOpen(false) {
  _scriptServer = new ScriptServer(_objectStore);
  _fileName.clear();
}


CoreDocument::~CoreDocument() {
  delete _scriptServer;
  delete _objectStore;
  _objectStore = 0;
}


QString CoreDocument::fileName() const {
  return _fileName;
}


ObjectStore* CoreDocument::objectStore() const {
  return _objectStore;
}


bool CoreDocument::save(const QString& to) {
  Q_UNUSED(to);
  return true;
}


bool CoreDocument::open(const QString& file) {
  Q_UNUSED(file);
  _isOpen = false;

  return _isOpen = true;
}


QString CoreDocument::lastError() const {
  return _lastError;
}


bool CoreDocument::isChanged() const {
  return _dirty;
}


bool CoreDocument::isOpen() const {
  return _isOpen;
}


void CoreDocument::setChanged(bool dirty) {
  _dirty = dirty;
}


}

// vim: ts=2 sw=2 et
