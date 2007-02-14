/*
   Copyright (c) 2002 Carlos Moro <cfmoro@correo.uniovi.es>
   Copyright (c) 2002 Hans Petter Bieker <bieker@kde.org>
   Copyright (c) 2004 Jason Harris <jharris@30doradus.org>

	 This class has been derived from KCalendarSystemGregorian;
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

#ifndef KSTEXTCALENDARSYSTEMGREGORIAN_H
#define KSTEXTCALENDARSYSTEMGREGORIAN_H


#include "kstextcalendarsystem.h"

namespace KST {
class ExtCalendarSystemGregorianPrivate;

/**
 * @internal
 * This is the Gregorian calendar implementation.
 *
 * The Gregorian calender is the most used calendar today. The first year in
 * the calendar is set to the birth of Christ.
 *
 * @see KLocale,ExtCalendarSystem,ExtCalendarSystemFactory
 *
 * @author Carlos Moro <cfmoro@correo.uniovi.es>
 * @license GNU-GPL v.2
 * @version $Id$
 * @since 3.2
 */
class ExtCalendarSystemGregorian: public ExtCalendarSystem
{
public:
  ExtCalendarSystemGregorian (const KLocale * locale = 0);
  virtual ~ExtCalendarSystemGregorian ();

  virtual int year (const ExtDate & date) const;
  virtual int month (const ExtDate & date) const;
  virtual int day (const ExtDate & date) const;
  virtual int dayOfWeek (const ExtDate & date) const;
  virtual int dayOfYear (const ExtDate & date) const;

  virtual bool setYMD(ExtDate & date, int y, int m, int d) const;

  virtual ExtDate addYears(const ExtDate & date, int nyears) const;
  virtual ExtDate addMonths(const ExtDate & date, int nmonths) const;
  virtual ExtDate addDays(const ExtDate & date, int ndays) const;

  virtual int monthsInYear (const ExtDate & date) const;

  virtual int daysInYear (const ExtDate & date) const;
  virtual int daysInMonth (const ExtDate & date) const;
  virtual int weeksInYear(int year) const;
  virtual int weekNumber(const ExtDate& date, int * yearNum = 0) const;

  virtual int yearStringToInteger(const QString & sNum, int & iLength) const;

  virtual QString monthName (int month, int year, bool shortName = false) const;
  virtual QString monthName (const ExtDate & date, bool shortName = false ) const;
  virtual QString monthNamePossessive(int month, int year, bool shortName = false) const;
  virtual QString monthNamePossessive(const ExtDate & date, bool shortName = false ) const;
  virtual QString weekDayName (int weekDay, bool shortName = false) const;
  virtual QString weekDayName (const ExtDate & date, bool shortName = false) const;

  virtual int minValidYear () const;
  virtual int maxValidYear () const;
  virtual int weekDayOfPray () const;

  virtual QString calendarName() const;

  virtual bool isLunar() const;
  virtual bool isLunisolar() const;
  virtual bool isSolar() const;

private:
  ExtCalendarSystemGregorianPrivate * d;
};

}

#endif
