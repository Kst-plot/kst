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

#ifndef PLOTITEMINTERFACE_H
#define PLOTITEMINTERFACE_H

#include <QString>
#include <QMetaType>

namespace Kst {

class PlotItemInterface
{
  public:
    PlotItemInterface();
    virtual ~PlotItemInterface();

    virtual QString plotName() const = 0;
    virtual void updateObject() = 0;
};

}

Q_DECLARE_METATYPE(Kst::PlotItemInterface*)

#endif

// vim: ts=2 sw=2 et
