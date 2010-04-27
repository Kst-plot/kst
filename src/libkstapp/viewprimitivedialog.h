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

#ifndef VIEWPRIMITIVEDIALOG_H
#define VIEWPRIMITIVEDIALOG_H

#include <QDialog>

#include "ui_viewprimitivedialog.h"

#include "kst_export.h"

namespace Kst {

class Document;

class ViewPrimitiveDialog : public QDialog, Ui::ViewPrimitiveDialog
{
  Q_OBJECT

  public:
    enum PrimitiveType { Scalar, String };

    ViewPrimitiveDialog(QWidget *parent, Document *doc, PrimitiveType type);
    virtual ~ViewPrimitiveDialog();

  private:
    void refresh();

    Document *_doc;
    QAbstractItemModel *_model;
    PrimitiveType _type;
};

}

#endif

// vim: ts=2 sw=2 et
