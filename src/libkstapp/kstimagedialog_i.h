/***************************************************************************
                       kstimagedialog_i.h  -  Part of KST
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

#ifndef KSTIMAGEDIALOGI_H
#define KSTIMAGEDIALOGI_H

#include "kstdatadialog.h"
#include "kstimage.h"
#include "kst_export.h"

class ImageDialogWidget;

class KST_EXPORT KstImageDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstImageDialogI(QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags fl = 0 );
    virtual ~KstImageDialogI();
    static KstImageDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Image"); }
 
  public slots:
    void update();
    void updateWindow();
    bool newObject();
    bool editObject();
    void populateEditMultiple();
    void setMatrix(const QString& name);

  private:
    static QGuardedPtr<KstImageDialogI> _inst;
    void placeInPlot(KstImagePtr image);
    bool checkParameters(double& lowerZDouble, double& upperZDouble);
    void fillFieldsForEditNoUpdate();
    
    // the following are for the multiple edit mode
    bool _matrixDirty : 1;
    bool _colorOnlyDirty : 1;
    bool _contourOnlyDirty : 1;
    bool _colorAndContourDirty : 1;
    bool _paletteDirty : 1;
    bool _lowerZDirty : 1;
    bool _upperZDirty : 1;
    bool _realTimeAutoThresholdDirty : 1;
    bool _numContourLinesDirty : 1;
    bool _contourWeightDirty : 1;
    bool _useVariableWeightDirty : 1;
    bool _contourColorDirty : 1;
    bool editSingleObject(KstImagePtr imPtr);
    
  private slots:
    void calcAutoThreshold();
    void calcSmartThreshold();
    void updateGroups();
    void updateEnables();
    
    // for multiple edit mode
    void setColorOnlyDirty() { _colorOnlyDirty = true; }
    void setContourOnlyDirty() { _contourOnlyDirty = true; }
    void setColorAndContourDirty() { _colorAndContourDirty = true; }
    void setRealTimeAutoThresholdDirty();
    void setUseVariableWeightDirty();
    void setContourColorDirty() { _contourColorDirty = true; }
    
  private:
    void fillFieldsForEdit();
    void fillFieldsForNew();
    void cleanup();
    ImageDialogWidget *_w;
};

#endif
// vim: ts=2 sw=2 et
