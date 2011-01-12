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

#ifndef KST_TEST_H
#define KST_TEST_H

#include <QtTest>
#include <QByteArray>

#include "math_kst.h"

template<>
bool QTest::qCompare(double const& val, double const * const& nan,  const char *actual, const char *expected,
                    const char *file, int line)
{
	return qCompare(
		QByteArray((char*) &val, sizeof(double)),
		QByteArray((char*) nan, sizeof(double)),
		actual, expected, file, line);
}

#endif

// vim: ts=2 sw=2 et
