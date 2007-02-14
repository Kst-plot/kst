/***************************************************************************
                                   elogthread.cpp
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
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>


#include <kmdcodec.h>

#include "elogthread.h"

ElogThread::ElogThread(KstELOG* elog) : QObject(), _textStreamResult(_byteArrayResult, IO_ReadWrite) {
  _elog = elog;
  _job  = 0L;
}

ElogThread::~ElogThread() {
}

void ElogThread::addAttachment( QDataStream& stream,
                                const QString& boundary,
                                const QByteArray& byteArray,
                                int iFileNumber,
                                const QString& name ) {
  if (byteArray.count() > 0) {
    QString strStart = QString("Content-Disposition: form-data; name=\"attfile%1\"; filename=\"%2\"\r\n\r\n").arg(iFileNumber).arg(name);
    QString strEnd   = QString("%1\r\n").arg(boundary);

    stream.writeRawBytes(strStart.ascii(), strStart.length());
    stream.writeRawBytes(byteArray.data(), byteArray.count());
    stream.writeRawBytes(strEnd.ascii(), strEnd.length());
  }
}

void ElogThread::addAttribute( QDataStream& stream,
                               const QString& boundary,
                               const QString& tag,
                               const QString& strValue,
                               bool bEncode ) {
  if (!strValue.isEmpty()) {
    QString str;

    if( bEncode ) {
      QCString enc = KCodecs::base64Encode(strValue.latin1());

      str = QString("%1\r\nContent-Disposition: form-data; name=\"%2\"\r\n\r\n%3\r\n").arg(boundary).arg(tag).arg(enc.data());
    } else {
      str = QString("%1\r\nContent-Disposition: form-data; name=\"%2\"\r\n\r\n%3\r\n").arg(boundary).arg(tag).arg(strValue);
    }
    stream.writeRawBytes(str.ascii(), str.length());
  }
}

void ElogThread::doError( const QString& text, KstDebug::LogLevel level ) {
  KstDebug::self()->log(text, level );
}

#include "elogthread.moc"

// vim: ts=2 sw=2 et
