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

namespace Kst {

Document::Document()
: _session(new SessionModel) {
}


Document::~Document() {
  delete _session;
  _session = 0;
}


SessionModel* Document::session() const {
  return _session;
}


bool Document::save(const QString& to) {
}


bool Document::open(const QString& file) {
}


QString Document::lastError() const {
  return QString::null;
}


bool Document::isChanged() const {
  return true;  // FIXME!
}


bool Document::isOpen() const {
  return true;  // FIXME!
}

}

// vim: ts=2 sw=2 et
