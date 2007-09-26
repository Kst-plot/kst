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

#include <QCoreApplication>

#include <QtTest>

#include "testdatasource.h"
#include "testvector.h"
#include "testscalar.h"
#include "testmatrix.h"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  TestDataSource test1;
  QTest::qExec(&test1, argc, argv);

  TestVector test2;
  QTest::qExec(&test2, argc, argv);

  TestScalar test3;
  QTest::qExec(&test3, argc, argv);

  TestMatrix test4;
  QTest::qExec(&test4, argc, argv);

  return 0;
}

// vim: ts=2 sw=2 et
