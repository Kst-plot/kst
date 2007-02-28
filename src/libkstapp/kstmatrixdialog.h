/***************************************************************************
                       kstmatrixdialog_i.h  -  Part of KST
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

#ifndef KSTMATRIXDIALOGI_H
#define KSTMATRIXDIALOGI_H

#include "kstdatadialog.h"
#include "kstrmatrix.h"
#include "kstsmatrix.h"
#include "kst_export.h"

class KCompletion;
class MatrixDialogWidget;

class KST_EXPORT KstMatrixDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstMatrixDialogI(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    virtual ~KstMatrixDialogI();
    static KstMatrixDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Matrix"); }
 
  public slots:
    void update();
    void updateWindow();
    void updateCompletion();
    bool newObject();
    bool editObject();
    void populateEditMultiple();

  signals:
    void matrixCreated(KstMatrixPtr v);

  private:
    QPointer<KCompletion> _fieldCompletion;
    QPointer<KstDataSourceConfigWidget> _configWidget;

    static QPointer<KstMatrixDialogI> _inst;
    bool checkParameters(bool ok1, bool ok2, bool ok3, bool ok4, double xStep, double yStep);

    bool new_ISMatrix();
    bool new_IRMatrix();

    bool editSingleSMatrix(KstSMatrixPtr smp);
    bool editSingleRMatrix(KstRMatrixPtr rmp);

    void populateEditMultipleRMatrix();
    void populateEditMultipleSMatrix();

    void fillFieldsForRMatrixEdit();
    void fillFieldsForSMatrixEdit();

    bool _inTest; // testing URL for datasource

    bool _fileNameDirty : 1;
    bool _fieldDirty : 1;
    bool _xStartDirty : 1;
    bool _xStartCountFromEndDirty : 1;
    bool _yStartDirty : 1;
    bool _yStartCountFromEndDirty : 1;
    bool _xNumStepsDirty : 1;
    bool _xNumStepsReadToEndDirty : 1;
    bool _yNumStepsDirty : 1;
    bool _yNumStepsReadToEndDirty : 1;
    bool _gradientXDirty : 1;
    bool _gradientYDirty : 1;
    bool _gradientZAtMinDirty : 1;
    bool _gradientZAtMaxDirty : 1;
    bool _minXDirty : 1;
    bool _minYDirty : 1;
    bool _xStepDirty : 1;
    bool _yStepDirty : 1;
    bool _nXDirty : 1;
    bool _nYDirty : 1;
    bool _doSkipDirty : 1;
    bool _skipDirty : 1;
    bool _doAveDirty : 1;

    bool editSingleObject(KstMatrixPtr mxPtr);

  private slots:
    void updateEnables();
    void xStartCountFromEndClicked();
    void xNumStepsReadToEndClicked();
    void yStartCountFromEndClicked();
    void yNumStepsReadToEndClicked();
    void configureSource();
    void enableSource();
    void markSourceAndSave();
    void testURL();

    void setXStartCountFromEndDirty();
    void setYStartCountFromEndDirty();
    void setXNumStepsReadToEndDirty();
    void setYNumStepsReadToEndDirty();
    void setDoSkipDirty();
    void setDoAveDirty();

  protected:
    KstObjectPtr findObject(const QString& name);

  private:
    void fillFieldsForEdit();
    void fillFieldsForNew();
    void cleanup();
    MatrixDialogWidget *_w;
};

#endif
// vim: ts=2 sw=2 et
