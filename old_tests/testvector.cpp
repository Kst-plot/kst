/*
 *  Copyright 2005, The University of Toronto
 *  Licensed under GPL.
 */

#include "ksttestcase.h"
#include <kstdataobjectcollection.h>

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
    

void doTests() {
  KstVectorPtr v1 = new KstVector(KstObjectTag::fromString("V1"), 15);
  doTest(v1->tagName() == "V1");
  doTest(v1->length() == 15);
  v1->zero();
  for (int i = 0; i < 15; ++i) {
    doTest(v1->value()[i] == 0);
  }
  doTest(!v1->isScalarList());
  v1->value()[0] = -42;
  v1->resize(3);
  doTest(v1->length() == 3);
  doTest(v1->value()[0] == -42);

  KstVectorPtr v2 = new KstVector(KstObjectTag::fromString(QString::null), 0);
  doTest(v2->length() == 1);
  doTest(v2->tagName().startsWith("Anonymous")); // valid only in en_*
  v2 = new KstVector(KstObjectTag::fromString("V2"), 1);
  doTest(v2->length() == 1);
  v2 = new KstVector(KstObjectTag::fromString("V2"), 2);
  doTest(v2->length() == 2);

  v2->resize(3);
  double *data = v2->value();
  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  doTest(v2->interpolate(0, 5) == 1);
  doTest(v2->interpolate(1, 5) == 1.5);
  doTest(v2->interpolate(2, 5) == 2);
  doTest(v2->interpolate(3, 5) == 2.5);
  doTest(v2->interpolate(4, 5) == 3);
}


int main(int argc, char **argv) {
  atexit(exitHelper);

  QCoreApplication app(argc, argv);

  doTests();
  // Don't put tests in main because we need to ensure that no KstObjects
  // remain past the exit handler

  exitHelper(); // need to run it here before kapp goes away in some cases.
  if (rc == KstTestSuccess) {
    printf("All tests passed!\n");
  }
  return -rc;
}

// vim: ts=2 sw=2 et
