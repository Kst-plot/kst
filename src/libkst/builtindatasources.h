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

#ifndef BUILTINDATASOURCES_H
#define BUILTINDATASOURCES_H

#include "kst_export.h"

namespace Kst {
  class ObjectStore;

  namespace Builtins {
    KSTCORE_EXPORT void initDataSources();
  }
}

#endif

// vim: ts=2 sw=2 et
