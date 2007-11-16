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

#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "image.h"

#include "ui_imagetab.h"

#include <QPointer>

#include "kst_export.h"

namespace Kst {

  class ObjectStore;

class KST_EXPORT ImageTab : public DataTab, Ui::ImageTab {
  Q_OBJECT
  public:
    ImageTab(QWidget *parent = 0);
    virtual ~ImageTab();

    CurvePlacement* curvePlacement() const;
    ColorPalette* colorPalette() const;

    void setObjectStore(ObjectStore *store);

    bool realTimeAutoThreshold() const;
    void setRealTimeAutoThreshold(const bool realTimeAutoThreshold);

    bool colorOnly() const;
    void setColorOnly(const bool colorOnly);

    bool contourOnly() const;
    void setContourOnly(const bool contourOnly);

    bool colorAndContour() const;
    void setColorAndContour(const bool colorAndContour);

    double lowerThreshold() const;
    void setLowerThreshold(const double lowerThreshold);

    double upperThreshold() const;
    void setUpperThreshold(const double upperThreshold);

    int numberOfContourLines() const;
    void setNumberOfContourLines(const int numberOfContourLines);

    int contourWeight() const;
    void setContourWeight(const int contourWeight);

    QColor contourColor() const;
    void setContourColor(const QColor contourColor);

    MatrixPtr matrix() const;
    void setMatrix(const MatrixPtr matrix);

    bool useVariableLineWeight() const;
    void setUseVariableLineWeight(const bool useVariableLineWeight);

    void hidePlacementOptions();

  private Q_SLOTS:
    void realTimeAutoThresholdToggled(const bool checked);
    void updateEnabled(const bool checked);
    void selectionChanged();
    void calculateAutoThreshold();
    void calculateSmartThreshold();

  Q_SIGNALS:
    void optionsChanged();

  private:
    ObjectStore* _store;
};

class KST_EXPORT ImageDialog : public DataDialog {
  Q_OBJECT
  public:
    ImageDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~ImageDialog();

  protected:
    virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject() const;
    virtual ObjectPtr editExistingDataObject() const;

  private Q_SLOTS:
    void updateButtons();

  private:
    void configureTab(ObjectPtr object);

    ImageTab *_imageTab;
};

}

#endif
