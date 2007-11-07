/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "builtinrelations.h"
#include "curvefactory.h"

namespace Kst {
  namespace Builtins {
    void initRelations() {
      new CurveFactory;
    }
  }
}

// vim: ts=2 sw=2 et
