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

#ifndef VECTOREDITORDIALOG_H
#define VECTOREDITORDIALOG_H

#include <QDialog>

#include "ui_vectoreditordialog.h"

#include "kst_export.h"

namespace Kst {

class Document;
class VectorTableModel;

class KST_EXPORT VectorEditorDialog : public QDialog, Ui::VectorEditorDialog
{
  Q_OBJECT
  public:
    VectorEditorDialog(QWidget *parent, Document *doc);
    virtual ~VectorEditorDialog();

  private:
    Document *_doc;
    VectorTableModel *_model;
};

}

#endif

// vim: ts=2 sw=2 et
