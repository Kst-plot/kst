/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// use KCodecs::base64Encode() in kmdcodecs.h
// Create QDataStream into a QByteArray
// qCompress the bytearray
#include <QXmlStreamWriter>

#include "editablevector.h"
#include "debug.h"
#include "kst_i18n.h"

namespace Kst {

const QString EditableVector::staticTypeString = I18N_NOOP("Editable Vector");
const QString EditableVector::staticTypeTag = I18N_NOOP("editablevector");

EditableVector::EditableVector(ObjectStore *store)
    : Vector(store) {
  _editable = true;
  _saveable = true;
  _saveData = true;
}


const QString& EditableVector::typeString() const {
  return staticTypeString;
}


void EditableVector::setSaveData(bool save) {
  Q_UNUSED(save)
}

/** Save vector information */
void EditableVector::save(QXmlStreamWriter &s) {
  s.writeStartElement("editablevector");
  saveNameInfo(s, VNUM|XNUM);

  if (_saveData) {
    QByteArray qba(length()*sizeof(double), '\0');
    QDataStream qds(&qba, QIODevice::WriteOnly);

    for (int i = 0; i < length(); i++) {
      qds << _v[i];
    }

    s.writeTextElement("data", qCompress(qba).toBase64());
  }
  s.writeEndElement();
}

QString EditableVector::_automaticDescriptiveName() const {
  QString name("(");
  if (length()>=1) {
    name += QString::number(_v[0]);
  }
  if (length()>=2) {
    name += ", " + QString::number(_v[1]);
  }

  if (length()>=3) {
    name += ", ...";
  }

  name += ")";

  return name;
}

QString EditableVector::descriptionTip() const {
    return i18n("Editable Vector: %1\n"
      "  %2 values").arg(Name()).arg(length());

}

}
// vim: ts=2 sw=2 et
