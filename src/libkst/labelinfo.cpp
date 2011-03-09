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

#include "labelinfo.h"

namespace Kst {

QString LabelInfo::singleRenderItemLabel() const {
  if (units.isEmpty() && quantity.isEmpty()) {           // Nxx xxx
    return name;
  } else if (quantity.isEmpty()) {
    if (!name.isEmpty()) {
      return QString("%1 \\[%2\\]").arg(name).arg(units); // NxU
    } else {
      return units;                                                // xxU
    }
  } else if (units.isEmpty()) {
    if (name.isEmpty()) {
      return quantity;                                             // xQx
    } else {
      if (name.toLower().contains(quantity.toLower())) {
        return name;                                               // NQx A
      } else {
        return QString("%1: %2").arg(name).arg(quantity); // NQx B
      }
    }
  } else if (name.isEmpty()) {
    return QString("%1 \\[%2\\]").arg(quantity).arg(units); // xQU
  } else {
    return QString("%1 \\[%2\\]").arg(quantity).arg(units); // NQU B
  }
}


}
