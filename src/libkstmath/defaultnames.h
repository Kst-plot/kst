/***************************************************************************
                             defaultnames.h
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

#ifndef DEFAULTNAMES_H
#define DEFAULTNAMES_H

#include <qstring.h>

#include "kst_export.h"

namespace Kst {
  extern KST_EXPORT QString suggestPlotName();
  extern KST_EXPORT QString suggestCurveName(ObjectTag vector_name, bool add_c=false);
  extern KST_EXPORT QString suggestPSDName(ObjectTag vector_name);
  extern KST_EXPORT QString suggestCSDName(ObjectTag vector_name);
  extern KST_EXPORT QString suggestEQName(const QString& name_in);
  extern KST_EXPORT QString suggestHistogramName(ObjectTag vector_name);
  extern KST_EXPORT QString suggestPluginName(const QString& pname, ObjectTag vname = ObjectTag::invalidTag);
  extern KST_EXPORT QString suggestImageName(ObjectTag matrix_name);
}

#endif
// vim: ts=2 sw=2 et
