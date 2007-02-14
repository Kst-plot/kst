/***************************************************************************
                                   elogthreadsubmit.cpp
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <klocale.h>
#include <kmdcodec.h>

#include "elogthreadsubmit.h"
#include <kst.h>

ElogThreadSubmit::ElogThreadSubmit(KstELOG* elog,
                                  bool bIncludeCapture,
                                  bool bIncludeConfiguration,
                                  bool bIncludeDebugInfo,
                                  QByteArray* pByteArrayCapture,
                                  const QString& strMessage,
                                  const QString& strUserName,
                                  const QString& strUserPassword,
                                  const QString& strWritePassword,
                                  const QString& strLogbook,
                                  const QString& strAttributes,
                                  bool bSubmitAsHTML,
                                  bool bSuppressEmail)
: ElogThread(elog), _dataStreamAll(_byteArrayAll, IO_ReadWrite) {
  _byteArrayCapture.duplicate( *pByteArrayCapture );
  _bIncludeCapture        = bIncludeCapture;
  _bIncludeConfiguration  = bIncludeConfiguration;
  _bIncludeDebugInfo      = bIncludeDebugInfo;
  _strMessage             = strMessage;
  _strUserName            = strUserName;
  _strUserPassword        = strUserPassword;
  _strWritePassword       = strWritePassword;
  _strLogbook             = strLogbook;
  _strAttributes          = strAttributes;
  _bSubmitAsHTML          = bSubmitAsHTML;
  _bSuppressEmail         = bSuppressEmail;
}

ElogThreadSubmit::~ElogThreadSubmit() {
  if (_job) {
    _job->kill();
    _job = 0L;
  }
}

void ElogThreadSubmit::doTransmit( ) {
  KURL destination;
  QStringList::iterator it;
  QStringList	strListAttributes;
  QStringList	strListAttribute;
  QString boundary;
  int iAttachment = 0;

  destination.setProtocol("http");
  destination.setHost(_elog->configuration()->ipAddress());
  destination.setPort((short)_elog->configuration()->portNumber());
  destination.setQuery("");
  if (!_strLogbook.isEmpty()) {
    destination.setPath(QString("/%1/").arg(_strLogbook));
  }

  srand((unsigned) time(NULL));
  boundary = QString("---------------------------%1%2%3").arg(rand(), 4, 16).arg(rand(), 4, 16).arg(rand(), 4, 16);

  //
  // add the attributes...
  //
  addAttribute( _dataStreamAll, boundary, "cmd", "Submit", false );
  addAttribute( _dataStreamAll, boundary, "unm", _strUserName, false );
  addAttribute( _dataStreamAll, boundary, "upwd", _strUserPassword, true );
  addAttribute( _dataStreamAll, boundary, "exp", _strLogbook, false );

  strListAttributes = QStringList::split( '\n', _strAttributes, FALSE );
  for ( it = strListAttributes.begin(); it != strListAttributes.end(); ++it ) {
    strListAttribute = QStringList::split( '=', *it, FALSE );
    if( strListAttribute.count() == 2 ) {
      addAttribute( _dataStreamAll, boundary,
                    strListAttribute.first().stripWhiteSpace(),
                    strListAttribute.last().stripWhiteSpace(), false );
    }
  }

  if( _bSubmitAsHTML ) {
    addAttribute( _dataStreamAll, boundary, "html", "1", false );
  }
  if( _bSuppressEmail ) {
    addAttribute( _dataStreamAll, boundary, "suppress", "1", false );
  }

  addAttribute( _dataStreamAll, boundary, "Text", _strMessage, false );
  QString str = QString("%1\r\n").arg(boundary);
  _dataStreamAll.writeRawBytes(str.ascii(), str.length());

  //
  // add the attachments...
  //
  if( _bIncludeCapture ) {
    iAttachment++;
    addAttachment( _dataStreamAll, boundary, _byteArrayCapture, iAttachment, "Capture.png" );
  }
  if( _bIncludeConfiguration ) {
    QByteArray byteArrayConfigure;
    QTextStream textStreamConfigure( byteArrayConfigure, IO_ReadWrite );
    QCustomEvent eventConfigure(KstELOGConfigureEvent);

    eventConfigure.setData( &textStreamConfigure );
    QApplication::sendEvent( (QObject*)_elog->app(), (QEvent*)&eventConfigure );
    iAttachment++;
    addAttachment( _dataStreamAll, boundary, byteArrayConfigure, iAttachment, "Configure.kst" );
  }
  if( _bIncludeDebugInfo ) {
    QByteArray byteArrayDebugInfo;
    QTextStream textStreamDebugInfo( byteArrayDebugInfo, IO_ReadWrite );
    QCustomEvent eventDebugInfo(KstELOGDebugInfoEvent);

    eventDebugInfo.setData( &textStreamDebugInfo );
    QApplication::sendEvent( (QObject*)_elog->app(), (QEvent*)&eventDebugInfo );
    iAttachment++;
    addAttachment( _dataStreamAll, boundary, byteArrayDebugInfo, iAttachment, "DebugInfo.txt" );
  }

  _job = KIO::http_post(destination, _byteArrayAll, false);
  if (_job) {
    _job->addMetaData("content-type", QString("multipart/form-data; boundary=%1").arg(boundary));
    if (!_strWritePassword.isEmpty()) {
      QCString enc = KCodecs::base64Encode(_strWritePassword.ascii());

      _job->addMetaData("cookies", "manual");
      _job->addMetaData("setcookies", QString("Cookie: wpwd=%1").arg(enc.data()));
    }

    QObject::connect(_job, SIGNAL(result(KIO::Job*)), this, SLOT(result(KIO::Job*)));
    QObject::connect(_job, SIGNAL(dataReq(KIO::Job*, QByteArray&)), this, SLOT(dataReq(KIO::Job*, QByteArray&)));
    QObject::connect(_job, SIGNAL(data(KIO::Job*, const QByteArray&)), this, SLOT(data(KIO::Job*, const QByteArray&)));

    KIO::Scheduler::scheduleJob(_job);
  } else {
    doError( i18n("%1: unable to create KIO::Job").arg(_strType), KstDebug::Warning );
  }
}

bool ElogThreadSubmit::doResponseCheck( const char* response ) {
  QString strError;
  char str[80];

  if (strstr(response, "Location:")) {
    if (strstr(response, "wpwd")) {
      doError( i18n("Failed to add %1: invalid password").arg(_strType) );
    } else if (strstr(response, "wusr")) {
      doError( i18n("Failed to add %1: invalid username").arg(_strType) );
    } else {
      strncpy(str, strstr(response, "Location:") + 10, sizeof(str));
      if (strchr(str, '?')) {
        *strchr(str, '?') = 0;
      }
      if (strchr(str, '\n')) {
        *strchr(str, '\n') = 0;
      }
      if (strchr(str, '\r')) {
        *strchr(str, '\r') = 0;
      }

      if (strrchr(str, '/')) {
        strError = i18n("Successfully added %1: ID=%2\n").arg(_strType).arg( strrchr(str, '/') + 1 );
        doError( strError, KstDebug::Notice );
      } else {
        strError = i18n("Successfully added %1: ID=%2\n").arg(_strType).arg( str );
        doError( strError, KstDebug::Notice );
      }
    }
  } else {
    doError( i18n("Successfully added %1").arg(_strType), KstDebug::Notice );
  }

  return TRUE;
}

bool ElogThreadSubmit::doResponseError( const char* response, const QString& strDefault ) {
  QString strError;
  char str[80];
  
  if (strstr(response, "Logbook Selection"))
    doError( i18n("Failed to add %1: no logbook specified").arg(_strType) );
  else if (strstr(response, "enter password")) {
    doError( i18n("Failed to add %1: missing or invalid password").arg(_strType) );
  } else if (strstr(response, "form name=form1")) {
    doError( i18n("Failed to add %1: missing or invalid username/password").arg(_strType) );
  } else if (strstr(response, "Error: Attribute")) {
    strncpy(str, strstr(response, "Error: Attribute") + 20, sizeof(str));
    if (strchr(str, '<')) {
      *strchr(str, '<') = 0;
    }
    strError = i18n("Failed to add %1: missing required attribute \"%2\"").arg(_strType).arg( str );
    doError( strError );
  } else {
    strError = i18n("Failed to add %1: error: %2").arg(_strType).arg( strDefault );
    doError( strError ); 
  }
  
  return TRUE;
}

void ElogThreadSubmit::dataReq(KIO::Job *job, QByteArray &array)
{
  Q_UNUSED(job)

  array.resize(0);

  return;
}

void ElogThreadSubmit::data(KIO::Job *job, const QByteArray &array)
{
  Q_UNUSED(job)

  if (array.count() > 0) {
    _textStreamResult << array.data();
  }
}

void ElogThreadSubmit::result(KIO::Job *job) {
  if (_job) {
    _job = 0L;

    if (job->error()) {
      _textStreamResult << '\0';
      doResponseError(_byteArrayResult.data(), job->errorText());
    } else {
      if (_byteArrayResult.count() > 0) {
        _textStreamResult << '\0';
        doResponseCheck(_byteArrayResult.data());
      } else {
        doError( i18n("%1: unable to receive response").arg(_strType), KstDebug::Notice );
      }
    }
  }

  delete this;
}


#include "elogthreadsubmit.moc"

// vim: ts=2 sw=2 et
