/***************************************************************************
                   convolve.cpp
                             -------------------
    begin                : 11/29/06
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
#include "convolve.h"

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#include <kgenericfactory.h>

static const QString& ARRAY_ONE = KGlobal::staticQString("Array One");
static const QString& ARRAY_TWO = KGlobal::staticQString("Array Two");
static const QString& CONVOLVED = KGlobal::staticQString("Convolved");

KST_KEY_DATAOBJECT_PLUGIN( convolve )

K_EXPORT_COMPONENT_FACTORY( kstobject_convolve,
    KGenericFactory<Convolve>( "kstobject_convolve" ) )

Convolve::Convolve( QObject * /*parent*/, const char * /*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Convolve::~Convolve() {
}


bool Convolve::algorithm() {

  KstVectorPtr arrayOne    = inputVector(ARRAY_ONE);
  KstVectorPtr arrayTwo    = inputVector(ARRAY_TWO);
  KstVectorPtr convolved   = outputVector(CONVOLVED);

  if (arrayOne->length() <= 0 && arrayTwo->length() <= 0)
    return false;

  double* pdResponse;
  double* pdConvolve;
  double* pdResult;
  double  dReal;
  double  dImag;

  KstVectorPtr response;
  KstVectorPtr convolve;

  int iLength;
  int iLengthNew;

  bool iReturn = false;
  int iResponseMidpoint;

  //
  // determine which is the response function:
  //  i.e. which is shorter...
  //
  if (arrayOne->length() < arrayTwo->length()) {
    response = arrayOne;
    convolve = arrayTwo;
  } else {
    response = arrayTwo;
    convolve = arrayOne;
  }

  convolved->resize(convolve->length(), false);

  iResponseMidpoint = response->length() / 2;
  iLength = convolve->length() + iResponseMidpoint;

  //
  // round iLength up to the nearest factor of two...
  //
  iLengthNew = 64;
  while (iLengthNew < iLength && iLengthNew > 0) {
    iLengthNew *= 2;
  }
  iLength = iLengthNew;

  if (iLength <= 0)
    return false;

  pdResponse = new double[iLength];
  pdConvolve = new double[iLength];
  if (pdResponse != NULL && pdConvolve != NULL) {
    //
    // sort the response function into wrap-around order...
    //
    memset( pdResponse, 0, iLength * sizeof( double ) );

    for (int i = 0; i < iResponseMidpoint; i++) {
      pdResponse[i]                           = response->value()[iResponseMidpoint+i];
      pdResponse[iLength-iResponseMidpoint+i] = response->value()[i];
    }

    //
    // handle the case where the response function has an odd number of points...
    //
    if (iResponseMidpoint % 2 == 1) {
      pdResponse[iResponseMidpoint]           = response->value()[response->length()-1];
    }

    //
    // zero-pad the convolve array...
    //
    memset( pdConvolve, 0, iLength * sizeof( double ) );
    memcpy( pdConvolve, convolve->value(), convolve->length() * sizeof( double ) );

    //
    // calculate the FFTs of the two functions...
    //
    if (gsl_fft_real_radix2_transform( pdResponse, 1, iLength ) == 0) {
      if (gsl_fft_real_radix2_transform( pdConvolve, 1, iLength ) == 0) {
        //
        // multiply the FFTs together...
        //
        for (int i=0; i < iLength/2; i++) {
          if (i==0 || i==(iLength/2)-1) {
            pdResponse[i] = pdResponse[i] * pdConvolve[i];
          } else {
            dReal = pdResponse[i] * pdConvolve[i] - pdResponse[iLength-i] * pdConvolve[iLength-i];
            dImag = pdResponse[i] * pdConvolve[iLength-i] + pdResponse[iLength-i] * pdConvolve[i];

            pdResponse[i]         = dReal;
            pdResponse[iLength-i] = dImag;
          }
        }

        //
        // do the inverse FFT...
        //
        if (gsl_fft_halfcomplex_radix2_inverse( pdResponse, 1, iLength) == 0) {
          if (convolved->length() != convolve->length()) {
            pdResult = (double*)realloc( convolved->value(), convolve->length() * sizeof( double ) );
          } else {
            pdResult = convolved->value();
          }

          if (pdResult != NULL) {
            for (int i = 0; i < convolve->length(); ++i) {
              convolved->value()[i] = pdResult[i];
            }

            memcpy( pdResult, pdResponse, convolve->length() * sizeof( double ) );

            iReturn = true;
          }
        }
      }
    }
  }
  delete[] pdResponse;
  delete[] pdConvolve;

  return iReturn;
}


QStringList Convolve::inputVectorList() const {
  return QStringList( ARRAY_ONE ) << ARRAY_TWO;
}


QStringList Convolve::inputScalarList() const {
  return QStringList();
}


QStringList Convolve::inputStringList() const {
  return QStringList();
}


QStringList Convolve::outputVectorList() const {
  return QStringList( CONVOLVED );
}


QStringList Convolve::outputScalarList() const {
  return QStringList();
}


QStringList Convolve::outputStringList() const {
  return QStringList();
}

#include "convolve.moc"
