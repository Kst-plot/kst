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

#include "testvector.h"

#include <QtTest>

#include <vector.h>
#include <datacollection.h>
#include <kstdataobjectcollection.h>

void TestVector::cleanupTestCase() {
  Kst::vectorList.clear();
  Kst::scalarList.clear();
  KST::dataObjectList.clear();
}

void TestVector::testVector() {
  Kst::VectorPtr v1 = new Kst::Vector(Kst::ObjectTag::fromString("V1"), 15);
  QCOMPARE(v1->tagName(), QLatin1String("V1"));
  QCOMPARE(v1->length(), 15);
  v1->zero();
  for (int i = 0; i < 15; ++i) {
    QCOMPARE(v1->value()[i], 0.0);
  }
  QVERIFY(!v1->isScalarList());
  v1->value()[0] = -42;
  v1->resize(3);
  QCOMPARE(v1->length(), 3);
  QCOMPARE(v1->value()[0], -42.0);

  Kst::VectorPtr v2 = new Kst::Vector(Kst::ObjectTag::fromString(QString::null), 0);
  QCOMPARE(v2->length(), 1);
  QVERIFY(v2->tagName().startsWith("Anonymous")); // valid only in en_*
  v2 = new Kst::Vector(Kst::ObjectTag::fromString("V2"), 1);
  QCOMPARE(v2->length(), 1);
  v2 = new Kst::Vector(Kst::ObjectTag::fromString("V2"), 2);
  QCOMPARE(v2->length(), 2);

  v2->resize(3);
  double *data = v2->value();
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  QCOMPARE(v2->interpolate(0, 5), 1.0);
  QCOMPARE(v2->interpolate(1, 5), 1.5);
  QCOMPARE(v2->interpolate(2, 5), 2.0);
  QCOMPARE(v2->interpolate(3, 5), 2.5);
  QCOMPARE(v2->interpolate(4, 5), 3.0);
}

// vim: ts=2 sw=2 et
