/**************************************************************************
        kstmonochromedialog.h - source file: inherits designer dialog
                             -------------------
    begin                :  2005
    copyright            : (C) 2005 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTMONOCHROMEDIALOGI_H
#define KSTMONOCHROMEDIALOGI_H

#include <QDialog>

#include "ui_kstmonochromedialog4.h"

class KstMonochromeDialogI : public QDialog {
  Q_OBJECT
  public:
    KstMonochromeDialogI(QWidget* parent = 0,
                         const char* name = 0,
                         bool modal = TRUE, WFlags fl = 0 );
    virtual ~KstMonochromeDialogI();
    
    void setOptions(const QMap<QString,QString>& opts);
    void getOptions(QMap<QString,QString> &opts, bool include_def = false);

  public slots:

    void updateMonochromeDialog();

    /** calls updateMonochromeDialog(), then shows and raises the dialog */
    void showMonochromeDialog();
    
  private slots:
    void updateButtons();
    void removeClicked();
    void addClicked();
    void upClicked();
    void downClicked();
};

#endif
// vim: ts=2 sw=2 et
