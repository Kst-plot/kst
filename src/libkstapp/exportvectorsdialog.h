/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef EXPORTVECTORSDIALOG_H
#define EXPORTVECTORSDIALOG_H

#include <QDialog>

#include "ui_exportvectorsdialog.h"

namespace Kst {

class ObjectStore;

class ExportVectorsDialog : public QDialog, Ui::ExportVectorsDialog
{
    Q_OBJECT

public:
    ExportVectorsDialog(QWidget *parent = 0);
    ~ExportVectorsDialog();

    void show();



private Q_SLOTS:
    void addButtonClicked();
    void removeButtonClicked();
    void addAll();
    void removeAll();
    void availableDoubleClicked(QListWidgetItem * item);
    void selectedDoubleClicked(QListWidgetItem * item);
    void updateButtons();
    void OKClicked();
    bool apply();


private:
    Ui::ExportVectorsDialog *ui;

    void updateVectorList();

    ObjectStore *_store;

};

}
#endif // EXPORTVECTORSDIALOG_H
