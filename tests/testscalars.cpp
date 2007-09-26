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

#include "testscalars.h"

#include <QtTest>

#include <QDomDocument>

#include <kstscalar.h>
#include <kstdatacollection.h>
#include <kstdataobjectcollection.h>

#include <math.h>

#ifdef NAN
double NOPOINT = NAN;
#else
double NOPOINT = 0.0/0.0; // NaN
#endif

#ifndef INF
double _INF = 1.0/0.0;
#endif

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


void TestScalar::cleanupTestCase() {
  KST::vectorList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}


void TestScalar::testScalar() {
  KstScalarPtr sp = new KstScalar;
  QVERIFY(!sp->tagName().isEmpty());
  QCOMPARE(sp->value(), 0.0);
  *sp = 3.1415;
  QVERIFY(sp->displayable());
  QCOMPARE(sp->value(), 3.1415);
  sp->setValue(2.1415);
  QCOMPARE(sp->value(), 2.1415);
  sp->setValue(NOPOINT);
  QVERIFY(sp->value() != sp->value());
  sp->setValue(_INF );

  QEXPECT_FAIL("", "Bug in Qt qFuzzyCompare can not compare inf...", Continue);
  QCOMPARE(sp->value(), _INF );

  QCOMPARE((*sp = 2.0).value(), 2.0);
  SListener *listener = new SListener;
  sp->connect(sp, SIGNAL(trigger()), listener, SLOT(trigger()));
  *sp = 3.1415;
  QCOMPARE(listener->_trigger, 1);
  sp->setValue(3.1415);
  QCOMPARE(listener->_trigger, 1);
  *sp = 1.1415;
  QCOMPARE(listener->_trigger, 2);

  KstScalarPtr sp2 = new KstScalar(KstObjectTag::fromString(sp->tagName()));

  QVERIFY(sp->displayable());
  QVERIFY(sp2->displayable());

  QDomNode n;
  QDomElement e;
  n = makeDOM1("load1", "2.14159265").firstChild();
  e = n.toElement();
  KstScalarPtr sp3 = new KstScalar(e);
  QCOMPARE(sp3->orphan(), false);
  QCOMPARE(sp3->value(), 2.14159265);
  QCOMPARE(sp3->tagName(), QLatin1String("load1"));
  QVERIFY(sp3->displayable());

  n = makeDOM1("55.4232", "55.4232", true).firstChild();
  e = n.toElement();
  KstScalarPtr sp4 = new KstScalar(e);
  QVERIFY(sp4->orphan());
  QCOMPARE(sp4->value(), 55.4232);
  QCOMPARE(sp4->tagName(), QLatin1String("55.4232"));
  QVERIFY(!sp4->displayable());

  n = makeDOM1("load2", "NAN").firstChild();
  e = n.toElement();
  sp4 = new KstScalar(e);
  QVERIFY(sp4->value() != sp4->value());

  n = makeDOM1("load3", "INF").firstChild();
  e = n.toElement();
  sp4 = new KstScalar(e);

  QEXPECT_FAIL("", "Bug in Qt qFuzzyCompare can not compare inf...", Continue);
  QCOMPARE(sp4->value(), _INF);

  n = makeDOM1("load4", "-INF").firstChild();
  e = n.toElement();
  sp4 = new KstScalar(e);

  QEXPECT_FAIL("", "Bug in Qt qFuzzyCompare can not compare inf...", Continue);
  QCOMPARE(sp4->value(), -_INF );

  delete listener;
}

// vim: ts=2 sw=2 et
