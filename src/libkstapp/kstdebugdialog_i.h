/***************************************************************************
                       kstdebugdialog_i.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2004 The University of British Columbia
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
#ifndef KSTDEBUGI_H
#define KSTDEBUGI_H

#include "debugdialog.h"

class KstLogWidget;

class KstDebugDialogI : public DebugDialog {
  Q_OBJECT
  public:
    KstDebugDialogI(QWidget* parent = 0, const char* name = 0,
        bool modal = FALSE, WFlags fl = 0);
    virtual ~KstDebugDialogI();

  public:
    KstLogWidget *logWidget() const;

  public slots:
    void email();
    void clear();
    void show_I();

  private:  
    KstLogWidget *_log;
};

#endif
// vim: ts=2 sw=2 et
