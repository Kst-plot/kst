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

#ifndef TESTEQPARSER_H
#define TESTEQPARSER_H

#include <QObject>

class TestEqParser : public QObject
{
  Q_OBJECT
  private:
    bool validateText(const char *equation, const char *expect);
    bool validateParserFailures(const char *equation);
    bool validateEquation(const char *equation, double x, double result, const double tol = 0.00000000001);
  private Q_SLOTS:
    void cleanupTestCase();

    void testEqParser();
};

#endif

// vim: ts=2 sw=2 et
