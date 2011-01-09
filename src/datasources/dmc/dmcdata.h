/***************************************************************************
                           dmcdata.h  -  Part of KST
                             -------------------
    begin                : Wed July 4 2007
    copyright            : (C) 2007 The University of Toronto
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

#ifndef _KST_DMC_H
#define _KST_DMC_H

#include <config.h>

#include <kstsharedptr.h>

extern "C" {
#include <HL2_DMC/PIODB.h>
}

#include <qsize.h>
#include <qstring.h>
#include <qstringlist.h>

namespace DMC {
    extern bool haveDMC();
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

