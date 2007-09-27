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

#include "testamatrix.h"

#include <QtTest>

#include <kstdatacollection.h>
#include <kstdataobjectcollection.h>
#include <kstamatrix.h>


void TestAMatrix::cleanupTestCase() {
  KST::matrixList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}


QDomDocument TestAMatrix::makeDOMElement(const QString& tag, const int nx, const int ny, const double xmin, const double ymin, const double xstep, const double ystep, const int dataSize) {
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

void TestAMatrix::testAMatrix() {

  bool ok = true;
  
  QDomNode n = makeDOMElement("amDOM", 0, 0, 0, 0, 1, 1, 9).firstChild();
  QDomElement e = n.toElement();

  //basic default constructor values
  KstAMatrix* am1 = new KstAMatrix(e);
  QVERIFY(am1->tagName().startsWith("amDOM"));
  QCOMPARE(am1->sampleCount(), 0);
  QCOMPARE(am1->minValue(), 0.0);
  QCOMPARE(am1->maxValue(), 0.0);
  QCOMPARE(am1->value(0, 0, &ok), 0.0);
  QVERIFY(!ok);
  QCOMPARE(am1->value(10, 10, &ok), 0.0); //should be outside the boundaries.
  QVERIFY(!ok);
  QCOMPARE(am1->sampleCount(), 0);
  QCOMPARE(am1->meanValue(), 0.0);

  //basic symetrical matrix
  n = makeDOMElement("Symetrical", 3, 3, 0, 0, 1, 1, 9).firstChild();
  e = n.toElement();

  //basic default constructor values
  KstAMatrix* am2 = new KstAMatrix(e);
  
  QCOMPARE(am2->tagName(), QLatin1String("Symetrical"));
  QVERIFY(am2->resize(3, 3, true));

  for(int i =0 ; i < 3; i++){
    for(int j = 0; j < 3; j++){
      QCOMPARE(am2->value(i, j, &ok), 1.1);
      QVERIFY(ok);
    }
  }

  QVERIFY(am2->editable());
  QCOMPARE(am2->xNumSteps(), 3);
  QCOMPARE(am2->yNumSteps(), 3);
  QCOMPARE(am2->minX(), 0.0);
  QCOMPARE(am2->minY(), 0.0);
  QCOMPARE(am2->xStepSize(), 1.0);
  QCOMPARE(am2->yStepSize(), 1.0);
  QCOMPARE(am2->sampleCount(), 9);

  QVERIFY(am2->setValueRaw(1, 1, 5));
  ok = true;
  QCOMPARE(am2->value(1, 1, &ok), 5.0);
  QVERIFY(ok);

  am2->blank();

  am2->change(KstObjectTag::fromString(am2->tagName()), 3, 3, 0, 0, 0, 0); //should not be legal
  QCOMPARE(am2->xNumSteps(), 3);
  QCOMPARE(am2->yNumSteps(), 3);
  QCOMPARE(am2->minX(), 0.0);
  QCOMPARE(am2->minY(), 0.0);
  QCOMPARE(am2->xStepSize(), 0.0);
  QCOMPARE(am2->yStepSize(), 0.0);
  QCOMPARE(am2->sampleCount(), 9);

  QVERIFY(!am2->setValue(0, 0, 1));
  ok = true;
  QCOMPARE(am2->value(0, 0, &ok), 0.0);
  QVERIFY(!ok);

  QVERIFY(!am2->setValue(1, 1, 5.0));
  QVERIFY(am2->value(1, 1) != 5.0);
  QVERIFY(am2->setValueRaw(2, 2, 6.0)); //fails

  KstAMatrix* um1 = new KstAMatrix(KstObjectTag::fromString("Unity"), 3, 3, 0.0, 0.0, 1.0, 1.0);
  um1->setEditable(true);
  QVERIFY(um1->setValue(0, 0, 1));
  QVERIFY(um1->setValue(1, 1, 1));
  QVERIFY(um1->setValue(2, 2, 1));

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
  QCOMPARE(um1->value(2, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 2, &ok), 1.0);
  QVERIFY(ok);

  QVERIFY(um1->resize(3, 3, false));
  um1->zero();
  QCOMPARE(um1->value(0, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(0, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(0, 2, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 2, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 2, &ok), 0.0);
  QVERIFY(ok);
  
  QVERIFY(um1->setValue(0, 0, 1));
  QVERIFY(um1->setValue(1, 1, 1));
  QVERIFY(um1->setValue(2, 2, 1));

  QVERIFY(um1->resize(2, 2, false));
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
  QCOMPARE(um1->value(0, 3, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 1, &ok), 1.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 2, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(1, 3, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 2, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(2, 3, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(3, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(3, 1, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(3, 2, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(um1->value(3, 3, &ok), 0.0);
  QVERIFY(ok);



  QVERIFY(um1->resize(3, 3, false));
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

  KstAMatrix* sm = new KstAMatrix(KstObjectTag::fromString("Spike"), 2, 2, 0.0, 0.0, 1.0, 1.0);
  
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
