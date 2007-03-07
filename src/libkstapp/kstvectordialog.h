/***************************************************************************
                      kstvectordialog.h  -  Part of KST
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
#ifndef KSTVECTORDIALOGI_H
#define KSTVECTORDIALOGI_H

#include "kstdatadialog.h"
#include "kst_export.h"

#include "ui_kstvectordialog4.h"

class KCompletion;

class KST_EXPORT KstVectorDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstVectorDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
    virtual ~KstVectorDialogI();
    static KstVectorDialogI *globalInstance();

  protected:
    QString objectName() { return tr("Vector"); }
 
  public slots:
    bool newObject();
    bool editObject();
    void populateEditMultiple();
    void updateCompletion();

  signals:
    void vectorCreated(KstVectorPtr v);

  private:
    static QPointer<KstVectorDialogI> _inst;
    QPointer<KCompletion> _fieldCompletion;
    QPointer<KstDataSourceConfigWidget> _configWidget;

    bool _inTest : 1;

    // the following are for the multiple edit mode
    bool _fileNameDirty : 1;
    bool _f0Dirty : 1;
    bool _nDirty : 1;
    bool _countFromEndDirty : 1;
    bool _readToEndDirty : 1;
    bool _doFilterDirty : 1;
    bool _doSkipDirty : 1;
    bool _skipDirty : 1;
    bool _NDirty : 1;
    bool _xMinDirty : 1;
    bool _xMaxDirty : 1;
    bool editSingleObject(KstVectorPtr vcPtr);
    bool editSingleObjectRV(KstVectorPtr vcPtr);
    bool editSingleObjectSV(KstVectorPtr vcPtr);
    void populateEditMultipleRV();
    void populateEditMultipleSV();

  private slots:
    void configureSource();
    void enableSource();
    void enableGenerate();
    void markSourceAndSave();
    void testUrl();

    // for multiple edit mode
    void setF0Dirty() { _f0Dirty = true; }
    void setNDirty() { _nDirty = true; }
    void setCountFromEndDirty();
    void setReadToEndDirty();
    void setDoFilterDirty();
    void setDoSkipDirty();

  protected:
    KstObjectPtr findObject(const QString& name);

  private:
    void fillFieldsForEdit();
    void fillFieldsForSVEdit();
    void fillFieldsForRVEdit();
    void fillFieldsForNew();
    void cleanup();
    static const QString& defaultTag;
    Ui::KstVectorDialog *_w;
};

#endif
// vim: ts=2 sw=2 et
