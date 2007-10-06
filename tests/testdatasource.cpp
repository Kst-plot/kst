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

#include "testdatasource.h"

#include <QtTest>

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTemporaryFile>

#include <math_kst.h>
#include <datavector.h>
#include <datacollection.h>
#include <dataobjectcollection.h>

void TestDataSource::initTestCase() {
  QSettings *settingsObject = new QSettings("kstdatarc", QSettings::IniFormat);
  Kst::DataSource::setupOnStartup(settingsObject);
  _plugins = Kst::DataSource::pluginList();
}


void TestDataSource::cleanupTestCase() {
  Kst::dataSourceList.clear();
  Kst::vectorList.clear();
  Kst::scalarList.clear();
  Kst::dataObjectList.clear();
}


void TestDataSource::testAscii() {
  if (!_plugins.contains("ASCII File Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);

  {
    QTemporaryFile tf;
    tf.open();
    QTextStream ts(&tf);
    ts << ";" << endl;
    ts << " ;" << endl;
    ts << "#;" << endl;
    ts << "c comment comment" << endl;
    ts << "\t!\t!\t!\t!" << endl;
    ts << "2.0" << endl;
    ts << "1" << endl;
    ts << ".2" << endl;
    ts.flush();

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII"));
    QVERIFY(dsp->isValidField("INDEX"));
    QVERIFY(dsp->isValidField("1"));
    QVERIFY(!dsp->isValidField("0"));
    QVERIFY(!dsp->isValidField("2"));
    QCOMPARE(dsp->samplesPerFrame(QString::null), 1);
    QCOMPARE(dsp->samplesPerFrame("INDEX"), 1);
    QCOMPARE(dsp->samplesPerFrame("1"), 1);
    QCOMPARE(dsp->frameCount(QString::null), 3);
    QCOMPARE(dsp->frameCount("1"), 3);
    QCOMPARE(dsp->frameCount("INDEX"), 3);
    QCOMPARE(dsp->fileName(), tf.fileName());
    QCOMPARE(dsp->fieldList().count(), 2);
    QVERIFY(dsp->fieldListIsComplete());
    QVERIFY(!dsp->isEmpty());

    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "1", Kst::ObjectTag::fromString("RVTestAscii1"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 3);
    QCOMPARE(rvp->value()[0], 2.0);
    QCOMPARE(rvp->value()[1], 1.0);
    QCOMPARE(rvp->value()[2], 0.2);
    rvp = new Kst::DataVector(dsp, "INDEX", Kst::ObjectTag::fromString("RVTestAscii2"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 3);
    QCOMPARE(rvp->value()[0], 0.0);
    QCOMPARE(rvp->value()[1], 1.0);
    QCOMPARE(rvp->value()[2], 2.0);

    tf.close();
  }

  {
    QTemporaryFile tf;
    tf.open();
    QTextStream ts(&tf);
    ts << "2e-1 \t .1415" << endl;
    ts << "nan -.4e-2" << endl;
    ts << "inf\t1" << endl;
    ts << "0.000000000000000000000000000000000000000000000000 0" << endl;

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII"));
    QVERIFY(dsp->isValidField("INDEX"));
    QVERIFY(dsp->isValidField("1"));
    QVERIFY(!dsp->isValidField("0"));
    QVERIFY(dsp->isValidField("2"));
    QVERIFY(!dsp->isValidField("3"));
    QCOMPARE(dsp->samplesPerFrame(QString::null), 1);
    QCOMPARE(dsp->samplesPerFrame("INDEX"), 1);
    QCOMPARE(dsp->samplesPerFrame("1"), 1);
    QCOMPARE(dsp->samplesPerFrame("2"), 1);
    QCOMPARE(dsp->frameCount(QString::null), 4);
    QCOMPARE(dsp->frameCount("1"), 4);
    QCOMPARE(dsp->frameCount("2"), 4);
    QCOMPARE(dsp->frameCount("INDEX"), 4);
    QCOMPARE(dsp->fieldList().count(), 3);
    QVERIFY(!dsp->isEmpty());

    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "1", Kst::ObjectTag::fromString("RVTestAscii1"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 4);
    QCOMPARE(rvp->value()[0], 0.2);
    QVERIFY(rvp->value()[1] != rvp->value()[1]);

    QVERIFY(rvp->value()[2] == INF);

    QCOMPARE(rvp->value()[3], 0.0);
    rvp = new Kst::DataVector(dsp, "2", Kst::ObjectTag::fromString("RVTestAscii2"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 4);
    QCOMPARE(rvp->value()[0], 0.1415);
    QCOMPARE(rvp->value()[1], -0.004);
    QCOMPARE(rvp->value()[2], 1.0);
    QCOMPARE(rvp->value()[3], 0.0);

    tf.close();
  }

  {
    QTemporaryFile tf;
    tf.open();
    QTextStream ts(&tf);
    ts << "2 4" << endl;

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII"));
    QVERIFY(dsp->isValidField("INDEX"));
    QVERIFY(dsp->isValidField("1"));
    QVERIFY(!dsp->isValidField("0"));
    QVERIFY(dsp->isValidField("2"));
    QVERIFY(!dsp->isValidField("3"));
    QCOMPARE(dsp->samplesPerFrame(QString::null), 1);
    QCOMPARE(dsp->samplesPerFrame("INDEX"), 1);
    QCOMPARE(dsp->samplesPerFrame("1"), 1);
    QCOMPARE(dsp->samplesPerFrame("2"), 1);
    QCOMPARE(dsp->frameCount(QString::null), 1);
    QCOMPARE(dsp->frameCount("1"), 1);
    QCOMPARE(dsp->frameCount("2"), 1);
    QCOMPARE(dsp->frameCount("INDEX"), 1);
    QCOMPARE(dsp->fieldList().count(), 3);
    QVERIFY(!dsp->isEmpty());

    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "1", Kst::ObjectTag::fromString("RVTestAscii1"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 1); // Are we allowed to have vectors of 1?
    QCOMPARE(rvp->value()[0], 2.0);
    rvp = new Kst::DataVector(dsp, "2", Kst::ObjectTag::fromString("RVTestAscii2"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 1);
    QCOMPARE(rvp->value()[0], 4.0);

    tf.close();
  }

  {
    QTemporaryFile tf;
    tf.open();
    QTextStream ts(&tf);
    ts << ";" << endl;

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII"));
    tf.close();
  }

  {
    QTemporaryFile tf;
    tf.open();
    QTextStream ts(&tf);
    for (int i = 0; i < 39000; ++i) {
      ts << i << " " <<  i + 100 << " " << i + 1000 << endl;
    }

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(tf.fileName());
    dsp->update(0);

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII"));
    QCOMPARE(dsp->frameCount(QString::null), 39000);
    QCOMPARE(dsp->frameCount("1"), 39000);
    QCOMPARE(dsp->frameCount("2"), 39000);
    QCOMPARE(dsp->frameCount("3"), 39000);
    QCOMPARE(dsp->frameCount("INDEX"), 39000);
    QCOMPARE(dsp->fieldList().count(), 4);
    QVERIFY(!dsp->isEmpty());

    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "1", Kst::ObjectTag::fromString("RVTestAscii1"), 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 39000);
    rvp = new Kst::DataVector(dsp, "2", Kst::ObjectTag::fromString("RVTestAscii2"), 0, -1, 10, true, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 3900);
    QCOMPARE(rvp->value()[0], 100.0);
    QCOMPARE(rvp->value()[1], 110.0);
    QCOMPARE(rvp->value()[2], 120.0);
    QCOMPARE(rvp->value()[3898], 39080.0);

    rvp = new Kst::DataVector(dsp, "3", Kst::ObjectTag::fromString("RVTestAscii2"), 0, -1, 10, true, true);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 3900);
    QCOMPARE(rvp->value()[0], 1004.5);
    QCOMPARE(rvp->value()[1], 1014.5);
    QCOMPARE(rvp->value()[2], 1024.5);
    QCOMPARE(rvp->value()[3898], 39984.5);

    QFile::remove(dsp->fileName());
    tf.close();

    rvp->writeLock();
    rvp->reload();
    rvp->unlock();
#ifndef Q_WS_WIN32
    // Win32 you can't erase a file that's open
    QVERIFY(!rvp->isValid());
#endif
  }
}


void TestDataSource::testDirfile() {
  if (!_plugins.contains("DirFile Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);

  //These tests assume that the dirfile was generated with dirfile_maker

  {
    QString fifteen = QDir::currentPath() + QDir::separator() + QString("dirfile_testcase") +
                      QDir::separator() + QString("15count");
    if (!QFile::exists(fifteen + QDir::separator() + "format")) {
      return;
    }
    printf("Opening dirfile = %s for test.\n", fifteen.toLatin1().data());

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(fifteen);
    dsp->update(0);

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("DirFile"));
    QVERIFY(dsp->isValidField("INDEX"));
    QCOMPARE(dsp->frameCount("INDEX"), 15);
    QVERIFY(dsp->isValidField("cos"));
    QVERIFY(dsp->isValidField("fcount"));
    QVERIFY(dsp->isValidField("scount"));
    QVERIFY(dsp->isValidField("sine"));
    QVERIFY(dsp->isValidField("ssine"));
    QVERIFY(!dsp->isValidField("foo"));

    //TODO test samples per frame?

    QCOMPARE(dsp->fileName(), fifteen);
    QCOMPARE(dsp->fieldList().count(), 6);
    QVERIFY(dsp->fieldListIsComplete());

    QVERIFY(!dsp->isEmpty());

  {
    //Skip FIVE frames...
    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "INDEX", Kst::ObjectTag::fromString("RVTestDirfile"), 0, -1, 5, true, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();

    //We should have length equal to three...  items {0, 5, 10}
    //NOTE: The last item, index #14, does not fit in the skip boundary...
    QCOMPARE(3, rvp->length());
    QCOMPARE(0.0, rvp->value(0));
    QCOMPARE(5.0, rvp->value(1));
    QCOMPARE(10.0, rvp->value(2));

    //The numFrames should report 11 as it lies on the skip boundary
    QCOMPARE(11, rvp->numFrames());
    //The startFrame should report 0 as it lies on the skip boundary
    QCOMPARE(0, rvp->startFrame());

    QCOMPARE(-1, rvp->reqNumFrames());
    QCOMPARE(0, rvp->reqStartFrame());

    QCOMPARE(true, rvp->readToEOF());
    QCOMPARE(false, rvp->countFromEOF());
    QCOMPARE(true, rvp->doSkip());
    QCOMPARE(5, rvp->skip());
    QCOMPARE(false, rvp->doAve());
  }
  {
    //Skip FIVE frames...
    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "INDEX", Kst::ObjectTag::fromString("RVTestDirfile"), 3, -1, 5, true, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();

    //We should have length equal to three...  items {5, 10}
    QCOMPARE(2, rvp->length());
    QCOMPARE(5.0, rvp->value(0));
    QCOMPARE(10.0, rvp->value(1));
    QCOMPARE(13.0, rvp->value(2));

    //The numFrames should still report 11 as it lies on the skip boundary
    QCOMPARE(6, rvp->numFrames());
    //The startFrame should report 3 as it lies on the skip boundary and was requested
    QCOMPARE(5, rvp->startFrame());

    QCOMPARE(-1, rvp->reqNumFrames());
    QCOMPARE(3, rvp->reqStartFrame());

    QCOMPARE(true, rvp->readToEOF());
    QCOMPARE(false, rvp->countFromEOF());
    QCOMPARE(true, rvp->doSkip());
    QCOMPARE(5, rvp->skip());
    QCOMPARE(false, rvp->doAve());
  }
  {
    //Skip FIVE frames...
    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "INDEX", Kst::ObjectTag::fromString("RVTestDirfile"), 0, 11, 5, true, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();

    //We should have length equal to three...  items {0, 5, 10}
    QCOMPARE(3, rvp->length());
    QCOMPARE(0.0, rvp->value(0));
    QCOMPARE(5.0, rvp->value(1));
    QCOMPARE(10.0, rvp->value(2));

    //The numFrames should still report 11 as it lies on the skip boundary
    QCOMPARE(11, rvp->numFrames());
    //The startFrame should still report 0 as it lies on the skip boundary
    QCOMPARE(0, rvp->startFrame());

    QCOMPARE(11, rvp->reqNumFrames());
    QCOMPARE(0, rvp->reqStartFrame());

    QCOMPARE(false, rvp->readToEOF());
    QCOMPARE(false, rvp->countFromEOF());
    QCOMPARE(true, rvp->doSkip());
    QCOMPARE(5, rvp->skip());
    QCOMPARE(false, rvp->doAve());
  }
  {
    //Skip FIVE frames and countFromEOF()...
    Kst::DataVectorPtr rvp = new Kst::DataVector(dsp, "INDEX", Kst::ObjectTag::fromString("RVTestDirfile"), -1, 10, 5, true, false);
    rvp->writeLock();
    rvp->update(0);
    rvp->unlock();

    //We should have length equal to two...  items {5, 10}
    QCOMPARE(2, rvp->length());
    QCOMPARE(5.0, rvp->value(0));
    QCOMPARE(10.0, rvp->value(1));

    //The numFrames should report 6 as it lies on the skip boundary
    QCOMPARE(6, rvp->numFrames());
    //The startFrame should report 5 as it lies on the skip boundary
    QCOMPARE(5, rvp->startFrame());

    QCOMPARE(10, rvp->reqNumFrames());
    QCOMPARE(-1, rvp->reqStartFrame());

    QCOMPARE(false, rvp->readToEOF());
    QCOMPARE(true, rvp->countFromEOF());
    QCOMPARE(true, rvp->doSkip());
    QCOMPARE(5, rvp->skip());
    QCOMPARE(false, rvp->doAve());
  }
  }
}


void TestDataSource::testCDF() {
  return; //FIXME remove when we actually have some tests for this datasource.

  if (!_plugins.contains("CDF File Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);
}


void TestDataSource::testFrame() {
  return; //FIXME remove when we actually have some tests for this datasource.

  if (!_plugins.contains("Frame Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);
}


void TestDataSource::testIndirect() {
  return; //FIXME remove when we actually have some tests for this datasource.

  if (!_plugins.contains("Indirect File Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);
}


void TestDataSource::testLFI() {
  return; //FIXME remove when we actually have some tests for this datasource.

  if (!_plugins.contains("LFIIO Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);
}


void TestDataSource::testPlanck() {
  return; //FIXME remove when we actually have some tests for this datasource.

  if (!_plugins.contains("PLANCK Plugin"))
    QSKIP("...couldn't find plugin.", SkipAll);
}


void TestDataSource::testStdin() {
}

// vim: ts=2 sw=2 et
