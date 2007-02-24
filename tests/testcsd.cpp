/*
 *  Copyright 2005, The University of Toronto
 *  Licensed under GPL.
 */

#include "ksttestcase.h"
#include "kstcsd.h"

#include <kstdataobjectcollection.h>
#include <kstandarddirs.h>
#include <kstobject.h>
#include <qdir.h>
#include <qfile.h>


static void exitHelper() {
  KST::dataSourceList.clear();
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

QDomDocument makeDOMElement(const QString& tag, const QString& val) {
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

void doTests() {

  KstVectorPtr vp = new KstVector(KstObjectTag::fromString("tempVector"), 10);
  for (int i = 0; i < 10; i++){
    vp->value()[i] = i;
  }

  KstCSDPtr csd = new KstCSD("csdTest", vp, 0.0, false, false, false, WindowUndefined, 0, 0, 0.0, PSDUndefined, QString::null, QString::null);
  doTest(csd->tagName() == "csdTest");
  doTest(csd->vTag() == "tempVector");
  doTest(csd->output() == PSDUndefined);
  doTest(!csd->apodize());
  doTest(!csd->removeMean());
  doTest(!csd->average());
  doTest(csd->freq() == 1.0);
  doTest(csd->apodizeFxn() == WindowUndefined);
  doTest(csd->length() == 0);
  doTest(csd->windowSize() == 0);
  doTest(csd->gaussianSigma() == 0);
  doTest(csd->vectorUnits().isEmpty());
  doTest(csd->rateUnits().isEmpty());

  csd->setOutput(PSDAmplitudeSpectralDensity);
  csd->setApodize(true);
  csd->setRemoveMean(true);
  csd->setAverage(true);
  csd->setFreq(0.1);
  csd->setApodizeFxn(WindowOriginal);
  csd->setLength(3);
  csd->setWindowSize(50);
  csd->setGaussianSigma(0.2);

  doTest(csd->tagName() == "csdTest");
  doTest(csd->vTag() == "tempVector");
  doTest(csd->output() == PSDAmplitudeSpectralDensity);
  doTest(csd->apodize());
  doTest(csd->removeMean());
  doTest(csd->average());
  doTest(csd->freq() == 0.1);
  doTest(csd->windowSize() == 50);
  doTest(csd->apodizeFxn() == WindowOriginal);
  doTest(csd->gaussianSigma() == 0.2);

//   KTempFile tf(locateLocal("tmp", "kst-csd"), "txt");
//   QFile *qf = tf.file();
//   QTextStream ts(qf);
//   csd->save(ts, "");

  QDomNode n = makeDOMElement("csdDOMCsd", "csdDOMVector").firstChild();
  QDomElement e = n.toElement();
  KstCSDPtr csdDOM = new KstCSD(e);

  doTest(csdDOM->tagName() == "csdDOMCsd");
  doTest(csdDOM->output() == PSDPowerSpectralDensity);
  doTest(csdDOM->apodize());
  doTest(csdDOM->removeMean());
  doTest(csdDOM->average());
  doTest(csdDOM->freq() == 1);
  doTest(csdDOM->apodizeFxn() == WindowOriginal);
  doTest(csdDOM->gaussianSigma() == 0.01);
  doTest(csdDOM->windowSize() == 5000);

  KstVectorPtr vp2 = new KstVector(KstObjectTag::fromString("tempVector2"), 10);
  for (int i = 0; i < 10; i++){
    vp2->value()[i] = i;
  }
  csdDOM->setVector(vp2);
  doTest(csdDOM->vTag() == "tempVector2");
  csdDOM->setWindowSize(9);
  KstMatrixPtr outMatrix = csdDOM->outputMatrix();
  
  doTest(outMatrix->resize(3, 3, false)); // very odd thing to do?
  doTest(outMatrix->setValue(0, 0, 1.716299));
  doTest(outMatrix->setValue(0, 1, -0.485527));
  doTest(outMatrix->setValue(0, 2, -0.288690));
  doTest(outMatrix->setValue(1, 0, 1.716299));
  doTest(outMatrix->setValue(1, 1, NAN));
  doTest(outMatrix->setValue(1, 2, -0.274957));
  doTest(outMatrix->setValue(2, 0, 1.711721));
  doTest(outMatrix->setValue(2, 1, -0.485527));
  doTest(outMatrix->setValue(2, 2, -0.293267));

  doTest(outMatrix->sampleCount() == 9);
  doTest(outMatrix->value(0, 0) == 1.716299);
  doTest(outMatrix->value(0, 1) ==  -0.485527);
  doTest(outMatrix->value(0, 2) == -0.288690);
  doTest(outMatrix->value(1, 0) == 1.716299);
  doTest(outMatrix->value(1, 1) == 0);
  doTest(outMatrix->value(1, 2) == -0.274957);
  doTest(outMatrix->value(2, 0) == 1.711721);
  doTest(outMatrix->value(2, 1) == -0.485527);
  doTest(outMatrix->value(2, 2) == -0.293267);

  doTest(csdDOM->update(0) == KstObject::UPDATE);
  outMatrix = csdDOM->outputMatrix();
  doTest(outMatrix->sampleCount() == 128);

  csdDOM->setWindowSize(11);
  doTest(outMatrix->sampleCount() == 128);

  doTest(outMatrix->value(0, 0) == 1.716299);
  doTest(outMatrix->value(0, 1) ==  -0.485527);
  doTest(outMatrix->value(0, 2) == -0.288690);
  doTest(outMatrix->value(1, 0) == 1.716299);
  doTest(outMatrix->value(1, 1) == 0);
  doTest(outMatrix->value(1, 2) == -0.274957);
  doTest(outMatrix->value(2, 0) == 1.711721);
  doTest(outMatrix->value(2, 1) == -0.485527);
  doTest(outMatrix->value(2, 2) == -0.293267);

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
