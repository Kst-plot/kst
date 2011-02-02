/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testgeneratedvector.h"

#include "generatedvector.h"
#include "datasource.h"
#include "datacollection.h"
#include "objectstore.h"


#include <QXmlStreamAttributes>

#include "ksttest.h"


static Kst::ObjectStore _store;

void TestGeneratedVector::cleanupTestCase() {
  _store.clear();
}


void TestGeneratedVector::testGeneratedVector() {

  //basic default constructor values
  Kst::GeneratedVectorPtr m1 = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  QCOMPARE(m1->length(), 1);
  QCOMPARE(m1->min(), 0.0);
  QCOMPARE(m1->max(), 0.0);
  QCOMPARE(m1->value(0), &Kst::NOPOINT);
  QCOMPARE(m1->value(10), 0.0); //should be outside the boundaries.
  QCOMPARE(m1->mean(), &Kst::NOPOINT);

  m1->changeRange(0, 100, 100);
  QCOMPARE(m1->length(), 100);

  m1->writeLock();
  m1->internalUpdate();
  m1->unlock();

  QCOMPARE(m1->length(), 100);
  QCOMPARE(m1->min(), 0.0);
  QCOMPARE(m1->max(), 100.0);
  QCOMPARE(m1->value(0), 0.0);
  QCOMPARE(m1->value(99), 100.0);
}

#ifdef KST_USE_QTEST_MAIN
QTEST_MAIN(TestGeneratedVector)
#endif

// vim: ts=2 sw=2 et
