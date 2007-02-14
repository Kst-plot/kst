/*
   Copyright (c) 2002 Carlos Moro <cfmoro@correo.uniovi.es>
   Copyright (c) 2002-2003 Hans Petter Bieker <bieker@kde.org>
   Copyright (c) 2004 Jason Harris <jharris@30doradus.org>

	 This class has been derived from KCalendarSystem;
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

#ifndef KSTEXTCALENDARSYSTEM_H
#define KSTEXTCALENDARSYSTEM_H

#include "kstextdatetime.h"

class KLocale;

namespace KST {
class ExtCalendarSystemPrivate;

/**
 * CalendarSystem abstract class, default derived kde gregorian class and
 * factory class. Provides support for different calendar types for kde
 * calendar widget and related stuff.
 *
 * Derived classes must be created through ExtCalendarFactory class
 *
 * @author Carlos Moro <cfmoro@correo.uniovi.es>
 * @version $Id$
 * @since 3.2
 */
class ExtCalendarSystem
{
public:
  /**
   * Constructor of abstract calendar class. This will be called by the derived classes.
   *
   * @param locale It will use this locale for translations, 0 means global.
   */
  ExtCalendarSystem(const KLocale * locale = 0);

  /**
   * Descructor.
   */
  virtual ~ExtCalendarSystem();

  /**
   * Gets specific calendar type year for a given gregorian date
   *
   * @param date gregorian date
   * @return year
   */
  virtual int year (const ExtDate & date) const = 0;

  /**
   * Converts a date into a year literal
   *
   * @param pDate The date to convert
   * @param bShort If the short version of should be used
   * @return The year literal of the date
   */
  virtual QString yearString(const ExtDate & pDate, bool bShort) const;

  /**
   * Converts a year literal of a part of a string into a integer starting at the beginning of the string
   *
   * @param sNum The string to parse
   * @param iLength The number of QChars used, and 0 if no valid symbols was found in the string
   * @return An integer corresponding to the year
   */
  virtual int yearStringToInteger(const QString & sNum, int & iLength) const;

  /**
   * Gets specific calendar type month for a given gregorian date
   *
   * @param date gregorian date
   * @return month number
   */
  virtual int month (const ExtDate & date) const = 0;

  /**
   * Converts a date into a month literal
   *
   * @param pDate The date to convert
   * @param bShort If the short version of should be used
   * @return The month literal of the date
   */
  virtual QString monthString(const ExtDate & pDate, bool bShort) const;

  /**
   * Converts a month literal of a part of a string into a integer starting at the beginning of the string
   *
   * @param sNum The string to parse
   * @param iLength The number of QChars used, and 0 if no valid symbols was found in the string
   * @return An integer corresponding to the month
   */
  virtual int monthStringToInteger(const QString & sNum, int & iLength) const;

  /**
   * Gets specific calendar type day number of month for a given date
   *
   * @param date gregorian date equivalent to the specific one
   * @return day of the month
   */
  virtual int day (const ExtDate & date) const = 0;

  /**
   * Converts a date into a day literal
   *
   * @param pDate The date to convert
   * @param bShort If the short version of should be used
   * @return The day literal of the date
   */
  virtual QString dayString(const ExtDate & pDate, bool bShort) const;

  /**
   * Converts a day literal of a part of a string into a integer starting at the beginning of the string
   *
   * @param sNum The string to parse
   * @param iLength The number of QChars used, and 0 if no valid symbols was found in the string
   * @return An integer corresponding to the day
   */
  virtual int dayStringToInteger(const QString & sNum, int & iLength) const;

  /**
   * Gets specific calendar type number of day of week number for a given
   * date
   *
   * @param date gregorian date
   * @return day of week
   */
  virtual int dayOfWeek (const ExtDate & date) const = 0;

  /**
   * Gets specific calendar type day number of year for a given date
   *
   * @param date gregorian date equivalent to the specific one
   * @return day number
   */
  virtual int dayOfYear (const ExtDate & date) const = 0;

  /**
   * Changes the date's year, month and day. The range of the year, month
   * and day depends on which calendar is being used.
   *
   * @param date Date to change
   * @param y Year
   * @param m Month number
   * @param d Day of month
   * @return true if the date is valid; otherwise returns false.
   */
  virtual bool setYMD(ExtDate & date, int y, int m, int d) const = 0;

  /**
   * Returns a QDate object containing a date nyears later.
   *
   * @param date The old date
   * @param nyears The number of years to add
   * @return The new date
   */
  virtual ExtDate addYears(const ExtDate & date, int nyears) const = 0;

  /**
   * Returns a QDate object containing a date nmonths later.
   *
   * @param date The old date
   * @param nmonths The number of months to add
   * @return The new date
   */
  virtual ExtDate addMonths(const ExtDate & date, int nmonths) const = 0;

  /**
   * Returns a QDate object containing a date ndays later.
   *
   * @param date The old date
   * @param ndays The number of days to add
   * @return The new date
   */
  virtual ExtDate addDays(const ExtDate & date, int ndays) const = 0;

  /**
   * Gets specific calendar type number of month for a given year
   *
   * @param date The date whose year to use
   * @return The number of months in that year
   */
  virtual int monthsInYear (const ExtDate & date) const = 0;

  /**
   * Gets the number of days in date whose years specified.
   *
   * @param date Gregorian date equivalent to the specific one
   * @return The number of days in year
   */
  virtual int daysInYear (const ExtDate & date) const = 0;

  /**
   * Gets specific calendar type number of days in month for a given date
   *
   * @param date gregorian date
   * @return number of days for month in date
   */
  virtual int daysInMonth (const ExtDate & date) const = 0;

  /**
   * Gets the number of weeks in a specified year
   *
   * @param year the year
   * @return number of weeks in year
   */
  virtual int weeksInYear(int year) const = 0;

  /**
   * Gets specific calendar type week number for a given date
   *
   * @param date gregorian date
   * @param yearNum The year the date belongs to
   * @return week number
   */
  virtual int weekNumber(const ExtDate& date, int * yearNum = 0) const = 0;

  /**
   * Gets specific calendar type month name for a given month number
   * If an invalid month is specified, QString::null is returned.
   *
   * @param month The month number
   * @param year The year the month belongs to
   * @param shortName Specifies if the short month name should be used
   * @return The name of the month
   */
  virtual QString monthName (int month, int year, bool shortName = false) const = 0;

  /**
   * Gets specific calendar type month name for a given gregorian date
   *
   * @param date Gregorian date
   * @param shortName Specifies if the short month name should be used
   * @return The name of the month
   */
  virtual QString monthName (const ExtDate & date, bool shortName = false ) const = 0;

  /**
   * Returns a string containing the possessive form of the month name.
   * ("of January", "of February", etc.)
   * It's needed in long format dates in some languages.
   * If an invalid month is specified, QString::null is returned.
   *
   * @param month The month number
   * @param year The year the month belongs to
   * @param shortName Specifies if the short month name should be used
   *
   * @return The possessive form of the name of the month
   */
  virtual QString monthNamePossessive(int month, int year, bool shortName = false) const = 0;

  /**
   * Returns a string containing the possessive form of the month name.
   * ("of January", "of February", etc.)
   * It's needed in long format dates in some languages.
   *
   * @param date Gregorian date
   * @param shortName Specifies if the short month name should be used
   *
   * @return The possessive form of the name of the month
   */
  virtual QString monthNamePossessive(const ExtDate & date, bool shortName = false) const = 0;

  /**
   * Gets specific calendar type week day name
   * If an invalid week day is specified, QString::null is returned.
   *
   * @param weekDay number of day in week (1 -> Monday)
   * @param shortName short or complete day name
   * @return day name
   */
  virtual QString weekDayName (int weekDay, bool shortName = false) const = 0;

  /**
   * Gets specific calendar type week day name
   *
   * @param date the date
   * @param shortName short or complete day name
   * @return day name
   */
  virtual QString weekDayName (const ExtDate & date, bool shortName = false) const = 0;

  /**
   * Gets the first year value supported by specific calendar type
   * algorithms.
   *
   * @return first year supported
   */
  virtual int minValidYear () const = 0;

  /**
   * Gets the maximum year value supported by specific calendar type
   * algorithms (QDate, 8000)
   *
   * @return maximum year supported
   */
  virtual int maxValidYear () const = 0;

  /**
   * Gets the day of the week traditionaly associated with pray
   *
   * @return day number
   */
  virtual int weekDayOfPray () const = 0;

  /**
   * Gets the string representing the calendar
   */
  virtual QString calendarName() const = 0;

  /**
   * Gets if the calendar is lunar based
   *
   * @return if the calendar is lunar based
   */
  virtual bool isLunar() const = 0;

  /**
   * Gets if the calendar is lunisolar based
   *
   * @return if the calendar is lunisolar based
   */
  virtual bool isLunisolar() const = 0;

  /**
   * Gets if the calendar is solar based
   *
   * @return if the calendar is solar based
   */
  virtual bool isSolar() const = 0;

protected:
  const KLocale * locale() const;

private:
  ExtCalendarSystemPrivate * d;
};

}

#endif
