/***************************************************************************
                             labelparser.cpp
                             ----------------
    begin                : Dec 14 2004
                           Copyright (C) 2004, The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "labelparser.h"

#include <assert.h>
#include <stdlib.h>

#include <qregexp.h>
#include <qstring.h>

using namespace Label;

// Debug output for Parsing - 0 Off 1 On
#define DEBUG_PARSING 0

#if DEBUG_PARSING
#define dumpattr(node, text) do { printf("%s: bold:%d italic:%d underline:%d\n", text, (node)->attributes.bold, (node)->attributes.italic, (node)->attributes.underline); } while(0)
#else
#define dumpattr(node, text)
#endif

Chunk::Chunk(Chunk *parent, VOffset dir, bool isGroup, bool inherit)
: next(0L), prev(0L), up(0L), down(0L), group(0L), scalar(false), linebreak(false), tab(false), vector(false), vOffset(dir) {
  assert(parent || vOffset == None);
  if (parent) {  // attach and inherit
    switch (vOffset) {
      case None:
        if (isGroup) {
          parent->group = this;
        } else {
          while (parent->next) {
            parent = parent->next;
          }
          parent->next = this;
        }
        break;
      case Up:
        assert(!parent->up);
        parent->up = this;
        break;
      case Down:
        assert(!parent->down);
        parent->down = this;
        break;
    }

    if (inherit) {
      // inherit these properties from the parent
      attributes = parent->attributes;
    }

    prev = parent;
  }
}


Chunk::~Chunk() {
  // These are set to 0 by the child if they're non-zero
  delete next;
  delete up;
  delete down;
  delete group;
  group = 0L;
  if (prev) {
    switch (vOffset) {
      case None:
        prev->next = 0L;
        // Note: kind of does the wrong thing if we're a group...  no issue though
        break;
      case Up:
        prev->up = 0L;
        break;
      case Down:
        prev->down = 0L;
        break;
    }
    prev = 0L;
  }
}


bool Chunk::locked() const {
  return scalar || group || linebreak || tab || vector;
}


Parsed::Parsed() : chunk(0L) {
}


Parsed::~Parsed() {
  delete chunk;
  chunk = 0L;
}


inline void setNormalChar(QChar c, Chunk **tail) {
  if (*tail && !(*tail)->locked()) {
    (*tail)->text += c;
  } else {
    Chunk *f = new Chunk(*tail, Chunk::None, false, true);
    f->text += c;
    *tail = f;
  }
}


inline QColor parseColor(const QString& txt, int *skip) {
  const int end = txt.indexOf('}');
  if (skip) {
    *skip = end;
  }

  if (end == -1) {
    return QColor();
  }

  const QString endPart = txt.left(end);

  QColor color(endPart); // This one is slow.  If we support our own formats
                         // outside of QColor, make sure that this is called
                         // -after- we try our own formats.  Every cycle
                         // counts in here.
#if 0
  // This is rr,gg,bb support.  I'm not sure about supporting H,S,V or even
  // about compatibility with LaTeX so for now we don't support it.  If it's
  // ever re-enabled, make sure that testcases are added.
  if (!color.isValid()) {
    // the color is in the format "r,g,b"
    QStringList components = QStringList::split(',', endPart, true);
    if (components.count() == 3) {
      int colors[3] = { 0, 0, 0 };
      int base = 10;

      // assume the colors are given as decimal numbers unless we have a hex value in the string
      if (endPart.find(QRegExp("[A-F]", false)) != -1) {
        base = 16;
      }

      bool ok = true;
      colors[0] = components[0].toInt(&ok, base);
      if (ok) {
        colors[1] = components[1].toInt(&ok, base);
      }
      if (ok) {
        colors[2] = components[2].toInt(&ok, base);
      }

      if (ok) {
        color.setRgb(colors[0], colors[1], colors[2]);
      } // Should error out?
    }
  }
#endif
  return color;
}


static Chunk *parseInternal(Chunk *ctail, const QString& txt, uint& start, uint cnt, bool interpretNewLine);

#define EXPAND_GREEK(L_U, L_L, REST, SKIP, UCODE)    \
  case L_L:                                   \
    x=0x20;                                   \
  case L_U:                                   \
    if (txt.mid(from + 1).startsWith(REST)) { \
      *skip = SKIP;                           \
      setNormalChar(QChar(UCODE+x), tail);    \
      return true;                            \
    }                                         \
  break;


inline bool parseOutChar(const QString& txt, uint from, int *skip, Chunk **tail, bool interpretNewLine) {
  QChar c = txt[from];
  bool upper = false;
  *skip = 1;
  short x = 0;

#if DEBUG_PARSING
  qDebug() << "----- parsing " << txt;
#endif

  switch (c.unicode()) {
    EXPAND_GREEK('B', 'b', "eta",  4, 0x392)
    EXPAND_GREEK('D', 'd', "elta", 5, 0x394)
    EXPAND_GREEK('Z', 'z', "eta",  4, 0x396)
    EXPAND_GREEK('K', 'k', "appa", 5, 0x39a)
    EXPAND_GREEK('M', 'm', "u",    2, 0x39c)
    EXPAND_GREEK('X', 'x', "i",    2, 0x39e)
    EXPAND_GREEK('R', 'r', "ho",   3, 0x3a1)

    case 'a':
      x = 0x20;
    case 'A':
      if (txt.mid(from + 1).startsWith("lpha")) {
        *skip = 5;
        setNormalChar(QChar(0x391+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("pprox")) {
        *skip = 6;
        setNormalChar(QChar(0x2248), tail);
        return true;
      }
      break;


    case 'c':
      x = 0x20;
    case 'C':
      if (txt.mid(from + 1).startsWith("hi")) {
        *skip = 3;
        setNormalChar(QChar(0x3a7+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("dot")) {
        *skip = 4;
        setNormalChar(QChar(0x2219), tail);
        return true;
      }
      break;

    case 'e':
      x = 0x20;
    case 'E':
      if (txt.mid(from + 1).startsWith("psilon")) {
        *skip = 7;
        setNormalChar(QChar(0x395+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("ta")) {
        *skip = 3;
        setNormalChar(QChar(0x397+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("ll")) {
        *skip = 3;
        setNormalChar(QChar(0x2113), tail);
        return true;
      }
      break;

    case 'g':
      x = 0x20;
    case 'G':
      if (txt.mid(from + 1).startsWith("amma")) {
        *skip = 5;
        setNormalChar(QChar(0x393+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("eq")) {
        *skip = 3;
        setNormalChar(QChar(0x2265), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith('e')) {
        *skip = 2;
        setNormalChar(QChar(0x2265), tail);
        return true;
      }
      break;

    case 'i':
      x = 0x20;
    case 'I':
      if (txt.mid(from + 1).startsWith("ota")) {
        *skip = 4;
        setNormalChar(QChar(0x399+x), tail);
        return true;
      } else if (!upper && txt.mid(from + 1).startsWith("nf")) {
        *skip = 3;
        setNormalChar(QChar(0x221E), tail);
        return true;
      } else if (!upper && txt.mid(from + 1).startsWith("nt")) {
        *skip = 3;
        setNormalChar(QChar(0x222B), tail);
        return true;
      }
      break;

    case 'l':
      x = 0x20;
    case 'L':
      if (txt.mid(from + 1).startsWith("ambda")) {
        *skip = 6;
        setNormalChar(QChar(0x39b+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("eq")) {
        *skip = 3;
        setNormalChar(QChar(0x2264), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith('e')) {
        *skip = 2;
        setNormalChar(QChar(0x2264), tail);
        return true;
      }
      break;

    case 'n':
      x = 0x20;
    case 'N':
      if (txt.mid(from + 1).startsWith('u')) {
        *skip = 2;
        setNormalChar(QChar(0x39D+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith('e')) {
        *skip = 2;
        setNormalChar(QChar(0x2260), tail);
        return true;
      } else if (interpretNewLine) {
        *skip = 1;
        if (!*tail || !(*tail)->text.isEmpty() || (*tail)->locked()) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        (*tail)->linebreak = true;
        *tail = new Chunk(*tail, Chunk::None, false, true);
        return true;
      } else {
        *skip = 1;
        setNormalChar(QChar(0x20), tail);  
        return true;      
      }
      break;

    case 'o':
      x = 0x20;
    case 'O':
      if (txt.mid(from + 1).startsWith("verline{")) {
        if ((*tail)->group) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        Chunk *working = new Chunk(*tail, Chunk::None, true, true);
        dumpattr(working, "start group for overline");
        uint parseStart = from + 9;
        working->attributes.overline = true;
        parseInternal(working, txt, parseStart, txt.length(), interpretNewLine);
        *skip = parseStart - from + 1;
        dumpattr(working, "end group for overline");
        return true;
      } else if (txt.mid(from + 1).startsWith("micron")) {
        *skip = 7;
        setNormalChar(QChar(0x39F+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("mega")) {
        *skip = 5;
        setNormalChar(QChar(0x3A9+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("dot")) {
        *skip = 4;
        setNormalChar(QChar(0x2299), tail);
        return true;
      }
      break;

    case 'p':
      x = 0x20;
    case 'P':
      if (txt.mid(from + 1).startsWith('i')) {
        *skip = 2;
        setNormalChar(QChar(0x3a0+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("hi")) {
        *skip = 3;
        setNormalChar(QChar(0x3A6+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("si")) {
        *skip = 3;
        setNormalChar(QChar(0x3A8+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("artial")) {
        *skip = 7;
        setNormalChar(QChar(0x2202), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("rod")) {
        *skip = 4;
        setNormalChar(QChar(0x220F), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith('m')) {
        *skip = 2;
        setNormalChar(QChar(0xb1), tail);
        return true;
      }
      break;

    case 't':
      x = 0x20;
    case 'T':
      if (txt.mid(from + 1).startsWith("extcolor{")) { // \textcolor{color}{text}
        if ((*tail)->group) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        Chunk *working = new Chunk(*tail, Chunk::None, true, true);
        dumpattr(working, "start group for textcolor");
        uint parseStart = from + 10;
        int firstSkip = 0;
        working->attributes.color = parseColor(txt.mid(parseStart), &firstSkip);
        if (!working->attributes.color.isValid() || txt[parseStart + firstSkip + 1] != '{') {
          return false;
        }
        parseStart += firstSkip + 2;
        parseInternal(working, txt, parseStart, txt.length(), interpretNewLine);
        *skip = parseStart - from + 1;
        dumpattr(working, "end group for textcolor");
        return true;
      } else if (txt.mid(from + 1).startsWith("extbf{")) {
        if ((*tail)->group) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        Chunk *working = new Chunk(*tail, Chunk::None, true, true);
        dumpattr(working, "start group for textbf");
        uint parseStart = from + 7;
        working->attributes.bold = true;
        parseInternal(working, txt, parseStart, txt.length(), interpretNewLine);
        *skip = parseStart - from + 1;
        dumpattr(working, "end group for textbf");
        return true;
      } else if (txt.mid(from + 1).startsWith("extit{")) {
        if ((*tail)->group) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        Chunk *working = new Chunk(*tail, Chunk::None, true, true);
        dumpattr(working, "start group for textit");
        uint parseStart = from + 7;
        working->attributes.italic = true;
        parseInternal(working, txt, parseStart, txt.length(), interpretNewLine);
        *skip = parseStart - from + 1;
        dumpattr(working, "end group for textit");
        return true;
      } else if (txt.mid(from + 1).startsWith("heta")) {
        *skip = 5;
        setNormalChar(QChar(0x398+x), tail);
        return true;
      } else if (txt.mid(from + 1).startsWith("au")) {
        *skip = 3;
        setNormalChar(QChar(0x3A4+x), tail);
        return true;
      } else {
        *skip = 1;
        if (!*tail || !(*tail)->text.isEmpty() || (*tail)->locked()) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        (*tail)->tab = true;
        *tail = new Chunk(*tail, Chunk::None, false, true);
        return true;
      }
      break;

    case 's':
      x = 0x20;
    case 'S':
      if (txt.mid(from + 1).startsWith("igma")) {
        *skip = 5;
        setNormalChar(QChar(0x3A3+x), tail);
        return true;
      } else if (!upper && txt.mid(from + 1).startsWith("um")) {
        *skip = 3;
        setNormalChar(QChar(0x2211), tail);
        return true;
      } else if (!upper && txt.mid(from + 1).startsWith("qrt")) {
        *skip = 4;
        setNormalChar(QChar(0x221A), tail);
        return true;
      }
      break;

    case 'u':
      x = 0x20;
    case 'U':
      if (txt.mid(from + 1).startsWith("nderline{")) {
        if ((*tail)->group) {
          *tail = new Chunk(*tail, Chunk::None, false, true);
        }
        Chunk *working = new Chunk(*tail, Chunk::None, true, true);
        dumpattr(working, "start group for underline");
        uint parseStart = from + 10;
        working->attributes.underline = true;
        parseInternal(working, txt, parseStart, txt.length(), interpretNewLine);
        *skip = parseStart - from + 1;
        dumpattr(working, "end group for underline");
        return true;
      } else if (txt.mid(from + 1).startsWith("psilon")) {
        *skip = 7;
        setNormalChar(QChar(0x3A5+x), tail);
        return true;
      }
      break;

    default:
      break;
  }

  return false;
}


static Chunk *parseInternal(Chunk *ctail, const QString& txt, uint& start, uint cnt, bool interpretNewLine) {
  Chunk *chead = ctail;

  if (ctail->group) {
    ctail = new Chunk(ctail, Chunk::None, false, true);
  }

  for (uint& i = start; i < cnt; ++i) {
    QChar c = txt[i];
    Chunk::VOffset dir = Chunk::Down;
    switch (c.unicode()) {
      case '\n':
        if (!ctail->text.isEmpty() || ctail->locked()) {
          if (ctail->vOffset != Chunk::None) {
            ctail = new Chunk(ctail->prev, Chunk::None, false, true);
          } else {
            ctail = new Chunk(ctail, Chunk::None, false, true);
          }
        }
        ctail->linebreak = true;
        ctail = new Chunk(ctail, Chunk::None, false, true);
        break;
      case '\t':
        if (!ctail->text.isEmpty() || ctail->locked()) {
          if (ctail->vOffset != Chunk::None) {
            ctail = new Chunk(ctail->prev, Chunk::None, false, true);
          } else {
            ctail = new Chunk(ctail, Chunk::None, false, true);
          }
        }
        ctail->tab = true;
        ctail = new Chunk(ctail, Chunk::None, false, true);
        break;
      case 0x5c:   // \ /**/
        if (ctail->vOffset != Chunk::None && !ctail->text.isEmpty()) {
          ctail = new Chunk(ctail->prev, Chunk::None, false, true);
        }
        ++i;
        if (i == cnt) {
          setNormalChar('\\', &ctail);
        } else {
          int skip = 0;
          if (!parseOutChar(txt, i, &skip, &ctail, interpretNewLine)) {
            setNormalChar(txt[i], &ctail);
          } else {
            i += skip - 1;
          }
        }
        break;
      case 0x5e:   // ^
        dir = Chunk::Up;
      case 0x5f:   // _
        if (ctail->text.isEmpty() && !ctail->group) {
          setNormalChar(c, &ctail);
        } else {
          if (ctail->vOffset != Chunk::None) {
            if (ctail->vOffset != dir) {
              ctail = new Chunk(ctail->prev, dir, false, true);
            } else if (ctail->group) {
              ctail = new Chunk(ctail, dir, false, true);
            } else {
              return 0L; // parse error - x^y^z etc
            }
          } else {
            ctail = new Chunk(ctail, dir, false, true);
          }
        }
        break;
      case 0x7b:   // {
        if (ctail->text.isEmpty() && !ctail->group) {
          bool rc = false;
          new Chunk(ctail, Chunk::None, true, true);
          dumpattr(ctail->group, "start group with non-group and empty text");
          rc = 0L != parseInternal(ctail->group, txt, ++i, cnt, interpretNewLine);
          assert(rc);
          dumpattr(ctail->group, "after start group with non-group and empty text");
          if (!rc) {
            return 0L;
          }
        } else {
          bool rc = false;
          if (ctail->vOffset == Chunk::None) {
            rc = 0L != parseInternal(new Chunk(ctail, Chunk::None, true, true), txt, ++i, cnt, interpretNewLine);
          } else {
            rc = 0L != parseInternal(new Chunk(ctail->prev, Chunk::None, true, true), txt, ++i, cnt,  interpretNewLine);
          }
          if (!rc) {
            return 0L;
          }
        }
        break;
      case 0x7d:   // }
        if (chead->prev && chead->prev->group == chead) {
          return chead;
        } else {
          setNormalChar(c, &ctail);
        }
        break;
      case '[':
        {
          bool vector = false;
          int vectorIndexStart = -1;
          int vectorIndexEnd = -1;
          int bracketStack = 1;
          int pos = -1;
          bool equation = txt[i + 1] == '=';
          for (uint searchPt = i + 1; bracketStack != 0 && searchPt < cnt; ++searchPt) {
            if (txt[searchPt] == ']') {
              if (--bracketStack == 0) {
                pos = searchPt;
                break;
              } else if (bracketStack == 1 && vector && vectorIndexEnd == -1) {
                vectorIndexEnd = searchPt - 1;
              }
            } else if (txt[searchPt] == '[') {
              ++bracketStack;
              if (!vector && !equation) {
                vector = true;
                vectorIndexStart = searchPt + 1;
              }
            }
          }

          if (pos < 0 || pos == int(i) + 1 /* empty [] */) {
            return 0L;
          }

          if (ctail->locked() || !ctail->text.isEmpty()) {
            if (ctail->vOffset != Chunk::None) {
              ctail = new Chunk(ctail->prev, Chunk::None, false, true);
            } else {
              ctail = new Chunk(ctail, Chunk::None, false, true);
            }
          }

          if (vector) {
            ctail->text = txt.mid(i + 1, vectorIndexStart - i - 2).trimmed();
            ctail->expression = txt.mid(vectorIndexStart, vectorIndexEnd - vectorIndexStart + 1);
            ctail->vector = true;
          } else {
            ctail->text = txt.mid(i + 1, pos - i - 1).trimmed();
            ctail->scalar = true;
          }
          i = uint(pos);
        }
        break;
      default:
#if 0
        if (ctail->vOffset != Chunk::None && (!ctail->text.isEmpty() || ctail->locked())) {
          ctail = new Chunk(ctail->prev, Chunk::None, false, true);
        }
#endif
        if (ctail->vOffset != Chunk::None && (!ctail->text.isEmpty() || ctail->locked())) {
          while (ctail->vOffset != Chunk::None && (!ctail->text.isEmpty() || ctail->locked())) {
            ctail = ctail->prev;
          }
          ctail = new Chunk(ctail, Chunk::None, false, true);
        }
        setNormalChar(c, &ctail);
        break;
    }
  }

  return chead;
}


Parsed *Label::parse(const QString& txt, bool interpret, bool interpretNewLine) {
  Parsed *parsed = new Parsed;
  Chunk *ctail = parsed->chunk = new Chunk(0L);
  if (!interpret) {
    ctail->text = txt;
    return parsed;
  }

  uint start = 0;
  uint cnt = txt.length();
  Chunk *rc = parseInternal(ctail, txt, start, cnt, interpretNewLine);
  if (!rc) {
    // Parse error - how to recover?
    delete parsed;
    parsed = 0L;
  }
  return parsed;
}


// vim: ts=2 sw=2 et
