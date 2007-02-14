/***************************************************************************
                                    elogthreadattrs.h
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

#ifndef ELOGTHREADATTRS_H
#define ELOGTHREADATTRS_H


#include "elogthread.h"

class ElogThreadAttrs : public ElogThread {
  Q_OBJECT
  public:
    ElogThreadAttrs(KstELOG*);
    virtual ~ElogThreadAttrs();
    
    virtual void doTransmit( );

  public slots:
    void result(KIO::Job *);
    void dataReq(KIO::Job *, QByteArray &);
    void data(KIO::Job *, const QByteArray &);

  protected:
    virtual bool doResponseError( const char* response, const QString& strDefault );
    
  private:
    void doResponse( char* response );    
};

#endif

// vim: ts=2 sw=2 et
