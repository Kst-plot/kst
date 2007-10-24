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

#ifndef MATRIXEDITORDIALOG_H
#define MATRIXEDITORDIALOG_H

#include <QDialog>

#include "ui_matrixeditordialog.h"

#include "kst_export.h"

namespace Kst {

class Document;
class MatrixModel;

class KST_EXPORT MatrixEditorDialog : public QDialog, Ui::MatrixEditorDialog
{
  Q_OBJECT
  public:
    MatrixEditorDialog(QWidget *parent, Document *doc);
    virtual ~MatrixEditorDialog();

    virtual void show();

private Q_SLOTS:
    void matrixSelected();

  private:
    Document *_doc;
    MatrixModel *_model;
};

}

#endif

// vim: ts=2 sw=2 et
