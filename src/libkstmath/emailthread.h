#if 0
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

#ifndef EMAILTHREAD_H
#define EMAILTHREAD_H

#include "settings.h"

namespace Kst {

class EMailThread : public QObject {
  Q_OBJECT
  public:
    EMailThread( const QString& strTo,
                 const QString& strSubject,
                 const QString& strBody );
    EMailThread( const QString& strTo,
                 const QString& strSubject,
                 const QString& strBody,
                 const QString& strFrom,
                 const QString& strSMTPServer,
                 const int iPort = 25,
                 const EMailEncryption encryption = EMailEncryptionNone,
                 const QString& strUsername = QString::null,
                 const QString& strPassword = QString::null,
                 const EMailAuthentication authentication = EMailAuthenticationPLAIN,
                 const bool useAuthentication = false );
    virtual ~EMailThread();

    void send();

    QString       _strFrom;
    QString       _strTo;
    QString      _strBody;
    QString       _strSubject;
    QString       _strSMTPServer;
    QString       _strMessage;
    QString       _strUsername;
    QString       _strPassword;
    bool          _useAuthentication;
    int           _iPort;
    EMailEncryption _encryption;
    EMailAuthentication _authentication;

    bool              _sendOk;
    uint              _bodyLength;
    uint              _bodyOffset;
};

}
#endif
#endif
// vim: ts=2 sw=2 et
