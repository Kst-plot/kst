/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LINEITEMDIALOG_H
#define LINEITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class LineItem;
class LineDimensionsTab;

class LineItemDialog : public ViewItemDialog
{
    Q_OBJECT
  public:
    explicit LineItemDialog(LineItem *item, QWidget *parent = 0);

  public Q_SLOTS:
    virtual void saveDimensions(ViewItem *item);

  private:
    virtual void setupDimensions();
    LineDimensionsTab *_lineDimensionsTab;

    ViewItem *_viewItem;
};
}

#endif // LINEITEMDIALOG_H
