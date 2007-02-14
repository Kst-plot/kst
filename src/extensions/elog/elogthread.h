/***************************************************************************
                                    elogthread.h
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

#ifndef ELOGTHREAD_H
#define ELOGTHREAD_H

#include <kio/scheduler.h>

#include <kstdebug.h>
#include "elog.h"

class ElogThread : public QObject {
  Q_OBJECT
  public:
    ElogThread(KstELOG*);
    virtual ~ElogThread();
    
  protected:    
    void addAttachment( QDataStream& stream, const QString& boundary, const QByteArray& byteArray, int iFileNumber, const QString& name );
    void addAttribute( QDataStream& stream, const QString& boundary, const QString& tag, const QString& strValue, bool bEncode );
    void doError( const QString& text, KstDebug::LogLevel level = KstDebug::Warning);
    
    KIO::TransferJob  *_job;
    KstELOG*          _elog;
    QByteArray        _byteArrayResult;
    QTextStream       _textStreamResult;
};

#endif

// vim: ts=2 sw=2 et
