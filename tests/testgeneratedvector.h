/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTGENERATEVECTOR_H
#define TESTGENERATEVECTOR_H

#include <QObject>

class TestGeneratedVector : public QObject
{
  Q_OBJECT
  private Q_SLOTS:
    void cleanupTestCase();

    void testGeneratedVector();
};

#endif

// vim: ts=2 sw=2 et
