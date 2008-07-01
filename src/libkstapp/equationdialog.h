/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EQUATIONDIALOG_H
#define EQUATIONDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "kst_export.h"

#include "ui_equationtab.h"

namespace Kst {

class KST_EXPORT EquationTab : public DataTab, Ui::EquationTab {
  Q_OBJECT
  public:
    EquationTab(QWidget *parent = 0);
    virtual ~EquationTab();

    void setObjectStore(ObjectStore *store);

    VectorPtr xVector() const;
    bool xVectorDirty() const;
    void setXVector(VectorPtr vector);

    QString equation() const;
    bool equationDirty() const;
    void setEquation(const QString &equation);

    bool doInterpolation() const;
    bool doInterpolationDirty() const;
    void setDoInterpolation(bool doInterpolation);

    CurveAppearance *curveAppearance() const;
    CurvePlacement *curvePlacement() const;

    void hideCurveOptions();
    void clearTabValues();

  public Q_SLOTS:
    void updateVectorCombos();

  Q_SIGNALS:
    void optionsChanged();

  private Q_SLOTS:
    void selectionChanged();
    void equationUpdate(const QString& string);
    void equationOperatorUpdate(const QString& string);

  private:
    void populateFunctionList();
};

class KST_EXPORT EquationDialog : public DataDialog {
  Q_OBJECT
  public:
    EquationDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~EquationDialog();

  protected:
//     virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr editExistingDataObject() const;

  private Q_SLOTS:
    void updateButtons();
    void editMultipleMode();
    void editSingleMode();

  private:
    void configureTab(ObjectPtr object);

    EquationTab *_equationTab;
};

}

#endif

// vim: ts=2 sw=2 et
