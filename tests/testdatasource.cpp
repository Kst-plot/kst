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
#include <datacollection.h>
#include <objectstore.h>

#define protected public
#include <datavector.h>
#include <datamatrix.h>
#undef protected

static Kst::ObjectStore _store;

void TestDataSource::initTestCase() {
  Kst::DataSource::init();
  _plugins = Kst::DataSource::pluginList();
}


void TestDataSource::cleanupTestCase() {
  _store.clear();
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

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII file"));
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

    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "1", 0, -1, 0, false, false);
    rvp->update();
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 3);
    QCOMPARE(rvp->value()[0], 2.0);
    QCOMPARE(rvp->value()[1], 1.0);
    QCOMPARE(rvp->value()[2], 0.2);

    rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", 0, -1, 0, false, false);
    rvp->update();
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

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII file"));
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

    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "1", 0, -1, 0, false, false);
    rvp->update();
    rvp->unlock();

    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 4);
    QCOMPARE(rvp->value()[0], 0.2);
    QVERIFY(rvp->value()[1] != rvp->value()[1]);

    QVERIFY(rvp->value()[2] == INF);

    QCOMPARE(rvp->value()[3], 0.0);

    rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "2", 0, -1, 0, false, false);
    rvp->writeLock();
    rvp->update();
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

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII file"));
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


    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "1", 0, -1, 0, false, false);
    rvp->update();
    rvp->unlock();

    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 1); // Are we allowed to have vectors of 1?
    QCOMPARE(rvp->value()[0], 2.0);

    rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "2", 0, -1, 0, false, false);
    rvp->update();
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

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, tf.fileName());

    QVERIFY(dsp);
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII file"));
    tf.close();
  }

  {
    QTemporaryFile tf;
    tf.open();
    QTextStream ts(&tf);
    for (int i = 0; i < 39000; ++i) {
      ts << i << " " <<  i + 100 << " " << i + 1000 << endl;
    }

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, tf.fileName());
    dsp->update();

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("ASCII file"));
    QCOMPARE(dsp->frameCount(QString::null), 39000);
    QCOMPARE(dsp->frameCount("1"), 39000);
    QCOMPARE(dsp->frameCount("2"), 39000);
    QCOMPARE(dsp->frameCount("3"), 39000);
    QCOMPARE(dsp->frameCount("INDEX"), 39000);
    QCOMPARE(dsp->fieldList().count(), 4);
    QVERIFY(!dsp->isEmpty());

    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "1", 0, -1, 0, false, false);
    rvp->update();
    rvp->unlock();
    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 39000);

    rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "2", 0, -1, 10, true, false);
    rvp->update();
    rvp->unlock();

    QVERIFY(rvp->isValid());
    QCOMPARE(rvp->length(), 3900);
    QCOMPARE(rvp->value()[0], 100.0);
    QCOMPARE(rvp->value()[1], 110.0);
    QCOMPARE(rvp->value()[2], 120.0);
    QCOMPARE(rvp->value()[3898], 39080.0);

    rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "3", 0, -1, 10, true, true);
    rvp->update();
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
    QString fifteen = QDir::currentPath() + QDir::separator() + QString("tests") +
                      QDir::separator() + QString("dirfile_testcase") +
                      QDir::separator() + QString("15count");
    if (!QFile::exists(fifteen + QDir::separator() + "format")) {
      QSKIP("...unable to perform test.  Datafile missing", SkipAll);
    }
    printf("Opening dirfile = %s for test.\n", fifteen.toLatin1().data());

    Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, fifteen);
    dsp->update();

    QVERIFY(dsp);
    QVERIFY(dsp->isValid());
    QVERIFY(!dsp->hasConfigWidget());
    QCOMPARE(dsp->fileType(), QLatin1String("Directory of Binary Files"));
    QVERIFY(dsp->isValidField("INDEX"));
    QCOMPARE(dsp->frameCount("INDEX"), 17);
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

    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", 0, -1, 5, true, false);
    rvp->update();
    rvp->unlock();

    //We should have length equal to three...  items {0, 5, 10}
    //NOTE: The last item, index #14, does not fit in the skip boundary...
    QCOMPARE(3, rvp->length());
    QCOMPARE(0.0, rvp->value(0));
    QCOMPARE(5.0, rvp->value(1));
    QCOMPARE(10.0, rvp->value(2));

    QCOMPARE(15, rvp->numFrames());
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
    //Skip FIVE frames start at 3...
    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", 3, -1, 5, true, false);
    rvp->update();
    rvp->unlock();

    //We should have length equal to two...  items {5, 10}
    QCOMPARE(2, rvp->length());
    QCOMPARE(5.0, rvp->value(0));
    QCOMPARE(10.0, rvp->value(1));

    QCOMPARE(10, rvp->numFrames());
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
    //Skip FIVE frames 11 from end...
    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", 0, 11, 5, true, false);
    rvp->update();
    rvp->unlock();

    //We should have length equal to two...  items {0, 5}
    QCOMPARE(2, rvp->length());
    QCOMPARE(0.0, rvp->value(0));
    QCOMPARE(5.0, rvp->value(1));

    QCOMPARE(10, rvp->numFrames());
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
    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", -1, 10, 5, true, false);
    rvp->update();
    rvp->unlock();

    //We should have length equal to two...  items {5, 10}
    QCOMPARE(2, rvp->length());
    QCOMPARE(10.0, rvp->value(0));
    QCOMPARE(15.0, rvp->value(1));

    QCOMPARE(10, rvp->numFrames());
    QCOMPARE(10, rvp->startFrame());

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

void TestDataSource::testQImageSource() {
  bool ok = true;

  if (!_plugins.contains("QImage Source Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);

  //These tests assume that the image kst.png exists in src/images
  QString imageFile = QDir::currentPath() + QDir::separator() + QString("src") +
                      QDir::separator() + QString("images") + QDir::separator() + QString("kst.png");

  if (!QFile::exists(imageFile)) {
    QSKIP("...unable to perform test.  Image file missing.", SkipAll);
  }

  printf("Opening image = %s for test.\n", imageFile.toLatin1().data());

  Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, imageFile);
  dsp->update();

  QVERIFY(dsp);
  QVERIFY(dsp->isValid());
  QVERIFY(!dsp->hasConfigWidget());
  QCOMPARE(dsp->fileType(), QLatin1String("QImage image"));
  QVERIFY(dsp->isValidField("INDEX"));
  QCOMPARE(dsp->frameCount("INDEX"), 1024);
  QVERIFY(dsp->isValidField("RED"));
  QVERIFY(dsp->isValidField("BLUE"));
  QVERIFY(dsp->isValidField("GREEN"));
  QVERIFY(dsp->isValidField("GRAY"));
  QVERIFY(!dsp->isValidField("foo"));

  //TODO test samples per frame?

  QCOMPARE(dsp->fileName(), imageFile);
  QCOMPARE(dsp->fieldList().count(), 5);
  QVERIFY(dsp->fieldListIsComplete());

  QVERIFY(!dsp->isEmpty());

  QVERIFY(dsp->isValidMatrix("RED"));
  QVERIFY(dsp->isValidMatrix("BLUE"));
  QVERIFY(dsp->isValidMatrix("GREEN"));
  QVERIFY(dsp->isValidMatrix("GRAY"));
  QVERIFY(!dsp->isValidMatrix("foo"));

  {
    Kst::DataMatrixPtr matrix = Kst::kst_cast<Kst::DataMatrix>(_store.createObject<Kst::DataMatrix>());
    matrix->change(dsp, "GRAY", 0, 0,
        -1, -1, false,
        false, 0, 0, 0, 1, 1);

    matrix->writeLock();
    matrix->update();
    matrix->unlock();


    QCOMPARE(matrix->xNumSteps(), 32);
    QCOMPARE(matrix->yNumSteps(), 32);
    QCOMPARE(matrix->xStepSize(), 1.0);
    QCOMPARE(matrix->yStepSize(), 1.0);
    QCOMPARE(matrix->minX(), 0.0);
    QCOMPARE(matrix->minY(), 0.0);

    QCOMPARE(matrix->minValue(), 0.0);
    QCOMPARE(matrix->maxValue(), 255.0);

    QCOMPARE(matrix->minValuePositive(), 7.0);

    QCOMPARE(matrix->sampleCount(), 1024);

    QCOMPARE(matrix->value(0, 0, &ok), 0.0);
    QVERIFY(ok);
    QCOMPARE(matrix->value(25, 3, &ok), 81.0);
    QVERIFY(ok);
  }
  {
    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", 0, -1, 1, false, false);
    rvp->update();
    rvp->unlock();

    QCOMPARE(1024, rvp->length());
    QCOMPARE(0.0, rvp->value(0));
    QCOMPARE(1.0, rvp->value(1));
    QCOMPARE(2.0, rvp->value(2));
    QCOMPARE(1023.0, rvp->value(1023));

    QCOMPARE(1024, rvp->numFrames());
    QCOMPARE(0, rvp->startFrame());

    QCOMPARE(-1, rvp->reqNumFrames());
    QCOMPARE(0, rvp->reqStartFrame());

    QCOMPARE(true, rvp->readToEOF());
    QCOMPARE(false, rvp->countFromEOF());
    QCOMPARE(false, rvp->doSkip());
    QCOMPARE(0, rvp->skip());
    QCOMPARE(false, rvp->doAve());
  }
}


void TestDataSource::testFITSImage() {
  bool ok = true;

  if (!_plugins.contains("FITS Image Source Reader"))
    QSKIP("...couldn't find plugin.", SkipAll);

  //These tests assume that the fits image test.fits exists in tests/fitsimage_testcase
  QString imageFile = QDir::currentPath() + QDir::separator() + QString("tests") +
                      QDir::separator() + QString("fitsimage_testcase") + QDir::separator() + QString("test.fits");

  if (!QFile::exists(imageFile)) {
    QSKIP("...unable to perform test.  Image file missing.", SkipAll);
  }

  printf("Opening image = %s for test.\n", imageFile.toLatin1().data());

  Kst::DataSourcePtr dsp = Kst::DataSource::loadSource(&_store, imageFile);
  dsp->update();

  QVERIFY(dsp);
  QVERIFY(dsp->isValid());
  QVERIFY(!dsp->hasConfigWidget());
  QCOMPARE(dsp->fileType(), QLatin1String("FITS image"));
  QVERIFY(dsp->isValidField("INDEX"));
  QCOMPARE(dsp->frameCount("INDEX"), 58800);
  QVERIFY(dsp->isValidField("1"));
  QVERIFY(!dsp->isValidField("foo"));

  //TODO test samples per frame?

  QCOMPARE(dsp->fileName(), imageFile);
  QCOMPARE(dsp->fieldList().count(), 2);
  QVERIFY(dsp->fieldListIsComplete());

  QVERIFY(!dsp->isEmpty());

  QVERIFY(dsp->isValidMatrix("1"));
  QVERIFY(!dsp->isValidMatrix("foo"));

  {
    Kst::DataMatrixPtr matrix = Kst::kst_cast<Kst::DataMatrix>(_store.createObject<Kst::DataMatrix>());
    matrix->change(dsp, "1", 0, 0,
        -1, -1, false, false, 0, 0, 0, 1, 1);

    matrix->writeLock();
    matrix->update();
    matrix->unlock();

    QCOMPARE(matrix->xNumSteps(), 280);
    QCOMPARE(matrix->yNumSteps(), 210);

    QCOMPARE(matrix->xStepSize(), 1.0);
    QCOMPARE(matrix->yStepSize(), 1.0);
    QCOMPARE(matrix->minX(), 0.0);
    QCOMPARE(matrix->minY(), 0.0);

    QCOMPARE(matrix->minValue(), -86.297431945800781);
    QCOMPARE(matrix->maxValue(), 487.873565673828125);

    QCOMPARE(matrix->minValuePositive(), 0.000308976683300);

    QCOMPARE(matrix->sampleCount(), 58800);

    QCOMPARE(matrix->value(0, 0, &ok), 0.0);
    QVERIFY(!ok);

    QCOMPARE(matrix->value(12, 61, &ok), -17.691156387329102);
    QVERIFY(ok);
  }
  {
    Kst::DataVectorPtr rvp = Kst::kst_cast<Kst::DataVector>(_store.createObject<Kst::DataVector>());

    rvp->writeLock();
    rvp->change(dsp, "INDEX", 0, -1, 1, false, false);
    rvp->update();
    rvp->unlock();

    QCOMPARE(58800, rvp->length());
    QCOMPARE(0.0, rvp->value(0));
    QCOMPARE(1.0, rvp->value(1));
    QCOMPARE(2.0, rvp->value(2));
    QCOMPARE(1023.0, rvp->value(1023));

    QCOMPARE(58800, rvp->numFrames());
    QCOMPARE(0, rvp->startFrame());

    QCOMPARE(-1, rvp->reqNumFrames());
    QCOMPARE(0, rvp->reqStartFrame());

    QCOMPARE(true, rvp->readToEOF());
    QCOMPARE(false, rvp->countFromEOF());
    QCOMPARE(false, rvp->doSkip());
    QCOMPARE(0, rvp->skip());
    QCOMPARE(false, rvp->doAve());
  }
}

// vim: ts=2 sw=2 et
