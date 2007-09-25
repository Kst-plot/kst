/*
 *  Copyright 2005, The University of Toronto
 *  Licensed under GPL.
 */

#include "ksttestcase.h"
#include <kstdataobjectcollection.h>
#include <kstamatrix.h>
#include <kstandarddirs.h>


static void exitHelper() {
  KST::matrixList.clear();
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

QDomDocument makeDOMElement(const QString& tag, const int nx, const int ny, const double xmin, const double ymin, const double xstep, const double ystep, const int dataSize) {
// Should be some boundary checking in the constructor.
  QDomDocument amDOM("amdocument");
  QDomElement amElement, child, dataset;
  QDomText text;

  amElement = amDOM.createElement("amDOMTest");

  child = amDOM.createElement("tag");
  text = amDOM.createTextNode(tag);
  child.appendChild(text);
  amElement.appendChild(child);

  child = amDOM.createElement("nx");
  text = amDOM.createTextNode(QString::number(nx));
  child.appendChild(text);
  amElement.appendChild(child);

  child = amDOM.createElement("ny");
  text = amDOM.createTextNode(QString::number(ny));
  child.appendChild(text);
  amElement.appendChild(child);

  child = amDOM.createElement("xmin");
  text = amDOM.createTextNode(QString::number(xmin));
  child.appendChild(text);
  amElement.appendChild(child);

  child = amDOM.createElement("ymin");
  text = amDOM.createTextNode(QString::number(ymin));
  child.appendChild(text);
  amElement.appendChild(child);

  child = amDOM.createElement("xstep");
  text = amDOM.createTextNode(QString::number(xstep));
  child.appendChild(text);
  amElement.appendChild(child);

  child = amDOM.createElement("ystep");
  text = amDOM.createTextNode(QString::number(ystep));
  child.appendChild(text);
  amElement.appendChild(child);


  child = amDOM.createElement("data");
  QByteArray qba;
  qba.reserve(dataSize*sizeof(double));
  QDataStream qds(&qba, QIODevice::WriteOnly);

  for (int i = 0; i < dataSize; i++) {
    qds << 1.1;
  }
  
  text = amDOM.createTextNode(QString(qCompress(qba).toBase64()));

  child.appendChild(text);
  amElement.appendChild(child);

  amDOM.appendChild(amElement);

  return amDOM;
}

void doTests() {
  bool ok = true;
  
  QDomNode n = makeDOMElement("amDOM", 0, 0, 0, 0, 1, 1, 9).firstChild();
  QDomElement e = n.toElement();

  //basic default constructor values
  KstAMatrix* am1 = new KstAMatrix(e);
  doTest(am1->tagName().startsWith("amDOM"));
  doTest(am1->sampleCount() == 0);
  doTest(am1->minValue() == 0);
  doTest(am1->maxValue() == 0);
  doTest(am1->value(0, 0, &ok) == 0);
  doTest(!ok);
  doTest(am1->value(10, 10, &ok) == 0); //should be outside the boundaries.
  doTest(!ok);
  doTest(am1->sampleCount() == 0);
  doTest(am1->meanValue() == 0);

  //basic symetrical matrix
  n = makeDOMElement("Symetrical", 3, 3, 0, 0, 1, 1, 9).firstChild();
  e = n.toElement();

  //basic default constructor values
  KstAMatrix* am2 = new KstAMatrix(e);
  
  doTest(am2->tagName() == "Symetrical");
  doTest(am2->resize(3, 3, true));

  for(int i =0 ; i < 3; i++){
    for(int j = 0; j < 3; j++){
      doTest(am2->value(i, j, &ok) == 1.1);
      doTest(ok);
    }
  }

  doTest(am2->editable());
  doTest(am2->xNumSteps() == 3);
  doTest(am2->yNumSteps() == 3);
  doTest(am2->minX() == 0);
  doTest(am2->minY() == 0);
  doTest(am2->xStepSize() == 1);
  doTest(am2->yStepSize() == 1);
  doTest(am2->sampleCount() == 9);

  doTest(am2->setValueRaw(1, 1, 5));
  ok = true;
  doTest(am2->value(1, 1, &ok) == 5);
  doTest(ok);

  am2->blank();

  am2->change(KstObjectTag::fromString(am2->tagName()), 3, 3, 0, 0, 0, 0); //should not be legal
  doTest(am2->xNumSteps() == 3);
  doTest(am2->yNumSteps() == 3);
  doTest(am2->minX() == 0);
  doTest(am2->minY() == 0);
  doTest(am2->xStepSize() == 0);
  doTest(am2->yStepSize() == 0);
  doTest(am2->sampleCount() == 9);

  doTest(!am2->setValue(0, 0, 1));
  ok = true;
  doTest(am2->value(0, 0, &ok) == 0);
  doTest(!ok);

  doTest(!am2->setValue(1, 1, 5.0));
  doTest(am2->value(1, 1) != 5.0);
  doTest(am2->setValueRaw(2, 2, 6.0)); //fails

  KstAMatrix* um1 = new KstAMatrix(KstObjectTag::fromString("Unity"), 3, 3, 0.0, 0.0, 1.0, 1.0);
  um1->setEditable(true);
  doTest(um1->setValue(0, 0, 1));
  doTest(um1->setValue(1, 1, 1));
  doTest(um1->setValue(2, 2, 1));

  doTest(um1->value(0, 0, &ok) == 1);
  doTest(ok);
  doTest(um1->value(0, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(0, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 1, &ok) == 1);
  doTest(ok);
  doTest(um1->value(1, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 2, &ok) == 1);
  doTest(ok);

  doTest(um1->resize(3, 3, false));
  um1->zero();
  doTest(um1->value(0, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(0, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(0, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 2, &ok) == 0);
  doTest(ok);
  
  doTest(um1->setValue(0, 0, 1));
  doTest(um1->setValue(1, 1, 1));
  doTest(um1->setValue(2, 2, 1));

  doTest(um1->resize(2, 2, false));
  doTest(um1->sampleCount() == 4); 

  doTest(um1->value(0, 0, &ok) == 1);
  doTest(ok);
  doTest(um1->value(0, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(0, 2, &ok) == 0);
  doTest(!ok);
  doTest(um1->value(1, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 1, &ok) == 1);
  doTest(ok);
  doTest(um1->value(1, 2, &ok) == 0);
  doTest(!ok);

  doTest(um1->resize(4, 4, false));
  doTest(um1->value(0, 0, &ok) == 1);
  doTest(ok);
  doTest(um1->value(0, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(0, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(0, 3, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 1, &ok) == 1);
  doTest(ok);
  doTest(um1->value(1, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(1, 3, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(2, 3, &ok) == 0);
  doTest(ok);
  doTest(um1->value(3, 0, &ok) == 0);
  doTest(ok);
  doTest(um1->value(3, 1, &ok) == 0);
  doTest(ok);
  doTest(um1->value(3, 2, &ok) == 0);
  doTest(ok);
  doTest(um1->value(3, 3, &ok) == 0);
  doTest(ok);



  doTest(um1->resize(3, 3, false));
  doTest(um1->setValue(0, 0, 1.716299));
  doTest(um1->setValue(0, 1, -0.485527));
  doTest(um1->setValue(0, 2, -0.288690));
  doTest(um1->setValue(1, 0, 1.716299));
  doTest(um1->setValue(1, 1, NAN));
  doTest(um1->setValue(1, 2, -0.274957));
  doTest(um1->setValue(2, 0, 1.711721));
  doTest(um1->setValue(2, 1, -0.485527));
  doTest(um1->setValue(2, 2, -0.293267));

  doTest(um1->value(0, 0) == 1.716299);
  doTest(um1->value(0, 1) ==  -0.485527);
  doTest(um1->value(0, 2) == -0.288690);
  doTest(um1->value(1, 0) == 1.716299);
  doTest(um1->value(1, 1) == 0);
  doTest(um1->value(1, 2) == -0.274957);
  doTest(um1->value(2, 0) == 1.711721);
  doTest(um1->value(2, 1) == -0.485527);
  doTest(um1->value(2, 2) == -0.293267);

  doTest(um1->minValue() == 0);
  doTest(um1->maxValue() == 0);

  KstAMatrix* sm = new KstAMatrix(KstObjectTag::fromString("Spike"), 2, 2, 0.0, 0.0, 1.0, 1.0);
  
  sm->setEditable(true);
  doTest(sm->resize(2, 2, false));
  doTest(sm->xNumSteps() == 2);
  doTest(sm->yNumSteps() == 2);

  doTest(sm->setValueRaw(0, 0, 0.0));
  doTest(sm->setValueRaw(0, 1, 0.1));
  doTest(sm->setValueRaw(1, 0, 1.0));
  doTest(sm->setValueRaw(1, 1, 1.1));

  sm->calcNoSpikeRange(0);
  doTest(sm->minValueNoSpike() == 0.0);
  doTest(sm->maxValueNoSpike() == 0.0);

  sm->calcNoSpikeRange(-100);
  doTest(sm->minValueNoSpike() == 0.0);
  doTest(sm->maxValueNoSpike() == 0.0);
  
  sm->calcNoSpikeRange(0.9);
  doTest(sm->minValueNoSpike() >= 1E+300 );
  doTest(sm->maxValueNoSpike() <= -1E+300);
}


int main(int argc, char **argv) {
  atexit(exitHelper);

  QCoreApplication app(argc, argv);

  doTests();
  // Don't put tests in main because we need to ensure that no KstObjects
  // remain past the exit handler

  exitHelper(); // need to run it here before app goes away in some cases.
  if (rc == KstTestSuccess) {
    printf("All tests passed!\n");
  }
  return -rc;
}
