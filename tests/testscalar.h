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

#ifndef TESTSCALAR_H
#define TESTSCALAR_H

#include <QObject>
#include <QDomDocument>

class SListener : public QObject
{
  Q_OBJECT

  public:
    SListener();
    virtual ~SListener();

    int _trigger;

  public Q_SLOTS:
    void trigger(qint64);
};


class TestScalar : public QObject
{
  Q_OBJECT
  private:
    QDomDocument makeDOMDocument(const QString& tag, const QString& val, bool orphan = false);
  private Q_SLOTS:
    void cleanupTestCase();

    void testScalar();
};

#endif

// vim: ts=2 sw=2 et
