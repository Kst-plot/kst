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

#include <math_kst.h>
#include <object.h>
#include <qdir.h>
#include <qfile.h>
#include <datacollection.h>
#include <dataobjectcollection.h>
#include <objectstore.h>

#define protected public
#include <csd.h>
#undef protected

static Kst::ObjectStore _store;

void TestCSD::cleanupTestCase() {
	_store.clear();
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

  child = csdDOM.createElement("vector");
  text = csdDOM.createTextNode(val);
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("samplerate");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("average");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("fftlength");
  text = csdDOM.createTextNode("5");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("apodize");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("apodizefunction");
  text = csdDOM.createTextNode("WindowOriginal");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("gaussiansigma");
  text = csdDOM.createTextNode("0.01");
  child.appendChild(text);
  csdElement.appendChild(child);

  child = csdDOM.createElement("removemean");
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

  child = csdDOM.createElement("outputtype");
  text = csdDOM.createTextNode("1");
  child.appendChild(text);
  csdElement.appendChild(child);

  csdDOM.appendChild(csdElement);

  return csdDOM;
}

void TestCSD::testCSD() {

  Kst::VectorPtr vp = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>(Kst::ObjectTag::fromString("tempVector")));
  Q_ASSERT(vp);
  vp->resize(10);
  for (int i = 0; i < 10; i++){
    vp->value()[i] = i;
  }

  Kst::CSDPtr csd = new Kst::CSD(&_store, Kst::ObjectTag::fromString("csdTest"), vp, 0.0, false, false, false, WindowUndefined, 0, 0, 0.0, PSDUndefined, QString::null, QString::null);
  QCOMPARE(csd->tag().tagString(), QLatin1String("csdTest"));
  QCOMPARE(csd->vTag(), QLatin1String("tempVector"));
  QCOMPARE(csd->output(), PSDUndefined);
  QVERIFY(!csd->apodize());
  QVERIFY(!csd->removeMean());
  QVERIFY(!csd->average());
  QCOMPARE(csd->frequency(), 1.0);
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
  csd->setFrequency(0.1);
  csd->setApodizeFxn(WindowOriginal);
  csd->setLength(3);
  csd->setWindowSize(50);
  csd->setGaussianSigma(0.2);

  QCOMPARE(csd->tag().tagString(), QLatin1String("csdTest"));
  QCOMPARE(csd->vTag(), QLatin1String("tempVector"));
  QCOMPARE(csd->output(), PSDAmplitudeSpectralDensity);
  QVERIFY(csd->apodize());
  QVERIFY(csd->removeMean());
  QVERIFY(csd->average());
  QCOMPARE(csd->frequency(), 0.1);
  QCOMPARE(csd->windowSize(), 50);
  QCOMPARE(csd->apodizeFxn(), WindowOriginal);
  QCOMPARE(csd->gaussianSigma(), 0.2);

//   KTempFile tf(locateLocal("tmp", "kst-csd"), "txt");
//   QFile *qf = tf.file();
//   QTextStream ts(qf);
//   csd->save(ts, "");

  QDomNode n = makeDOMElement("csdDOMCsd", "csdDOMVector").firstChild();
  QDomElement e = n.toElement();
  Kst::CSDPtr csdDOM = new Kst::CSD(&_store, e);

  QCOMPARE(csdDOM->tag().tagString(), QLatin1String("csdDOMCsd"));
  QCOMPARE(csdDOM->output(), PSDPowerSpectralDensity);
  QVERIFY(csdDOM->apodize());
  QVERIFY(csdDOM->removeMean());
  QVERIFY(csdDOM->average());
  QCOMPARE(csdDOM->frequency(), 1.0);
  QCOMPARE(csdDOM->apodizeFxn(), WindowOriginal);
  QCOMPARE(csdDOM->gaussianSigma(), 0.01);
  QCOMPARE(csdDOM->windowSize(), 5000);

  Kst::VectorPtr vp2 = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>(Kst::ObjectTag::fromString("tempVector2")));
  Q_ASSERT(vp2);
  vp2->resize(10);
  for (int i = 0; i < 10; i++){
    vp2->value()[i] = i;
  }
  csdDOM->setVector(vp2);
  QCOMPARE(csdDOM->vTag(), QLatin1String("tempVector2"));
  csdDOM->setWindowSize(9);
  Kst::MatrixPtr outMatrix = csdDOM->outputMatrix();

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
  QCOMPARE(csdDOM->update(), Kst::Object::UPDATE);
  csdDOM->unlock();

  outMatrix = csdDOM->outputMatrix();
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->sampleCount(), 128);

  csdDOM->setWindowSize(11);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->sampleCount(), 128);

  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(0, 0), 1.716299);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(0, 1),  -0.485527);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(0, 2), -0.288690);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(1, 0), 1.716299);
  QCOMPARE(outMatrix->value(1, 1), 0.0);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(1, 2), -0.274957);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(2, 0), 1.711721);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(2, 1), -0.485527);
  QEXPECT_FAIL("", "This has always failed", Continue);
  QCOMPARE(outMatrix->value(2, 2), -0.293267);

}
