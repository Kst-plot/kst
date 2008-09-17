/***************************************************************************
                   bin.cpp
                             -------------------
    begin                : 11/28/06
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
#include "bin.h"

#include <kgenericfactory.h>

//in
static const QString& INPUT = KGlobal::staticQString("Input Vector");
static const QString& SIZE = KGlobal::staticQString("Bin Size");

//out
static const QString& BINS = KGlobal::staticQString("Bins");

KST_KEY_DATAOBJECT_PLUGIN( bin )

K_EXPORT_COMPONENT_FACTORY( kstobject_bin,
    KGenericFactory<Bin>( "kstobject_bin" ) )

Bin::Bin( QObject */*parent*/, const char */*name*/, const QStringList &/*args*/ )
    : KstBasicPlugin() {
}


Bin::~Bin() {
}


//Bin the elements into the given size bins, additional elements at the end of the
//input vector are ignored.
//Returns -1 on error, 0 on success.
bool Bin::algorithm() {

  KstVectorPtr input    = inputVector(INPUT);
  KstScalarPtr size     = inputScalar(SIZE);
  KstVectorPtr bins     = outputVector(BINS);

  //Make sure there is at least 1 element in the input vector
  //Make sure the bin size is at least 1
  if (input->length() < 1 || size->value() < 1) {
    return -1;
  }

  // allocate the lengths
  bins->resize(int(input->length() / size->value()), false);

  //now bin the data
  for (int i=0; i<bins->length(); i++)
  {
      bins->value()[i]=0;
      //add up the elements for this bin
      for (int j=0; j<size->value(); j++)
      {
          bins->value()[i]+=input->value()[int(i*size->value()+j)];
      }
      //find the mean
      bins->value()[i]/=size->value();
  }
  return true;
}


QStringList Bin::inputVectorList() const {
  return QStringList( INPUT );
}


QStringList Bin::inputScalarList() const {
  return QStringList( SIZE );
}


QStringList Bin::inputStringList() const {
  return QStringList();
}


QStringList Bin::outputVectorList() const {
  return QStringList( BINS );
}


QStringList Bin::outputScalarList() const {
  return QStringList();
}


QStringList Bin::outputStringList() const {
  return QStringList();
}

#include "bin.moc"
