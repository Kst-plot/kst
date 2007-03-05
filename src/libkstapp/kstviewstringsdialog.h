/***************************************************************************
                       kstviewstringsdialog.h  -  Part of KST
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

#include <QDialog>

#include "ui_kstviewstringsdialog4.h"

#include <k3listviewsearchline.h>
#include "kststringlistview.h"

class KstViewStringsDialogI : public QDialog {
  Q_OBJECT
  public:
    KstViewStringsDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    virtual ~KstViewStringsDialogI();
    K3ListViewSearchLineWidget *searchWidget;
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
