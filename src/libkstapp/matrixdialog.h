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

#ifndef MATRIXDIALOG_H
#define MATRIXDIALOG_H

#include "datadialog.h"
#include "datatab.h"

#include "ui_matrixtab.h"

#include <QPointer>

#include "kst_export.h"

#include "datasource.h"

namespace Kst {

class ObjectStore;

class MatrixTab : public DataTab, Ui::MatrixTab {
  Q_OBJECT
  public:
    enum MatrixMode { DataMatrix, GeneratedMatrix };

    MatrixTab(ObjectStore *store, QWidget *parent = 0);
    virtual ~MatrixTab();

    MatrixMode matrixMode() const { return _mode; }
    void setMatrixMode(MatrixMode mode);

    //DataMatrix mode methods...
    DataSourcePtr dataSource() const;
    void setDataSource(DataSourcePtr dataSource);

    QString file() const;
    void setFile(const QString &file);

    QString field() const;
    void setField(const QString &field);

    void setFieldList(const QStringList &fieldList);

    uint nX() const;
    bool nXDirty() const;
    void setNX(uint nX);

    uint nY() const;
    bool nYDirty() const;
    void setNY(uint nY);

    double minX() const;
    bool minXDirty() const;
    void setMinX(double minX);

    double minY() const;
    bool minYDirty() const;
    void setMinY(double minY);

    double stepX() const;
    bool stepXDirty() const;
    void setStepX(double stepX);

    double stepY() const;
    bool stepYDirty() const;
    void setStepY(double stepY);

    int xStart() const;
    bool xStartDirty() const;
    void setXStart(int xStart);

    int yStart() const;
    bool yStartDirty() const;
    void setYStart(int yStart);

    int xNumSteps() const;
    bool xNumStepsDirty() const;
    void setXNumSteps(int xNumSteps);

    int yNumSteps() const;
    bool yNumStepsDirty() const;
    void setYNumSteps(int yNumSteps);

    double gradientZAtMin() const;
    bool gradientZAtMinDirty() const;
    void setGradientZAtMin(double gradientZAtMin);

    double gradientZAtMax() const;
    bool gradientZAtMaxDirty() const;
    void setGradientZAtMax(double gradientZAtMax);

    bool xDirection() const;
    void setXDirection(bool xDirection);

    bool doAverage() const;
    bool doAverageDirty() const;
    void setDoAverage(bool doAve);

    bool doSkip() const;
    bool doSkipDirty() const;
    void setDoSkip(bool doSkip);

    int skip() const;
    bool skipDirty() const;
    void setSkip(int skip);

    bool xStartCountFromEnd() const;
    bool xStartCountFromEndDirty() const;
    void setXStartCountFromEnd(bool xStartCountFromEnd);

    bool yStartCountFromEnd() const;
    bool yStartCountFromEndDirty() const;
    void setYStartCountFromEnd(bool yStartCountFromEnd);

    bool xReadToEnd() const;
    bool xReadToEndDirty() const;
    void setXReadToEnd(bool xReadToEnd);

    bool yReadToEnd() const;
    bool yReadToEndDirty() const;
    void setYReadToEnd(bool yReadToEnd);

    void hideGeneratedOptions();
    void hideDataOptions();
    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  Q_SIGNALS:
    void sourceChanged();

  private Q_SLOTS:
    void readFromSourceChanged();
    void fileNameChanged(const QString &file);
    void showConfigWidget();
    void sourceValid(QString filename, int requestID);

    void updateEnables();
    void xStartCountFromEndClicked();
    void xNumStepsReadToEndClicked();
    void yStartCountFromEndClicked();
    void yNumStepsReadToEndClicked();

  private:
    MatrixMode _mode;
    DataSourcePtr _dataSource;
    ObjectStore *_store;
    QString _initField;
    int _requestID;
};

class MatrixDialog : public DataDialog {
  Q_OBJECT
  public:
    MatrixDialog(ObjectPtr dataObject, QWidget *parent = 0);
    virtual ~MatrixDialog();

  protected:
//     virtual QString tagString() const;
    virtual ObjectPtr createNewDataObject();
    virtual ObjectPtr editExistingDataObject() const;

  private:
    ObjectPtr createNewDataMatrix();
    ObjectPtr createNewGeneratedMatrix();
    void configureTab(ObjectPtr vector);

  private Q_SLOTS:
    void updateButtons();
    void editMultipleMode();
    void editSingleMode();

  private:
    MatrixTab *_matrixTab;
};

}

#endif

// vim: ts=2 sw=2 et
