/***************************************************************************
                       ksteditviewobjectdialog_i.h  -  Part of KST
                             -------------------
    begin                : 2005
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

#ifndef KSTEDITVIEWOBJECTDIALOGI_H
#define KSTEDITVIEWOBJECTDIALOGI_H

#include "editviewobjectdialog.h"
#include "kstviewobject.h"
#include "ksttoplevelview.h"
#include "kst_export.h"
#include <qguardedptr.h>

class QComboBox;
class QGridLayout;

class KST_EXPORT KstEditViewObjectDialogI : public KstEditViewObjectDialog {
  Q_OBJECT
  public:
    KstEditViewObjectDialogI(QWidget* parent = 0,
                             const char* name = 0,
                             bool modal = false, WFlags fl = 0 );
    virtual ~KstEditViewObjectDialogI();

  public slots:
    void updateEditViewObjectDialog();
    void showEditViewObjectDialog(KstViewObjectPtr viewObject, KstTopLevelViewPtr top);
    void setNew();
    
  private:
    void updateWidgets();
    void clearWidgets();
    
    void fillPenStyleWidget(QComboBox* widget);
    void fillHJustifyWidget(QComboBox* widget);
    void fillVJustifyWidget(QComboBox* widget);
    
    KstViewObjectPtr _viewObject; // the view object we are currently editing
    KstTopLevelViewPtr _top; // the top level view that invoked this dialog
    
    // for layout purposes
    QValueList<QWidget*> _inputWidgets; // the widgets used to change properties
    QValueList<QWidget*> _widgets; // all other widgets
    QGridLayout* _grid;
    QGuardedPtr<QWidget> _customWidget;
    bool _isNew;
    
  private slots:
    void applyClicked();
    void okClicked();
    void modified();
};


#endif
// vim: ts=2 sw=2 et
