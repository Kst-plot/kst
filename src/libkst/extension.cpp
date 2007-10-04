/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for KDE

// application specific includes
#include "extension.h"
#include <qmainwindow.h>
#include <QTextStream>

namespace Kst {

Extension::Extension(QObject *parent, const QStringList&) : QObject(parent) {
}


Extension::~Extension() {
  emit unregister();
}


void Extension::load(const QDomElement& e) {
  Q_UNUSED(e)
}


void Extension::save(QTextStream& ts, const QString& indent) {
  Q_UNUSED(ts)
  Q_UNUSED(indent)
}


void Extension::clear() {
}


QMainWindow* Extension::app() const {
  return qobject_cast<QMainWindow*>(parent());
}


void Extension::processArguments(const QString& args) {
  Q_UNUSED(args)
}

}
// vim: ts=2 sw=2 et
