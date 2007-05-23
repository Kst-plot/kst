/***************************************************************************
                      kstbasicdialog.h  -  Part of KST
                             -------------------
    begin                : 09/15/06
    copyright            : (C) 2006 The University of Toronto
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

#ifndef KSTBASICDIALOGI_H
#define KSTBASICDIALOGI_H

#include <qpointer.h>
//Added by qt3to4:
#include <QGridLayout>
#include <Q3ValueList>

#include "kst_export.h"
#include "kstdatadialog.h"
#include "kstbasicplugin.h"

#include "ui_kstbasicdialog4.h"

class QLineEdit;
class VectorSelector;
class ScalarSelector;
class StringSelector;

class KST_EXPORT KstBasicDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstBasicDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    virtual ~KstBasicDialogI();
    static KstBasicDialogI *globalInstance();

  public slots:
    virtual void update();
    virtual bool newObject();
    virtual bool editObject();
    virtual void showNew(const QString &field);
    virtual void showEdit(const QString &field);

  protected:
    virtual void fillFieldsForEdit();
    virtual void fillFieldsForNew();

  private slots:
    void init();

  private:
    bool editSingleObject(KstBasicPluginPtr ptr);

    void createInputVector(const QString &name, int row);
    void createInputScalar(const QString &name, int row);
    void createInputString(const QString &name, int row);
    void createOutputWidget(const QString &name, int row);

    VectorSelector *vector(const QString &name) const;
    ScalarSelector *scalar(const QString &name) const;
    StringSelector *string(const QString &name) const;
    QLineEdit *output(const QString &name) const;

    static const QString& defaultTag;
    static QPointer<KstBasicDialogI> _inst;

    QString _pluginName;
    Ui::KstBasicDialog *_w;

    // layout items
    QGridLayout *_grid;
    Q3ValueList<QWidget*> _widgets;
};

#endif
// vim: ts=2 sw=2 et
