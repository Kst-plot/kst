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

#ifndef ARROWITEMDIALOG_H
#define ARROWITEMDIALOG_H

#include "viewitemdialog.h"

#include "kst_export.h"

namespace Kst {

class ArrowPropertiesTab;
class ArrowItem;

class ArrowItemDialog : public ViewItemDialog
{
  Q_OBJECT
  public:
    explicit ArrowItemDialog(ArrowItem *item, QWidget *parent = 0);
    virtual ~ArrowItemDialog();

  private Q_SLOTS:
    void propertiesChanged();

  private:
    void setupProperties();

    ArrowPropertiesTab *_propertiesTab;
    ArrowItem *_arrowItem;
};

}

#endif

// vim: ts=2 sw=2 et
