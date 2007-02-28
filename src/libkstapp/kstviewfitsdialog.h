/***************************************************************************
                       kstviewfitsdialog.h  -  Part of KST
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

#ifndef KSTVIEWFITSDIALOGI_H
#define KSTVIEWFITSDIALOGI_H

#include "viewfitsdialog.h"
#include "kstfittable.h"

class KstViewFitsDialogI : public KstViewFitsDialog {
  Q_OBJECT
  public:
    KstViewFitsDialogI(QWidget* parent = 0,
                        const char* name = 0,
                        bool modal = false, Qt::WFlags fl = 0 );
    virtual ~KstViewFitsDialogI();
    KstFitTable* tableFits;

    bool hasContent() const;

  public slots:
    void updateViewFitsDialog();
    void showViewFitsDialog();
    void showViewFitsDialog(const QString& strVector);
    void updateDefaults(int index = 0);

  protected slots:
    virtual void fitChanged(const QString& strFit);

  private:
    void fillComboBox(const QString& str);
};


#endif
// vim: ts=2 sw=2 et
