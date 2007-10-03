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

#include "equationfactory.h"

#include "kstdebug.h"
#include "kstequation.h"
#include "kstdatacollection.h"

namespace Kst {

EquationFactory::EquationFactory()
: ObjectFactory() {
  registerFactory("equation", this);
}


EquationFactory::~EquationFactory() {
}


DataObjectPtr EquationFactory::generateObject(QXmlStreamReader& xml) {
  QString tag, expression, xVector, output;
  bool interpolate = false;

  while (!xml.atEnd()) {
    const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "equation") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = attrs.value("tag").toString();
        expression = attrs.value("expression").toString();
        interpolate = attrs.value("interpolate").toString().toLower() == "true";
        xVector = attrs.value("xvector").toString();
        output = attrs.value("output").toString();
        //FIXME Don't know if this is a bug in QXmlStreamReader or what, but readElementNext takes us
        //past the </equation> node to the </objects> node...
        //eq = xml.readElementText();
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "equation") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating equation from Kst file."), KstDebug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  VectorPtr vector = 0;
  if (!xVector.isEmpty()) {
    vector = *KST::vectorList.findTag(xVector);
  }

  if (!vector) {
    KstDebug::self()->log(QObject::tr("Error creating equation from Kst file.  Could not find xVector."), KstDebug::Warning);
    return 0;
  }

  KstEquationPtr ep = new KstEquation(tag, expression, vector, interpolate);
  return ep.data();
}

}

// vim: ts=2 sw=2 et
