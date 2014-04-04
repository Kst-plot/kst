/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "scriptinterface.h"

#include "namedobject.h"

#include <QStringList>

namespace Kst {

  QString ScriptInterface::doNamedObjectCommand(QString command, NamedObject *n) {
    if (command.startsWith("setName(")) {
      command.remove("setName(").chop(1);
      n->setDescriptiveName(command);
      return QString("Done");
    } else if (command.startsWith("name(")) {
      return n->Name();
    }

    return QString();
  }

  // convenience functions... for parsing commands.
  QStringList ScriptInterface::getArgs(const QString &command) {
    int i0 = command.indexOf('(')+1;
    int i1 = command.lastIndexOf(')');
    int n = i1-i0;

    QString x = command.mid(i0,n);
    return x.split(',');
  }

  QString ScriptInterface::getArg(const QString &command) {
    int i0 = command.indexOf('(')+1;
    int i1 = command.lastIndexOf(')');
    int n = i1-i0;

    QString x = command.mid(i0,n);
    return x;

  }

}
