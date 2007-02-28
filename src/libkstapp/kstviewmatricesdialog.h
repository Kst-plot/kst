/***************************************************************************
                       kstviewmatricesdialog_i.h  -  Part of KST
                             -------------------
    begin                :
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

#ifndef KSTVIEWMATRICESDIALOGI_H
#define KSTVIEWMATRICESDIALOGI_H

#include "viewmatricesdialog.h"
#include "kstmatrixtable.h"

class KstViewMatricesDialogI : public KstViewMatricesDialog {
  Q_OBJECT
  public:
    KstViewMatricesDialogI(QWidget* parent = 0,
                        const char* name = 0,
                        bool modal = false, Qt::WFlags fl = 0 );
    virtual ~KstViewMatricesDialogI();
    KstMatrixTable* _tableMatrices;

    bool hasContent() const;

  public slots:
    void updateViewMatricesDialog();
    void updateViewMatricesDialog(const QString& strVector);
    void showViewMatricesDialog();
    void updateDefaults(int index = 0);

  protected slots:
    virtual void languageChange();  
    virtual void matrixChanged(const QString& str);
};

#endif
// vim: ts=2 sw=2 et
