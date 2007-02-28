/***************************************************************************
                      kstfitdialog_i.h  -  Part of KST
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

#ifndef KSTFITDIALOGI_H
#define KSTFITDIALOGI_H

#include "kstplugindialog_i.h"
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3ValueList>

class KstFitDialogI : public KstPluginDialogI {
  Q_OBJECT
  public:
    KstFitDialogI(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    virtual ~KstFitDialogI();
    static KstFitDialogI *globalInstance();

  public slots:
    void show_setCurve(const QString& strCurve, const QString& strPlotName, const QString& strWindow);
    bool newObject();
    void updatePluginList();

  private:
    static QPointer<KstFitDialogI> _inst;
    
    QString _xvector;
    QString _yvector;
    QString _evector;
    QString _strWindow;
    QString _strPlotName;
    QString _strCurve;
    
    bool createCurve(KstCPluginPtr plugin);
    
    void generateEntries(bool input, int& cnt, QWidget *parent, Q3GridLayout *grid, const Q3ValueList<Plugin::Data::IOValue>& table);
    
    bool saveInputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p);
};

#endif
// vim: ts=2 sw=2 et
