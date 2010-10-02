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

#ifndef EXPORTGRAPHICSDIALOG_H
#define EXPORTGRAPHICSDIALOG_H

#include <QDialog>

#include "ui_exportgraphicsdialog.h"

#include "kst_export.h"

namespace Kst {

class MainWindow;

class ExportGraphicsDialog : public QDialog, Ui::ExportGraphicsDialog
{
  Q_OBJECT
  public:
    ExportGraphicsDialog(MainWindow *win);
    virtual ~ExportGraphicsDialog();

  public slots:
    void createFile();
    void enableWidthHeight();
    void OKClicked();
    void apply();
    void applyAutosave();
    void updateButtons();

  Q_SIGNALS:
    void exportGraphics(const QString &filename, const QString &format, int w, int h, int display);

  private:
    QTimer *_autoSaveTimer;
};

}

#endif

// vim: ts=2 sw=2 et
