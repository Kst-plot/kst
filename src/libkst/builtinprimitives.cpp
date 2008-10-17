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

#include "builtinprimitives.h"
#include "vectorfactory.h"
#include "scalarfactory.h"
#include "stringfactory.h"
#include "matrixfactory.h"

namespace Kst {
  namespace Builtins {
    void initPrimitives() {
      new VectorFactory();
      new GeneratedVectorFactory();
      new EditableVectorFactory();
      new DataVectorFactory();
      new ScalarFactory();
      new DataScalarFactory();
      new VScalarFactory();
      new StringFactory();
      new GeneratedMatrixFactory();
      new EditableMatrixFactory();
      new DataMatrixFactory();
    }
  }
}

// vim: ts=2 sw=2 et
