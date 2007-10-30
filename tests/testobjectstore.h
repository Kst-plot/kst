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

#ifndef TESTOBJECTSTORE_H
#define TESTOBJECTSTORE_H

#include <QObject>

class TestObjectStore : public QObject
{
  Q_OBJECT

  private Q_SLOTS:
    void cleanupTestCase();

    void testObjectStore();
};

#endif

// vim: ts=2 sw=2 et
