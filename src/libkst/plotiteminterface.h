/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
#include "kst_export.h"

namespace Kst {

class KSTCORE_EXPORT PlotItemInterface
{
  public:
    static const qint64 Forced = -1;

    PlotItemInterface();
    virtual ~PlotItemInterface();

    virtual QString plotName() const = 0;
    virtual QString plotSizeLimitedName(const QWidget *widget) const = 0;

    virtual bool handleChangedInputs(qint64 serial) = 0;

    void registerChange() {_serialOfLastChange = Forced;}

  protected:
    qint64 _serialOfLastChange;
};

}

Q_DECLARE_METATYPE(Kst::PlotItemInterface*)

#endif

// vim: ts=2 sw=2 et
