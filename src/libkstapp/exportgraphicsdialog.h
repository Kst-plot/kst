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

#ifndef EXPORTGRAPHICSDIALOG_H
#define EXPORTGRAPHICSDIALOG_H

#include <QDialog>
#include "ui_exportgraphicsdialog.h"

namespace Kst {
class MainWindow;

class ExportGraphicsDialog : public QDialog
{
  Q_OBJECT
  public:
    ExportGraphicsDialog(MainWindow *win);
    ~ExportGraphicsDialog();

  private:
    Ui::ExportGraphicsDialog ui;
    MainWindow *_win;
};

}

#endif

// vim: ts=2 sw=2 et
