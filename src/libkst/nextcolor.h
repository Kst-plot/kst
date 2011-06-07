/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NEXTCOLOR_H
#define NEXTCOLOR_H

#include <QColor>

#include "kst_export.h"

class KPalette;

namespace Kst {

class KSTCORE_EXPORT NextColor
{
  public:
    virtual ~NextColor();
    virtual QColor next() = 0;
    static NextColor& self();

  protected:
    NextColor();

  private:
    static NextColor* _instance;
};

}
#endif

// vim: ts=2 sw=2 et
