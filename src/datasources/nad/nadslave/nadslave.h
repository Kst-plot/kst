/***************************************************************************
                  nadslave.cpp  - NAD KIOSlave
                             -------------------
    begin                : Mon Apr 04 2006
    copyright            : (C) 2006 Staikos Computing Services Inc.
    email                : info@staikos.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as               *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef _NADSLAVE_H_
#define _NADSLAVE_H_

#include <qobject.h>
#include <qstring.h>

#include <kio/global.h>
#include <kio/slavebase.h>

class NadProtocol : public QObject, public KIO::SlaveBase {
  Q_OBJECT
  public:
    NadProtocol(const QCString& pool, const QCString& app);
    ~NadProtocol();

    void get(const KURL& url);
    void put(const KURL& url, int mode, bool overwrite, bool resume);
    void copy(const KURL& src, const KURL& dest, int mode, bool overwrite);
    void rename(const KURL& src, const KURL& dest, bool overwrite);

    void stat(const KURL& url);
    void listDir(const KURL& url);
    void mkdir(const KURL& url, int permissions);
    void chmod(const KURL& url, int permissions);
    void del(const KURL& url, bool isfile);

  protected slots:

  private:
};

#endif

// vim: ts=2 sw=2 et
