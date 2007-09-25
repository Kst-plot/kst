/*
 *  Copyright 2005, The University of Toronto
 *  Licensed under GPL.
 */

#include "ksttestcase.h"
// #include <kstdataobjectcollection.h>
#include <kstsmatrix.h>



static void exitHelper() {
  KST::matrixList.clear();
  KST::scalarList.clear();
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

QDomDocument makeDOMElem(const QString& tag, const int nx, const int ny, const double xmin, const double ymin, const double xstep, const double ystep, const double gradzmin, const double gradzmax, const bool xdirection) {
// Should be some boundary checking in the constructor.
  QDomDocument smDOM("smdocument");
  QDomElement smElement, child, dataset;
  QDomText text;

  smElement = smDOM.createElement("smDOMTest");

  child = smDOM.createElement("tag");
  text = smDOM.createTextNode(tag);
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("nx");
  text = smDOM.createTextNode(QString::number(nx));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("ny");
  text = smDOM.createTextNode(QString::number(ny));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("xmin");
  text = smDOM.createTextNode(QString::number(xmin));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("ymin");
  text = smDOM.createTextNode(QString::number(ymin));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("xstep");
  text = smDOM.createTextNode(QString::number(xstep));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("ystep");
  text = smDOM.createTextNode(QString::number(ystep));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("gradzmin");
  text = smDOM.createTextNode(QString::number(gradzmin));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("gradzmax");
  text = smDOM.createTextNode(QString::number(gradzmax));
  child.appendChild(text);
  smElement.appendChild(child);

  child = smDOM.createElement("xdirection");
  text = smDOM.createTextNode(QString::number(xdirection));
  child.appendChild(text);
  smElement.appendChild(child);

  smDOM.appendChild(smElement);

  return smDOM;
}

void doTests() {
  bool ok = true;
  
  QDomNode n = makeDOMElem("smDOM", 0, 0, 0, 0, 1, 1, 1, 1, true).firstChild();
  QDomElement e = n.toElement();

  //basic default constructor values
  KstSMatrix* sm1 = new KstSMatrix(e);
  doTest(sm1->tagName().startsWith("smDOM"));
  doTest(sm1->sampleCount() == 1);
  doTest(sm1->minValue() == 0);
  doTest(sm1->maxValue() == 0);
  sm1->value(0, 0, &ok); // undefined value
  doTest(ok);
  doTest(sm1->value(10, 10, &ok) == 0); //should be outside the boundaries.
  doTest(!ok);
  doTest(sm1->sampleCount() == 1);
  // meaningless doTest(sm1->meanValue() == 0);

  //basic symetrical matrix
  n = makeDOMElem("Symetrical", 3, 3, 0, 0, 1, 1, 1, 1, true).firstChild();
  e = n.toElement();

  //basic default constructor values
  KstSMatrix* sm2 = new KstSMatrix(e);
  
  doTest(sm2->tagName() == "Symetrical");
  doTest(sm2->resize(3, 3, true));

  doTest(sm2->editable());
  doTest(sm2->xNumSteps() == 3);
  doTest(sm2->yNumSteps() == 3);
  doTest(sm2->minX() == 0);
  doTest(sm2->minY() == 0);
  doTest(sm2->xStepSize() == 1);
  doTest(sm2->yStepSize() == 1);
  doTest(sm2->sampleCount() == 9);

  doTest(sm2->setValueRaw(1, 1, 5));
  ok = true;
  doTest(sm2->value(1, 1, &ok) == 5);
  doTest(ok);

  sm2->blank();

  sm2->change(KstObjectTag::fromString(sm2->tagName()), 3, 3, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false); //should not be legal
  doTest(sm2->xNumSteps() == 3);
  doTest(sm2->yNumSteps() == 3);
  doTest(sm2->minX() == 0);
  doTest(sm2->minY() == 0);
  // meaningless
  //doTest(sm2->xStepSize() == 0);
  //doTest(sm2->yStepSize() == 0);
  doTest(sm2->sampleCount() == 9);

  doTest(sm2->setValue(0, 0, 1.0));
  ok = true;
  doTest(sm2->value(0, 0, &ok) == 1.0);
  doTest(ok);

  doTest(!sm2->setValue(1, 1, 5.0));
  doTest(sm2->value(1, 1) != 5.0);
  doTest(sm2->setValueRaw(2, 2, 6.0)); //fails

  KstSMatrix* um1 = new KstSMatrix(KstObjectTag::fromString("Unity"), 3, 3, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, false);

  um1->setEditable(true);
  doTest(um1->resize(3, 3, false));
  doTest(um1->setValueRaw(0, 0, 1.0));
  doTest(um1->setValueRaw(1, 1, 1.0));
  doTest(um1->setValueRaw(2, 2, 1.0));
  doTest(um1->value(0, 0, &ok) == 1.0);
  doTest(um1->value(1, 1, &ok) == 1.0);
  doTest(um1->value(2, 2, &ok) == 1.0);
  doTest(ok);

  doTest(um1->setValue(0, 0, 1.716299));
  doTest(um1->setValue(0, 1, -0.485527));
  doTest(um1->setValue(0, 2, -0.288690));
  doTest(um1->setValue(1, 0, 1.716299));
  doTest(um1->setValue(1, 1, NAN));
  doTest(um1->setValue(1, 2, -0.274957));
  doTest(um1->setValue(2, 0, 1.711721));
  doTest(um1->setValue(2, 1, -0.485527));
  doTest(um1->setValue(2, 2, -0.293267));

  doTest(um1->minValue() == 0);
  doTest(um1->maxValue() == 0);

  um1->calcNoSpikeRange(1.9); //this wil produce no change because the variable _NS has no default
  doTest(um1->maxValueNoSpike() == 0.0);
  doTest(um1->minValueNoSpike() == 0.0);


//   KTempFile tf(locateLocal("tmp", "kst-smatrix"), "xml");
//   QFile *qf = tf.file();
//   QTextStream ts(qf);
//   sm2->save(ts, " ");
//   tf.close();
//   QFile::remove(tf.name());

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
