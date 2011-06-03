#if 0
/***************************************************************************
                              emailthread.cpp
                              ----------------
    begin                : Jul 7 2004
    copyright            : (C) 2004 The University of british Columbia
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

// include files for Qt
#include <qregexp.h>

// application specific includes
#include "emailthread.h"
#include "debug.h"

namespace Kst {

EMailThread::EMailThread( const QString& strTo,
                          const QString& strSubject,
                          const QString& strBody ) : QObject() {
  _strTo          = strTo;
  _strSubject     = strSubject;
  _strBody        = strBody.toLatin1();

  _strFrom        =  Settings::globalSettings()->emailSender;
  _strSMTPServer  =  Settings::globalSettings()->emailSMTPServer;
  _useAuthentication =  Settings::globalSettings()->emailRequiresAuthentication;
  _strUsername    =  Settings::globalSettings()->emailUsername;
  _strPassword    =  Settings::globalSettings()->emailPassword;
  _iPort          =  Settings::globalSettings()->emailSMTPPort;
  _encryption     =  Settings::globalSettings()->emailEncryption;
  _authentication =  Settings::globalSettings()->emailAuthentication;

}

EMailThread::EMailThread( const QString& strTo,
                          const QString& strSubject,
                          const QString& strBody,
                          const QString& strFrom,
                          const QString& strSMTPServer,
                          const int iPort,
                          const EMailEncryption encryption,
                          const QString& strUsername,
                          const QString& strPassword,
                          const EMailAuthentication authentication,
                          const bool useAuthentication ) : QObject() {
  _strTo          = strTo;
  _strSubject     = strSubject;
  _strBody        = strBody.toLatin1();
  _strFrom        = strFrom;
  _strSMTPServer  = strSMTPServer;
  _useAuthentication = useAuthentication;
  _strUsername    = strUsername;
  _strPassword    = strPassword;
  _iPort          = iPort;
  _encryption     = encryption;
  _authentication = authentication;
}

EMailThread::~EMailThread() {
}

void EMailThread::send() {
  //TODO Implement Send.
}

}
// vim: ts=2 sw=2 et
#endif
