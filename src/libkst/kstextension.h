/***************************************************************************
                                kstextension.h
                             -------------------
    begin                : Feb 09 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef KSTEXTENSION_H
#define KSTEXTENSION_H

#include <qdom.h>
#include <qobject.h>

#include "kst_export.h"

class QMainWindow;

class KST_EXPORT KstExtension : public QObject {
  Q_OBJECT
  public:
    KstExtension(QObject *parent, const QStringList&);
    virtual ~KstExtension();

    virtual void processArguments(const QString& args);

    // To save state
    virtual void load(const QDomElement& e);
    virtual void save(QTextStream& ts, const QString& indent = QString::null);

    // Clear internal state
    virtual void clear();

    QMainWindow *app() const;

  signals:
    void unregister();
};

#endif

// vim: ts=2 sw=2 et
