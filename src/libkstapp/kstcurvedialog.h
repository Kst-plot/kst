/***************************************************************************
                       kstcurvedialog.h  -  Part of KST
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

#ifndef KSTCURVEDIALOGI_H
#define KSTCURVEDIALOGI_H

#include "kstdatadialog.h"
#include "kstvcurve.h"
#include "kst_export.h"

#include "ui_kstcurvedialog4.h"

class KstCurveDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstCurveDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    virtual ~KstCurveDialogI();
    KST_EXPORT static KstCurveDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Curve"); }
 
  public slots:
    void update();
    void updateWindow();
    bool newObject();
    bool editObject();
    void populateEditMultiple();
    void setVector(const QString& name);

  private:
    static QPointer<KstCurveDialogI> _inst;

    // the following are for the multiple edit mode
    bool _xVectorDirty : 1;
    bool _yVectorDirty : 1;
    bool _xErrorDirty : 1;
    bool _xMinusErrorDirty : 1;
    bool _yErrorDirty : 1;
    bool _yMinusErrorDirty : 1;
    bool _checkBoxXMinusSameAsPlusDirty : 1;
    bool _checkBoxYMinusSameAsPlusDirty : 1;
    bool _colorDirty : 1;
    bool _showPointsDirty : 1;
    bool _showLinesDirty : 1;
    bool _showBarsDirty : 1;
    bool _comboDirty : 1;
    bool _comboPointDensityDirty : 1;
    bool _comboLineStyleDirty : 1;
    bool _spinBoxLineWidthDirty : 1;
    bool _barStyleDirty : 1;
    bool _checkBoxIgnoreAutoscaleDirty : 1;
    bool editSingleObject(KstVCurvePtr cvPtr);

  private slots:
    // multiple edit mode
    void setCheckBoxXMinusSameAsPlusDirty();
    void setCheckBoxYMinusSameAsPlusDirty();
    void setColorDirty() { _colorDirty = true; }
    void setShowPointsDirty();
    void setShowLinesDirty();
    void setShowBarsDirty();
    void setCheckBoxIgnoreAutoscaleDirty();
    void toggledXErrorSame(bool);
    void toggledYErrorSame(bool);
    void toggledXErrorSame();
    void toggledYErrorSame();

  private:
    static const QString& defaultTag;
    void fillFieldsForEdit();
    void fillFieldsForNew();
    void cleanup();
    Ui::KstCurveDialog *_w;
};

#endif
// vim: ts=2 sw=2 et
