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

#ifndef TESTDATASOURCES_H
#define TESTDATASOURCES_H

// HACK to get at methods we shouldn't be getting at
#define protected public
#include <kstdatasource.h>
#undef protected

#include <QtTest>

#include <QDir>
#include <QObject>
#include <QFile>
#include <QSettings>
#include <QTemporaryFile>

#include <kstrvector.h>
#include <kstdatacollection.h>
#include <kstdataobjectcollection.h>

#include <math.h>

class TestDataSources : public QObject
{
  Q_OBJECT
  public:
    TestDataSources() : QObject() {}
    virtual ~TestDataSources() {}

  private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testAscii();
    void testDirfile();
    void testCDF();
    void testFrame();
    void testIndirect();
    void testLFI();
    void testPlanck();
    void testStdin();

  private:
    QStringList _plugins;
};

#endif

// vim: ts=2 sw=2 et
