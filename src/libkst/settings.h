/***************************************************************************
                 object.h: abstract base class for all Kst objects
                             -------------------
    copyright            : (C) 2013 The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "kstcore_export.h"

#include <QSettings>


namespace Kst {


// QSettings objects should only be instantiated with this function
KSTCORE_EXPORT QSettings& createSettings(const QString& scope);

KSTCORE_EXPORT void deleteAllSettings();


}

#endif

// vim: ts=2 sw=2 et
