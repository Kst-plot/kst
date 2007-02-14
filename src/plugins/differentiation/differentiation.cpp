/***************************************************************************
                   differentiation.cpp
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
#include "differentiation.h"

#include <kgenericfactory.h>

static const QString& INPUTVECTOR = KGlobal::staticQString("InputVector");
static const QString& TIME_STEP = KGlobal::staticQString("Time step");
static const QString& DERIVATIVE = KGlobal::staticQString("Derivative");

KST_KEY_DATAOBJECT_PLUGIN( differentiation )

K_EXPORT_COMPONENT_FACTORY( kstobject_differentiation,
    KGenericFactory<Differentiation>( "kstobject_differentiation" ) )

Differentiation::Differentiation( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Differentiation::~Differentiation() {
}


bool Differentiation::algorithm() {

  KstVectorPtr inputvector  = inputVector(INPUTVECTOR);
  KstScalarPtr time_step    = inputScalar(TIME_STEP);
  KstVectorPtr derivative   = outputVector(DERIVATIVE);

  /* Memory allocation */
  if (derivative->length() != inputvector->length()) {
    derivative->resize(inputvector->length(), true);
  }

  derivative->value()[0] = (inputvector->value()[1] - inputvector->value()[0]) / time_step->value();

  int i = 1;
  for (; i < inputvector->length()-1; i++) {
      derivative->value()[i] = (inputvector->value()[i+1] - inputvector->value()[i-1])/(2*time_step->value());
  }

  derivative->value()[i] = (inputvector->value()[i] - inputvector->value()[i-1]) / time_step->value();

  return true;
}


QStringList Differentiation::inputVectorList() const {
  return QStringList( INPUTVECTOR );
}


QStringList Differentiation::inputScalarList() const {
  return QStringList( TIME_STEP );
}


QStringList Differentiation::inputStringList() const {
  return QStringList();
}


QStringList Differentiation::outputVectorList() const {
  return QStringList( DERIVATIVE );
}


QStringList Differentiation::outputScalarList() const {
  return QStringList();
}


QStringList Differentiation::outputStringList() const {
  return QStringList();
}

#include "differentiation.moc"
