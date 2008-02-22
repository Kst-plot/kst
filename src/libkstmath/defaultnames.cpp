/***************************************************************************
                            defaultnames.cpp
                             -------------------
    begin                : July 31, 2004
    copyright            : (C) 2003 C. Barth Netterfield
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dataobjectcollection.h"
#include "datacollection.h"
#include "defaultnames.h"
#include "histogram.h"
#include "psd.h"
#include "curve.h"

#include <qregexp.h>
#include "kst_i18n.h"
#include <stdio.h>

namespace Kst {

// FIXME: THESE ALL NEED FIXING

ObjectTag suggestPlotName() {
  static int last = 0;

  QString tag;

//  do {
    tag = QString("P%1").arg(++last);
//  } while (Data::self()->viewObjectNameNotUnique(tag));

  return ObjectTag::fromString(tag);
}


ObjectTag suggestDataObjectName(const QString& field,
                              const QString &A, bool add_c) {
  QString name;
  QString cleanedField = ObjectTag::cleanTagComponent(field);

  if (add_c) {
    name = QString("%1-%2").arg(cleanedField).arg(A);
  } else {
    name = cleanedField;
  }

//  while (dataObjectList.findTag(name) != dataObjectList.end()) {
//    name = QString("%1-%2%3").arg(cleanedField).arg(A).arg(++i);
//  }

  return ObjectTag::fromString(name);
}


/* takes a vector or plugin name of the form V2-GYRO1 and returns a unique */
/* curve name of the form GYRO1, or GYRO1-N if there are already N curves  */
/* of that name.  If add_c is true, add a -C to the end, even if not */
/* adding it would appear unique.  This is important because in many */
/* dialogs, a curve is created from an object before the object has */
/* been put on the list */
ObjectTag suggestCurveName( ObjectTag vector_name, bool add_c ) {
  return suggestDataObjectName(vector_name.displayString(),
                      i18nc("Minimal abbreviation for 'Curve'", "C"),
                      add_c);
}


ObjectTag suggestPSDName( ObjectTag vector_name ) {
  return suggestDataObjectName(vector_name.name(),
                      i18nc("Minimal abbreviation for 'Power spectrum'", "P"),
                      true);
}


ObjectTag suggestCSDName( ObjectTag vector_name ) {
  return suggestDataObjectName(vector_name.name(),
                               i18nc("Minimal abbreviation for 'Cumulative Spectral Decay'", "S"),
                               true);
}


ObjectTag suggestHistogramName( ObjectTag vector_name ) {
  return suggestDataObjectName(vector_name.name(),
                      i18nc("Minimal abbreviation for 'Histogram'", "H"),
                      true);
}


ObjectTag suggestEQName(const QString& name_in) {
  return suggestDataObjectName(name_in,
                      i18nc("Minimal abbreviation for 'Equation'", "E"),
                      false);
}


ObjectTag suggestPluginName(const QString& pname, ObjectTag vname) {
  QString tag;

  if (!vname.isValid()) {
    tag = pname;
  } else {
    tag = vname.name() + "-" + pname;
  }
  return suggestDataObjectName(tag,
                      i18nc("Minimal abbreviation for 'pluGin'", "G"),
                      false);
}


ObjectTag suggestImageName(ObjectTag matrix_name) {
  return suggestDataObjectName(matrix_name.name(),
                      i18nc("Minimal abbreviation for 'Image'", "I"),
                      true);
}

}
// vim: ts=2 sw=2 et
