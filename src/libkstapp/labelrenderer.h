/***************************************************************************
                              labelrenderer.h
                             ------------------
    begin                : Jun 17 2005
    copyright            : (C) 2005 by The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LABELRENDERER_H
#define LABELRENDERER_H

#include <qfont.h>
#include <qpainter.h>
#include <qpair.h>
#include <qstring.h>
#include <qvariant.h>

#include "dataref.h"
#include "kst_export.h"

namespace Label {
// inline for speed.
struct RenderContext {
  RenderContext(const QFont& font, QPainter *p)
  : p(p), _fm(_font) {
    x = y = xMax = xStart = 0;
    ascent = descent = 0;
    precision = 8;
    _cache = 0L;
    substitute = true;
    setFont(font);
    lines = 0;
  }

  inline const QFont& font() const {
    if (p) {
      return p->font();
    } else {
      return _font;
    }
  }

  inline void setFont(const QFont& f_in) {
    QFont f = f_in;
    _fontSize = f.pixelSize();

    if (p) {
      p->setFont(f);
      _ascent = p->fontMetrics().ascent();
      _descent = p->fontMetrics().descent();
      _height = p->fontMetrics().height();
      _lineSpacing = p->fontMetrics().lineSpacing();
    } else {
      _font = f;
      _fm = QFontMetrics(_font);
      _ascent = _fm.ascent();
      _lineSpacing = _fm.lineSpacing();
      _descent = _fm.descent();
      _height = _fm.height();
    }
  }

  inline int fontSize() const {
    return _fontSize;
  }

  inline int fontAscent() const {
    return _ascent;
  }

  inline int lineSpacing() const {
    return _lineSpacing;
  }

  inline int fontDescent() const {
    return _descent;
  }

  inline int fontHeight() const {
    return _height;
  }

  inline int fontWidth(const QString& txt) const {
    if (p) {
      return p->fontMetrics().width(txt);
    } else {
      return _fm.width(txt);
    }
  }

  inline bool substituteScalars() const {
    return substitute;
  }

  inline void setSubstituteScalars(bool on) {
    substitute = on;
  }

  int x, y; // Coordinates we're rendering at
  int xMax, xStart;
  int ascent, descent;
  QString fontName;
  int size;
  QPainter *p;
  int precision;
  bool substitute;
  QVector<DataRef> *_cache;
  QPen pen;
  int lines;

  private:
    QFont _font;
    QFontMetrics _fm;
    int _ascent, _descent, _height, _lineSpacing; // caches to avoid performance problem                                        // with QFont*
    int _fontSize;
};

struct Chunk;
KST_EXPORT void renderLabel(RenderContext& rc, Chunk *fi);
}

#endif
// vim: ts=2 sw=2 et
