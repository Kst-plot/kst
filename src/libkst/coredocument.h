/***************************************************************************
                              coredocument.h
                             -------------------
    begin                : October 3, 2007
    copyright            : (C) 2007 by The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COREDOCUMENT_H
#define COREDOCUMENT_H

#include <QPointer>
#include <QString>

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KSTCORE_EXPORT CoreDocument 
{
  public:
    CoreDocument();
    virtual ~CoreDocument();

    virtual QString fileName() const;

    virtual ObjectStore *objectStore() const;

    virtual bool open(const QString& file);
    virtual bool save(const QString& to = QString::null);

    virtual bool isChanged() const;
    virtual void setChanged(bool changed);
    virtual bool isOpen() const;

    virtual QString lastError() const;

  private:
    ObjectStore *_objectStore;
    bool _dirty;
    bool _isOpen;
    QString _fileName;
    QString _lastError;
};

}

#endif

// vim: ts=2 sw=2 et
