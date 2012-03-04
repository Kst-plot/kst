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

#ifndef CIRCLEITEMDIALOG_H
#define CIRCLEITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class CircleItem;
class CircleDimensionsTab;

class CircleItemDialog : public ViewItemDialog
{
    Q_OBJECT
  public:
    explicit CircleItemDialog(CircleItem *item, QWidget *parent = 0);

  protected:
    virtual void saveDimensions(ViewItem *item);
  private:
    void setupProperties();
    CircleDimensionsTab *_circleDimensionsTab;

    ViewItem *_viewItem;
};

}

#endif // CIRCLEITEMDIALOG_H

