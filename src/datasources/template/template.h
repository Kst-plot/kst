/***************************************************************************
                  template.h  -  data source plugin template
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

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <kstdatasource.h>


class TemplateSource : public KstDataSource {
  public:
    TemplateSource(KConfig *cfg, const QString& filename, const QString& type);

    ~TemplateSource();

    KstObject::UpdateType update(int = -1);

    int readField(double *v, const QString &field, int s, int n);

    bool isValidField(const QString &field) const;

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString::null);

    bool isEmpty() const;
};


#endif
// vim: ts=2 sw=2 et
