/***************************************************************************
                   phase.cpp
                             -------------------
    begin                : 12/08/06
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
#include "phase.h"

#include <kgenericfactory.h>

static const QString& TIME = KGlobal::staticQString("Time Array");
static const QString& DATA_I = KGlobal::staticQString("Data In Array");
static const QString& PERIOD = KGlobal::staticQString("Period");
static const QString& ZERO = KGlobal::staticQString("Zero Phase");
static const QString& PHASE = KGlobal::staticQString("Phase Array");
static const QString& DATA_O = KGlobal::staticQString("Data Out Array");

KST_KEY_DATAOBJECT_PLUGIN( phase )

K_EXPORT_COMPONENT_FACTORY( kstobject_phase,
                            KGenericFactory<Phase>( "kstobject_phase" ) )

Phase::Phase( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {}


Phase::~Phase() {}


bool Phase::algorithm() {

  KstVectorPtr time     = inputVector(TIME);
  KstVectorPtr data_i   = inputVector(DATA_I);
  KstScalarPtr period   = inputScalar(PERIOD);
  KstScalarPtr zero     = inputScalar(ZERO);
  KstVectorPtr phase    = outputVector(PHASE);
  KstVectorPtr data_o   = outputVector(DATA_O);

  double* pResult[2];
  double  dPhasePeriod = period->value();
  double dPhaseZero = zero->value();
  int iLength;

  bool iRetVal = false;

  if (dPhasePeriod > 0.0) {
    if (time->length() == data_i->length()) {
      iLength = time->length();

      if (phase->length() != iLength) {
        phase->resize(iLength, true);
        pResult[0] = (double*)realloc( phase->value(), iLength * sizeof( double ) );
      } else {
        pResult[0] = phase->value();
      }

      if (data_o->length() != iLength) {
        data_o->resize(iLength, true);
        pResult[1] = (double*)realloc( data_o->value(), iLength * sizeof( double ) );
      } else {
        pResult[1] = data_o->value();
      }

      if (pResult[0] != NULL && pResult[1] != NULL) {
        for (int i = 0; i < phase->length(); ++i) {
          phase->value()[i] = pResult[0][i];
        }
        for (int i = 0; i < data_o->length(); ++i) {
          data_o->value()[i] = pResult[1][i];
        }

        /*
        determine the phase...
        */
        for (int i=0; i<iLength; i++) {
          phase->value()[i] = fmod( ( time->value()[i] - dPhaseZero ) / dPhasePeriod, 1.0 );
        }

        /*
        sort by phase...
        */
        memcpy( data_o->value(), data_i->value(), iLength * sizeof( double ) );
        double* sort[2];
        sort[0] = phase->value();
        sort[1] = data_o->value();
        quicksort( sort, 0, iLength-1 );

        iRetVal = true;
      }
    }
  }

  return iRetVal;
}

QStringList Phase::inputVectorList() const {
  return QStringList( TIME ) << DATA_I;
}


QStringList Phase::inputScalarList() const {
  return QStringList( PERIOD ) << ZERO;
}


QStringList Phase::inputStringList() const {
  return QStringList();
}


QStringList Phase::outputVectorList() const {
  return QStringList( PHASE ) << DATA_O;
}


QStringList Phase::outputScalarList() const {
  return QStringList();
}


QStringList Phase::outputStringList() const {
  return QStringList();
}

void Phase::swap(double* pData[], int iOne, int iTwo) {
  double dTemp;

  for (int i=0; i<2; i++) {
    dTemp = pData[i][iOne];
    pData[i][iOne] = pData[i][iTwo];
    pData[i][iTwo] = dTemp;
  }
}

void Phase::quicksort( double* pData[], int iLeft, int iRight) {
  double dVal = pData[0][iRight];
  int i = iLeft - 1;
  int j = iRight;

  if (iRight <= iLeft) {
    return;
  }

  while (1) {
    while (pData[0][++i] < dVal) {}

    while (dVal < pData[0][--j]) {
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

#include "phase.moc"
