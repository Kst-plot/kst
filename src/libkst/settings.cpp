/***************************************************************************
              object.cpp: abstract base class for all Kst objects
                             -------------------
    copyright            : (C) 2013 The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "settings.h"

#include <QApplication>
#include <QVector>
#include <QDebug>

static QVector<QSettings*> s_settings;


QSettings& Kst::createSettings(const QString& scope)
{
  const QString organization = "kst";
  QSettings* setting =
    //new QSettings(QApplication::applicationDirPath() + "/" + organization + "-" + scope + ".ini", QSettings::IniFormat);
    new QSettings(organization, scope);
  s_settings << setting;
  return *setting;
}


void Kst::deleteAllSettings()
{
  Q_FOREACH(QSettings* s, s_settings) {
    delete s;
  }
}


// vim: ts=2 sw=2 et
