/***************************************************************************
                      kstfilterdialog.h  -  Part of KST
                             -------------------
    begin                : Wed Jul 28 2004
    copyright            : (C) 2004 The University of British Columbia
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

#ifndef KSTFILTERDIALOGI_H
#define KSTFILTERDIALOGI_H

#include "kstplugindialog.h"

#include <QDialog>

//Added by qt3to4:
#include <QGridLayout>
#include <Q3ValueList>

class KstFilterDialogI : public KstPluginDialogI {
  Q_OBJECT
  public:
    KstFilterDialogI(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0);
    virtual ~KstFilterDialogI();
    static KstFilterDialogI *globalInstance();

  public slots:
    void show_setCurve(const QString& strCurve, const QString& strPlotName, const QString& strWindow);
    bool newObject();
    void updatePluginList();

  private:
    QString _xvector;
    QString _yvector;
    QString _evector;
    QString _window;
    QString _plotName;
    QString _curve;
    static QPointer<KstFilterDialogI> _inst;

    bool createCurve(KstCPluginPtr plugin);
    bool saveInputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p);
    void generateEntries(bool input, int& cnt, QWidget *parent, QGridLayout *grid, const Q3ValueList<Plugin::Data::IOValue>& table);
};

#endif
// vim: ts=2 sw=2 et
