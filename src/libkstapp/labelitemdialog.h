/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
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
class LabelItem;

class KST_EXPORT LabelItemDialog : public ViewItemDialog
{
  Q_OBJECT
  public:
    LabelItemDialog(LabelItem *item, QWidget *parent = 0);
    virtual ~LabelItemDialog();

  private Q_SLOTS:
    void propertiesChanged();

  private:
    void setupProperties();

    LabelPropertiesTab *_propertiesTab;
    LabelItem *_labelItem;
};

}

#endif

// vim: ts=2 sw=2 et
