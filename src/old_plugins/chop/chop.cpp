/***************************************************************************
                   chop.cpp
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
#include "chop.h"

#include <kgenericfactory.h>

static const QString& ARRAY = KGlobal::staticQString("Array");
static const QString& ODD = KGlobal::staticQString("Odd Array");
static const QString& EVEN = KGlobal::staticQString("Even Array");
static const QString& DIFF = KGlobal::staticQString("Difference Array");
static const QString& INDEX = KGlobal::staticQString("Index Array");

KST_KEY_DATAOBJECT_PLUGIN( chop )

K_EXPORT_COMPONENT_FACTORY( kstobject_chop,
    KGenericFactory<Chop>( "kstobject_chop" ) )

Chop::Chop( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Chop::~Chop() {
}


bool Chop::algorithm() {

  KstVectorPtr array  = inputVector(ARRAY);

  KstVectorPtr odd    = outputVector(ODD);
  KstVectorPtr even   = outputVector(EVEN);
  KstVectorPtr diff   = outputVector(DIFF);
  KstVectorPtr index  = outputVector(INDEX);

  QValueList<KstVectorPtr> outputs;
  outputs << odd << even << diff << index;

  int iLength = array->length();
  int iLengthNew = (int)ceil(iLength / 2.0);

  if (iLength > 1) {

    QValueList<KstVectorPtr>::iterator it = outputs.begin();
    for(; it != outputs.end(); ++it) {
      if ((*it)->length() != iLengthNew) {
        (*it)->resize(iLengthNew, false);
      }
    }

    for (int i = 0; i < iLength; i+=2) {
      odd->value()[i/2] = array->value()[i];
      even->value()[i/2] = array->value()[i+1];
      diff->value()[i/2] = array->value()[i] - array->value()[i+1];
      index->value()[i/2] = i/2;
    }
  }

  return true;
}


QStringList Chop::inputVectorList() const {
  return QStringList( ARRAY );
}


QStringList Chop::inputScalarList() const {
  return QStringList();
}


QStringList Chop::inputStringList() const {
  return QStringList();
}


QStringList Chop::outputVectorList() const {
  return QStringList( ODD ) << EVEN << DIFF << INDEX;
}


QStringList Chop::outputScalarList() const {
  return QStringList();
}


QStringList Chop::outputStringList() const {
  return QStringList();
}

#include "chop.moc"
