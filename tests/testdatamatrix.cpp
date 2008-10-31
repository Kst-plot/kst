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

#include <datamatrix.h>
#include <datasource.h>
#include <math_kst.h>
#include <datacollection.h>
#include <objectstore.h>

#include <QXmlStreamAttributes>

static Kst::ObjectStore _store;

void TestDataMatrix::cleanupTestCase() {
  _store.clear();
}


void TestDataMatrix::testDataMatrix() {
  bool ok = true;

//   QSettings *settingsObject = new QSettings("kstdatarc", QSettings::IniFormat);
//   Kst::DataSource::setupOnStartup(settingsObject);
//   QStringList _plugins = Kst::DataSource::pluginList();
// 
//   if (!_plugins.contains("ASCII File Reader"))
//     QSKIP("...couldn't find plugin.", SkipAll);

    // for ascii data sources, matrix fields start with [MATRIXNAME,Y,x,y,w,l]
    // where Y = size of y dimension
    //       x = x coordinate of minimum
    //       y = y coordinate of minimum
    //       w = x step size
    //       l = y step size

//   QTemporaryFile tf;
//   tf.open();
//   QTextStream ts(&tf);
//   ts << "[MATRIX,3,0,0,1,1]" << endl;
//   ts << "1.0" << endl;
//   ts << "2.0" << endl;
//   ts << "3.0" << endl;
//   ts << "4.0" << endl;
//   ts << "5.0" << endl;
//   ts << "6.0" << endl;
//   ts << "7.0" << endl;
//   ts << "8.0" << endl;
//   ts << "9.0" << endl;
//   ts.flush();
// 
// 
//   Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, tf.fileName());
// 
//   QVERIFY(dsp);
//   QVERIFY(dsp->isValid());
// 
//   QXmlStreamAttributes attributes;
//   attributes.append("intepretation", "0");
//   attributes.append("delimiters", "#/c!;");
//   attributes.append("columntype", "0");
//   attributes.append("columndelimiters", "");
//   attributes.append("headerstart", "1");
//   attributes.append("fields", "0");
//   attributes.append("readfields", "true");
// 
//   dsp->parseProperties(attributes);
// 
//   dsp->writeLock();
//   dsp->reset();
//   dsp->unlock();

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

//   m1->change(dsp, "[MATRIX,3,0,0,1,1]", 0, 0, -1, -1, false, false, 1, 0, 0, 1, 1);
// 
//   m1->writeLock();
//   m1->update();
//   m1->unlock();

  QSKIP("No Data Test currently exists for DataMatrices.", SkipAll);
//   qDebug() << "No Data Test currently exists for DataMatrices.";
//   QCOMPARE(m1->sampleCount(), 9);
//   QCOMPARE(m1->minValue(), 1.0);
//   QCOMPARE(m1->maxValue(), 9.0);
//   QCOMPARE(m1->value(0, 0, &ok), 1.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(0, 1, &ok), 2.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(0, 2, &ok), 3.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(1, 0, &ok), 4.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(1, 1, &ok), 5.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(1, 2, &ok), 6.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(2, 0, &ok), 7.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(2, 1, &ok), 8.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(2, 2, &ok), 9.0);
//   QVERIFY(ok);
//   QCOMPARE(m1->value(10, 10, &ok), 0.0); //should be outside the boundaries.
//   QVERIFY(!ok);
}

// vim: ts=2 sw=2 et
