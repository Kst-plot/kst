/***************************************************************************
                       ksthsdialog_i.h  -  Part of KST
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

#ifndef KSTHSDIALOGI_H
#define KSTHSDIALOGI_H

#include "kstdatadialog.h"
#include "ksthistogram.h"
#include "kst_export.h"

class HistogramDialogWidget;

class KST_EXPORT KstHsDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstHsDialogI(QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    virtual ~KstHsDialogI();
    static KstHsDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Histogram"); }
 
  public slots:
    void update();
    void updateWindow();
    bool newObject();
    bool editObject();
    void autoBin();
    void populateEditMultiple();
    void setVector(const QString& name);

  private:
    static QGuardedPtr<KstHsDialogI> _inst;
    // the following are for the multiple edit mode
    bool _vectorDirty;
    bool _minDirty;
    bool _maxDirty;
    bool _nDirty;
    bool _realTimeAutoBinDirty;
    bool _normIsPercentDirty;
    bool _normIsFractionDirty;
    bool _peakIs1Dirty;
    bool _normIsNumberDirty;
    bool editSingleObject(KstHistogramPtr hsPtr);

  private slots:
    void updateButtons();
    void setVectorDirty() { _vectorDirty = true; }
    void setMinDirty() { _minDirty = true; }
    void setMaxDirty() { _maxDirty = true; }
    void setRealTimeAutoBinDirty();
    void setNormIsPercentDirty() { _normIsPercentDirty = true; }
    void setNormIsFractionDirty() { _normIsFractionDirty = true; }
    void setPeakIs1Dirty() { _peakIs1Dirty = true; }
    void setNormIsNumberDirty() { _normIsNumberDirty = true; }

  private:
    static const QString& defaultTag;
    void fillFieldsForEdit();
    void fillFieldsForNew();
    void cleanup();
    HistogramDialogWidget *_w;
};

#endif
// vim: ts=2 sw=2 et
