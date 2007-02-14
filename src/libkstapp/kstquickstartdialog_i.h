/***************************************************************************
                       kstquickstartdialog_i.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2004 University of British Columbia
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
#ifndef KSTQUICKSTARTDIALOGI_H
#define KSTQUICKSTARTDIALOGI_H

#include "quickstartdialog.h"

class KstApp;

class KstQuickStartDialogI : public KstQuickStartDialog {
  Q_OBJECT
  public:
    KstQuickStartDialogI(QWidget *parent = 0, const char *name = 0,
        bool modal = false, WFlags fl = 0 );
    ~KstQuickStartDialogI();

  public slots:
    void update();
    void show_I();

  private slots:
    void wizard_I();
    void open_I();
    void changeURL(const QString& name);
    void updateSettings();
    void deselectRecentFile();

  private:
    KstApp* _app;
    bool _isRecentFile;

  signals:
    void settingsChanged();
};

#endif
// vim: ts=2 sw=2 et
