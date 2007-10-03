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

#include "testhistogram.h"

#include <QtTest>

#include <vector.h>
#include <ksthistogram.h>
#include <datacollection.h>
#include <kstdataobjectcollection.h>

void TestHistogram::cleanupTestCase() {
  Kst::vectorList.clear();
  Kst::scalarList.clear();
  KST::dataObjectList.clear();
}

void TestHistogram::testHistogram() {
  Kst::VectorPtr vp = Kst::Vector::generateVector(0, 10, 100, KstObjectTag::fromString("V1"));
  KstHistogramPtr h1 = new KstHistogram("H1", vp, 0, 10, 10, KST_HS_NUMBER);
  KST::dataObjectList.append(h1.data());

  QCOMPARE(h1->propertyString(), QLatin1String("Histogram: V1"));
  QVERIFY(!h1->realTimeAutoBin()); // should be false by default
  QCOMPARE(h1->nBins(), 10);

  h1->writeLock();
  h1->update(0);
  h1->unlock();

  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);
  int count = 0;
  for (int i=0; i<10; i++) {
    count += int(h1->vY()->value(i));
  }
  h1->setRealTimeAutoBin(true);
  QVERIFY(h1->realTimeAutoBin());
  //dumpPoints(h1, 10);
  QCOMPARE(count, 100); // should account for the whole vector
  h1->setNBins(11);
  QVERIFY(!h1->realTimeAutoBin());
  QCOMPARE(h1->nBins(), 11);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);

  h1->writeLock();
  h1->update(0);
  h1->unlock();

  count = 0;
  for (int i=0; i<11; i++) {
    count += int(h1->vY()->value(i));
  }
  //dumpPoints(h1, 11);
  QCOMPARE(count, 100); // should still account for the whole vector
  h1->setNBins(9);
  QCOMPARE(h1->nBins(), 9);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);

  h1->writeLock();
  h1->update(0);
  h1->unlock();

  count = 0;
  for (int i=0; i<9; i++) {
    count += int(h1->vY()->value(i));
  }
  //dumpPoints(h1, 9);
  QCOMPARE(count, 100); // should still account for the whole vector
  // min > max
  h1 = new KstHistogram("H2", vp, 10, 0, 10, KST_HS_NUMBER);
  QCOMPARE(h1->nBins(), 10);
  QCOMPARE(h1->xMin(), 0.0);
  QCOMPARE(h1->xMax(), 10.0);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);
  // min == max
  h1 = new KstHistogram("H3", vp, 10, 10, 2, KST_HS_NUMBER);
  QCOMPARE(h1->nBins(), 2);
  QCOMPARE(h1->xMin(), 9.0);
  QCOMPARE(h1->xMax(), 11.0);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);
  // max < min
  h1 = new KstHistogram("H4", vp, 11, 9, 1, KST_HS_NUMBER);
  QCOMPARE(h1->nBins(), 2);
  QCOMPARE(h1->xMax(), 11.0);
  QCOMPARE(h1->xMin(), 9.0);
  QCOMPARE(h1->vMin(), 0.0);
  QCOMPARE(h1->vMax(), 10.0);
  QCOMPARE(h1->vNumSamples(), 100);
  // set to max == min
  h1->setXRange(10, 10);
  QCOMPARE(h1->xMin(), 9.0);
  QCOMPARE(h1->xMax(), 11.0);
  // set to max > min
  h1->setXRange(1,2);
  QCOMPARE(h1->xMax() - h1->xMin(), 1.0);
  h1->setXRange(8, 10);
  QCOMPARE(h1->xMin(), 8.0);
  QCOMPARE(h1->xMax(), 10.0);
}

// vim: ts=2 sw=2 et
