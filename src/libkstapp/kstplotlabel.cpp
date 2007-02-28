/***************************************************************************
                              kstplotlabel.cpp
                             ------------------
    begin                : Jun 22 2005
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

#include "kstplotlabel.h"

#include "enodes.h"
#include "kst.h"
#include "kstmath.h"
#include "kstdatacollection.h"
#include "kstsettings.h"
#include "labelrenderer.h"

#include <qdom.h>
#include <q3stylesheet.h>
#include <q3textstream.h>

#include <kglobal.h>
#include <klocale.h>

#include <stdlib.h>
#include <stdio.h>

#define MIN_FONT_SIZE 5

KstPlotLabel::KstPlotLabel(float rotation) {
  commonConstructor(QString::null, KstApp::inst()->defaultFont(), 1, 0, rotation);
}


KstPlotLabel::KstPlotLabel(const QString& txt, const QString& font, int fontSize, KstLJustifyType justify, float rotation) {
  commonConstructor(txt, font, fontSize, justify, rotation);
}


void KstPlotLabel::commonConstructor(const QString& txt, const QString& font, int fontSize, KstLJustifyType justify, float rotation) {
  _txt = txt;
  _interpret = true;
  _replace = true;
  _rotation = rotation;
  _sinr = fabs(sin(_rotation * M_PI / 180.0));
  _cosr = fabs(cos(_rotation * M_PI / 180.0));
  _justify = justify;
  _fontName = font;
  _fontSize = fontSize;
  _absFontSize = fontSize;// + KstSettings::globalSettings()->plotFontSize;
  _parsed = 0L;
  reparse();
  computeTextSize();
}


KstPlotLabel::~KstPlotLabel() {
  delete _parsed;
  _parsed = 0L;
}


void KstPlotLabel::reparse() {
  delete _parsed;
   _parsed = Label::parse(_txt, _interpret, false);
}


void KstPlotLabel::setText(const QString& text) {
  if (_txt != text) {
    _txt = text;
    reparse();
    computeTextSize();
    //setDirty(true);
  }
}


const QString& KstPlotLabel::text() const {
  return _txt;
}


void KstPlotLabel::setRotation(float rotation) {
  if (_rotation != rotation) {
    _rotation = rotation;
    _sinr = fabs(sin(_rotation * M_PI / 180.0));
    _cosr = fabs(cos(_rotation * M_PI / 180.0));
    //setDirty(true);
  }
}


void KstPlotLabel::setJustification(KstLJustifyType justify) {
  if (_justify != justify) {
    _justify = justify;
    //setDirty(true);
  }
}


int KstPlotLabel::ascent() const {
  return _ascent;
}


void KstPlotLabel::setFontName(const QString& fontName) {
  if (_fontName != fontName) {
    _fontName = fontName;
    computeTextSize();
    //setDirty(true);
  }
}


const QString& KstPlotLabel::fontName() const {
  return _fontName;
}


void KstPlotLabel::setInterpreted(bool interpreted) {
  if (_interpret != interpreted) {
    _interpret = interpreted;
    reparse();
    //setDirty(true);
  }
}


void KstPlotLabel::setDoScalarReplacement(bool replace) {
  if (_replace != replace) {
    _replace = replace;
    //setDirty(true);
  }
}


void KstPlotLabel::drawToPainter(Label::Parsed *lp, QPainter& p) {
  RenderContext rc(_fontName, _absFontSize, &p);
  rc.setSubstituteScalars(_replace);

  int tx = 0, ty = 0; // translation
  rc.x = -_textWidth / 2;
  tx = int(_textWidth * _cosr + _textHeight * _sinr) / 2;
  rc.y = _ascent - _textHeight / 2;
  ty = int(_textHeight * _cosr + _textWidth * _sinr) / 2;

  p.translate(tx, ty);
  p.rotate(_rotation);

  rc.pen = p.pen();

  if (lp && lp->chunk) {
    renderLabel(rc, lp->chunk);
  }
}


void KstPlotLabel::computeTextSize() {
  if (_parsed && _parsed->chunk) {
    RenderContext rc(_fontName, _absFontSize, 0L);
    rc.setSubstituteScalars(_replace);
    renderLabel(rc, _parsed->chunk);
    _textWidth = rc.x;
    _ascent = rc.ascent;
    _textHeight = 1 + rc.ascent + rc.descent;
    _lineSpacing = rc.lineSpacing();
  }
}


void KstPlotLabel::paint(QPainter& p) {
  drawToPainter(_parsed, p);
}


void KstPlotLabel::setFontSize(int size) {
  if (_fontSize != size) {
    _fontSize = size;
    //setDirty(true);
  }
}


int KstPlotLabel::fontSize() const {
  return _fontSize;
}


bool KstPlotLabel::interpreted() const {
  return _interpret;
}


bool KstPlotLabel::doScalarReplacement() const {
  return _replace;
}


QSize KstPlotLabel::size() const {
  return QSize(d2i(_textWidth*_cosr + _textHeight*_sinr), d2i(_textHeight*_cosr + _textWidth*_sinr));
}


void KstPlotLabel::load(const QDomElement& e) {
  bool upLS = false;
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if (!e.isNull()) { // the node was really an element.
      if (e.tagName() == "text") {
        setText(e.text());
      } else if  (e.tagName() == "justify") {
        _justify = KstLJustifyType(e.text().toInt());
      } else if  (e.tagName() == "rotation") {
        _rotation = e.text().toFloat();
      } else if  (e.tagName() == "interpret") {
        _interpret = true;
      } else if (e.tagName() == "fontfamily") {
        _fontName = e.text();
        upLS = true;
      } else if ((e.tagName() == "fontsize") || (e.tagName() == "size")) {
        _fontSize = e.text().toInt();
        upLS = true;
      } else if (e.tagName() == "useusercolor") {
        // Unsupported
      } else if (e.tagName() == "color") {
        // Unsupported
      }
    }
    n = n.nextSibling();
  }

  if (upLS) {
    computeTextSize();
  }
}


void KstPlotLabel::save(Q3TextStream &ts, const QString& indent, bool save_pos) const {
  QString l2 = indent + "  ";
  ts << indent << "<text>" << Q3StyleSheet::escape(_txt) << "</text>" << endl;
  if (_interpret) {
    ts << indent << "<interpret/>" << endl;
  }
  ts << indent << "<rotation>" << _rotation << "</rotation>" << endl;
  if (save_pos) {
    ts << indent << "<justify>" << _justify << "</justify>" << endl;
  }
  ts << indent << "<fontfamily>" << Q3StyleSheet::escape(_fontName) << "</fontfamily>" << endl;
  ts << indent << "<size>" << _fontSize << "</size>" << endl;
}


KstPlotLabel& KstPlotLabel::operator=(const KstPlotLabel& l) {
  _fontName = l._fontName;
  _fontSize = l._fontSize;
  _absFontSize = l._absFontSize;
  _rotation = l._rotation;
  _interpret = l._interpret;
  _replace = l._replace;
  setText(l._txt); // calls computeTextSize()
  return *this;
}


void KstPlotLabel::updateAbsFontSize(int x_pix, int y_pix) {
  double x_s, y_s;
  
  x_s = y_s = _fontSize + (double)KstSettings::globalSettings()->plotFontSize;

  if (x_pix < y_pix) {
    x_s *= x_pix/540.0;
    y_s *= y_pix/748.0;
  } else {
    y_s *= y_pix/540.0;
    x_s *= x_pix/748.0;
  }

  _absFontSize = int((x_s + y_s)/2);
  if (_absFontSize < KstSettings::globalSettings()->plotFontMinSize) {
    _absFontSize = KstSettings::globalSettings()->plotFontMinSize;
  }  
    
  computeTextSize();
}

// vim: ts=2 sw=2 et
