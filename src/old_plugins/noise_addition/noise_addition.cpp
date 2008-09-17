/***************************************************************************
                   noise_addition.cpp
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
#include "noise_addition.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <kgenericfactory.h>

static const QString& ARRAY = KGlobal::staticQString("Array");
static const QString& SIGMA = KGlobal::staticQString("Sigma");
static const QString& OUTPUT = KGlobal::staticQString("Output Array");

KST_KEY_DATAOBJECT_PLUGIN( noise_addition )

K_EXPORT_COMPONENT_FACTORY( kstobject_noise_addition,
    KGenericFactory<NoiseAddition>( "kstobject_noise_addition" ) )

NoiseAddition::NoiseAddition( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


NoiseAddition::~NoiseAddition() {
}


bool NoiseAddition::algorithm() {

  KstVectorPtr array    = inputVector(ARRAY);
  KstScalarPtr sigma    = inputScalar(SIGMA);
  KstVectorPtr output   = outputVector(OUTPUT);

  const gsl_rng_type* pGeneratorType;
  gsl_rng* pRandomNumberGenerator;
  double* pResult[1];
  int iRetVal = false;
  int iLength = array->length();

  if (iLength > 0) {
    if (output->length() != iLength) {
      output->resize(iLength, false);
      pResult[0] = (double*)realloc( output->value(), iLength * sizeof( double ) );
    } else {
      pResult[0] = output->value();
    }
  }

  pGeneratorType = gsl_rng_default;
  pRandomNumberGenerator = gsl_rng_alloc( pGeneratorType );
  if (pRandomNumberGenerator != NULL) {
    if (pResult[0] != NULL) {
      for (int i=0; i<iLength; i++) {
        output->value()[i] = array->value()[i] + gsl_ran_gaussian( pRandomNumberGenerator, sigma->value() );
      }

      iRetVal = true;
    }
    gsl_rng_free( pRandomNumberGenerator );
  }

  return iRetVal;
}


QStringList NoiseAddition::inputVectorList() const {
  return QStringList( ARRAY );
}


QStringList NoiseAddition::inputScalarList() const {
  return QStringList( SIGMA );
}


QStringList NoiseAddition::inputStringList() const {
  return QStringList();
}


QStringList NoiseAddition::outputVectorList() const {
  return QStringList( OUTPUT );
}


QStringList NoiseAddition::outputScalarList() const {
  return QStringList();
}


QStringList NoiseAddition::outputStringList() const {
  return QStringList();
}

#include "noise_addition.moc"
