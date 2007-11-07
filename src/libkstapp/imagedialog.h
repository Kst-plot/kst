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

class KST_EXPORT ImageTab : public DataTab, Ui::ImageTab {
  Q_OBJECT
  public:
    ImageTab(QWidget *parent = 0);
    virtual ~ImageTab();

    CurvePlacement* curvePlacement() const;
    ColorPalette* colorPalette() const;

    void setObjectStore(ObjectStore *store);

    bool realTimeAutoThreshold() const;
    bool colorOnly() const;
    bool contourOnly() const;
    bool colorAndContour() const;
    double lowerZ() const;
    double upperZ() const;
    int numberOfContourLines() const;
    int contourWeight() const;
    QColor contourColor() const;

    MatrixPtr matrix() const;

  private Q_SLOTS:
    void realTimeAutoThresholdToggled(const bool checked);
    void updateEnabled(const bool checked);
  private:
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

  private:
    ImageTab *_imageTab;
};

}

#endif
