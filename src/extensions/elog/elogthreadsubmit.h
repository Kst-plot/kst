/***************************************************************************
                                    elogthreadsubmit.h
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

#ifndef ELOGTHREADSUBMIT_H
#define ELOGTHREADSUBMIT_H



#include "elogthread.h"

class ElogThreadSubmit : public ElogThread {
  Q_OBJECT
  public:
    ElogThreadSubmit(KstELOG* pELOG,
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
                     bool bSuppressEmail);

    virtual ~ElogThreadSubmit();
    virtual void doTransmit( );

  public slots:
    void result(KIO::Job *);
    void dataReq(KIO::Job *, QByteArray &);
    void data(KIO::Job *, const QByteArray &);
            
  protected:
    virtual bool doResponseError( const char* response, const QString& strDefault );
    virtual bool doResponseCheck( const char* response );

    QString           _strType;
    
  protected:
    QByteArray        _byteArrayAll;
    QDataStream       _dataStreamAll;
    QByteArray        _byteArrayCapture;
    QString           _strMessage;
    QString           _strUserName;
    QString           _strUserPassword;
    QString           _strWritePassword;
    QString           _strLogbook;
    QString           _strAttributes;
    bool              _bSubmitAsHTML;
    bool              _bSuppressEmail;
    bool              _bIncludeCapture;
    bool              _bIncludeConfiguration;
    bool              _bIncludeDebugInfo;
    int               _iCaptureWidth;
    int               _iCaptureHeight;
};

#endif

// vim: ts=2 sw=2 et
