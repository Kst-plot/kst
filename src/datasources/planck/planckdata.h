/***************************************************************************
                           planck.h  -  Part of KST
                             -------------------
    begin                : Mon Oct 06 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef _KST_PLANCK_H
#define _KST_PLANCK_H

#include <config.h>

#include <kstsharedptr.h>

extern "C" {
#include <HL2_PIOLIB/PIOLib.h>
}

#include <qsize.h>
#include <qstring.h>
#include <qstringlist.h>

namespace Planck {
    extern bool havePlanck();
    extern bool validDatabase(const QString&);

class Source : public KstShared {
  public:
    Source();
    virtual ~Source();

    virtual bool isValid() const;
    
    virtual bool updated() const;

    virtual bool setGroup(const QString& group);
    
    virtual QStringList fields() const;

    virtual void reset();

    virtual int readObject(const QString& object, double *buf, long start, long end);

    virtual QSize range(const QString& object) const;

  protected:
    bool _isValid;
    QString _group;
};

}

#endif

// vim: ts=2 sw=2 et
