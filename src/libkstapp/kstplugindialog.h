/***************************************************************************
                      kstplugindialog.h  -  Part of KST
                             -------------------
    begin                : Mon May 12 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef KSTPLUGINDIALOGI_H
#define KSTPLUGINDIALOGI_H

#include "kstcplugin.h"

#include "kstdatadialog.h"
#include "kst_export.h"
//Added by qt3to4:
#include <QGridLayout>
#include <Q3ValueList>

class PluginDialogWidget;

class KST_EXPORT KstPluginDialogI : public KstDataDialog {
  Q_OBJECT
  public:
    KstPluginDialogI(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    virtual ~KstPluginDialogI();
    static KstPluginDialogI *globalInstance();
    static const QString& plugin_defaultTag;

  protected:
    QString objectName() { return tr("Plugin"); }
 
  public slots:
    void updateForm();
    void update();
    bool newObject();
    bool editObject();
    virtual void showNew(const QString &field);
    virtual void updatePluginList();

  protected slots:
    void pluginChanged(int);
    void showPluginManager();
    void fixupLayout();
    void updateScalarTooltip(const QString& n);
    void updateStringTooltip(const QString& n);

  protected:
    QStringList _pluginList;

    void fillVectorScalarCombos(KstSharedPtr<Plugin> pPtr);
    virtual bool saveInputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p);
    bool saveOutputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p);
    virtual void generateEntries(bool input, int& cnt, QWidget *parent,
        QGridLayout *grid, const Q3ValueList<Plugin::Data::IOValue>& table);

    QMap<QString,QString> cacheInputs(const Q3ValueList<Plugin::Data::IOValue>& table);
    void restoreInputs(const Q3ValueList<Plugin::Data::IOValue>& table, const QMap<QString,QString>& v);

    // layout items
    QGridLayout* _pluginInfoGrid;
    QGridLayout* _pluginInputOutputGrid;
    Q3ValueList<QWidget*> _pluginWidgets;

  private:
    QString _pluginName;
    static QPointer<KstPluginDialogI> _inst;

  protected:
    void fillFieldsForEdit();
    void fillFieldsForNew();
    PluginDialogWidget *_w;
};

#endif
// vim: ts=2 sw=2 et
