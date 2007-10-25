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

#ifndef CHOOSECOLORDIALOG_H
#define CHOOSECOLORDIALOG_H

#include <QDialog>
#include <QLineEdit>

#include "colorbutton.h"

#include "ui_choosecolordialog.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT ChooseColorDialog : public QDialog, Ui::ChooseColorDialog
{
  Q_OBJECT
  public:
    ChooseColorDialog(QWidget *parent);
    virtual ~ChooseColorDialog();

    void show();

  private:
    QGridLayout* grid;

    void updateColorGroup();
    void cleanColorGroup();

    QList<QLineEdit*> lineEdits;
    QList<ColorButton*> colorButtons;

};

}

#endif

// vim: ts=2 sw=2 et
