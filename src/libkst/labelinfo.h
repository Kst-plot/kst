/*
    Keeps track of information relevant to label creation.
    Copyright (C) 2011  Barth Netterfield netterfield@astro.utoronto.ca

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef LABELINFO_H
#define LABELINFO_H

#include <QString>

#include "kst_export.h"


namespace Kst {

struct KSTCORE_EXPORT LabelInfo
{
  LabelInfo() : name(QString()), quantity(QString()), units(QString()), file(QString()) {}
  bool operator==(const LabelInfo &l) const { return (l.name==name) && (l.quantity==quantity) && (l.units==units) && (l.file==file); }
  bool operator!=(const LabelInfo &l) const { return !(*this==l); }
  QString name;
  QString quantity;
  QString units;
  QString file;
  QString escapedFile();
  QString singleRenderItemLabel() const;
};

}

#endif // LABELINFO_H
