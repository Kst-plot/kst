/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2009 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include <QDialog>

#include "ui_shortcutdialog.h"

#include "kstcore_export.h"

namespace Kst {

class ShortcutDialog : public QDialog, Ui::ShortcutDialog
{
  Q_OBJECT
  public:
    explicit ShortcutDialog(QWidget *parent);
    virtual ~ShortcutDialog();

    void resetWidth();

};
}

#endif

// vim: ts=2 sw=2 et
