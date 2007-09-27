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

#ifndef TESTLABELPARSER_H
#define TESTLABELPARSER_H

#include <QObject>
#include <labelparser.h>

class TestLabelParser : public QObject
{
  Q_OBJECT
  private:
    void dumpAttributes(Label::Chunk *l, QString indent);
    void dumpTree(Label::Chunk *l, QString indent = "");
    void dumpTree(Label::Parsed *l);
  private Q_SLOTS:
    void cleanupTestCase();

    void testLabelParser();
};

#endif

// vim: ts=2 sw=2 et
