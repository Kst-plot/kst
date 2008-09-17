/***************************************************************************
                   syncbin.cpp
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
#include "syncbin.h"

#include <kgenericfactory.h>

// macros to find the top, bottom, and middle of a bin
#define BINMID(x) XMin+(XMax-XMin)*(double(x)+0.5)/double(nbins)

#define BIN( x ) int(double(nbins)*(x-XMin)/(XMax-XMin))

static const QString& X_IN = KGlobal::staticQString("X in");
static const QString& Y_IN = KGlobal::staticQString("Y in");
static const QString& BINS = KGlobal::staticQString("Number of Bins");
static const QString& X_MIN = KGlobal::staticQString("X min");
static const QString& X_MAX = KGlobal::staticQString("X max");
static const QString& X_OUT = KGlobal::staticQString("X out");
static const QString& Y_OUT = KGlobal::staticQString("Y out");
static const QString& Y_ERROR = KGlobal::staticQString("Y error");
static const QString& N = KGlobal::staticQString("N");

KST_KEY_DATAOBJECT_PLUGIN( syncbin )

K_EXPORT_COMPONENT_FACTORY( kstobject_syncbin,
    KGenericFactory<Syncbin>( "kstobject_syncbin" ) )

Syncbin::Syncbin( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Syncbin::~Syncbin() {
}


bool Syncbin::algorithm() {

  KstVectorPtr x_in     = inputVector(X_IN);
  KstVectorPtr y_in     = inputVector(Y_IN);
  KstScalarPtr bins     = inputScalar(BINS);
  KstScalarPtr x_min    = inputScalar(X_MIN);
  KstScalarPtr x_max    = inputScalar(X_MAX);
  KstVectorPtr x_out    = outputVector(X_OUT);
  KstVectorPtr y_out    = outputVector(Y_OUT);
  KstVectorPtr y_error  = outputVector(Y_ERROR);
  KstVectorPtr n        = outputVector(N);

  int nbins=int( bins->value() );
  int n_in;
  double XMin = x_min->value();
  double XMax = x_max->value();
  double *Xout, *Yout, *Yerr, *N;

  // check for ill defined inputs...
  if ((x_in->length() < 1) ||
      (y_in->length() != x_in->length() ) ||
      (nbins < 2))  {
    return -1;
  }

  //resize the output arrays
  x_out->resize(nbins, true);
  y_out->resize(nbins, true);
  y_error->resize(nbins, true);
  n->resize(nbins, true);

  // convenience definitions
  n_in = int( x_in->length() );
  const double *Xin = x_in->value();
  const double *Yin = y_in->value();
  Xout = x_out->value();
  Yout = y_out->value();
  Yerr = y_error->value();
  N    = n->value();

  // set/check XMax and XMin
  if ( XMax <= XMin ) { // autobin
    XMax = XMin = Xin[0];
    for (int i=1; i<n_in; i++ ) {
      if ( XMax>Xin[i] ) XMax = Xin[i];
      if ( XMin<Xin[i] ) XMin = Xin[i];
    }
    // make sure end points are included.
    double d = (XMax - XMin)/double(nbins*100.0);
    XMax+=d;
    XMin-=d;
  }

  if ( XMax == XMin ) { // don't want divide by zero...
    XMax +=1;
    XMin -=1;
  }

  // Fill Xout and zero Yout and Yerr
  for ( int i=0; i<nbins; i++ ) {
    Xout[i] = BINMID( i );
    Yout[i] = Yerr[i] = 0.0;
    N[i] = 0.0;
  }

  //bin the data
  int bin, last_bin=-1;
  int last_N=0;
  double last_sY=0;

  for ( int i=0; i<n_in; i++ ) {
    bin = BIN( Xin[i] );
    if (bin == last_bin) {
      last_sY += Yin[i];
      last_N++;
    } else { // new bin
      if (last_N>0) {
	last_sY/=last_N;
	if ( (last_bin>=0) && (last_bin<nbins) ) {
	  Yout[last_bin]+=last_sY;
	  Yerr[last_bin]+=last_sY*last_sY;
	  N[last_bin]++;
	}
      }
      last_sY = Yin[i];
      last_N = 1;
      last_bin = bin;
    }
  }
  if (last_N>0) {
    last_sY/=last_N;
    if ( (last_bin>=0) && (last_bin<nbins) ) {
      Yout[last_bin]+=last_sY;
      Yerr[last_bin]+=last_sY*last_sY;
      N[last_bin]++;
    }
  }

  // normalize the bins
  for ( int i = 0; i<nbins; i++ ) {
    if ( N[i]>0 ) {
      Yerr[i] = sqrt( Yerr[i] - Yout[i]*Yout[i]/N[i] )/N[i];
      Yout[i]/=N[i];
    }
  }

  return true;
}


QStringList Syncbin::inputVectorList() const {
  return QStringList( X_IN ) << Y_IN;
}


QStringList Syncbin::inputScalarList() const {
  return QStringList( BINS ) << X_MIN << X_MAX;
}


QStringList Syncbin::inputStringList() const {
  return QStringList();
}


QStringList Syncbin::outputVectorList() const {
  return QStringList( X_OUT ) << Y_OUT << Y_ERROR << N;
}


QStringList Syncbin::outputScalarList() const {
  return QStringList();
}


QStringList Syncbin::outputStringList() const {
  return QStringList();
}

#include "syncbin.moc"
