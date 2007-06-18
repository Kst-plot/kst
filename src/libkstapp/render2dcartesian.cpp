/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 Theodore Kisner <tsk@humanityforward.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "render2dcartesian.h"

namespace Kst {

Render2DCartesian::Render2DCartesian(const QString &name) : PlotRenderer2D(name) {
  setType(Cartesian);
}

Render2DCartesian::~Render2DCartesian() {
}

}

// vim: ts=2 sw=2 et
