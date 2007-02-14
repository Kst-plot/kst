/***************************************************************************
                                  pulse.h
                             -------------------
    begin                : Sep 25 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef PULSE_H
#define PULSE_H

#include <kstextension.h>
#include <kxmlguiclient.h>

#include <qguardedptr.h>

#include "kstpulsegenerator.h"

namespace KParts {
  class Part;
}

class KAction;

class KstPulse : public KstExtension, public KXMLGUIClient {
  Q_OBJECT
  public:
    KstPulse(QObject *parent, const char *name, const QStringList&);
    virtual ~KstPulse();

    // To save state
    virtual void load(QDomElement& e);
    virtual void save(QTextStream& ts, const QString& indent = QString::null);

  public slots:
    void doShow();

  private slots:
    void restoreUI();

  private:
    KAction *_showAction;
    QGuardedPtr<KstPulseGenerator> _generator;
};

#endif

// vim: ts=2 sw=2 et
