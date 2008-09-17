/***************************************************************************
                   cumulative_sum.cpp
                             -------------------
    begin                : 12/06/06
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
#include "cumulative_sum.h"

#include <kgenericfactory.h>

static const QString& INPUTVECTOR = KGlobal::staticQString("InputVector");
static const QString& SCALE_FACTOR = KGlobal::staticQString("Scale factor (time step)");
static const QString& CUMULATIVE_SUM = KGlobal::staticQString("Cumulative Sum");

KST_KEY_DATAOBJECT_PLUGIN( cumulative_sum )

K_EXPORT_COMPONENT_FACTORY( kstobject_cumulative_sum,
    KGenericFactory<CumulativeSum>( "kstobject_cumulative_sum" ) )

CumulativeSum::CumulativeSum( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


CumulativeSum::~CumulativeSum() {
}


bool CumulativeSum::algorithm() {

  KstVectorPtr inputvector    = inputVector(INPUTVECTOR);
  KstScalarPtr scale_factor   = inputScalar(SCALE_FACTOR);
  KstVectorPtr cumulative_sum = outputVector(CUMULATIVE_SUM);

  /* Memory allocation */
  if (cumulative_sum->length() != inputvector->length()) {
    cumulative_sum->resize(inputvector->length()+1, true);
  }

  cumulative_sum->value()[0] = 0.0;

  for (int i = 0; i < inputvector->length(); i++) {
    cumulative_sum->value()[i+1] =
      inputvector->value()[i]*scale_factor->value() + cumulative_sum->value()[i];
  }

  return true;
}


QStringList CumulativeSum::inputVectorList() const {
  return QStringList( INPUTVECTOR );
}


QStringList CumulativeSum::inputScalarList() const {
  return QStringList( SCALE_FACTOR );
}


QStringList CumulativeSum::inputStringList() const {
  return QStringList();
}


QStringList CumulativeSum::outputVectorList() const {
  return QStringList( CUMULATIVE_SUM );
}


QStringList CumulativeSum::outputScalarList() const {
  return QStringList();
}


QStringList CumulativeSum::outputStringList() const {
  return QStringList();
}

#include "cumulative_sum.moc"
