/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "eventmonitorfactory.h"

#include "debug.h"
#include "eventmonitorentry.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

EventMonitorFactory::EventMonitorFactory()
: ObjectFactory() {
  registerFactory(EventMonitorEntry::staticTypeTag, this);
}


EventMonitorFactory::~EventMonitorFactory() {
}


DataObjectPtr EventMonitorFactory::generateObject(ObjectStore *store, QXmlStreamReader& xml) {
  Q_ASSERT(store);

  QString equation, description, emailRecipients, script;
  bool logDebug=false, logEmail=false, logELOG=false;
  int logLevel=1;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == EventMonitorEntry::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        equation = attrs.value("equation").toString();
        description = attrs.value("description").toString();
        emailRecipients = attrs.value("emailrecipients").toString();
        script = attrs.value("script").toString();
        logLevel = attrs.value("loglevel").toString().toInt();

        logDebug = attrs.value("logdebug").toString() == "true" ? true : false;
        logEmail = attrs.value("logemail").toString() == "true" ? true : false;
        logELOG = attrs.value("logelog").toString() == "true" ? true : false;
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == EventMonitorEntry::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating EventMonitorEntry from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  EventMonitorEntryPtr eventMonitor = store->createObject<EventMonitorEntry>();
  Q_ASSERT(eventMonitor);

  eventMonitor->setScriptCode(script);
  eventMonitor->setEvent(equation);
  eventMonitor->setDescription(description);
  eventMonitor->setLevel((Debug::LogLevel)logLevel);
  eventMonitor->setLogDebug(logDebug);
  eventMonitor->setLogEMail(logEmail);
  eventMonitor->setLogELOG(logELOG);
  eventMonitor->setEMailRecipients(emailRecipients);

  eventMonitor->reparse();

  eventMonitor->writeLock();
  eventMonitor->registerChange();
  eventMonitor->unlock();

  return eventMonitor;
}

}

// vim: ts=2 sw=2 et
