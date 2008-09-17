/***************************************************************************
                   genericfilter.cpp
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
#include "genericfilter.h"

#include "filter.h"
#include "polynom.h"

#include <qregexp.h>

#include <kgenericfactory.h>

static const QString& Y = KGlobal::staticQString("Y");
static const QString& NUMERATOR = KGlobal::staticQString("Numerator (increasing order)");
static const QString& DENOMINATOR = KGlobal::staticQString("Denominator (increasing order)");
static const QString& SAMPLING = KGlobal::staticQString("Sampling interval (s)");
static const QString& FILTERED = KGlobal::staticQString("Filtered");

KST_KEY_DATAOBJECT_PLUGIN( discretizing_filters )

K_EXPORT_COMPONENT_FACTORY( kstobject_discretizing_filters,
    KGenericFactory<GenericFilter>( "kstobject_discretizing_filters" ) )

GenericFilter::GenericFilter( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


GenericFilter::~GenericFilter() {
}


bool GenericFilter::algorithm() {

  KstVectorPtr y           = inputVector(Y);
  KstStringPtr numerator   = inputString(NUMERATOR);
  KstStringPtr denominator = inputString(DENOMINATOR);
  KstScalarPtr sampling    = inputScalar(SAMPLING);
  KstVectorPtr filtered    = outputVector(FILTERED);

  int length = y->length();

  // Extract polynom coefficients and instantiate polynoms
  QStringList numCoeffs =
    QStringList::split(QRegExp("\\s*(,|;|:)\\s*"), numerator->value());
  QStringList denCoeffs =
    QStringList::split(QRegExp("\\s*(,|;|:)\\s*"), denominator->value());
  int numDegree = numCoeffs.count() - 1, denDegree = denCoeffs.count() - 1;
  polynom<double> Num(numDegree), Den(denDegree);
  double tmpDouble = 0.0;
  bool ok = false;
  for (int i=0; i<=numDegree; i++) {
    tmpDouble = numCoeffs[i].toDouble(&ok);
    if (ok) Num[i]= tmpDouble;
    else Num[i] = 0.0;
  }
  for (int i=0; i<=denDegree; i++) {
    tmpDouble = denCoeffs[i].toDouble(&ok);
    if (ok) Den[i] = tmpDouble;
    else Den[i] = 0.0;
  }

  // Time step
  double DeltaT = sampling->value();

  // Allocate storage for output vectors
  filtered->resize(length, true);

  // Create filter
  filter<double> theFilter(Num,Den,DeltaT);
  double in = 0.0;
  theFilter.ConnectTo(in);
  theFilter.Reset();
  for (int i=0; i<length; i++) {
    in = y->value()[i];
    theFilter.NextTimeStep();
    filtered->value()[i] = theFilter.out;
  }

  return true;
}


QStringList GenericFilter::inputVectorList() const {
  return QStringList( Y );
}


QStringList GenericFilter::inputScalarList() const {
  return QStringList( SAMPLING );
}


QStringList GenericFilter::inputStringList() const {
  return QStringList( NUMERATOR ) << DENOMINATOR;
}


QStringList GenericFilter::outputVectorList() const {
  return QStringList( FILTERED );
}


QStringList GenericFilter::outputScalarList() const {
  return QStringList();
}


QStringList GenericFilter::outputStringList() const {
  return QStringList();
}

#include "genericfilter.moc"
