/***************************************************************************
          effective_bandwidth.cpp  -  Calculate Effective Bandwidth
                             -------------------
    begin                : Jun 6, 2006
    copyright            : (C) 2006 by Duncan Hanson
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
#include "effective_bandwidth.h"

#include <math.h>
#include <kgenericfactory.h>

//in
static const QString& X_ARRAY = KGlobal::staticQString("X Array");
static const QString& Y_ARRAY = KGlobal::staticQString("Y Array");
static const QString& MIN_WN_FREQ = KGlobal::staticQString("Min. White Noise Freq.");
static const QString& SAMPLING_FREQ = KGlobal::staticQString("SamplingFrequency (Hz)");
static const QString& K = KGlobal::staticQString("K");

//out
static const QString& LIMIT = KGlobal::staticQString("White Noise Limit");
static const QString& SIGMA = KGlobal::staticQString("White Noise Sigma");
static const QString& EFF_BANDWIDTH = KGlobal::staticQString("Effective Bandwidth");

KST_KEY_DATAOBJECT_PLUGIN( effbandwidth )

K_EXPORT_COMPONENT_FACTORY( kstobject_effbandwidth,
    KGenericFactory<EffBandwidth>( "kstobject_effbandwidth" ) )

EffBandwidth::EffBandwidth( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


EffBandwidth::~EffBandwidth() {
}


bool EffBandwidth::algorithm() {
  KstVectorPtr xArray   = inputVector(X_ARRAY);
  KstVectorPtr yArray   = inputVector(Y_ARRAY);

  KstScalarPtr min      = inputScalar(MIN_WN_FREQ);
  KstScalarPtr sampling = inputScalar(SAMPLING_FREQ);
  KstScalarPtr k        = inputScalar(K);

  KstScalarPtr limit    = outputScalar(LIMIT);
  KstScalarPtr sigma    = outputScalar(SIGMA);
  KstScalarPtr eff      = outputScalar(EFF_BANDWIDTH);

  if ((yArray->length() != xArray->length() ) || (xArray->length() < 1))  {
    return false;
  }

  double minWhiteNoiseFreq, samplingFrequency, radiometerConstantK;

  minWhiteNoiseFreq = min->value();
  samplingFrequency = sampling->value();
  radiometerConstantK = k->value();

  int minWhiteNoiseIndex;

  //fast calculation of index for minWhiteNoiseFreq
  int i_bot, i_top;
  i_bot = 0;
  i_top = xArray->length() - 1;

  while (i_bot + 1 < i_top) {
    int i0 = (i_top + i_bot)/2;
    if (minWhiteNoiseFreq < xArray->value()[i0]) {
      i_top = i0;
    } else {
      i_bot = i0;
    }
  }
  minWhiteNoiseIndex = i_top;

  //verify calculated indices.
  if ( !(minWhiteNoiseIndex>0) || !(minWhiteNoiseIndex<(xArray->length()-1)) ) {
    return false;
  }

  // calculate white noise limit
  double sumY, sumY2;
  sumY = sumY2 = 0;

  int i;
  double yi;

  for (i = minWhiteNoiseIndex; i < xArray->length(); i++) {
    yi = yArray->value()[i];
    sumY    +=  yi;
    sumY2   +=  pow(yi,2);
  }

  double ybar, ysigma;
  ybar = sumY/(xArray->length() - minWhiteNoiseIndex);
  ysigma = sqrt((sumY2 - 2*ybar*sumY + pow(ybar,2)*(xArray->length() - minWhiteNoiseIndex))/(xArray->length() - minWhiteNoiseIndex));
  // end calculate white noise limit

  double effectiveBandwidth = 2*samplingFrequency*pow(radiometerConstantK*yArray->value()[0]/ysigma,2);

  // output fit data
  limit->setValue(ybar);
  sigma->setValue(ysigma);
  eff->setValue(effectiveBandwidth);

  return true;
}


QStringList EffBandwidth::inputVectorList() const {
  return QStringList( X_ARRAY ) << Y_ARRAY;
}


QStringList EffBandwidth::inputScalarList() const {
  return QStringList( MIN_WN_FREQ ) << SAMPLING_FREQ << K;
}


QStringList EffBandwidth::inputStringList() const {
  return QStringList();
}


QStringList EffBandwidth::outputVectorList() const {
  return QStringList();
}


QStringList EffBandwidth::outputScalarList() const {
  return QStringList( LIMIT ) << SIGMA << EFF_BANDWIDTH;
}


QStringList EffBandwidth::outputStringList() const {
  return QStringList();
}

#include "effective_bandwidth.moc"
