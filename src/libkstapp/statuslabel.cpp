/***************************************************************************
                             statuslabel.cpp
                             -------------------
    begin                : July 20, 2005
    copyright            : (C) 2005 The University of British Columbia
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

// include files for Qt
#include <qlabel.h> 

//include files for KDE
#include <klocale.h>

// application specific includes
#include "statuslabel.h"

StatusLabel::StatusLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
: QLabel(text, parent, f) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setMinimumWidth(0);
  _width = 0;
}

StatusLabel::~StatusLabel() {
}

void StatusLabel::setFullText() {
  QToolTip::remove(this);
  QToolTip::hide();
  
  setMaximumWidth(32767);  
  setText(_fullText);
}

QString StatusLabel::squeeze(const QString& s, const QFontMetrics& fm, uint width) {
  uint currentWidth = fm.width(s);
  
  if (s.isEmpty() || currentWidth <= width) {
     return s;
  }

  QString str(s);
  uint ellipsisWidth = fm.width("...");
  
  if (currentWidth > ellipsisWidth) {
    const uint maxWidth = width - ellipsisWidth;
    const uint emWidth  = fm.maxWidth( );
    int truncate;
  
    while (currentWidth > maxWidth && !str.isEmpty()) {
      truncate = (currentWidth-maxWidth)/emWidth;
      if (truncate == 0) {
        truncate = 1;
      }
      str.truncate(str.length()-truncate);  
      currentWidth = fm.width(str); 
    }
    str += "...";
  } else {
    str = "...";
  }
    
  return str;
}

void StatusLabel::setTextWidth(const QFontMetrics &metrics, int width) {
  QString str;
  
  QToolTip::remove(this);
  
  if (width < 0) {
    width = 0;
  }
  
  setMaximumWidth(width);

  str = squeeze(_fullText, metrics, width);
  
  if (str != _fullText) {
    QToolTip::add(this, _fullText);  
  }

  setText(str);
}

void StatusLabel::setFullText(const QString &text) {
  _fullText = text;
}

const QString& StatusLabel::fullText() const {
  return _fullText;
}

#include "statuslabel.moc"
// vim: ts=2 sw=2 et
