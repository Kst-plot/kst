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

#ifndef TESTDATASOURCE_H
#define TESTDATASOURCE_H

#include <QObject>
#include <QStringList>

class TestDataSource : public QObject
{
  Q_OBJECT
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
