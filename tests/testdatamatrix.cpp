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

#include "testdatamatrix.h"

#include <QtTest>

#include "datamatrix.h"
#include "datasource.h"
#include "math_kst.h"
#include "datacollection.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"

#include <QXmlStreamAttributes>

static Kst::ObjectStore _store;

void TestDataMatrix::cleanupTestCase() {
  _store.clear();
}


void TestDataMatrix::testDataMatrix() {
  bool ok = true;

  QStringList _plugins = Kst::DataSourcePluginManager::pluginList();

  //basic default constructor values
  Kst::DataMatrixPtr m1 = Kst::kst_cast<Kst::DataMatrix>(_store.createObject<Kst::DataMatrix>());
  QCOMPARE(m1->sampleCount(), 0);
  QCOMPARE(m1->minValue(), 0.0);
  QCOMPARE(m1->maxValue(), 0.0);
  QCOMPARE(m1->value(0, 0, &ok), 0.0);
  QVERIFY(!ok);
  QCOMPARE(m1->value(10, 10, &ok), 0.0); //should be outside the boundaries.
  QVERIFY(!ok);
  QCOMPARE(m1->meanValue(), 0.0);

  if (!_plugins.contains("QImage Source Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);

  //These tests assume that the image kst.png exists in src/images
  QString imageFile = QDir::currentPath() + QDir::separator() + QString("src") +
                      QDir::separator() + QString("images") + QDir::separator() + QString("kst.png");

  if (!QFile::exists(imageFile)) {
    QSKIP("...unable to perform test.  Image file missing.", SkipAll);
  }

  printf("Opening image = %s for test.\n", imageFile.toLatin1().data());

  Kst::DataSourcePtr dsp = Kst::DataSourcePluginManager::loadSource(&_store, imageFile);
  dsp->internalUpdate();

  QVERIFY(dsp);
  QVERIFY(dsp->isValid());

  m1->change(dsp, "GRAY", 0, 0, -1, -1, false, false, 1, 0, 0, 1, 1);

  m1->writeLock();
  m1->internalUpdate();
  m1->unlock();

  QCOMPARE(m1->xNumSteps(), 32);
  QCOMPARE(m1->yNumSteps(), 32);
  QCOMPARE(m1->xStepSize(), 1.0);
  QCOMPARE(m1->yStepSize(), 1.0);
  QCOMPARE(m1->minX(), 0.0);
  QCOMPARE(m1->minY(), 0.0);

  QCOMPARE(m1->minValue(), 0.0);
  QCOMPARE(m1->maxValue(), 255.0);

  QCOMPARE(m1->minValuePositive(), 7.0);

  QCOMPARE(m1->sampleCount(), 1024);

  QCOMPARE(m1->value(0, 0, &ok), 0.0);
  QVERIFY(ok);
  QCOMPARE(m1->value(25, 3, &ok), 81.0);
  QVERIFY(ok);
}

// vim: ts=2 sw=2 et
