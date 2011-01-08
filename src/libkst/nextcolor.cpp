/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "nextcolor.h"

using namespace Kst;

NextColor* NextColor::_instance = 0;


NextColor::NextColor() { 
  NextColor::_instance = this;
}

NextColor::~NextColor() { 
  NextColor::_instance = 0;
}

QColor NextColor::next() { 
  return QColor();
}

NextColor& NextColor::self() { 
  Q_ASSERT(NextColor::_instance);
  return *NextColor::_instance;
}

