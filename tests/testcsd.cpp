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

#include "testcsd.h"

#include <QtTest>

#include <kstmath.h>
#include <kstobject.h>
#include <qdir.h>
#include <qfile.h>
#include <kstcsd.h>
#include <datacollection.h>
#include <kstdataobjectcollection.h>


void TestCSD::cleanupTestCase() {
  Kst::vectorList.clear();
  Kst::scalarList.clear();
  KST::dataObjectList.clear();
}

QDomDocument TestCSD::makeDOMElement(const QString& tag, const QString& val) {
// Should be some boundary checking in the constructor.
  QDomDocument csdDOM("csddocument");
  QDomElement csdElement, child;
  QDomText text;

  csdElement = csdDOM.createElement("csdDOMTest");

  child = csdDOM.createElement("tag");
  text = csdDOM.createTextNode(tag);
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("vectag");
  text = csdDOM.createTextNode(val);
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("sampRate");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("average");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("fiftLen");
  text = csdDOM.createTextNode("5");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("adopize");
  text = csdDOM.createTextNode("0");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("apodizefxn");
  text = csdDOM.createTextNode("WindowOriginal");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("gaussiansigma");
  text = csdDOM.createTextNode("0.01");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("removeMean");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("windowsize");
  text = csdDOM.createTextNode("5000");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("vectorunits");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("rateunits");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("output");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  csdDOM.appendChild(csdElement);

  return csdDOM;
}

void TestCSD::testCSD() {

  Kst::VectorPtr vp = new Kst::Vector(KstObjectTag::fromString("tempVector"), 10);
  for (int i = 0; i < 10; i++){
    vp->value()[i] = i;
  }

  KstCSDPtr csd = new KstCSD("csdTest", vp, 0.0, false, false, false, WindowUndefined, 0, 0, 0.0, PSDUndefined, QString::null, QString::null);
  QCOMPARE(csd->tagName(), QLatin1String("csdTest"));
  QCOMPARE(csd->vTag(), QLatin1String("tempVector"));
  QCOMPARE(csd->output(), PSDUndefined);
  QVERIFY(!csd->apodize());
  QVERIFY(!csd->removeMean());
  QVERIFY(!csd->average());
  QCOMPARE(csd->freq(), 1.0);
  QCOMPARE(csd->apodizeFxn(), WindowUndefined);
  QCOMPARE(csd->length(), 0);
  QCOMPARE(csd->windowSize(), 0);
  QCOMPARE(csd->gaussianSigma(), 0.0);
  QVERIFY(csd->vectorUnits().isEmpty());
  QVERIFY(csd->rateUnits().isEmpty());

  csd->setOutput(PSDAmplitudeSpectralDensity);
  csd->setApodize(true);
  csd->setRemoveMean(true);
  csd->setAverage(true);
  csd->setFreq(0.1);
  csd->setApodizeFxn(WindowOriginal);
  csd->setLength(3);
  csd->setWindowSize(50);
  csd->setGaussianSigma(0.2);

  QCOMPARE(csd->tagName(), QLatin1String("csdTest"));
  QCOMPARE(csd->vTag(), QLatin1String("tempVector"));
  QCOMPARE(csd->output(), PSDAmplitudeSpectralDensity);
  QVERIFY(csd->apodize());
  QVERIFY(csd->removeMean());
  QVERIFY(csd->average());
  QCOMPARE(csd->freq(), 0.1);
  QCOMPARE(csd->windowSize(), 50);
  QCOMPARE(csd->apodizeFxn(), WindowOriginal);
  QCOMPARE(csd->gaussianSigma(), 0.2);

//   KTempFile tf(locateLocal("tmp", "kst-csd"), "txt");
//   QFile *qf = tf.file();
//   QTextStream ts(qf);
//   csd->save(ts, "");

  QDomNode n = makeDOMElement("csdDOMCsd", "csdDOMVector").firstChild();
  QDomElement e = n.toElement();
  KstCSDPtr csdDOM = new KstCSD(e);

  QCOMPARE(csdDOM->tagName(), QLatin1String("csdDOMCsd"));
  QCOMPARE(csdDOM->output(), PSDPowerSpectralDensity);
  QVERIFY(csdDOM->apodize());
  QVERIFY(csdDOM->removeMean());
  QVERIFY(csdDOM->average());
  QCOMPARE(csdDOM->freq(), 1.0);
  QCOMPARE(csdDOM->apodizeFxn(), WindowOriginal);
  QCOMPARE(csdDOM->gaussianSigma(), 0.01);
  QCOMPARE(csdDOM->windowSize(), 5000);

  Kst::VectorPtr vp2 = new Kst::Vector(KstObjectTag::fromString("tempVector2"), 10);
  for (int i = 0; i < 10; i++){
    vp2->value()[i] = i;
  }
  csdDOM->setVector(vp2);
  QCOMPARE(csdDOM->vTag(), QLatin1String("tempVector2"));
  csdDOM->setWindowSize(9);
  KstMatrixPtr outMatrix = csdDOM->outputMatrix();
  
  QVERIFY(outMatrix->resize(3, 3, false)); // very odd thing to do?
  QVERIFY(outMatrix->setValue(0, 0, 1.716299));
  QVERIFY(outMatrix->setValue(0, 1, -0.485527));
  QVERIFY(outMatrix->setValue(0, 2, -0.288690));
  QVERIFY(outMatrix->setValue(1, 0, 1.716299));
  QVERIFY(outMatrix->setValue(1, 1, NAN));
  QVERIFY(outMatrix->setValue(1, 2, -0.274957));
  QVERIFY(outMatrix->setValue(2, 0, 1.711721));
  QVERIFY(outMatrix->setValue(2, 1, -0.485527));
  QVERIFY(outMatrix->setValue(2, 2, -0.293267));

  QCOMPARE(outMatrix->sampleCount(), 9);
  QCOMPARE(outMatrix->value(0, 0), 1.716299);
  QCOMPARE(outMatrix->value(0, 1),  -0.485527);
  QCOMPARE(outMatrix->value(0, 2), -0.288690);
  QCOMPARE(outMatrix->value(1, 0), 1.716299);
  QCOMPARE(outMatrix->value(1, 1), 0.0);
  QCOMPARE(outMatrix->value(1, 2), -0.274957);
  QCOMPARE(outMatrix->value(2, 0), 1.711721);
  QCOMPARE(outMatrix->value(2, 1), -0.485527);
  QCOMPARE(outMatrix->value(2, 2), -0.293267);

  csdDOM->writeLock();
  QCOMPARE(csdDOM->update(0), KstObject::UPDATE);
  csdDOM->unlock();

  outMatrix = csdDOM->outputMatrix();
  QCOMPARE(outMatrix->sampleCount(), 128);

  csdDOM->setWindowSize(11);
  QCOMPARE(outMatrix->sampleCount(), 128);

  QCOMPARE(outMatrix->value(0, 0), 1.716299);
  QCOMPARE(outMatrix->value(0, 1),  -0.485527);
  QCOMPARE(outMatrix->value(0, 2), -0.288690);
  QCOMPARE(outMatrix->value(1, 0), 1.716299);
  QCOMPARE(outMatrix->value(1, 1), 0.0);
  QCOMPARE(outMatrix->value(1, 2), -0.274957);
  QCOMPARE(outMatrix->value(2, 0), 1.711721);
  QCOMPARE(outMatrix->value(2, 1), -0.485527);
  QCOMPARE(outMatrix->value(2, 2), -0.293267);

}
