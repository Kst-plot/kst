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

#ifndef VECTORDIALOG_H
#define VECTORDIALOG_H

#include "datadialog.h"
#include "dialogtab.h"

#include "ui_vectortab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT VectorTab : public DialogTab, Ui::VectorTab {
  Q_OBJECT
  public:
    enum Mode { ReadOnlyVector, SlaveVector };

    VectorTab(QWidget *parent = 0);
    virtual ~VectorTab();

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    //Slave vector mode methods...
    qreal from() const;
    void setFrom(qreal from);

    qreal to() const;
    void setTo(qreal to);

    int numberOfSamples() const;
    void setNumberOfSamples(int numberOfSamples);

  private Q_SLOTS:
    void sourceChanged();

  private:
    Mode _mode;
};

class KST_EXPORT VectorDialog : public DataDialog {
  Q_OBJECT
  public:
    VectorDialog(QWidget *parent = 0);
    VectorDialog(KstObjectPtr dataObject, QWidget *parent = 0);
    virtual ~VectorDialog();

  protected:
    virtual KstObjectPtr createNewDataObject() const;
    virtual KstObjectPtr editExistingDataObject() const;

  private:
    void setDefaults();

  private:
    VectorTab *_vectorTab;
};

}

#endif

// vim: ts=2 sw=2 et
