/*
   Copyright (c) 2002 Carlos Moro <cfmoro@correo.uniovi.es>
   Copyright (c) 2002 Hans Petter Bieker <bieker@kde.org>
   Copyright (c) 2004 Jason Harris <jharris@30doradus.org>

	 This class has been derived from ExtCalendarSystem;
	 the changesd made just replace QDate objects with ExtDate objects.
	 These changes by Jason Harris <jharris@30doradus.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Gregorian calendar system implementation factory for creation of kde calendar
// systems.
// Also default gregorian and factory classes

#include <kglobal.h>

#include "kstextcalendarsystem.h"
#include "klocale.h"

namespace KST {

class ExtCalendarSystemPrivate
{
public:
  const KLocale * locale;
};

ExtCalendarSystem::ExtCalendarSystem(const KLocale * locale)
  : d(new ExtCalendarSystemPrivate)
{
  d->locale = locale;
}

ExtCalendarSystem::~ExtCalendarSystem()
{
  delete d;
}

const KLocale * ExtCalendarSystem::locale() const
{
  if ( d->locale )
    return d->locale;

  return KGlobal::locale();
}

QString ExtCalendarSystem::dayString(const ExtDate & pDate, bool bShort) const
{
  QString sResult;

  sResult.setNum(day(pDate));
  if (!bShort && sResult.length() == 1 )
    sResult.prepend('0');

  return sResult;
}

QString ExtCalendarSystem::monthString(const ExtDate & pDate, bool bShort) const
{
  QString sResult;

  sResult.setNum(month(pDate));
  if (!bShort && sResult.length() == 1 )
    sResult.prepend('0');

  return sResult;
}

QString ExtCalendarSystem::yearString(const ExtDate & pDate, bool bShort) const
{
  QString sResult;

  sResult.setNum(year(pDate));
  if (bShort && sResult.length() == 4 )
    sResult = sResult.right(2);

  return sResult;
}

static int stringToInteger(const QString & sNum, int & iLength)
{
  unsigned int iPos = 0;

  int result = 0;
  for (; sNum.length() > iPos && sNum.at(iPos).isDigit(); iPos++)
    {
      result *= 10;
      result += sNum.at(iPos).digitValue();
    }

  iLength = iPos;
  return result;
}


int ExtCalendarSystem::dayStringToInteger(const QString & sNum, int & iLength) const
{
  return stringToInteger(sNum, iLength);
}

int ExtCalendarSystem::monthStringToInteger(const QString & sNum, int & iLength) const
{
  return stringToInteger(sNum, iLength);
}

int ExtCalendarSystem::yearStringToInteger(const QString & sNum, int & iLength) const
{
  return stringToInteger(sNum, iLength);
}

QString ExtCalendarSystem::weekDayName (int weekDay, bool shortName) const
{
  if ( shortName )
    switch ( weekDay )
      {
      case 1:  return locale()->translate("Monday", "Mon");
      case 2:  return locale()->translate("Tuesday", "Tue");
      case 3:  return locale()->translate("Wednesday", "Wed");
      case 4:  return locale()->translate("Thursday", "Thu");
      case 5:  return locale()->translate("Friday", "Fri");
      case 6:  return locale()->translate("Saturday", "Sat");
      case 7:  return locale()->translate("Sunday", "Sun");
      }
  else
    switch ( weekDay )
      {
      case 1:  return locale()->translate("Monday");
      case 2:  return locale()->translate("Tuesday");
      case 3:  return locale()->translate("Wednesday");
      case 4:  return locale()->translate("Thursday");
      case 5:  return locale()->translate("Friday");
      case 6:  return locale()->translate("Saturday");
      case 7:  return locale()->translate("Sunday");
      }

  return QString::null;
}

}
