/***************************************************************************
                   statistics.cpp
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
#include "statistics.h"

#include <kgenericfactory.h>

static const QString& DATA = KGlobal::staticQString("Data Array");
static const QString& MEAN = KGlobal::staticQString("Mean");
static const QString& MINIMUM = KGlobal::staticQString("Minimum");
static const QString& MAXIMUM = KGlobal::staticQString("Maximum");
static const QString& VARIANCE = KGlobal::staticQString("Variance");
static const QString& STANDARD_DEVIATION = KGlobal::staticQString("Standard deviation");
static const QString& MEDIAN = KGlobal::staticQString("Median");
static const QString& ABSOLUTE_DEVIATION = KGlobal::staticQString("Absolute deviation");
static const QString& SKEWNESS = KGlobal::staticQString("Skewness");
static const QString& KURTOSIS = KGlobal::staticQString("Kurtosis");

KST_KEY_DATAOBJECT_PLUGIN( statistics )

K_EXPORT_COMPONENT_FACTORY( kstobject_statistics,
    KGenericFactory<Statistics>( "kstobject_statistics" ) )

Statistics::Statistics( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Statistics::~Statistics() {
}


bool Statistics::algorithm() {

  KstVectorPtr data               = inputVector(DATA);
  KstScalarPtr mean               = outputScalar(MEAN);
  KstScalarPtr minimum            = outputScalar(MINIMUM);
  KstScalarPtr maximum            = outputScalar(MAXIMUM);
  KstScalarPtr variance           = outputScalar(VARIANCE);
  KstScalarPtr standard_deviation = outputScalar(STANDARD_DEVIATION);
  KstScalarPtr median             = outputScalar(MEDIAN);
  KstScalarPtr absolute_deviation = outputScalar(ABSOLUTE_DEVIATION);
  KstScalarPtr skewness           = outputScalar(SKEWNESS);
  KstScalarPtr kurtosis           = outputScalar(KURTOSIS);

  double* pCopy;
  double dMean = 0.0;
  double dMedian = 0.0;
  double dStandardDeviation = 0.0;
  double dTotal = 0.0;
  double dSquaredTotal = 0.0;
  double dMinimum = 0.0;
  double dMaximum = 0.0;
  double dVariance = 0.0;
  double dAbsoluteDeviation = 0.0;
  double dSkewness = 0.0;
  double dKurtosis = 0.0;
  int iLength;
  int iRetVal = false;

  if (data->length() > 0) {
    iLength = data->length();

    for (int i=0; i<iLength; i++) {
      if (i == 0 || data->value()[i] < dMinimum) {
        dMinimum = data->value()[i];
      }
      if (i == 0 || data->value()[i] > dMaximum) {
        dMaximum = data->value()[i];
      }
      dTotal += data->value()[i];
      dSquaredTotal += data->value()[i] * data->value()[i];
    }

    dMean = dTotal / (double)iLength;
    if (iLength > 1) {
      dVariance  = 1.0 / ( (double)iLength - 1.0 );
      dVariance *= dSquaredTotal - ( dTotal * dTotal / (double)iLength ); 
      if (dVariance > 0.0) {
        dStandardDeviation = sqrt( dVariance );
      } else {
        dVariance = 0.0;
        dStandardDeviation = 0.0;
      }
    }

    for (int i=0; i<iLength; i++) {
      dAbsoluteDeviation += fabs( data->value()[i] - dMean );
      dSkewness               += pow( data->value()[i] - dMean, 3.0 );
      dKurtosis               += pow( data->value()[i] - dMean, 4.0 );
    }
    dAbsoluteDeviation /= (double)iLength;
    dSkewness                 /= (double)iLength * pow( dStandardDeviation, 3.0 );
    dKurtosis                 /= (double)iLength * pow( dStandardDeviation, 4.0 );
    dKurtosis                 -= 3.0;

    /*
    sort by phase...
    */
    pCopy = (double*)calloc( iLength, sizeof( double ) );
    if (pCopy != NULL) {
      memcpy( pCopy, data->value(), iLength * sizeof( double ) );
      quicksort( pCopy, 0, iLength-1 );
      dMedian = pCopy[ iLength / 2 ];

      free( pCopy );
    }

    mean->setValue(dMean);
    minimum->setValue(dMinimum);
    maximum->setValue(dMaximum);
    variance->setValue(dVariance);
    standard_deviation->setValue(dStandardDeviation);
    median->setValue(dMedian);
    absolute_deviation->setValue(dAbsoluteDeviation);
    skewness->setValue(dSkewness);
    kurtosis->setValue(dKurtosis);

    iRetVal = true;
  }

  return iRetVal;
}


QStringList Statistics::inputVectorList() const {
  return QStringList( DATA );
}


QStringList Statistics::inputScalarList() const {
  return QStringList();
}


QStringList Statistics::inputStringList() const {
  return QStringList();
}


QStringList Statistics::outputVectorList() const {
  return QStringList();
}


QStringList Statistics::outputScalarList() const {
  return QStringList( MEAN ) << MINIMUM << MAXIMUM << VARIANCE << STANDARD_DEVIATION << MEDIAN << ABSOLUTE_DEVIATION << SKEWNESS << KURTOSIS;
}


QStringList Statistics::outputStringList() const {
  return QStringList();
}

void Statistics::swap( double* pData, int iOne, int iTwo) {
  double dTemp;

  dTemp = pData[iOne];
  pData[iOne] = pData[iTwo];
  pData[iTwo] = dTemp;
}


void Statistics::quicksort( double* pData, int iLeft, int iRight) {

  double dVal = pData[iRight];
  int i = iLeft - 1;
  int j = iRight;

  if (iRight <= iLeft) {
    return;
  }

  while (1) {
    while (pData[++i] < dVal) {
    }

    while(dVal < pData[--j]) {
      if (j == iLeft) {
        break;
      }
    }
    if (i >= j) {
      break;
    }
    swap( pData, i, j );
  }
  swap( pData, i, iRight );
  quicksort( pData, iLeft, i-1 );
  quicksort( pData, i+1, iRight );
}

#include "statistics.moc"
