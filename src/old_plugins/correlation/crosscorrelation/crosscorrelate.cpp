/***************************************************************************
                   crosscorrelate.cpp
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
#include "crosscorrelate.h"

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#include <kgenericfactory.h>

static const QString& ARRAY_ONE = KGlobal::staticQString("Array One");
static const QString& ARRAY_TWO = KGlobal::staticQString("Array Two");
static const QString& STEP_VALUE = KGlobal::staticQString("Step value");
static const QString& CORRELATED = KGlobal::staticQString("Correlated");

KST_KEY_DATAOBJECT_PLUGIN( crosscorrelate )

K_EXPORT_COMPONENT_FACTORY( kstobject_crosscorrelate,
    KGenericFactory<CrossCorrelate>( "kstobject_crosscorrelate" ) )

CrossCorrelate::CrossCorrelate( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


CrossCorrelate::~CrossCorrelate() {
}


bool CrossCorrelate::algorithm() {

  KstVectorPtr array_one    = inputVector(ARRAY_ONE);
  KstVectorPtr array_two    = inputVector(ARRAY_TWO);
  KstVectorPtr step_value   = outputVector(STEP_VALUE);
  KstVectorPtr correlated   = outputVector(CORRELATED);

  if (array_one->length() <= 0               ||
      array_two->length() <= 0               ||
      array_one->length() != array_two->length()) {
      return false;
  }

  double* pdArrayOne;
  double* pdArrayTwo;
  double* pdResult[2];
  double  dReal;
  double  dImag;

  int iLength;
  int iLengthNew;

  bool iReturn = false;

  //
  // zero-pad the array...
  //
  iLength  = array_one->length();
  iLength *= 2;

  step_value->resize(array_one->length(), false);
  correlated->resize(array_two->length(), false);

  //
  // round iLength up to the nearest power of two...
  //
  iLengthNew = 64;
  while( iLengthNew < iLength && iLengthNew > 0) {
    iLengthNew *= 2;
  }
  iLength = iLengthNew;

  if (iLength <= 0)
    return false;

  pdArrayOne = new double[iLength];
  pdArrayTwo = new double[iLength];
  if (pdArrayOne != NULL && pdArrayTwo != NULL) {
    //
    // zero-pad the two arrays...
    //
    memset( pdArrayOne, 0, iLength * sizeof( double ) );
    memcpy( pdArrayOne, array_one->value(), array_one->length() * sizeof( double ) );

    memset( pdArrayTwo, 0, iLength * sizeof( double ) );
    memcpy( pdArrayTwo, array_two->value(), array_two->length() * sizeof( double ) );

    //
    // calculate the FFTs of the two functions...
    //
    if (gsl_fft_real_radix2_transform( pdArrayOne, 1, iLength ) == 0) {
      if (gsl_fft_real_radix2_transform( pdArrayTwo, 1, iLength ) == 0) {
        //
        // multiply one FFT by the complex conjugate of the other...
        //
        for (int i=0; i<iLength/2; i++) {
          if (i==0 || i==(iLength/2)-1) {
            pdArrayOne[i] = pdArrayOne[i] * pdArrayTwo[i];
          } else {
            dReal = pdArrayOne[i] * pdArrayTwo[i] + pdArrayOne[iLength-i] * pdArrayTwo[iLength-i];
            dImag = pdArrayOne[i] * pdArrayTwo[iLength-i] - pdArrayOne[iLength-i] * pdArrayTwo[i];

            pdArrayOne[i] = dReal;
            pdArrayOne[iLength-i] = dImag;
          }
        }

        //
        // do the inverse FFT...
        //
        if (gsl_fft_halfcomplex_radix2_inverse( pdArrayOne, 1, iLength ) == 0) {
          if (step_value->length() != array_one->length()) {
            pdResult[0] = (double*)realloc( step_value->value(), array_one->length() * sizeof( double ) );
          } else {
            pdResult[0] = step_value->value();
          }

          if (correlated->length() != array_two->length()) {
            pdResult[1] = (double*)realloc( correlated->value(), array_two->length() * sizeof( double ) );
          } else {
            pdResult[1] = correlated->value();
          }

          if (pdResult[0] != NULL && pdResult[1] != NULL) {
            for (int i = 0; i < array_one->length(); ++i) {
              step_value->value()[i] = pdResult[0][i];
            }
            for (int i = 0; i < array_two->length(); ++i) {
              correlated->value()[i] = pdResult[1][i];
            }

            for (int i = 0; i < array_one->length(); i++) {
                step_value->value()[i] = (double)( i - ( array_one->length() / 2 ) );
            }

            memcpy( &(correlated->value()[array_one->length() / 2]),
                    &(pdArrayOne[0]),
                    ( ( array_one->length() + 1 ) / 2 ) * sizeof( double ) );

            memcpy( &(correlated->value()[0]),
                    &(pdArrayOne[iLength - (array_one->length() / 2)]),
                    ( array_one->length() / 2 ) * sizeof( double ) );

            iReturn = true;
          }
        }
      }
    }
  }
  delete[] pdArrayOne;
  delete[] pdArrayTwo;

  return iReturn;
}


QStringList CrossCorrelate::inputVectorList() const {
  return QStringList( ARRAY_ONE ) << ARRAY_TWO;
}


QStringList CrossCorrelate::inputScalarList() const {
  return QStringList();
}


QStringList CrossCorrelate::inputStringList() const {
  return QStringList();
}


QStringList CrossCorrelate::outputVectorList() const {
  return QStringList( STEP_VALUE ) << CORRELATED;
}


QStringList CrossCorrelate::outputScalarList() const {
  return QStringList();
}


QStringList CrossCorrelate::outputStringList() const {
  return QStringList();
}

#include "crosscorrelate.moc"
