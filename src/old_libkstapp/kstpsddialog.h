/***************************************************************************
                       kstpsddialog.h  -  Part of KST
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

#ifndef KSTPSDDIALOGI_H
#define KSTPSDDIALOGI_H

#include "kstdatadialog.h"
#include "kstpsd.h"
#include "kst_export.h"

#include "ui_kstpsddialog4.h"

class KstPsdDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstPsdDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    virtual ~KstPsdDialogI();
    KST_EXPORT static KstPsdDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Power Spectrum"); }
 
  public slots:
    /** update the entries in the psd dialog to represent current psds */
    void update();
    void updateWindow();
    bool newObject();
    bool editObject();
    void populateEditMultiple();
    void setVector(const QString& name);

  private:
    static QPointer<KstPsdDialogI> _inst;

    // the following are for the multiple edit mode
    bool _vectorDirty : 1;
    bool _apodizeDirty : 1;
    bool _apodizeFxnDirty : 1;
    bool _gaussianSigmaDirty : 1;
    bool _removeMeanDirty : 1;
    bool _interleavedDirty : 1;
    bool _vectorUnitsDirty : 1;
    bool _fFTLenDirty : 1;
    bool _sampRateDirty : 1;
    bool _rateUnitsDirty : 1;
    bool _outputDirty : 1;
    bool _interpolateHolesDirty : 1;
    bool editSingleObject(KstPSDPtr psPtr);

  private slots:
    void setApodizeDirty();
    void setRemoveMeanDirty();
    void setInterleavedDirty();
    void setInterpolateHolesDirty();

  private:
    static const QString& defaultTag;
    void fillFieldsForEdit();
    void fillFieldsForNew();
    void cleanup();
    Ui::KstPsdDialog *_w;
};

#endif
// vim: ts=2 sw=2 et
