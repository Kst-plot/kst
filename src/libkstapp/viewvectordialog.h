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

#ifndef VIEWVECTORDIALOG_H
#define VIEWVECTORDIALOG_H

#include <QDialog>

#include "ui_viewvectordialog.h"

#include "kst_export.h"

namespace Kst {

class Document;
class VectorModel;
class EditMultipleWidget;

class ViewVectorDialog : public QDialog, Ui::ViewVectorDialog
{
  Q_OBJECT
  public:
    ViewVectorDialog(QWidget *parent, Document *doc);
    virtual ~ViewVectorDialog();

    virtual void show();

public Q_SLOTS:
  void contextMenu(const QPoint& position);
  void update();

private Q_SLOTS:
  void addSelected();
  void removeSelected();
  void reset();

private:
  Document *_doc;
  VectorModel *_model;
  EditMultipleWidget *_showMultipleWidget;
};

}

#endif

// vim: ts=2 sw=2 et
