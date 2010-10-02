/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUGREPORTWIZARD_H
#define BUGREPORTWIZARD_H

#include <QDialog>

#include "ui_bugreportwizard.h"

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class BugReportWizard : public QDialog, Ui::BugReportWizard
{
  Q_OBJECT
  public:
    BugReportWizard(QWidget *parent);
    virtual ~BugReportWizard();

  private slots:
    void reportBug();
};

}

#endif

// vim: ts=2 sw=2 et
