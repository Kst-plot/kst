/***************************************************************************
                   kstsettingsdlg.h
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
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

#ifndef KSTSETTINGSDLG_H
#define KSTSETTINGSDLG_H

#include <QDialog>
#include "ui_kstsettingsdlg4.h"

#include <math.h>
#include <kdialog.h>
#include <kpalette.h>
#include "kstsettings.h"
#include "kstplotdefines.h"

class KstSettingsDlg : public QDialog, public Ui::KstSettingsDlg {
  Q_OBJECT

public:
  KstSettingsDlg(QWidget *parent = 0);
  ~KstSettingsDlg();

public slots:
  void setSettings(const KstSettings *settings);
  void updateAxesButtons();
  void updateAxesSettings();
  void updateEMailSettings();
  void updateCurveColorSettings();
  void fillAxesSettings();
  void configureSource();
  void sourceChanged(const QString &name);

private slots:
  void init();
  void defaults();
  void setDirty();
  void setClean();
  void save();

  void updateUTCOffset();
  void setUTCOffset(const QString &timezone);
  void updateTimezone(const QString &Hours);
  void updateTimezone(double hours);

signals:
  void settingsChanged();

private:
  int utcOffset(const QString &timezone);
  QString timezoneFromUTCOffset(double hours);

};

#endif
// vim: ts=2 sw=2 et
