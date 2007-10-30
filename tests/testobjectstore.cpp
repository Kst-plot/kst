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

#include "testobjectstore.h"

#include <QtTest>

#include <datasource.h>
#include <datavector.h>
#include <objectstore.h>
#include <scalar.h>
#include <vector.h>

using namespace Kst;

void TestObjectStore::cleanupTestCase() {
}

void TestObjectStore::testObjectStore() {
  ObjectStore store;
  QVERIFY(store.isEmpty());

  ScalarPtr sc = kst_cast<Scalar>(store.createObject<Scalar>());
  QVERIFY(sc);  // scalar was created
  QVERIFY(sc->tag().isValid()); // scalar was assigned a tag
  ScalarPtr sc2 = kst_cast<Scalar>(store.createObject<Scalar>());
  QVERIFY(sc2);
  QVERIFY(sc2->tag().isValid());
  QVERIFY(sc2->tag() != sc->tag());
  QCOMPARE(store.getObjects<Scalar>().count(), 2);
  QCOMPARE(store.getObjects<Vector>().count(), 0);

  ObjectTag scTag = sc->tag();
  QVERIFY(store.objectExists(scTag));
  QVERIFY(sc == store.retrieveObject(scTag)); // can retrieve the object from the store

  VectorPtr vec = kst_cast<Vector>(store.createObject<Vector>());
  QVERIFY(vec);
  QVERIFY(store.getObjects<Scalar>().count() > 2);  // to account for the vector's stats scalars
  QCOMPARE(store.getObjects<Vector>().count(), 1);

  QCOMPARE(store.getObjects<DataSource>().count(), 0);
  QCOMPARE(store.getObjects<DataVector>().count(), 0);
  QCOMPARE(store.dataSourceList().count(), 0);

  store.clear();
  QVERIFY(store.isEmpty());
  QVERIFY(sc);  // make sure objects didn't get deleted while references were held
  QVERIFY(sc2);
  QVERIFY(vec);

  QPointer<Scalar> p(sc);
  ScalarPtr tempSc = sc;
  sc = 0L;
  QVERIFY(p);
  tempSc = 0L;
  QVERIFY(!p);  // make sure object gets deleted when last reference is gone
}

// vim: ts=2 sw=2 et
