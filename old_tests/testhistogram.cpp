/*
 *  Copyright 2004, The University of Toronto
 *  Licensed under GPL.
 */

#include "ksttestcase.h"
#include <kstdataobjectcollection.h>
#include <ksthistogram.h>
#include <kstvector.h>

static void exitHelper() {
  KST::vectorList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}


int rc = KstTestSuccess;


#define doTest(x) testAssert(x, QString("Line %1").arg(__LINE__))
#define doTestD(x, y) testAssert(x, QString("%1: %2").arg(__LINE__).arg(y))

void testAssert(bool result, const QString& text = "Unknown") {
  if (!result) {
    KstTestFailed();
    printf("Test [%s] failed.\n", text.toLatin1().data());
  }
}

    
#define dumpPoints(histogram, n) do { \
  for (int i = 0; i < n*4; ++i) { \
    printf("%.15f, %.15f\n", histogram->vX()->value(i), histogram->vY()->value(i)); \
  } } while(0)

void doTests() {
  KstVectorPtr vp = KstVector::generateVector(0, 10, 100, KstObjectTag::fromString("V1"));
  KstHistogramPtr h1 = new KstHistogram("H1", vp, 0, 10, 10, KST_HS_NUMBER);
  KST::dataObjectList.append(h1.data());
  doTest(h1->propertyString() == "Histogram: V1");
  doTest(!h1->realTimeAutoBin()); // should be false by default
  doTest(h1->nBins() == 10);
  h1->update(0);
  doTest(h1->vMin() == 0.0);
  doTest(h1->vMax() == 10.0);
  doTest(h1->vNumSamples() == 100);
  int count = 0;
  for (int i=0; i<10; i++) {
    count += int(h1->vY()->value(i));
  }
  h1->setRealTimeAutoBin(true);
  doTest(h1->realTimeAutoBin());
  //dumpPoints(h1, 10);
  doTest(count == 100); // should account for the whole vector
  h1->setNBins(11);
  doTest(!h1->realTimeAutoBin());
  doTest(h1->nBins() == 11);
  doTest(h1->vMin() == 0.0);
  doTest(h1->vMax() == 10.0);
  doTest(h1->vNumSamples() == 100);
  h1->update(0);
  count = 0;
  for (int i=0; i<11; i++) {
    count += int(h1->vY()->value(i));
  }
  //dumpPoints(h1, 11);
  doTest(count == 100); // should still account for the whole vector
  h1->setNBins(9);
  doTest(h1->nBins() == 9);
  doTest(h1->vMin() == 0.0);
  doTest(h1->vMax() == 10.0);
  doTest(h1->vNumSamples() == 100);
  h1->update(0);
  count = 0;
  for (int i=0; i<9; i++) {
    count += int(h1->vY()->value(i));
  }
  //dumpPoints(h1, 9);
  doTest(count == 100); // should still account for the whole vector
  // min > max
  h1 = new KstHistogram("H2", vp, 10, 0, 10, KST_HS_NUMBER);
  doTest(h1->nBins() == 10);
  doTest(h1->xMin() == 0.0);
  doTest(h1->xMax() == 10.0);
  doTest(h1->vMin() == 0.0);
  doTest(h1->vMax() == 10.0);
  doTest(h1->vNumSamples() == 100);
  // min == max
  h1 = new KstHistogram("H3", vp, 10, 10, 2, KST_HS_NUMBER);
  doTest(h1->nBins() == 2);
  doTest(h1->xMin() == 9.0);
  doTest(h1->xMax() == 11.0);
  doTest(h1->vMin() == 0.0);
  doTest(h1->vMax() == 10.0);
  doTest(h1->vNumSamples() == 100);
  // max < min
  h1 = new KstHistogram("H4", vp, 11, 9, 1, KST_HS_NUMBER);
  doTest(h1->nBins() == 2);
  doTest(h1->xMax()==11);
  doTest(h1->xMin()==9);
  doTest(h1->vMin() == 0.0);
  doTest(h1->vMax() == 10.0);
  doTest(h1->vNumSamples() == 100);
  // set to max == min
  h1->setXRange(10, 10);
  doTest(h1->xMin() == 9.0);
  doTest(h1->xMax() == 11.0);
  // set to max > min
  h1->setXRange(1,2);
  doTest(h1->xMax() - h1->xMin() ==1.0);
  h1->setXRange(8, 10);
  doTest(h1->xMin() == 8.0);
  doTest(h1->xMax() == 10.0);
}


int main(int argc, char **argv) {
  atexit(exitHelper);

  QCoreApplication app(argc, argv);

  doTests();

  exitHelper(); // before app dies
  if (rc == KstTestSuccess) {
    printf("All tests passed!\n");
  }
  return -rc;
}

// vim: ts=2 sw=2 et
