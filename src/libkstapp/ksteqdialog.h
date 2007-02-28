/***************************************************************************
                       ksteqdialog_i.h  -  Part of KST
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

#ifndef KSTEQDIALOGI_H
#define KSTEQDIALOGI_H

#include "kstdatadialog.h"
#include "kstequation.h"
#include "kst_export.h"

class EqDialogWidget;

class KST_EXPORT KstEqDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstEqDialogI(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0);
    virtual ~KstEqDialogI();
    static KstEqDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Equation"); }
 
  public slots:
    void update();
    void updateWindow();
    void populateEditMultiple();
    bool newObject();
    bool editObject();

  private:
    static QPointer<KstEqDialogI> _inst;
    bool checkEntries();
    bool _equationDirty;
    bool _xVectorsDirty;
    bool _doInterpolationDirty;

    bool editSingleObject(KstEquationPtr eqPtr);

  private slots:
    void setDoInterpolationDirty();

  protected:
    void fillFieldsForEdit();
    void fillFieldsForNew();

  private:
    static const QString& defaultTag;
    void populateFunctionList();
    EqDialogWidget *_w;
};

#endif
// vim: ts=2 sw=2 et
