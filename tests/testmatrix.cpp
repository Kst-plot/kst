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

#include "testmatrix.h"

#include <QtTest>

#include <kstmath.h>
#include <kstdatacollection.h>
#include <kstdataobjectcollection.h>

void TestMatrix::cleanupTestCase() {
  KST::matrixList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}


void TestMatrix::testMatrix() {
  bool ok = true;

  //basic default constructor values
  KstMatrixPtr m1 = new KstMatrix(KstObjectTag::fromString(QString::null));
  QVERIFY(m1->tagName().startsWith("Anonymous"));
  QCOMPARE(m1->sampleCount(), 0);
  QCOMPARE(m1->minValue(), 0.0);
  QCOMPARE(m1->maxValue(), 0.0);
  QCOMPARE(m1->value(0, 0, &ok), 0.0);
  QVERIFY(!ok);
  QCOMPARE(m1->value(10, 10, &ok), 0.0); //should be outside the boundaries.
  QVERIFY(!ok);
  QCOMPARE(m1->sampleCount(), 0);
  QCOMPARE(m1->meanValue(), 0.0);

  //basic symetrical matrix
  KstMatrixPtr m2 = new KstMatrix(KstObjectTag::fromString("Symetrical"), 0L, 3, 3);
  QCOMPARE(m2->tagName(),  QLatin1String("Symetrical"));

  QVERIFY(m2->resize(3, 3, true));

  QVERIFY(!m2->editable());
  m2->setEditable(true);

  QVERIFY(m2->editable());

  QCOMPARE(m2->xNumSteps(), 3);
  QCOMPARE(m2->yNumSteps(), 3);
  QCOMPARE(m2->minX(), 0.0);
  QCOMPARE(m2->minY(), 0.0);
  QCOMPARE(m2->xStepSize(), 1.0);
  QCOMPARE(m2->yStepSize(), 1.0);
  QCOMPARE(m2->sampleCount(), 9);

  QVERIFY(m2->setValueRaw(1, 1, 5));
  ok = true;
  QCOMPARE(m2->value(1, 1, &ok), 5.0);
  QVERIFY(ok);

  m2->blank();

  m2->change(KstObjectTag::fromString(m2->tagName()), 3, 3, 0, 0, 0, 0); //should not be legal
  QCOMPARE(m2->xNumSteps(), 3);
  QCOMPARE(m2->yNumSteps(), 3);
  QCOMPARE(m2->minX(), 0.0);
  QCOMPARE(m2->minY(), 0.0);
  QCOMPARE(m2->xStepSize(), 0.0);
  QCOMPARE(m2->yStepSize(), 0.0);
  QCOMPARE(m2->sampleCount(), 9);

  QVERIFY(!m2->setValue(0, 0, 1.0));
  ok = true;
  QCOMPARE(m2->value(0, 0, &ok), 0.0);
  QVERIFY(!ok);

  QVERIFY(!m2->setValue(1, 1, 5.0));
  QVERIFY(m2->value(1, 1) != 5.0);
  QVERIFY(m2->setValueRaw(2, 2, 6.0)); //fails

  KstMatrixPtr um1 = new KstMatrix(KstObjectTag::fromString("Unity"), 0L, 3, 3, 0, 0, 1, 1);
  um1->setEditable(true);
  um1->zero();
  QVERIFY(!um1->setValue(0, 0, 1.0));
  QVERIFY(!um1->setValue(1, 1, 1.0));
  QVERIFY(!um1->setValue(2, 2, 1.0));

  QVERIFY(um1->resize(3, 3, false));
  um1->zero();
  QVERIFY(um1->setValue(0, 0, 1.0));
  QVERIFY(um1->setValue(1, 1, 1.0));
  QVERIFY(um1->setValue(2, 2, 1.0));

  QVERIFY(um1->resize(2, 2, true));
  QCOMPARE(um1->sampleCount(), 4);

  QCOMPARE(um1->value(0, 0, &ok), 1.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(0, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(0, 2, &ok), 0.0);
  QVERIFY(!ok);
  QCOMPARE(um1->value(1, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 1, &ok), 1.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 2, &ok), 0.0);
  QVERIFY(!ok);

  QVERIFY(um1->resize(4, 4, false));
  QCOMPARE(um1->value(0, 0, &ok), 1.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(0, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(0, 2, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 1, &ok), 1.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 2, &ok), 0.0);
  QVERIFY(ok);

  QVERIFY(um1->resize(3, 3, true));
  QVERIFY(um1->setValue(0, 0, 1.716299));
  QVERIFY(um1->setValue(0, 1, -0.485527));
  QVERIFY(um1->setValue(0, 2, -0.288690));
  QVERIFY(um1->setValue(1, 0, 1.716299));
  QVERIFY(um1->setValue(1, 1, NAN));
  QVERIFY(um1->setValue(1, 2, -0.274957));
  QVERIFY(um1->setValue(2, 0, 1.711721));
  QVERIFY(um1->setValue(2, 1, -0.485527));
  QVERIFY(um1->setValue(2, 2, -0.293267));

  QCOMPARE(um1->value(0, 0), 1.716299);
  QCOMPARE(um1->value(0, 1),  -0.485527);
  QCOMPARE(um1->value(0, 2), -0.288690);
  QCOMPARE(um1->value(1, 0), 1.716299);
  QCOMPARE(um1->value(1, 1), 0.0);
  QCOMPARE(um1->value(1, 2), -0.274957);
  QCOMPARE(um1->value(2, 0), 1.711721);
  QCOMPARE(um1->value(2, 1), -0.485527);
  QCOMPARE(um1->value(2, 2), -0.293267);

  QCOMPARE(um1->minValue(), 0.0);
  QCOMPARE(um1->maxValue(), 0.0);

  KstMatrixPtr sm = new KstMatrix(KstObjectTag::fromString("Spike"), 0L, 2, 2, 0, 0, 1, 1);

  sm->setEditable(true);
  QVERIFY(sm->resize(2, 2, false));
  QCOMPARE(sm->xNumSteps(), 2);
  QCOMPARE(sm->yNumSteps(), 2);

  QVERIFY(sm->setValueRaw(0, 0, 0.0));
  QVERIFY(sm->setValueRaw(0, 1, 0.1));
  QVERIFY(sm->setValueRaw(1, 0, 1.0));
  QVERIFY(sm->setValueRaw(1, 1, 1.1));

  sm->calcNoSpikeRange(0);
  QCOMPARE(sm->minValueNoSpike(), 0.0);
  QCOMPARE(sm->maxValueNoSpike(), 0.0);

  sm->calcNoSpikeRange(-100);
  QCOMPARE(sm->minValueNoSpike(), 0.0);
  QCOMPARE(sm->maxValueNoSpike(), 0.0);

  sm->calcNoSpikeRange(0.9);
  QVERIFY(sm->minValueNoSpike() >= 1E+300 );
  QVERIFY(sm->maxValueNoSpike() <= -1E+300);
}

// vim: ts=2 sw=2 et
