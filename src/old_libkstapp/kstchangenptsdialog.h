/***************************************************************************
                       kstchangenptsdialog.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003 C. Barth Netterfield
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

#ifndef KSTCHANGENPTSDIALOGI_H
#define KSTCHANGENPTSDIALOGI_H

#include <QDialog>

#include "ui_kstchangenptsdialog4.h"

class KstChangeNptsDialogI : public QDialog, public Ui::KstChangeNptsDialog {
    Q_OBJECT
public:
    KstChangeNptsDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    virtual ~KstChangeNptsDialogI();

public slots:
  /** update the entries in changenptsDialog to represent current vectors */
  bool updateChangeNptsDialog();

  /** calls updateChangeNptsDialog(), then shows and raises changeNptsDialog */
  void showChangeNptsDialog();

  /** select all in selection box */
  void selectAll();

  /** Update fields based on vector at index */
  void updateDefaults(int index=0);

private slots:
  void emitDocChanged();
  void applyNptsChange();
  void OKNptsChange();
  void updateTimeCombo();

signals:
  /** signal that vectors have changed */
  void docChanged();
};


#endif
// vim: ts=2 sw=2 et
