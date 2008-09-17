/***************************************************************************
                   linefit.cpp
                             -------------------
    begin                : 09/08/06
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
#include "linefit.h"

#include <kgenericfactory.h>

//in
static const QString& X_ARRAY = KGlobal::staticQString("X Array");
static const QString& Y_ARRAY = KGlobal::staticQString("Y Array");

//out
static const QString& X_INTERPOLATED = KGlobal::staticQString("X Interpolated");
static const QString& Y_INTERPOLATED = KGlobal::staticQString("Y Interpolated");

static const QString& A = KGlobal::staticQString("a");
static const QString& B = KGlobal::staticQString("b");
static const QString& CHI2 = KGlobal::staticQString("chi^2");

KST_KEY_DATAOBJECT_PLUGIN( linefit )

K_EXPORT_COMPONENT_FACTORY( kstobject_linefit,
    KGenericFactory<LineFit>( "kstobject_linefit" ) )

LineFit::LineFit( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


LineFit::~LineFit() {
}


bool LineFit::algorithm() {
  int i = 0;
  double a = 0.0, b = 0.0, sx = 0.0, sy = 0.0, sxoss = 0.0, st2 = 0.0, chi2 = 0.0;
  double xScale;

  KstVectorPtr xIn    = inputVector(X_ARRAY);
  KstVectorPtr yIn    = inputVector(Y_ARRAY);

  KstVectorPtr xOut   = outputVector(X_INTERPOLATED);
  KstVectorPtr yOut   = outputVector(Y_INTERPOLATED);

  KstScalarPtr _a     = outputScalar(A);
  KstScalarPtr _b     = outputScalar(B);
  KstScalarPtr _chi2  = outputScalar(CHI2);

  if (yIn->length() < 1 || xIn->length() < 1) {
    return false;
  }

  if (xOut->length() != 2) {
    xOut->resize( 2, false );
    }
  if (yOut->length() != 2) {
    yOut->resize( 2, false );
  }

  if (yIn->length() == 1) {
    xOut->value()[0] = xIn->value()[0];
    xOut->value()[1] = xIn->value()[xIn->length() - 1];
    yOut->value()[0] = yIn->value()[0];
    yOut->value()[1] = yIn->value()[0];
    _a->setValue( yIn->value()[0] );
    _b->setValue( 0 );
    _chi2->setValue( chi2 );
    return true;
  }

  xScale = xIn->length()/yIn->length();

  for (i = 0; i < yIn->length(); i++) {
    double z = xScale*i;
    long int idx = long(rint(z));
    double skew = z - floor(z); /* [0..1] */
    long int idx2 = idx + 1;
    sy += yIn->value()[i];
    while (idx2 >= yIn->length()) {
      idx2--;
    }
    sx += xIn->value()[idx] + (xIn->value()[idx2] - xIn->value()[idx])*skew;
  }

  sxoss = sx / xIn->length();

  for (i = 0; i < xIn->length(); i++) {
    double t = xIn->value()[i] - sxoss;
    st2 += t * t;
    b += t * yIn->value()[i];
  }

  b /= st2;
  a = (sy - sx*b)/xIn->length();

  /* could put goodness of fit, etc, in here */

  xOut->value()[0] = xIn->value()[0];
  xOut->value()[1] = xIn->value()[xIn->length()-1];
  yOut->value()[0] = a+b*xOut->value()[0];
  yOut->value()[1] = a+b*xOut->value()[1];

  for (i = 0; i < xIn->length(); i++) {
    double z = xScale*i;
    long int idx = long(rint(z));
    double skew = z - floor(z); /* [0..1] */
    long int idx2 = idx + 1;
    double newX;
    double ci;
    while (idx2 >= xIn->length()) {
      idx2--;
    }
    newX = xIn->value()[idx] + (xIn->value()[idx2] - xIn->value()[idx])*skew;
    ci = yIn->value()[i] - a - b*newX;
    chi2 += ci*ci;
  }

  _a->setValue( a );
  _b->setValue( b );
  _chi2->setValue( chi2 );
  return true;
}


QStringList LineFit::inputVectorList() const {
  return QStringList( X_ARRAY ) << Y_ARRAY;
}


QStringList LineFit::inputScalarList() const {
  return QStringList();
}


QStringList LineFit::inputStringList() const {
  return QStringList();
}


QStringList LineFit::outputVectorList() const {
  return QStringList( X_INTERPOLATED ) << Y_INTERPOLATED;
}


QStringList LineFit::outputScalarList() const {
  return QStringList( A ) << B << CHI2;
}


QStringList LineFit::outputStringList() const {
  return QStringList();
}

#include "linefit.moc"
