/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTTIMEZONE_H
#define KSTTIMEZONE_H

#include "kst_export.h"

#include <QStringList>

namespace Kst {

class KSTCORE_EXPORT KstTimeZone
{
public:
    KstTimeZone(QString name = "GMT");
    int gmtOffset(time_t t);
    bool setTZ(QString name);
    QString tzName() const {return _tzName;}
    static QStringList tzList();
    static bool recognised(QString name);
private:
    static void initTZList();
    bool _supportsDST;
    int _gmtOffset;
    QString _tzName;
};

}

#endif // KSTTIMEZONE_H
