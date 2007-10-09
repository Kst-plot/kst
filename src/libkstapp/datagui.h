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

#ifndef DATAGUI_H
#define DATAGUI_H

#include "datacollection.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT DataGui : public Data {
  public:
    DataGui();
    virtual ~DataGui();

    virtual QList<PlotItemInterface*> plotList() const;

    virtual int rows() const;

    virtual int columns() const;
};

}

#endif

// vim: ts=2 sw=2 et
