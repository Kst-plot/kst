/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2026 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTNEXTCOLOR_H
#define TESTNEXTCOLOR_H

#include "nextcolor.h"

// Datasource construction expects the application-owned color provider to exist.
class TestNextColor : public Kst::NextColor {
public:
  QColor next() override { return Qt::black; }
  QColor current() override { return Qt::black; }
};

#endif