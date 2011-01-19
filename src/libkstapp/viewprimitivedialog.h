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

#ifndef VIEWPRIMITIVEDIALOG_H
#define VIEWPRIMITIVEDIALOG_H

#include <QDialog>

#include "ui_viewprimitivedialog.h"

#include "kst_export.h"

namespace Kst {

class Document;
class ObjectStore;

class ViewPrimitiveDialog : public QDialog, Ui::ViewPrimitiveDialog
{
  Q_OBJECT

  public:
    enum PrimitiveType { Scalar, String };

    ViewPrimitiveDialog(QWidget *parent, Document *doc);
    virtual ~ViewPrimitiveDialog();

  protected:
    void refresh();

  private:
    virtual QAbstractItemModel* createModel(ObjectStore *store) = 0;
    void deleteModel();

    Document *_doc;
    QAbstractItemModel *_model;
};


class ViewStringDialog : public ViewPrimitiveDialog
{
  public:
    ViewStringDialog(QWidget *parent, Document *doc);

  private:
    QAbstractItemModel* createModel(ObjectStore *store);
};


class ViewScalarDialog : public ViewPrimitiveDialog
{
  public:
    ViewScalarDialog(QWidget *parent, Document *doc);

  private:
    QAbstractItemModel* createModel(ObjectStore *store);
};

}

#endif

// vim: ts=2 sw=2 et
