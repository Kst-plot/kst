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

#ifndef STRINGVIEWDIALOG_H
#define STRINGVIEWDIALOG_H

#include <QDialog>

#include "ui_stringeditordialog.h"

#include "kst_export.h"

namespace Kst {

class StringTableModel;

class KST_EXPORT StringEditorDialog : public QDialog, Ui::StringEditorDialog
{
  Q_OBJECT
  public:
    StringEditorDialog(QWidget *parent);
    virtual ~StringEditorDialog();

    virtual void show();

  private:
    void refreshStrings();

    StringTableModel *_model;

};

}

#endif

// vim: ts=2 sw=2 et
