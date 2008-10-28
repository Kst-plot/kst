/***************************************************************************
                    template.cpp  -  data source template
                             -------------------
    begin                : Fri Oct 17 2003
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

#include "template.h"

 
TemplateSource::TemplateSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type) {
}


TemplateSource::~TemplateSource() {
}


KstObject::UpdateType TemplateSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }
  // fill me in
  return setLastUpdateResult(KstObject::NO_CHANGE);
}


int TemplateSource::readField(double *v, const QString& field, int s, int n) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  return -1;
}


bool TemplateSource::isValidField(const QString& field) const {
  Q_UNUSED(field)
  return false;
}


int TemplateSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 0;
}


int TemplateSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return 0;
}


QString TemplateSource::fileType() const {
  return QString::null;
}


void TemplateSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}


bool TemplateSource::isEmpty() const {
  return false; // correct me
}


extern "C" {
KstDataSource *create_template(KConfig *cfg, const QString& filename, const QString& type) {
  return new TemplateSource(cfg, filename, type);
}


QStringList provides_template() {
  QStringList rc;
  // create the stringlist
  return rc;
}


int understands_template(KConfig *cfg, const QString& filename) {
  // determine if it is an X file
  // This should return a number 0 .. 100.  Normally you try to avoid 100
  // unless there is no way that this file is anything else except an X file.
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;
}


QStringList fieldList_template(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  Q_UNUSED(type)
  Q_UNUSED(typeSuggestion)
  Q_UNUSED(complete)
  return QStringList();
}


QWidget* widget_template(const QString& filename) {
  Q_UNUSED(filename)
  // Return 0L, or a widget that inherits from KstDataSourceConfigWidget
  return 0L;
}

}

KST_KEY_DATASOURCE_PLUGIN(template)

// vim: ts=2 sw=2 et
