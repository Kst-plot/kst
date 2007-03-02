/***************************************************************************
                       kstviewscalarsdialog.h  -  Part of KST
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

#ifndef KSTVIEWSCALARSDIALOGI_H
#define KSTVIEWSCALARSDIALOGI_H

#include <QDialog>

#include "ui_kstviewscalarsdialog4.h"

#include <klistviewsearchline.h>

//#include "kstscalartable.h"
#include "kstscalarlistview.h"

class KstViewScalarsDialogI : public QDialog {
  Q_OBJECT
  public:
    KstViewScalarsDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    virtual ~KstViewScalarsDialogI();
    KListViewSearchLineWidget *searchWidget;
    KstScalarListView *listViewScalars;

    bool hasContent() const;

  protected slots:
    virtual void languageChange();

  public slots:
    void updateViewScalarsDialog();
    void showViewScalarsDialog();
    void updateDefaults(int index = 0);

  signals:
    /** signal that vectors have changed */
    void docChanged();
};

#endif
// vim: ts=2 sw=2 et
