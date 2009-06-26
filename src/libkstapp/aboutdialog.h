/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2009 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#include "ui_aboutdialog.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT AboutDialog : public QDialog, Ui::AboutDialog
{
  Q_OBJECT
  public:
    AboutDialog(QWidget *parent);
    virtual ~AboutDialog();

  private Q_SLOTS:
   void launchURL(const QUrl &link);
};
}

#endif

// vim: ts=2 sw=2 et
