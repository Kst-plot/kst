/***************************************************************************
                                   elog.cpp
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

#include "elog.h"
#include "elogeventthreadsubmit.h"
#include "elogbasicthreadsubmit.h"

#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kmainwindow.h>

#include <kst.h>

K_EXPORT_COMPONENT_FACTORY(kstextension_elog, KGenericFactory<KstELOG>)

KstELOG::KstELOG(QObject *parent, const char *name, const QStringList& list) : KstExtension(parent, name, list), KXMLGUIClient() {
  QCustomEvent eventAlive(KstELOGAliveEvent);

  new KAction(i18n("&ELOG..."), 0, 0, this, SLOT(doShow()), actionCollection(), "elog_settings_show");
  new KAction(i18n("Add ELOG Entry..."), "addelogentry", CTRL+ALT+Key_E, this, SLOT(doEntry()), actionCollection(), "elog_entry_add");
  new KAction(i18n("Launch ELOG Browser..."), "launchelogbrowser", CTRL+ALT+Key_B, this, SLOT(launchBrowser()), actionCollection(), "elog_launch_browser");
  setInstance(app()->instance());
  setXMLFile("kstextension_elog.rc", true);
  app()->guiFactory()->addClient(this);

  _elogConfiguration = new ElogConfigurationI( this, app() );
  _elogEventEntry = new ElogEventEntryI( this, app() );
  _elogEntry = new ElogEntryI( this, app() );

  connect( app(), SIGNAL(ELOGConfigure()), this, SLOT(doEventEntry()));
  connect( app(), SIGNAL(ELOGSubmitEntry(const QString&)), this, SLOT(submitEventEntry(const QString&)));

  _elogEntry->initialize();
  _elogEventEntry->initialize();
  _elogConfiguration->initialize();

  QApplication::sendEvent( (QObject*)app(), (QEvent*)&eventAlive );
}


KstELOG::~KstELOG() {
  QCustomEvent eventDeath(KstELOGDeathEvent);

  if( app() ) {
    QApplication::sendEvent( (QObject*)app(), (QEvent*)&eventDeath );

    if( app()->guiFactory() ) {
      app()->guiFactory()->removeClient(this);
    }
  }
  
  delete _elogConfiguration;
  delete _elogEventEntry;
  delete _elogEntry;
}


void KstELOG::submitEventEntry(const QString& strMessage) {
  ElogEventThreadSubmit* pThread;
  KstELOGCaptureStruct captureStruct;
  QByteArray	byteArrayCapture;
  QByteArray	byteArrayCaptureRaw;
  QDataStream dataStreamCapture( byteArrayCaptureRaw, IO_ReadWrite );
  QCustomEvent eventCapture(KstELOGCaptureEvent);

  if( eventEntry()->includeCapture() ) {
    captureStruct.pBuffer     = &dataStreamCapture;
    captureStruct.iWidth      = configuration()->captureWidth();
    captureStruct.iHeight     = configuration()->captureHeight();
    eventCapture.setData( &captureStruct );

    QApplication::sendEvent( (QObject*)app(), (QEvent*)&eventCapture );
    if( byteArrayCaptureRaw.size() > 0 ) {
      byteArrayCapture.duplicate( byteArrayCaptureRaw.data() + 4, byteArrayCaptureRaw.size() - 4 );
    }
  }

  pThread = new ElogEventThreadSubmit(this,
                                 eventEntry()->includeCapture(),
                                 eventEntry()->includeConfiguration(),
                                 eventEntry()->includeDebugInfo(),
                                 &byteArrayCapture,
                                 strMessage,
                                 configuration()->userName(),
                                 configuration()->userPassword(),
                                 configuration()->writePassword(),
                                 configuration()->name(),
                                 eventEntry()->attributes(),
                                 configuration()->submitAsHTML(),
                                 configuration()->suppressEmail());
  pThread->doTransmit();
}


void KstELOG::submitEntry() {  
  ElogThreadSubmit* pThread;
  KstELOGCaptureStruct captureStruct;
  QByteArray	byteArrayCapture;
  QDataStream dataStreamCapture( byteArrayCapture, IO_ReadWrite );
  QCustomEvent eventCapture(KstELOGCaptureEvent);

  if( entry()->includeCapture() ) {
    captureStruct.pBuffer     = &dataStreamCapture;
    captureStruct.iWidth      = configuration()->captureWidth();
    captureStruct.iHeight     = configuration()->captureHeight();
    eventCapture.setData( &captureStruct );

    QApplication::sendEvent( (QObject*)app(), (QEvent*)&eventCapture );
  }

  pThread = new ElogBasicThreadSubmit(this,
                                 entry()->includeCapture(),
                                 entry()->includeConfiguration(),
                                 entry()->includeDebugInfo(),
                                 &byteArrayCapture,
                                 entry()->text(),
                                 configuration()->userName(),
                                 configuration()->userPassword(),
                                 configuration()->writePassword(),
                                 configuration()->name(),
                                 entry()->attributes(),
                                 configuration()->submitAsHTML(),
                                 configuration()->suppressEmail());
  pThread->doTransmit();
}


void KstELOG::doEventEntry() {
  if( _elogEventEntry ) {
    _elogEventEntry->show();
    _elogEventEntry->raise();
  }
}


void KstELOG::doEntry() {
  if( _elogEntry ) {
    _elogEntry->show();
    _elogEntry->raise();
  }
}


void KstELOG::doShow() {
  if( _elogConfiguration ) {
    _elogConfiguration->show();
    _elogConfiguration->raise();
  }
}


void KstELOG::launchBrowser() {
  // FIXME: be able to raise() an existing browser window one day
  QString url;

  if (!_elogConfiguration->ipAddress().startsWith("http://")) {
    url = "http://";
  }
  url += _elogConfiguration->ipAddress() + ":" + QString::number(_elogConfiguration->portNumber());
  if (!_elogConfiguration->name().isEmpty()) {
    url += "/";
    url += _elogConfiguration->name();
    url += "/";
  }
  
  kapp->invokeBrowser(url);
}


void KstELOG::load(QDomElement& e) {
  Q_UNUSED(e)
}


void KstELOG::save(QTextStream& ts, const QString& indent) {
  Q_UNUSED(ts)
  Q_UNUSED(indent)
}

#include "elog.moc"
// vim: ts=2 sw=2 et
