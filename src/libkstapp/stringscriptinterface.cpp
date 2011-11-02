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

#include "stringscriptinterface.h"

#include <QStringBuilder>

namespace Kst {

StringGenSI::StringGenSI(StringPtr it) {
    str=it;
}

QByteArrayList StringGenSI::commands() {
    return QByteArrayList()<<"setName("<<"setValue("<<"uncheckAuto()";
}

QString StringGenSI::doCommand(QString x) {
    if(x.startsWith("setName(")) {
        str->writeLock();
        str->setDescriptiveName(x.remove("setName(").remove(")"));
        str->unlock();
        return "Done";
    } else if(x.startsWith("setValue(")) {
        str->writeLock();
        str->setValue(x.remove("setValue(").remove(")"));
        str->unlock();
        return "Done";
    } else if(x.startsWith("uncheckAuto()")) {
        str->writeLock();
        str->setDescriptiveName("");
        str->unlock();
        return "Done";
    }
    return "No such command";
}

bool StringGenSI::isValid() {
    return str.isPtrValid();
}

QByteArray StringGenSI::getHandle() {
    return ("Finished editing "%str->Name()).toLatin1();
}

}
