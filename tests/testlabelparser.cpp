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

#include "testlabelparser.h"

#include <QtTest>

#include <datacollection.h>
#include <objectstore.h>

static Kst::ObjectStore _store;

void TestLabelParser::cleanupTestCase() {
  _store.clear();
}


void TestLabelParser::dumpAttributes(Label::Chunk *l, QString indent) {
  printf("%sb:%d i:%d u:%d c:%s\n", indent.toLatin1().data(), l->attributes.bold, l->attributes.italic, l->attributes.underline, l->attributes.color.name().toLatin1().data());
}


void TestLabelParser::dumpTree(Label::Chunk *l, QString indent) {
  while (l) {
    printf("%sNode: %p %s[%s]%s\n", indent.toLatin1().data(), l, l->vOffset == Label::Chunk::None ? "" : (l->vOffset == Label::Chunk::Up ? "U " : "D "), l->text.toLatin1().data(), l->group ? " [G]" : "");
    printf("%sPrev: %p Next: %p Up: %p Down: %p\n", indent.toLatin1().data(), l->prev, l->next, l->up, l->down);
    dumpAttributes(l, indent);
    if (l->group) {
      dumpTree(l->group, indent + "   ");
    }
    if (l->up) {
      dumpTree(l->up, indent + "   ");
    }
    if (l->down) {
      dumpTree(l->down, indent + "   ");
    }
    l = l->next;
  }
}


void TestLabelParser::dumpTree(Label::Parsed *l) {
  dumpTree(l->chunk);
}



void TestLabelParser::testLabelParser() {
  Label::Parsed *parsed = Label::parse("");
  QVERIFY(parsed != 0L);
  QVERIFY(parsed->chunk->prev == 0L);
  QVERIFY(parsed->chunk->next == 0L);
  QVERIFY(parsed->chunk->text == QString::null);
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("a");
  QVERIFY(parsed != 0L);
  QVERIFY(parsed->chunk->prev == 0L);
  QVERIFY(parsed->chunk->next == 0L);
  QCOMPARE(parsed->chunk->text, QString("a"));
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\]");
  QVERIFY(parsed != 0L);
  QVERIFY(parsed->chunk->prev == 0L);
  QVERIFY(parsed->chunk->next == 0L);
  QCOMPARE(parsed->chunk->text, QString("]"));
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\\\");
  QVERIFY(parsed != 0L);
  QVERIFY(parsed->chunk->prev == 0L);
  QVERIFY(parsed->chunk->next == 0L);
  QCOMPARE(parsed->chunk->text, QString("\\"));
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\tau");
  QVERIFY(parsed != 0L);
  QVERIFY(parsed->chunk->prev == 0L);
  QVERIFY(parsed->chunk->next == 0L);
  QCOMPARE(parsed->chunk->text, QString(QChar(0x3A4 + 0x20)));
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\t");
  QVERIFY(parsed != 0L);
  QCOMPARE(parsed->chunk->text, QString(""));
  QCOMPARE(parsed->chunk->tab, true);
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\n");
  QVERIFY(parsed != 0L);
  QCOMPARE(parsed->chunk->text, QString(""));
  QCOMPARE(parsed->chunk->linebreak, true);
  QCOMPARE(parsed->chunk->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\tau\\Theta");
  QVERIFY(parsed != 0L);
  QVERIFY(parsed->chunk->next == 0L);
  QVERIFY(parsed->chunk->text[0] == QChar(0x3A4 + 0x20));
  QVERIFY(parsed->chunk->text[1] == QChar(0x398));
  QCOMPARE(parsed->chunk->text.length(), 2);
  delete parsed;

  Label::Chunk *c = 0L;

  parsed = Label::parse("\\taufoo bar\\n\\Theta");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next != 0L);
  QCOMPARE(c->text, QString(QChar(0x3A4 + 0x20)) + "foo bar");
  c = c->next;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next != 0L);
  QCOMPARE(c->text, QString(""));
  QCOMPARE(c->linebreak, true);
  c = c->next;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QCOMPARE(c->text, QString(QChar(0x398)));
  delete parsed;

  parsed = Label::parse("x^y^z");
  QVERIFY(parsed == 0L);

  parsed = Label::parse("x_y_z");
  QVERIFY(parsed == 0L);

  parsed = Label::parse("x^y_z");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->prev == 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up != 0L);
  QVERIFY(c->down != 0L);
  QCOMPARE(c->text, QString("x"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->up;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("y"));
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  c = c->prev->down;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("z"));
  QCOMPARE(c->vOffset, Label::Chunk::Down);
  delete parsed;

  parsed = Label::parse("x^{}");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->prev == 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up != 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("x"));
  QVERIFY(c->group == 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->up;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group != 0L);
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  c = c->group;
  QCOMPARE(c->text, QString(""));
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group == 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x^{y+1}_{z-1}");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->prev == 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up != 0L);
  QVERIFY(c->down != 0L);
  QCOMPARE(c->text, QString("x"));
  QVERIFY(c->group == 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->up;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group != 0L);
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  c = c->group;
  QCOMPARE(c->text, QString("y+1"));
  QVERIFY(c->group == 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->prev->prev->down;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group != 0L);
  QCOMPARE(c->vOffset, Label::Chunk::Down);
  c = c->group;
  QCOMPARE(c->text, QString("z-1"));
  QVERIFY(c->group == 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x\\^y");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->prev == 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("x^y"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x^{y^{q+1} + 1}_{z-1}");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->prev == 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up != 0L);
  QVERIFY(c->down != 0L);
  QCOMPARE(c->text, QString("x"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->up;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group != 0L);
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  c = c->group;
  QCOMPARE(c->text, QString("y"));
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next != 0L);
  QVERIFY(c->up != 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group == 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->up;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group != 0L);
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  c = c->group;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("q+1"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->prev->prev->next;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString(" + 1"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->prev->prev->prev->down;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QVERIFY(c->group);
  QCOMPARE(c->vOffset, Label::Chunk::Down);
  c = c->group;
  QVERIFY(c->prev != 0L);
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("z-1"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("{2*2}");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next == 0L);
  QVERIFY(c->group != 0L);
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->group;
  QCOMPARE(c->text, QString("2*2"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x^100");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next != 0L);
  QVERIFY(c->up != 0L);
  QCOMPARE(c->text, QString("x"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->next;
  QVERIFY(c->next == 0L);
  QCOMPARE(c->text, QString("00"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->prev->up;
  QVERIFY(c->next == 0L);
  QCOMPARE(c->text, QString("1"));
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  delete parsed;

  parsed = Label::parse("x^100*200");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next != 0L);
  QVERIFY(c->up != 0L);
  QCOMPARE(c->text, QString("x"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->next;
  QVERIFY(c->next == 0L);
  QCOMPARE(c->text, QString("00*200"));
  QCOMPARE(c->vOffset, Label::Chunk::None);
  c = c->prev->up;
  QVERIFY(c->next == 0L);
  QCOMPARE(c->text, QString("1"));
  QCOMPARE(c->vOffset, Label::Chunk::Up);
  delete parsed;

  parsed = Label::parse("[ a ^label ]");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text,QString( "a ^label"));
  QVERIFY(c->scalar);
  delete parsed;

  parsed = Label::parse("[vector[2]]");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("vector"));
  QCOMPARE(c->expression, QString("2"));
  QVERIFY(c->vector);
  delete parsed;

  parsed = Label::parse("[a][b]");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next != 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("a"));
  QVERIFY(c->scalar);
  c = c->next;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("b"));
  QVERIFY(c->scalar);
  delete parsed;

  parsed = Label::parse("[a]*[b]");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next != 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("a"));
  QVERIFY(c->scalar);
  c = c->next;
  QVERIFY(c->next != 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("*"));
  QVERIFY(!c->scalar);
  c = c->next;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("b"));
  QVERIFY(c->scalar);
  delete parsed;

  parsed = Label::parse("[x]^[a]_[b][c]");
  QVERIFY(parsed != 0L);
  c = parsed->chunk;
  QVERIFY(c->next != 0L);
  QVERIFY(c->up != 0L);
  QVERIFY(c->down != 0L);
  QCOMPARE(c->text, QString("x"));
  QVERIFY(c->scalar);
  c = c->up;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("a"));
  QVERIFY(c->scalar);
  c = c->prev->down;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("b"));
  QVERIFY(c->scalar);
  c = c->prev->next;
  QVERIFY(c->next == 0L);
  QVERIFY(c->up == 0L);
  QVERIFY(c->down == 0L);
  QCOMPARE(c->text, QString("c"));
  QVERIFY(c->scalar);
  delete parsed;

  parsed = Label::parse("Ends in a \\");
  delete parsed;

  parsed = Label::parse("_");
  delete parsed;

  parsed = Label::parse("A weird { case }");
  delete parsed;

  parsed = Label::parse("x^y^foo{ case }");
  delete parsed;

  parsed = Label::parse("x^5\\");
  delete parsed;

  parsed = Label::parse("}");
  delete parsed;

  parsed = Label::parse("Average period (ADU)");
  delete parsed;

  parsed = Label::parse("PSD (ADU/{Hz^{1/2}})");
  delete parsed;

  parsed = Label::parse("\\Sigma^{(x+5)^2}_{5+1} + \\Pi^{\\Sigma^{i-j}_{i+j}}_{x = 0} + 5");
  QVERIFY(parsed != 0L);
  // more to test here
  delete parsed;

  parsed = Label::parse("A test \\textbf{bold \\textit{italicbold}} \\textit{italic}\n42^{\\textbf{42}^{42}} 42^\\textbf{42^{42}} 42^{42}\n\\underline{a whole\\tregion  underlined}");
  //dumpTree(parsed);
  // FIXME
  delete parsed;

  parsed = Label::parse("\\textbf{\\textit{italicbold}bold}");
  // FIXME
  delete parsed;

  parsed = Label::parse("\\textcolor{red}{red}black");
  // FIXME
  delete parsed;

  parsed = Label::parse("\\textbf{bold}unbold");
  // FIXME
  delete parsed;

  // more to test...
}


#ifdef KST_USE_QTEST_MAIN
QTEST_MAIN(TestLabelParser)
#endif

// vim: ts=2 sw=2 et
