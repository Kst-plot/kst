/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
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
#include <qtextdocument.h>
#include "kst_i18n.h"

namespace Kst {

EditableMatrix::EditableMatrix(const QDomElement &e) : Matrix() {
  _editable = true;

  double in_xMin = 0, in_yMin = 0, in_xStep = 1, in_yStep = 1;
  int in_nX = 2, in_nY = 2;
  QString in_tag = QString::null; 
  
  // must get the grid dimensions before the data
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(ObjectTag::fromString(e.text()));
      } else if (e.tagName() == "nx") {
        in_nX = e.text().toInt();
      } else if (e.tagName() == "ny") {
        in_nY = e.text().toInt();
      } else if (e.tagName() == "xmin") {
        in_xMin = e.text().toDouble();
      } else if (e.tagName() == "ymin") {
        in_yMin = e.text().toDouble();
      } else if (e.tagName() == "xstep") {
        in_xStep = e.text().toDouble();
      } else if (e.tagName() == "ystep") {
        in_yStep = e.text().toDouble();
      } 
    }
    n = n.nextSibling();
  }
  _saveable = true;
  resizeZ(in_nX*in_nY, true);

  // now get the z data
  if (in_nX*in_nY > 0) {
    QDomNode n = e.firstChild();
    while (!n.isNull()) {
      QDomElement e = n.toElement();
      if (!e.isNull()) {
        if (e.tagName() == "data") {
          QString qcs(e.text().toLatin1());
          QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
          QByteArray qba = qUncompress(qbca);
          QDataStream qds(&qba, QIODevice::ReadOnly);
          int i;
          // fill in the raw array with the data
          for (i = 0; i < in_nX*in_nY && !qds.atEnd(); i++) {
            qds >> _z[i];  // stored in the same order as it was saved
          }
          if (i < in_nX*in_nY) {
            Debug::self()->log(i18n("Saved matrix contains less data than it claims."), Debug::Warning);
            resizeZ(i, false);
          }
        }
      }
      n = n.nextSibling();
    }
  }
}


EditableMatrix::EditableMatrix(ObjectTag in_tag, uint nX, uint nY, double minX, double minY, double stepX, double stepY)
: Matrix(in_tag, 0L, nX, nY, minX, minY, stepX, stepY) {
  _editable = true;
  _saveable = true;
  resizeZ(nX*nY, true);
}


void EditableMatrix::save(QTextStream &ts, const QString& indent) {

  QString indent2 = "  ";
  
  QByteArray qba(_zSize*sizeof(double), '\0');
  QDataStream qds(&qba, QIODevice::WriteOnly);

  for (int i = 0; i < _zSize; i++) {
    qds << _z[i];
  }

  ts << indent << "<amatrix>" << endl;
  ts << indent << indent2 << "<tag>" << Qt::escape(tag().tagString()) << "</tag>" << endl;
  ts << indent << indent2 << "<xmin>" << minX() << "</xmin>" << endl;
  ts << indent << indent2 << "<ymin>" << minY() << "</ymin>" << endl;
  ts << indent << indent2 << "<nx>" << xNumSteps() << "</nx>" << endl;
  ts << indent << indent2 << "<ny>" << yNumSteps() << "</ny>" << endl;
  ts << indent << indent2 << "<xstep>" << xStepSize() << "</xstep>" << endl;
  ts << indent << indent2 << "<ystep>" << xStepSize() << "</ystep>" << endl;
  ts << indent << indent2 << "<data>" << qCompress(qba).toBase64() << "</data>" << endl;
  ts << indent << "</amatrix>" << endl;
}

}
// vim: ts=2 sw=2 et
