/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LABELITEMDIALOG_H
#define LABELITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class LabelPropertiesTab;
class LabelDimensionsTab;
class LabelItem;

class LabelItemDialog : public ViewItemDialog
{
  Q_OBJECT
  public:
    explicit LabelItemDialog(LabelItem *item, QWidget *parent = 0);
    virtual ~LabelItemDialog();

  private Q_SLOTS:
    void propertiesChanged();
    virtual void saveDimensions(ViewItem *item);


  private:
    virtual void setupDimensions();
    void setupProperties();

    LabelPropertiesTab *_propertiesTab;
    LabelDimensionsTab *_labelDimensionsTab;

    LabelItem *_labelItem;
};

}

#endif

// vim: ts=2 sw=2 et
