/***************************************************************************
                               kstuinames.cpp  
                             -------------------
    begin                : July 31, 2004
    copyright            : (C) 2003 C. Barth Netterfield
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

#include "kst.h"
#include "kstuinames.h"

QString KST::suggestWinName() {
  static int last = 0;
  QString suggestedName = "W" + QString::number(++last);
  while (KstApp::inst()->findWindow(suggestedName)) {
    suggestedName = "W" + QString::number(++last);
  }
  return suggestedName;
}

// vim: ts=2 sw=2 et
