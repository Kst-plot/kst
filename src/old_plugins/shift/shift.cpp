/***************************************************************************
                   shift.cpp
                             -------------------
    begin                : 12/07/06
    copyright            : (C) 2006 The University of Toronto
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
#include "shift.h"

#include <kgenericfactory.h>

static const QString& INPUTVECTOR = KGlobal::staticQString("InputVector");
static const QString& SHIFT_VALUE = KGlobal::staticQString("Shift value (# points, negative allowed)");
static const QString& SHIFTEDVECTOR = KGlobal::staticQString("ShiftedVector");

KST_KEY_DATAOBJECT_PLUGIN( shift )

K_EXPORT_COMPONENT_FACTORY( kstobject_shift,
    KGenericFactory<Shift>( "kstobject_shift" ) )

Shift::Shift( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Shift::~Shift() {
}


bool Shift::algorithm() {

  KstVectorPtr inputvector    = inputVector(INPUTVECTOR);
  KstScalarPtr shift_value    = inputScalar(SHIFT_VALUE);
  KstVectorPtr shiftedvector  = outputVector(SHIFTEDVECTOR);

  int delay = 0;
  const double nan = 0.0 / 0.0;

  /* Memory allocation */
  if (shiftedvector->length() != inputvector->length()) {
    shiftedvector->resize(inputvector->length(), false);
  }

  delay=(int)shift_value->value();
  /* Protect against invalid inputs */
  if (delay > inputvector->length()) delay = inputvector->length();
  if (delay < -inputvector->length()) delay = -inputvector->length();

  /* First case: positive shift (forwards/right shift)*/
  if (delay >= 0) {
    /* Pad beginning with zeros */
    for (int i = 0; i < delay; i++)
    {
      shiftedvector->value()[i] = nan;
    }
    /* Then, copy values with the right offset */
    for (int i = delay; i < inputvector->length(); i++) {
      shiftedvector->value()[i] = inputvector->value()[i-delay];
    }
  }

  /* Second case: negative shift (backwards/left shift)*/
  else if (delay < 0) {
    delay = -delay; /* Easier to visualize :-) */
    /* Copy values with the right offset */
    for (int i = 0; i < inputvector->length()-delay; i++) {
      shiftedvector->value()[i] = inputvector->value()[i+delay];
    }
    /* Pad end with zeros */
    for (int i = inputvector->length()-delay; i < inputvector->length(); i++) {
      shiftedvector->value()[i] = nan;
    }
  }

  return true;
}


QStringList Shift::inputVectorList() const {
  return QStringList( INPUTVECTOR );
}


QStringList Shift::inputScalarList() const {
  return QStringList( SHIFT_VALUE );
}


QStringList Shift::inputStringList() const {
  return QStringList();
}


QStringList Shift::outputVectorList() const {
  return QStringList( SHIFTEDVECTOR );
}


QStringList Shift::outputScalarList() const {
  return QStringList();
}


QStringList Shift::outputStringList() const {
  return QStringList();
}

#include "shift.moc"
