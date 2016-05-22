/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
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
#include "matrixscriptinterface.h"

#include "debug.h"
#include <qbytearray.h>

#include <QDataStream>
#include <QXmlStreamWriter>


namespace Kst {

const QString EditableMatrix::staticTypeString = "Editable Matrix";
const QString EditableMatrix::staticTypeTag = "editablematrix";

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
  saveNameInfo(xml, VNUM|MNUM|XNUM);
  xml.writeAttribute("xmin", QString::number(minX()));
  xml.writeAttribute("ymin", QString::number(minY()));
  xml.writeAttribute("nx", QString::number(xNumSteps()));
  xml.writeAttribute("ny", QString::number(yNumSteps()));
  xml.writeAttribute("xstep", QString::number(xStepSize()));
  xml.writeAttribute("ystep", QString::number(yStepSize()));
  xml.writeTextElement("data", qCompress(qba).toBase64());
  xml.writeEndElement();

}


QString EditableMatrix::descriptionTip() const {
    return tr("%1:\n"
      "  %2 x %3").arg(Name()).arg(xNumSteps()).arg(yNumSteps());
}

QString EditableMatrix::_automaticDescriptiveName() const {
  return tr("Editable Matrix");
}

/**  used for scripting IPC.
     accepts an open readable file.
     fails silently */
void EditableMatrix::loadFromTmpFile(QFile &fp, int nx, int ny ) {
  int size = nx*ny*sizeof(double);

  resize(nx, ny);

  fp.read((char *)_z, size);

  internalUpdate(); // not sure if we need this here.
}


ScriptInterface* EditableMatrix::createScriptInterface() {
  return new EditableMatrixSI(this);
}


}
// vim: ts=2 sw=2 et
