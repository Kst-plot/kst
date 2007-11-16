/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testgeneratedmatrix.h"

#include <QtTest>

#include <generatedmatrix.h>
#include <datasource.h>
#include <math_kst.h>
#include <datacollection.h>
#include <dataobjectcollection.h>
#include <objectstore.h>

#include <QXmlStreamAttributes>

static Kst::ObjectStore _store;

void TestGeneratedMatrix::cleanupTestCase() {
  _store.clear();
}


void TestGeneratedMatrix::testGeneratedMatrix() {
  bool ok = true;

  //basic default constructor values
  Kst::GeneratedMatrixPtr m1 = Kst::kst_cast<Kst::GeneratedMatrix>(_store.createObject<Kst::GeneratedMatrix>(Kst::ObjectTag::fromString(QString::null)));
  QVERIFY(m1->tag().tagString().startsWith("Anonymous"));
  QCOMPARE(m1->sampleCount(), 1);
  QCOMPARE(m1->minValue(), 0.0);
  QCOMPARE(m1->maxValue(), 0.0);
  QCOMPARE(m1->value(0, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(m1->value(10, 10, &ok), 0.0); //should be outside the boundaries.
  QVERIFY(!ok);
  QCOMPARE(m1->meanValue(), 0.0);

  m1->change(10, 10, 0, 0, 1, 1, 0, 100, 0);

  m1->writeLock();
  m1->update(0);
  m1->unlock();

  QCOMPARE(m1->sampleCount(), 100);
  QCOMPARE(m1->minValue(), 0.0);
  QCOMPARE(m1->maxValue(), 100.0);
  QCOMPARE(m1->value(0, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(m1->value(0, 10, &ok), 100.0);
  QVERIFY(ok);
}

// vim: ts=2 sw=2 et
