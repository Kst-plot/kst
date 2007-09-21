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

#ifndef DATARANGE_H
#define DATARANGE_H

#include <QWidget>
#include "ui_datarange.h"

#include "kst_export.h"

namespace Kst {

class  KST_EXPORT DataRange : public QWidget, public Ui::DataRange {
  Q_OBJECT
  public:
    DataRange(QWidget *parent = 0);
    virtual ~DataRange();
};

}

#endif

// vim: ts=2 sw=2 et
