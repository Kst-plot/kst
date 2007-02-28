/*
 *  Copyright 2004, The University of Toronto
 *  Licensed under GPL.
 */

#include <qdom.h>

#include "ksttestcase.h"
#include "testscalars.h"
#include "testscalars.moc"
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

SListener::SListener() : QObject(), _trigger(0) {}
SListener::~SListener() {}
void SListener::trigger() { _trigger++; }


QDomDocument makeDOM1(const QString& tag, const QString& val, bool orphan = false) {
  QDomDocument loadTest("scalardocument");
  QDomElement scalardoc, child;
  QDomText text;

  scalardoc = loadTest.createElement("scalar");

  child = loadTest.createElement("tag");
  text = loadTest.createTextNode(tag);
  child.appendChild(text);
  scalardoc.appendChild(child);
  
  child = loadTest.createElement("value");
  text = loadTest.createTextNode(val);
  child.appendChild(text);
  scalardoc.appendChild(child);

  if (orphan) {
    child = loadTest.createElement("orphan");
    scalardoc.appendChild(child);
  }

  loadTest.appendChild(scalardoc);

  return loadTest;
}


void doTests() {
  KstScalarPtr sp = new KstScalar;
  doTest(!sp->tagName().isEmpty());
  doTest(sp->value() == 0.0);
  *sp = 3.1415;
  doTest(sp->displayable());
  doTest(sp->value() == 3.1415);
  sp->setValue(2.1415);
  doTest(sp->value() == 2.1415);
  sp->setValue(NOPOINT);
  doTest(sp->value() != sp->value());
  sp->setValue(INF);
  doTest(sp->value() == INF);
  doTest((*sp = 2.0).value() == 2.0);
  SListener *listener = new SListener;
  sp->connect(sp, SIGNAL(trigger()), listener, SLOT(trigger()));
  *sp = 3.1415;
  doTest(listener->_trigger == 1);
  sp->setValue(3.1415);
  doTest(listener->_trigger == 1);
  *sp = 1.1415;
  doTest(listener->_trigger == 2);

  KstScalarPtr sp2 = new KstScalar(KstObjectTag::fromString(sp->tagName()));

  doTest(sp->displayable());
  doTest(sp2->displayable());

  QDomNode n;
  QDomElement e;
  n = makeDOM1("load1", "2.14159265").firstChild();
  e = n.toElement();
  KstScalarPtr sp3 = new KstScalar(e);
  doTest(sp3->orphan() == false);
  doTest(sp3->value() == 2.14159265);
  doTest(sp3->tagName() == "load1");
  doTest(sp3->displayable());

  n = makeDOM1("55.4232", "55.4232", true).firstChild();
  e = n.toElement();
  KstScalarPtr sp4 = new KstScalar(e);
  doTest(sp4->orphan());
  doTest(sp4->value() == 55.4232);
  doTest(sp4->tagName() == "55.4232");
  doTest(!sp4->displayable());

  n = makeDOM1("load2", "NAN").firstChild();
  e = n.toElement();
  sp4 = new KstScalar(e);
  doTest(sp4->value() != sp4->value());

  n = makeDOM1("load3", "INF").firstChild();
  e = n.toElement();
  sp4 = new KstScalar(e);
  doTest(sp4->value() == INF);

  n = makeDOM1("load4", "-INF").firstChild();
  e = n.toElement();
  sp4 = new KstScalar(e);
  doTest(sp4->value() == -INF);

  delete listener;
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
