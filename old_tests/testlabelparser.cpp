/*
 *  Copyright 2004, The University of Toronto
 *  Licensed under GPL.
 */

#include "ksttestcase.h"
#include <kstdataobjectcollection.h>
#include <labelparser.h>

static void exitHelper() {
  KST::vectorList.clear();
  KST::scalarList.clear();
  KST::dataObjectList.clear();
}


static void dumpAttributes(Label::Chunk *l, QString indent) {
  printf("%sb:%d i:%d u:%d c:%s\n", indent.toLatin1().data(), l->attributes.bold, l->attributes.italic, l->attributes.underline, l->attributes.color.name().toLatin1().data());
}


static void dumpTree(Label::Chunk *l, QString indent = "") {
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


static void dumpTree(Label::Parsed *l) {
  dumpTree(l->chunk);
}


int rc = KstTestSuccess;

#define doTest(x) testAssert(x, QString("Line %1").arg(__LINE__))
#define doTestD(x, y) testAssert(x, QString("%1: %2").arg(__LINE__).arg(y))

void testAssert(bool result, const QString& text = "Unknown") {
  if (!result) {
    KstTestFailed();
    printf("Test [%s] failed.\n", text.toLatin1().data());
  }
}


void doTests() {
  Label::Parsed *parsed = Label::parse("");
  doTestD(parsed != 0L, "Empty label parsed");
  doTest(parsed->chunk->prev == 0L);
  doTest(parsed->chunk->next == 0L);
  doTest(parsed->chunk->text == QString::null);
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("a");
  doTest(parsed != 0L);
  doTest(parsed->chunk->prev == 0L);
  doTest(parsed->chunk->next == 0L);
  doTest(parsed->chunk->text == "a");
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\]");
  doTest(parsed != 0L);
  doTest(parsed->chunk->prev == 0L);
  doTest(parsed->chunk->next == 0L);
  doTest(parsed->chunk->text == "]");
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\\\");
  doTest(parsed != 0L);
  doTest(parsed->chunk->prev == 0L);
  doTest(parsed->chunk->next == 0L);
  doTest(parsed->chunk->text == "\\");
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\tau");
  doTest(parsed != 0L);
  doTest(parsed->chunk->prev == 0L);
  doTest(parsed->chunk->next == 0L);
  doTest(parsed->chunk->text == QChar(0x3A4 + 0x20));
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\t");
  doTest(parsed != 0L);
  doTest(parsed->chunk->text == "");
  doTest(parsed->chunk->tab == true);
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\n");
  doTest(parsed != 0L);
  doTest(parsed->chunk->text == "");
  doTest(parsed->chunk->linebreak == true);
  doTest(parsed->chunk->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("\\tau\\Theta");
  doTest(parsed != 0L);
  doTest(parsed->chunk->next == 0L);
  doTest(parsed->chunk->text[0] == QChar(0x3A4 + 0x20));
  doTest(parsed->chunk->text[1] == QChar(0x398));
  doTest(parsed->chunk->text.length() == 2);
  delete parsed;

  Label::Chunk *c = 0L;

  parsed = Label::parse("\\taufoo bar\\n\\Theta");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next != 0L);
  doTest(c->text == QString(QChar(0x3A4 + 0x20)) + "foo bar");
  c = c->next;
  doTest(c->prev != 0L);
  doTest(c->next != 0L);
  doTest(c->text == "");
  doTest(c->linebreak == true);
  c = c->next;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->text == QChar(0x398));
  delete parsed;

  parsed = Label::parse("x^y^z");
  doTest(parsed == 0L);

  parsed = Label::parse("x_y_z");
  doTest(parsed == 0L);

  parsed = Label::parse("x^y_z");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->prev == 0L);
  doTest(c->next == 0L);
  doTest(c->up != 0L);
  doTest(c->down != 0L);
  doTest(c->text == "x");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->up;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "y");
  doTest(c->vOffset == Label::Chunk::Up);
  c = c->prev->down;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "z");
  doTest(c->vOffset == Label::Chunk::Down);
  delete parsed;

  parsed = Label::parse("x^{}");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->prev == 0L);
  doTest(c->next == 0L);
  doTest(c->up != 0L);
  doTest(c->down == 0L);
  doTest(c->text == "x");
  doTest(c->group == 0L);
  doTest(c->vOffset == Label::Chunk::None);
  c = c->up;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group != 0L);
  doTest(c->vOffset == Label::Chunk::Up);
  c = c->group;
  doTest(c->text == "");
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group == 0L);
  doTest(c->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x^{y+1}_{z-1}");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->prev == 0L);
  doTest(c->next == 0L);
  doTest(c->up != 0L);
  doTest(c->down != 0L);
  doTest(c->text == "x");
  doTest(c->group == 0L);
  doTest(c->vOffset == Label::Chunk::None);
  c = c->up;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group != 0L);
  doTest(c->vOffset == Label::Chunk::Up);
  c = c->group;
  doTest(c->text == "y+1");
  doTest(c->group == 0L);
  doTest(c->vOffset == Label::Chunk::None);
  c = c->prev->prev->down;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group != 0L);
  doTest(c->vOffset == Label::Chunk::Down);
  c = c->group;
  doTest(c->text == "z-1");
  doTest(c->group == 0L);
  doTest(c->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x\\^y");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->prev == 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "x^y");
  doTest(c->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x^{y^{q+1} + 1}_{z-1}");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->prev == 0L);
  doTest(c->next == 0L);
  doTest(c->up != 0L);
  doTest(c->down != 0L);
  doTest(c->text == "x");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->up;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group != 0L);
  doTest(c->vOffset == Label::Chunk::Up);
  c = c->group;
  doTest(c->text == "y");
  doTest(c->prev != 0L);
  doTest(c->next != 0L);
  doTest(c->up != 0L);
  doTest(c->down == 0L);
  doTest(c->group == 0L);
  doTest(c->vOffset == Label::Chunk::None);
  c = c->up;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group != 0L);
  doTest(c->vOffset == Label::Chunk::Up);
  c = c->group;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "q+1");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->prev->prev->next;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == " + 1");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->prev->prev->prev->down;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->group);
  doTest(c->vOffset == Label::Chunk::Down);
  c = c->group;
  doTest(c->prev != 0L);
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "z-1");
  doTest(c->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("{2*2}");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next == 0L);
  doTest(c->group != 0L);
  doTest(c->vOffset == Label::Chunk::None);
  c = c->group;
  doTest(c->text == "2*2");
  doTest(c->vOffset == Label::Chunk::None);
  delete parsed;

  parsed = Label::parse("x^100");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next != 0L);
  doTest(c->up != 0L);
  doTest(c->text == "x");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->next;
  doTest(c->next == 0L);
  doTest(c->text == "00");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->prev->up;
  doTest(c->next == 0L);
  doTest(c->text == "1");
  doTest(c->vOffset == Label::Chunk::Up);
  delete parsed;

  parsed = Label::parse("x^100*200");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next != 0L);
  doTest(c->up != 0L);
  doTest(c->text == "x");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->next;
  doTest(c->next == 0L);
  doTest(c->text == "00*200");
  doTest(c->vOffset == Label::Chunk::None);
  c = c->prev->up;
  doTest(c->next == 0L);
  doTest(c->text == "1");
  doTest(c->vOffset == Label::Chunk::Up);
  delete parsed;

  parsed = Label::parse("[ a ^label ]");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "a ^label");
  doTest(c->scalar);
  delete parsed;

  parsed = Label::parse("[vector[2]]");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "vector");
  doTest(c->expression == "2");
  doTest(c->vector);
  delete parsed;

  parsed = Label::parse("[a][b]");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next != 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "a");
  doTest(c->scalar);
  c = c->next;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "b");
  doTest(c->scalar);
  delete parsed;

  parsed = Label::parse("[a]*[b]");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next != 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "a");
  doTest(c->scalar);
  c = c->next;
  doTest(c->next != 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "*");
  doTest(!c->scalar);
  c = c->next;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "b");
  doTest(c->scalar);
  delete parsed;

  parsed = Label::parse("[x]^[a]_[b][c]");
  doTest(parsed != 0L);
  c = parsed->chunk;
  doTest(c->next != 0L);
  doTest(c->up != 0L);
  doTest(c->down != 0L);
  doTest(c->text == "x");
  doTest(c->scalar);
  c = c->up;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "a");
  doTest(c->scalar);
  c = c->prev->down;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "b");
  doTest(c->scalar);
  c = c->prev->next;
  doTest(c->next == 0L);
  doTest(c->up == 0L);
  doTest(c->down == 0L);
  doTest(c->text == "c");
  doTest(c->scalar);
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
  doTest(parsed != 0L);
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


int main(int argc, char **argv) {
  atexit(exitHelper);

  QCoreApplication app(argc, argv);

  doTests();
  // Don't put tests in main because we need to ensure that no KstObjects
  // remain past the exit handler

  exitHelper(); // need to run it here before kapp goes away in some cases.
  if (rc == KstTestSuccess) {
    printf("All tests passed!\n");
  }
  return -rc;
}

// vim: ts=2 sw=2 et
