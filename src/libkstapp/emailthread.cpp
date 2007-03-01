/***************************************************************************
                              emailthread.cpp
                              ----------------
    begin                : Jul 7 2004
    copyright            : (C) 2004 The University of british Columbia
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

#include <ctype.h>

// include files for Qt
#include <qregexp.h>

// application specific includes
#include "emailthread.h"
#include "kstdebug.h"

EMailThread::EMailThread( const QString& strTo,
                          const QString& strSubject,
                          const QString& strBody ) : QObject() {
  _strTo          = strTo;
  _strSubject     = strSubject;
  _strBody        = strBody.toLatin1();

  _strFrom        =  KstSettings::globalSettings()->emailSender;
  _strSMTPServer  =  KstSettings::globalSettings()->emailSMTPServer;
  _useAuthentication =  KstSettings::globalSettings()->emailRequiresAuthentication;
  _strUsername    =  KstSettings::globalSettings()->emailUsername;
  _strPassword    =  KstSettings::globalSettings()->emailPassword;
  _iPort          =  KstSettings::globalSettings()->emailSMTPPort;
  _encryption     =  KstSettings::globalSettings()->emailEncryption;
  _authentication =  KstSettings::globalSettings()->emailAuthentication;

  _job            = 0L;
  _slave          = 0L;
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

  _job            = 0L;
  _slave          = 0L;
}

EMailThread::~EMailThread() {
  if (_job) {
    _job->kill();
    _job = 0L;
    _slave = 0L;
  }
  if (_slave) {
    KIO::Scheduler::disconnectSlave(_slave);
    _slave = 0L;
  }
}

void EMailThread::send() {
  QStringList listTo;
  QString mQuery;
  KIO::MetaData slaveConfig;
  KUrl destination;
  int i, count;

  _sendOk = false;

  KIO::Scheduler::connect(SIGNAL(slaveError(KIO::Slave *, int, const QString &)), this,
                            SLOT(slaveError(KIO::Slave *, int, const QString &)));

  _strBody.insert( 0, QString("Subject:%1\n\n").arg(_strSubject).toLatin1().data());
  _strBody.insert( 0, QString("To:%1\n").arg(_strTo).toLatin1().data());

  _bodyOffset = 0;
  _bodyLength = _strBody.length();

  mQuery  = "headers=0&from=";
  mQuery += KUrl::encode_string(_strFrom);
  listTo = _strTo.split(QRegExp("[ ,;]"));
  count = listTo.count();
  if (count > 0) {
    for (i=0; i<count; i++) {
      mQuery += "&to=";
      mQuery += KUrl::encode_string(listTo[i]);
    }
  } else {
    mQuery += "&to=";
    mQuery += KUrl::encode_string(_strTo);
  }

  mQuery += "&size=";
  mQuery += QString::number(_bodyLength);

  if (_encryption == EMailEncryptionSSL) {
    destination.setProtocol("smtps");
  } else {
    destination.setProtocol("smtp");
  }
  destination.setHost(_strSMTPServer);
  destination.setPort((short)_iPort);
  destination.setPath("/send");
  destination.setQuery(mQuery);

  if (_useAuthentication) {
    destination.setUser(_strUsername);
    destination.setPass(_strPassword);
  }

  if (_encryption == EMailEncryptionTLS) {
    slaveConfig.insert("tls", "on");
  } else {
    slaveConfig.insert("tls", "off");
  }
  if (_useAuthentication) {
    switch (_authentication) {
      case EMailAuthenticationPLAIN:
        slaveConfig.insert("sasl", "PLAIN");
        break;
      case EMailAuthenticationLOGIN:
        slaveConfig.insert("sasl", "LOGIN");
        break;
      case EMailAuthenticationCRAMMD5:
        slaveConfig.insert("sasl", "CRAM-MD5");
        break;
      case EMailAuthenticationDIGESTMD5:
        slaveConfig.insert("sasl", "DIGEST-MD5");
        break;
      default:
        slaveConfig.insert("sasl", "PLAIN");
        break;
    }
  }
  _slave = KIO::Scheduler::getConnectedSlave(destination, slaveConfig);
  if (_slave) {
    _job = KIO::put(destination, -1, false, false, false);
    if (_job) {
      _job->addMetaData("lf2crlf+dotstuff", "slave");
      connect(_job, SIGNAL(result(KIO::Job *)), this, SLOT(result(KIO::Job *)));
      connect(_job, SIGNAL(dataReq(KIO::Job *, QByteArray &)), this, SLOT(dataReq(KIO::Job *, QByteArray &)));
      KIO::Scheduler::assignJobToSlave(_slave, _job);
      _sendOk = true;
    }
  }   
}

void EMailThread::dataReq(KIO::Job *job, QByteArray &array)
{
  Q_UNUSED(job)

  int chunkSize = qMin(_bodyLength - _bodyOffset, uint(0x8000));

  if (chunkSize > 0) {
    array = _strBody.data() + _bodyOffset;
    _bodyOffset += chunkSize;
  } else {
    array.resize(0);
    _strBody.resize(0);
  }
}

void EMailThread::result(KIO::Job *job)
{
  if (_job) {
    _job = 0L;

    if (job->error()) {
      KstDebug::self()->log(i18n("Error sending automated email notification: [%1].").arg(job->errorText()), KstDebug::Warning);
      _sendOk = false;
      if (job->error() == KIO::ERR_SLAVE_DIED) {
        _slave = 0L;
      }
    }
  }

  delete this;
}

void EMailThread::slaveError(KIO::Slave *aSlave, int error, const QString &errorMsg)
{
  if (aSlave == _slave) {
    KstDebug::self()->log(i18n("Error sending automated email notification: [%1,%2].").arg(error).arg(errorMsg), KstDebug::Warning);
    if (error == KIO::ERR_SLAVE_DIED) {
      _slave = 0L;
    }
    _sendOk = false;
    _job = 0L;
  }

  delete this;
}

#include "emailthread.moc"

// vim: ts=2 sw=2 et
