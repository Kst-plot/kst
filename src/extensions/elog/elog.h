/***************************************************************************
                                    elog.h
                             -------------------
    begin                : Feb 09 2004
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

#ifndef ELOG_H
#define ELOG_H

#include <kstextension.h>
#include <kxmlguiclient.h>
#include "elogconfiguration_i.h"
#include "elogevententry_i.h"
#include "elogentry_i.h"

class KstELOG : public KstExtension, public KXMLGUIClient {
  Q_OBJECT
  public:
    KstELOG(QObject *parent, const char *name, const QStringList&);
    virtual ~KstELOG();

    ElogConfigurationI* configuration() { return _elogConfiguration; }
    ElogEventEntryI* eventEntry() { return _elogEventEntry; }
    ElogEntryI* entry() { return _elogEntry; }
    
    virtual void load(QDomElement& e);
    virtual void save(QTextStream& ts, const QString& indent = QString::null);
    
  public slots:
    void submitEventEntry(const QString& strMessage);
    void submitEntry();
    void doEventEntry();
    void doEntry();
    void doShow();
    void launchBrowser();

  private:
    ElogConfigurationI* _elogConfiguration;
    ElogEventEntryI*    _elogEventEntry;
    ElogEntryI*         _elogEntry;
};

#endif

// vim: ts=2 sw=2 et
