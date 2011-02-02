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

#include "testpsd.h"

#include <QtTest>

#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <QXmlStreamWriter>


#include "psd.h"
#include "ksttest.h"

#include "datacollection.h"
#include "objectstore.h"

static Kst::ObjectStore _store;

void TestPSD::cleanupTestCase() {
  _store.clear();
}


QDomDocument TestPSD::makeDOMElement(const QString& tag, const QString& val) {
// Should be some boundary checking in the constructor.
  QDomDocument psdDOM("psddocument");
  QDomElement psdElement, child;
  QDomText text;

  psdElement = psdDOM.createElement("psdDOMTest");

  child = psdDOM.createElement("tag");
  text = psdDOM.createTextNode(tag);
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("vector");
  text = psdDOM.createTextNode(val);
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("samplerate");
  text = psdDOM.createTextNode("128");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("average");
  text = psdDOM.createTextNode("1");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("fftlength");
  text = psdDOM.createTextNode("5");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("apodize");
  text = psdDOM.createTextNode("1");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("apodizefunction");
  text = psdDOM.createTextNode("WindowOriginal");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("gaussiansigma");
  text = psdDOM.createTextNode("0.01");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("removemean");
  text = psdDOM.createTextNode("1");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("vectorunits");
  text = psdDOM.createTextNode("vUnits");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("rateunits");
  text = psdDOM.createTextNode("rUnits");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("outputtype");
  text = psdDOM.createTextNode("PSDAmplitudeSpectralDensity");
  child.appendChild(text);
  psdElement.appendChild(child);

  child = psdDOM.createElement("interpolateholes");
  text = psdDOM.createTextNode("1");
  child.appendChild(text);
  psdElement.appendChild(child);

  psdDOM.appendChild(psdElement);

  return psdDOM;
}

void TestPSD::testPSD() {

  Kst::VectorPtr vp = Kst::kst_cast<Kst::Vector>(_store.createObject<Kst::Vector>());
  Q_ASSERT(vp);
  vp->resize(10);
  vp->setDescriptiveName("tempVector");
  for (int i = 0; i < 10; i++){
    vp->value()[i] = i;
  }

  Kst::PSDPtr psd = Kst::kst_cast<Kst::PSD>(_store.createObject<Kst::PSD>());
  psd->change(vp, 0.0, false, 10, false, false, QString("vUnits"), QString("rUnits"), WindowUndefined, 0.0, PSDUndefined);
  QCOMPARE(psd->vector()->descriptiveName(), QLatin1String("tempVector"));
  QCOMPARE(psd->output(), PSDUndefined);
  QVERIFY(!psd->apodize());
  QVERIFY(!psd->removeMean());
  QVERIFY(!psd->average());
  QCOMPARE(psd->frequency(), 0.0);
  QCOMPARE(psd->apodizeFxn(), WindowUndefined);
  QCOMPARE(psd->gaussianSigma(), 0.0);
  Kst::VectorPtr vpVX = psd->vX();
  Kst::VectorPtr vpVY = psd->vY();

  QCOMPARE(vpVX->length(), 1);
  QVERIFY(vpVX->value()[0] != vpVX->value()[0]);
  QCOMPARE(vpVY->length(), 1);
  QVERIFY(vpVY->value()[0] != vpVY->value()[0]);

  for(int j = 0; j < vpVX->length(); j++){
      QCOMPARE(vpVX->value()[j], &Kst::NOPOINT);
  }

  psd->setOutput(PSDAmplitudeSpectralDensity);
  psd->setApodize(true);
  psd->setRemoveMean(true);
  psd->setAverage(true);
  psd->setFrequency(0.1);
  psd->setApodizeFxn(WindowOriginal);
  psd->setGaussianSigma(0.2);

  QCOMPARE(psd->vector()->descriptiveName(), QLatin1String("tempVector"));
  QCOMPARE(psd->output(), PSDAmplitudeSpectralDensity);
  QVERIFY(psd->apodize());
  QVERIFY(psd->removeMean());
  QVERIFY(psd->average());
  QCOMPARE(psd->frequency(), 0.1);
  QCOMPARE(psd->apodizeFxn(), WindowOriginal);
  QCOMPARE(psd->gaussianSigma(), 0.2);

//   doTest(psd->update(0) == Kst::Object::UPDATE);
//   doTest(psd->propertyString() == ps);
//    doTest(!psd->curveHints().curveName() == "");
//   printf("Curve name [%s]", kstCHL[0].curveName());
//   printf("X Vector name [%s]", kstCHL[0].xVectorName());
//   printf("Y Vector name [%s]", kstCHL[0].yVectorName());

  QTemporaryFile tf;
  tf.open();
  QXmlStreamWriter xml;
  xml.setDevice(&tf);
  xml.setAutoFormatting(true);
  psd->save(xml);
  QFile::remove(tf.fileName());

  QDomNode n = makeDOMElement("psdDOMPsd", "psdDOMVector").firstChild();
  QDomElement e = n.toElement();

  //FIXME: should use factory, not this constructor.  This constructor is no longer
  // used anywhere in kst.
//   Kst::PSDPtr psdDOM = new Kst::PSD(&_store, e);

//   QCOMPARE(psdDOM->tag().tagString(), QLatin1String("psdDOMPsd"));
//   QCOMPARE(psdDOM->output(), PSDAmplitudeSpectralDensity);
//   QVERIFY(psdDOM->apodize());
//   QVERIFY(psdDOM->removeMean());
//   QVERIFY(psdDOM->average());
//   QCOMPARE(psdDOM->frequency(), 128.0);
//   QCOMPARE(psdDOM->apodizeFxn(), WindowOriginal);
//   QCOMPARE(psdDOM->gaussianSigma(), 0.01);

//   Kst::VectorPtr vpVX = psdDOM->vX();
//   for(int j = 0; j < vpVX->length(); j++){
//       printf("[%d][%lf]", j, vpVX->value()[j]);
//   }
//   Kst::VectorPtr vpVY = psdDOM->vY();
}

#ifdef KST_USE_QTEST_MAIN
QTEST_MAIN(TestPSD)
#endif

// vim: ts=2 sw=2 et
