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

#include <scalar.h>

#include <math_kst.h>
#include <kst_inf.h>

#include <datacollection.h>
#include <objectstore.h>
#include "updatemanager.h"

static Kst::ObjectStore _store;

double NOPOINT = NAN;

SListener::SListener() : QObject(), _trigger(0) {}
SListener::~SListener() {}
void SListener::trigger(qint64) { 
    _trigger++; 
}


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
  _store.clear();
}


void TestScalar::testScalar() {
  Kst::ScalarPtr sp = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
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
  Kst::UpdateManager::self()->setStore(&_store);
  QObject::connect(Kst::UpdateManager::self(), SIGNAL(objectsUpdated(qint64)), listener, SLOT(trigger(qint64)));

  *sp = 3.1415;
  Kst::UpdateManager::self()->doUpdates(true);
  QCOMPARE(listener->_trigger, 1);

  sp->setValue(3.1415);
  QCOMPARE(listener->_trigger, 1);
  Kst::UpdateManager::self()->doUpdates(true);
  QCOMPARE(listener->_trigger, 2);

  *sp = 1.1415;
  Kst::UpdateManager::self()->doUpdates(true);
  QCOMPARE(listener->_trigger, 3);

  Kst::ScalarPtr sp2 = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());

  QVERIFY(sp->displayable());
  QVERIFY(sp2->displayable());

  // FIXME: This constructor is no longer used - test using the factory
//   QDomNode n;
//   QDomElement e;
//   n = makeDOMDocument("load1", "2.14159265").firstChild();
//   e = n.toElement();
// 
//   Kst::ScalarPtr sp3 = new Kst::Scalar(&_store, e);
//   QCOMPARE(sp3->orphan(), false);
//   QCOMPARE(sp3->value(), 2.14159265);
//   QCOMPARE(sp3->tag().tagString(), QLatin1String("load1"));
//   QVERIFY(sp3->displayable());
// 
//   n = makeDOMDocument("55.4232", "55.4232", true).firstChild();
//   e = n.toElement();
//   Kst::ScalarPtr sp4 = new Kst::Scalar(&_store, e);
//   QVERIFY(sp4->orphan());
//   QCOMPARE(sp4->value(), 55.4232);
//   QCOMPARE(sp4->tag().tagString(), QLatin1String("55.4232"));
//   QVERIFY(!sp4->displayable());
// 
//   n = makeDOMDocument("load2", "NAN").firstChild();
//   e = n.toElement();
//   sp4 = new Kst::Scalar(&_store, e);
//   QVERIFY(sp4->value() != sp4->value());
// 
//   n = makeDOMDocument("load3", "INF").firstChild();
//   e = n.toElement();
//   sp4 = new Kst::Scalar(&_store, e);
// 
//   QVERIFY(sp4->value() == INF);
// 
//   n = makeDOMDocument("load4", "-INF").firstChild();
//   e = n.toElement();
//   sp4 = new Kst::Scalar(&_store, e);
// 
//   QVERIFY(sp4->value() == -INF);

  delete listener;
}

#ifdef KST_USE_QTEST_MAIN
QTEST_MAIN(TestScalar)
#endif

// vim: ts=2 sw=2 et
