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

#include "testscalar.h"

#include <QtTest>

#include <QDomDocument>

#include <kstmath.h>
#include <scalar.h>
#include <datacollection.h>
#include <kstdataobjectcollection.h>

double NOPOINT = NAN;

SListener::SListener() : QObject(), _trigger(0) {}
SListener::~SListener() {}
void SListener::trigger() { _trigger++; }


QDomDocument TestScalar::makeDOMDocument(const QString& tag, const QString& val, bool orphan) {
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


void TestScalar::cleanupTestCase() {
  Kst::vectorList.clear();
  Kst::scalarList.clear();
  KST::dataObjectList.clear();
}


void TestScalar::testScalar() {
  Kst::ScalarPtr sp = new Kst::Scalar;
  QVERIFY(!sp->tagName().isEmpty());
  QCOMPARE(sp->value(), 0.0);
  *sp = 3.1415;
  QVERIFY(sp->displayable());
  QCOMPARE(sp->value(), 3.1415);
  sp->setValue(2.1415);
  QCOMPARE(sp->value(), 2.1415);
  sp->setValue(NOPOINT);
  QVERIFY(sp->value() != sp->value());
  sp->setValue(INF );

  QVERIFY(sp->value() == INF);

  QCOMPARE((*sp = 2.0).value(), 2.0);
  SListener *listener = new SListener;
  sp->connect(sp, SIGNAL(trigger()), listener, SLOT(trigger()));
  *sp = 3.1415;
  QCOMPARE(listener->_trigger, 1);
  sp->setValue(3.1415);
  QCOMPARE(listener->_trigger, 1);
  *sp = 1.1415;
  QCOMPARE(listener->_trigger, 2);

  Kst::ScalarPtr sp2 = new Kst::Scalar(Kst::ObjectTag::fromString(sp->tagName()));

  QVERIFY(sp->displayable());
  QVERIFY(sp2->displayable());

  QDomNode n;
  QDomElement e;
  n = makeDOMDocument("load1", "2.14159265").firstChild();
  e = n.toElement();
  Kst::ScalarPtr sp3 = new Kst::Scalar(e);
  QCOMPARE(sp3->orphan(), false);
  QCOMPARE(sp3->value(), 2.14159265);
  QCOMPARE(sp3->tagName(), QLatin1String("load1"));
  QVERIFY(sp3->displayable());

  n = makeDOMDocument("55.4232", "55.4232", true).firstChild();
  e = n.toElement();
  Kst::ScalarPtr sp4 = new Kst::Scalar(e);
  QVERIFY(sp4->orphan());
  QCOMPARE(sp4->value(), 55.4232);
  QCOMPARE(sp4->tagName(), QLatin1String("55.4232"));
  QVERIFY(!sp4->displayable());

  n = makeDOMDocument("load2", "NAN").firstChild();
  e = n.toElement();
  sp4 = new Kst::Scalar(e);
  QVERIFY(sp4->value() != sp4->value());

  n = makeDOMDocument("load3", "INF").firstChild();
  e = n.toElement();
  sp4 = new Kst::Scalar(e);

  QVERIFY(sp4->value() == INF);

  n = makeDOMDocument("load4", "-INF").firstChild();
  e = n.toElement();
  sp4 = new Kst::Scalar(e);

  QVERIFY(sp4->value() == -INF);

  delete listener;
}

// vim: ts=2 sw=2 et
