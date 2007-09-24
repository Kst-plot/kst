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

#ifndef VECTORDIALOG_H
#define VECTORDIALOG_H

#include <QDialog>

#include "ui_vectordialog.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT VectorDialog : public QDialog, Ui::VectorDialog {
  Q_OBJECT
  public:
    VectorDialog(QWidget *parent = 0);
    virtual ~VectorDialog();
};

}

#endif

// vim: ts=2 sw=2 et
