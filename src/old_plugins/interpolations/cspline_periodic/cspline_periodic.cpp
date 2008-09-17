/***************************************************************************
                   cspline_periodic.cpp
                             -------------------
    begin                : 12/13/06
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
#include "cspline_periodic.h"

#include <gsl/gsl_spline.h>
#include "../interpolations.h"

#include <kgenericfactory.h>

static const QString& X_ARRAY = KGlobal::staticQString("X Array");
static const QString& Y_ARRAY = KGlobal::staticQString("Y Array");
static const QString& X1_ARRAY = KGlobal::staticQString("X' Array");
static const QString& Y_INTERPOLATED = KGlobal::staticQString("Y Interpolated");

KST_KEY_DATAOBJECT_PLUGIN( cspline_periodic )

K_EXPORT_COMPONENT_FACTORY( kstobject_cspline_periodic,
    KGenericFactory<CSplinePeriodic>( "kstobject_cspline_periodic" ) )

CSplinePeriodic::CSplinePeriodic( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


CSplinePeriodic::~CSplinePeriodic() {
}


bool CSplinePeriodic::algorithm() {

  KstVectorPtr x_array        = inputVector(X_ARRAY);
  KstVectorPtr y_array        = inputVector(Y_ARRAY);
  KstVectorPtr x1_array       = inputVector(X1_ARRAY);
  KstVectorPtr y_interpolated = outputVector(Y_INTERPOLATED);

  return interpolate( x_array, y_array, x1_array, y_interpolated, gsl_interp_cspline_periodic);
}


QStringList CSplinePeriodic::inputVectorList() const {
  return QStringList( X_ARRAY ) << Y_ARRAY << X1_ARRAY;
}


QStringList CSplinePeriodic::inputScalarList() const {
  return QStringList();
}


QStringList CSplinePeriodic::inputStringList() const {
  return QStringList();
}


QStringList CSplinePeriodic::outputVectorList() const {
  return QStringList( Y_INTERPOLATED );
}


QStringList CSplinePeriodic::outputScalarList() const {
  return QStringList();
}


QStringList CSplinePeriodic::outputStringList() const {
  return QStringList();
}

#include "cspline_periodic.moc"
