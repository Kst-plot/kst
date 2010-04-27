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

#ifndef VIEWVECTORDIALOG_H
#define VIEWVECTORDIALOG_H

#include <QDialog>

#include "ui_viewvectordialog.h"

#include "kst_export.h"

namespace Kst {

class Document;
class VectorModel;

class ViewVectorDialog : public QDialog, Ui::ViewVectorDialog
{
  Q_OBJECT
  public:
    ViewVectorDialog(QWidget *parent, Document *doc);
    virtual ~ViewVectorDialog();

    virtual void show();

private Q_SLOTS:
    void vectorSelected();

  private:
    Document *_doc;
    VectorModel *_model;
};

}

#endif

// vim: ts=2 sw=2 et
