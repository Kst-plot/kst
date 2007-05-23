/***************************************************************************
                                dataref.h
                             ----------------
    begin                : Feb 22 2006
                           Copyright (C) 2006, The University of Toronto
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

#ifndef DATAREF_H
#define DATAREF_H

#include <qstring.h>
#include <qvariant.h>

struct DataRef {
  enum Type { DRScalar, DRString, DRVector, DRExpression, DRFit };
  DataRef() { type = DRScalar; }
  DataRef(Type type, const QString& name, const QString& index, double indexValue, const QVariant& value) : type(type), name(name), index(index), indexValue(indexValue), value(value) {}
  Type type;
  QString name;
  QString index;
  double indexValue;
  QVariant value;
};

#endif
// vim: ts=2 sw=2 et
