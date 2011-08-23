/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CURVEDIALOG_H
#define CURVEDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "ui_curvetab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

class CurveTab : public DataTab, Ui::CurveTab {
  Q_OBJECT
  public:
    CurveTab(QWidget *parent = 0);
    virtual ~CurveTab();

    VectorPtr xVector() const;
    bool xVectorDirty() const;
    void setXVector(VectorPtr vector);
    bool xVectorSelected() const;

    VectorPtr yVector() const;
    bool yVectorDirty() const;
    void setYVector(VectorPtr vector);
    bool yVectorSelected() const;

    VectorPtr xError() const;
    bool xErrorDirty() const;
    void setXError(VectorPtr vector);

    VectorPtr yError() const;
    bool yErrorDirty() const;
    void setYError(VectorPtr vector);

    VectorPtr xMinusError() const;
    bool xMinusErrorDirty() const;
    void setXMinusError(VectorPtr vector);

    VectorPtr yMinusError() const;
    bool yMinusErrorDirty() const;
    void setYMinusError(VectorPtr vector);

    CurveAppearance* curveAppearance() const;
    CurvePlacement* curvePlacement() const;

    bool ignoreAutoScale() const;
    bool ignoreAutoScaleDirty() const;
    void setIgnoreAutoScale(bool ignoreAutoScale);


    void setObjectStore(ObjectStore *store);
    void hidePlacementOptions();
    void clearTabValues();

    void setToLastX() {_xVector->setToLastX();}

  Q_SIGNALS:
    void vectorsChanged();

  public Q_SLOTS:
    void xCheckboxClicked();
    void yCheckboxClicked();
    void xErrorChanged();
    void yErrorChanged();
    void updateVectorCombos();
};

class CurveDialog : public DataDialog {
  Q_OBJECT
  public:
    explicit CurveDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~CurveDialog();

    void setVector(VectorPtr vector);

  protected:
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr editExistingDataObject() const;
    virtual bool dialogValid() const;

  private Q_SLOTS:
    void updateButtons();
    void editMultipleMode();
    void editSingleMode();

  private:
    void configureTab(ObjectPtr curve=0);

    CurveTab *_curveTab;
};

}

#endif

// vim: ts=2 sw=2 et
