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

#include "editablematrix.h"
#include "debug.h"
#include <qbytearray.h>
#include <QXmlStreamWriter>
#include "kst_i18n.h"

namespace Kst {

const QString EditableMatrix::staticTypeString = I18N_NOOP("Editable Matrix");
const QString EditableMatrix::staticTypeTag = I18N_NOOP("editablematrix");

EditableMatrix::EditableMatrix(ObjectStore *store)
: Matrix(store) {
  _editable = true;
  _saveable = true;
  resizeZ(1, true);
}


const QString& EditableMatrix::typeString() const {
  return staticTypeString;
}


void EditableMatrix::save(QXmlStreamWriter &xml) {

  QByteArray qba(_zSize*sizeof(double), '\0');
  QDataStream qds(&qba, QIODevice::WriteOnly);

  for (int i = 0; i < _zSize; i++) {
    qds << _z[i];
  }

  xml.writeStartElement(staticTypeTag);
  xml.writeAttribute("xmin", QString::number(minX()));
  xml.writeAttribute("ymin", QString::number(minY()));
  xml.writeAttribute("nx", QString::number(xNumSteps()));
  xml.writeAttribute("ny", QString::number(yNumSteps()));
  xml.writeAttribute("xstep", QString::number(xStepSize()));
  xml.writeAttribute("ystep", QString::number(yStepSize()));
  xml.writeTextElement("data", qCompress(qba).toBase64());
  saveNameInfo(xml, VNUM|MNUM|XNUM);
  xml.writeEndElement();
}


}
// vim: ts=2 sw=2 et
