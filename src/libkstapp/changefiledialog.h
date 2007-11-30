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

#ifndef CHANGEFILEDIALOG_H
#define CHANGEFILEDIALOG_H

#include <QDialog>
#include "vector.h"
#include "matrix.h"
#include "relation.h"

#include "ui_changefiledialog.h"

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT ChangeFileDialog : public QDialog, Ui::ChangeFileDialog
{
  Q_OBJECT
  public:
    ChangeFileDialog(QWidget *parent);
    virtual ~ChangeFileDialog();

    void exec();

  private Q_SLOTS:
    void selectAll();
    void updateSelection(const QString&);
    void selectAllFromFile();
    void OKClicked();
    void apply();
    void updateButtons();

  private:
    void updatePrimitiveList();
    void duplicateDependents(VectorPtr oldVector, VectorPtr newVector, QMap<RelationPtr, RelationPtr> &duplicatedRelations);
    void duplicateDependents(MatrixPtr oldMatrix, MatrixPtr newMatrix, QMap<RelationPtr, RelationPtr> &duplicatedRelations);

    ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
