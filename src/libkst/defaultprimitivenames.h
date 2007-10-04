/***************************************************************************
                           defaultprimitivenames.h
                             -------------------
    begin                : July 31, 2004
    copyright            : (C) 2004 C. Barth Netterfield
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

#ifndef DEFAULTPRIMITIVENAMES_H
#define DEFAULTPRIMITIVENAMES_H

#include <qstring.h>

#include "kst_export.h"
#include "object.h"

namespace Kst {
  extern KST_EXPORT QString suggestVectorName(const QString& field);

  extern KST_EXPORT QString suggestMatrixName(const QString& vectorName);

  extern KST_EXPORT ObjectTag suggestUniqueMatrixTag(ObjectTag baseTag);
  extern KST_EXPORT ObjectTag suggestUniqueScalarTag(ObjectTag baseTag);
  extern KST_EXPORT ObjectTag suggestUniqueStringTag(ObjectTag baseTag);
  extern KST_EXPORT ObjectTag suggestUniqueVectorTag(ObjectTag baseTag);
}

#endif
// vim: ts=2 sw=2 et
