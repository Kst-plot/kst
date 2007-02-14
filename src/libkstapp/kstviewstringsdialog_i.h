/***************************************************************************
                       kstviewstringsdialog_i.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2006 The University of Toronto
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

#ifndef KSTVIEWSTRINGSDIALOGI_H
#define KSTVIEWSTRINGSDIALOGI_H

#include <klistviewsearchline.h>

#include "viewstringsdialog.h"
#include "kststringlistview.h"

class KstViewStringsDialogI : public KstViewStringsDialog {
  Q_OBJECT
  public:
    KstViewStringsDialogI(QWidget* parent = 0,
        const char* name = 0,
        bool modal = false, WFlags fl = 0 );
    virtual ~KstViewStringsDialogI();
    KListViewSearchLineWidget *searchWidget;
    KstStringListView *listViewStrings;

    bool hasContent() const;

  protected slots:
    virtual void languageChange();

  public slots:
    void updateViewStringsDialog();
    void showViewStringsDialog();
    void updateDefaults(int index = 0);

  signals:
    /** signal that vectors have changed */
    void docChanged();
};

#endif
// vim: ts=2 sw=2 et
