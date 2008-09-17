/***************************************************************************
                   autocorrelate.cpp
                             -------------------
    begin                : 12/05/06
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
#include "autocorrelate.h"

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#include <kgenericfactory.h>

static const QString& ARRAY = KGlobal::staticQString("Array");
static const QString& STEP_VALUE = KGlobal::staticQString("Step value");
static const QString& AUTO_CORRELATED = KGlobal::staticQString("Auto-correlated");

KST_KEY_DATAOBJECT_PLUGIN( autocorrelate )

K_EXPORT_COMPONENT_FACTORY( kstobject_autocorrelate,
    KGenericFactory<AutoCorrelate>( "kstobject_autocorrelate" ) )

AutoCorrelate::AutoCorrelate( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


AutoCorrelate::~AutoCorrelate() {
}


bool AutoCorrelate::algorithm() {

  KstVectorPtr array            = inputVector(ARRAY);
  KstVectorPtr step_value       = outputVector(STEP_VALUE);
  KstVectorPtr auto_correlated  = outputVector(AUTO_CORRELATED);

  if (array->length() <= 0) {
    return false;
  }

  double* pdArrayOne;
  double* pdResult;
  double* pdCorrelate;
  double  dReal;
  double  dImag;

  int iLength;
  int iLengthNew;

  bool iReturn = false;

  //
  // zero-pad the array...
  //
  iLength  = array->length();
  iLength *= 2;

  step_value->resize(array->length(), false);
  auto_correlated->resize(array->length(), false);

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
  if (pdArrayOne != NULL) {
    //
    // zero-pad the two arrays...
    //
    memset( pdArrayOne, 0, iLength * sizeof( double ) );
    memcpy( pdArrayOne, array->value(), array->length() * sizeof( double ) );

    //
    // calculate the FFTs of the two functions...
    //
    if (gsl_fft_real_radix2_transform( pdArrayOne, 1, iLength ) == 0) {
      //
      // multiply the FFT by its complex conjugate...
      //
      for (int i=0; i<iLength/2; i++) {
        if (i==0 || i==(iLength/2)-1) {
          pdArrayOne[i] *= pdArrayOne[i];
        } else {
          dReal = pdArrayOne[i] * pdArrayOne[i] + pdArrayOne[iLength-i] * pdArrayOne[iLength-i];
          dImag = pdArrayOne[i] * pdArrayOne[iLength-i] - pdArrayOne[iLength-i] * pdArrayOne[i];

          pdArrayOne[i] = dReal;
          pdArrayOne[iLength-i] = dImag;
        }
      }

      //
      // do the inverse FFT...
      //
      if (gsl_fft_halfcomplex_radix2_inverse( pdArrayOne, 1, iLength ) == 0) {
        if (step_value->length() != array->length()) {
          pdResult = (double*)realloc( step_value->value(), array->length() * sizeof( double ) );
        } else {
          pdResult = step_value->value();
        }

        if (auto_correlated->length() != array->length()) {
          pdCorrelate = (double*)realloc( auto_correlated->value(), array->length() * sizeof( double ) );
        } else {
          pdCorrelate = auto_correlated->value();
        }

        if (pdResult != NULL && pdCorrelate != NULL) {
          for (int i = 0; i < array->length(); ++i) {
            step_value->value()[i] = pdResult[i];
          }
          for (int i = 0; i < array->length(); ++i) {
            auto_correlated->value()[i] = pdCorrelate[i];
          }

          for (int i = 0; i < array->length(); i++) {
              step_value->value()[i] = (double)( i - ( array->length() / 2 ) );
          }

          memcpy( &(auto_correlated->value()[array->length() / 2]),
                  &(pdArrayOne[0]),
                  ( ( array->length() + 1 ) / 2 ) * sizeof( double ) );

          memcpy( &(auto_correlated->value()[0]),
                  &(pdArrayOne[iLength - (array->length() / 2)]),
                  ( array->length() / 2 ) * sizeof( double ) );

          iReturn = true;
        }
      }
    }
  }
  delete[] pdArrayOne;

  return iReturn;
}


QStringList AutoCorrelate::inputVectorList() const {
  return QStringList( ARRAY );
}


QStringList AutoCorrelate::inputScalarList() const {
  return QStringList();
}


QStringList AutoCorrelate::inputStringList() const {
  return QStringList();
}


QStringList AutoCorrelate::outputVectorList() const {
  return QStringList( STEP_VALUE ) << AUTO_CORRELATED;
}


QStringList AutoCorrelate::outputScalarList() const {
  return QStringList();
}


QStringList AutoCorrelate::outputStringList() const {
  return QStringList();
}

#include "autocorrelate.moc"
