/*************************************************************************
** Definition of extended range date classe
** (c) 2004 by Michel Guitel <michel.guitel@sap.ap-hop-paris.fr>
** modifications by Jason Harris <kstars@30doradus.org>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "kstextdatetime.h"
#include <qregexp.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <assert.h>
#include <time.h>

namespace KST {
static const uint SECS_PER_DAY  = 86400;
static const uint MSECS_PER_DAY = 86400000;
static const uint SECS_PER_HOUR = 3600;
static const uint MSECS_PER_HOUR= 3600000;
static const uint SECS_PER_MIN  = 60;
static const uint MSECS_PER_MIN = 60000;

/*****************************************************************************
  ExtDate class
 *****************************************************************************/

/*****************************************************************************
 *
 * Concepts :
 * a date is represented internally by its Julian Day number, a simple count
 * of the number of days since a remote, arbitrary date (01 January, 4713 BC).
 * This date has Julian Day number zero.
 *
 * ***************************************************************************/

uint ExtDate::m_monthLength[] = {31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31};
uint ExtDate::m_monthOrigin[] = { 0, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#ifndef I18N_NOOP2
#define I18N_NOOP2(x, y) I18N_NOOP(y)
#endif

const char* const ExtDate::m_shortMonthNames[12] = {
	I18N_NOOP("Jan"), I18N_NOOP("Feb"), I18N_NOOP("Mar"), I18N_NOOP("Apr"), I18N_NOOP2("Short form may", "May"), I18N_NOOP("Jun"),
	I18N_NOOP("Jul"), I18N_NOOP("Aug"), I18N_NOOP("Sep"), I18N_NOOP("Oct"), I18N_NOOP("Nov"), I18N_NOOP("Dec")
};
const char* const ExtDate::m_shortDayNames[7] = {
	I18N_NOOP("Mon"), I18N_NOOP("Tue"), I18N_NOOP("Wed"), I18N_NOOP("Thu"), I18N_NOOP("Fri"), I18N_NOOP("Sat"), I18N_NOOP("Sun")
};
const char* const ExtDate::m_longMonthNames[12] = {
	I18N_NOOP("January"), I18N_NOOP("February"), I18N_NOOP("March"), I18N_NOOP("April"), I18N_NOOP2("Long form may", "May"),
	I18N_NOOP("June"), I18N_NOOP("July"), I18N_NOOP("August"), I18N_NOOP("September"), I18N_NOOP("October"),
	I18N_NOOP("November"), I18N_NOOP("December")
};
const char* const ExtDate::m_longDayNames[7] = {
	I18N_NOOP("Monday"), I18N_NOOP("Tuesday"), I18N_NOOP("Wednesday"), I18N_NOOP("Thursday"),
	I18N_NOOP("Friday"), I18N_NOOP("Saturday"), I18N_NOOP("Sunday")
};

ExtDate::ExtDate( int y, int m, int d)
{
	if ( !isValid(y,m,d) ) {
#if defined(QT_CHECK_RANGE)
		qWarning( "ExtDate: Invalid date %04d-%02d-%02d", y, m, d );
#endif
		m_year = 0;
		m_month = 0;
		m_day = 0;
		m_jd = INVALID_DAY;
	} else {
		m_year = y;
		m_month = m;
		m_day = d;
		m_jd = GregorianToJD(y, m, d);
	}
}

ExtDate::ExtDate( long int jd ) {
	m_jd = jd;
	JDToGregorian( jd, m_year, m_month, m_day );
}

long int ExtDate::GregorianToJD( int year, int month, int day )
{
	int m, y, A, B, C, D;

	if (month > 2) {
		m = month;
		y = year;
	} else {
		y = year - 1;
		m = month + 12;
	}

/*  If the date is after 10/15/1582, then take Pope Gregory's modification
	to the Julian calendar into account */

	if ( ( year  >1582 ) ||
		( year ==1582 && month  >9 ) ||
		( year ==1582 && month ==9 && day >15 ))
	{
		A = int(y/100);
		B = 2 - A + int(A/4);
	} else {
		B = 0;
	}

	if (y < 0) {
		C = int((365.25*y) - 0.75);
	} else {
		C = int(365.25*y);
	}

	D = int(30.6001*(m+1));

	long int jd = B + C + D + day + 1720995;

	return jd;
}

void	ExtDate::JDToGregorian( long int jd, int &year, int &month, int &day )
{
	int a, b, c, d, e, alpha;

	if (jd<2299161) {
		a = jd;
	} else {
		alpha = int ((jd-1867216.25)/ 36524.25);
		a = jd + 1 + alpha - int(alpha / 4.0);
	}
	b = a + 1524;
	c = int ((b-122.1)/ 365.25);
	d = int (365.25*c);
	e = int ((b-d)/ 30.6001);

	day = b-d-int(30.6001*e);
	month = (e<14) ? e-1 : e-13;
	year  = (month>2)  ? c-4716 : c-4715;
}

bool ExtDate::isValid() const
{
	return ( jd() != INVALID_DAY && isValid( year(), month(), day() ) );
}

int ExtDate::dayOfWeek() const
{
	//JD 2451545 (01 Jan 2000) was a Saturday, which is dayOfWeek=6.
	int a_day = (( jd() - 2451545 + 6 ) % 7);
	if ( a_day < 0 ) a_day += 7;
	return (a_day == 0) ? 7 : a_day;
}

int ExtDate::dayOfYear() const
{
	return jd() - GregorianToJD( year(), 1, 1) + 1;
}

int ExtDate::daysInMonth() const
{
	if ( isValid() ) {
	  int m = month();
	  int d = m_monthLength[m-1];
	  if (m==2 && leapYear(year())) d++;
	  return d;
	} else {
	  return 31;
	}
}

int ExtDate::daysInYear() const
{
	if ( ! isValid() ) return 365;
	return (leapYear(year()) ? 366 : 365);
}

int ExtDate::weekNumber( int *yearNum ) const
{
	// the year and week number are those of the next Sunday.
	ExtDate a_date( jd() - dayOfWeek() + 7);
	*yearNum = a_date.year();
	return 1 + int( a_date.dayOfYear()/7 );
}

#ifndef QT_NO_TEXTDATE
// ### to whoever wrote this code: Please read kcalendarsystem.h api documentation
QString ExtDate::shortMonthName( int month ) {return i18n(m_shortMonthNames[month-1]);}
QString ExtDate::shortDayName( int weekday ) {return i18n(m_shortDayNames[weekday-1]);}
QString ExtDate::longMonthName( int month ) {return i18n(m_longMonthNames[month-1]);}
QString ExtDate::longDayName( int weekday ) {return i18n(m_longDayNames[weekday-1]);}
#endif //QT_NO_TEXTDATE

#ifndef QT_NO_TEXTSTRING
#if !defined(QT_NO_SPRINTF)
QString ExtDate::toString( Qt::DateFormat f) const
{
	QString	a_format;

	if ( ! isValid() ) return QString::null;

	switch (f)
	{
		case Qt::TextDate :	// Sat May 20 1995
			a_format = "%a %b %e %Y";
			break;

		case Qt::ISODate :	// YYYY-MM-DD
			a_format = "%Y-%m-%d";
			break;

		case Qt::LocalDate :	// local settings
			a_format = KGlobal::locale()->dateFormat();
			break;

		default :
			a_format = "toString : unknown format";
			break;

	}
	return toString(a_format);
}
#endif

QString ExtDate::toString( const QString& format ) const
{
	if ( ! isValid() ) return QString::null;

	//We use the KDE Date format specs.
	//Replace occurences of the following tokens with their
	//corresponding values:
	//
	// %Y The year, including centuries prefix (e.g., "1984")
	// %y The year, excluding centuries prefix (e.g., "84")
	// %n Numerical month value (e.g., "3" for March)
	// %m Numerical month value, two digits (e.g., "03" for March)
	// %e Numerical day value (e.g., "3" on March 3rd)
	// %d Numerical day value, two digits (e.g., "03" on March 3rd)
	// %b Month name, short form (e.g., "Mar" for March)
	// %B Month name, long form (e.g., "March")
	// %a Weekday name, short form (e.g., "Wed" for Wednesday)
	// %A Weekday name, long form (e.g., "Wednesday")

	//All other characters are left as-is.

	QString result( format );

	result.replace( "%Y", QString().sprintf( "%d", year() ) );
	result.replace( "%y", QString().sprintf( "%02d", (year() % 100) ) );
	result.replace( "%n", QString().sprintf( "%d", month() ) );
	result.replace( "%m", QString().sprintf( "%02d", month() ) );
	result.replace( "%e", QString().sprintf( "%d", day() ) );
	result.replace( "%d", QString().sprintf( "%02d", day() ) );
	result.replace( "%b", shortMonthName( month() ) );
	result.replace( "%B", longMonthName( month() ) );
	result.replace( "%a", shortDayName( dayOfWeek() ) );
	result.replace( "%A", longDayName( dayOfWeek() ) );

	return result;
}
#endif

bool ExtDate::setYMD( int y, int m, int d )
{
	if ( ! isValid(y,m,d) ) {
#if defined(QT_CHECK_RANGE)
		qWarning( "ExtDate: Invalid date %04d-%02d-%02d", y, m, d );
#endif
		m_year = 0;
		m_month = 0;
		m_day = 0;
		m_jd = INVALID_DAY;
		return false;
	} else {
		m_year = y;
		m_month = m;
		m_day = d;
		m_jd = GregorianToJD( y, m, d );
		return true;
	}
}

bool ExtDate::setJD( long int _jd ) {
	if ( _jd == INVALID_DAY ) {
		m_jd = _jd;
		m_year = 0;
		m_month = 0;
		m_day = 0;
		return false;
	} else {
		m_jd = _jd;
		JDToGregorian( _jd, m_year, m_month, m_day );
		return true;
	}
}

ExtDate ExtDate::addDays( int days ) const
{
	ExtDate a_date;
	a_date.setJD( jd() + days );
	return a_date;
}

ExtDate  ExtDate::addMonths( int months ) const
{
	int a_month = month() + months%12;
	int a_year  = year()  + int(months/12);

	while ( a_month < 1 ) {
		a_month += 12;
		a_year--;
	}

	while ( a_month > 12 ) {
		a_month -= 12;
		a_year++;
	}

	return ExtDate(a_year, a_month, day());
}

ExtDate  ExtDate::addYears( int years ) const
{
	return ExtDate(year() + years, month(), day());
}

int   ExtDate::daysTo( const ExtDate & a_date) const
{
	return a_date.jd() - jd();
}

ExtDate ExtDate::currentDate(Qt::TimeSpec ts)
{
	time_t a_current_time;
	struct tm a_current_time_tm;

	time(&a_current_time);
	switch (ts)
	{
		case Qt::LocalTime :
			localtime_r(&a_current_time, &a_current_time_tm);
			break;

		case Qt::UTC :
			gmtime_r(&a_current_time, &a_current_time_tm);
			break;

		default :
			assert(0);
			break;
	}
	return ExtDate(a_current_time_tm.tm_year + 1900, a_current_time_tm.tm_mon + 1, a_current_time_tm.tm_mday);
}

#ifndef QT_NO_DATESTRING
//Try both DateFormat values
ExtDate ExtDate::fromString( const QString& s )
{
	ExtDate dResult = ExtDate::fromString( s, Qt::TextDate );
	if ( dResult.isValid() ) return dResult;

	dResult = ExtDate::fromString( s, Qt::ISODate );
	if ( dResult.isValid() ) return dResult;
	else return ExtDate(); //invalid	
}

ExtDate ExtDate::fromString( const QString& s, Qt::DateFormat f )
{
	ExtDate dt = ExtDate();  //initialize invalid date
	if ( s.isEmpty() ) { return dt; }
	if ( f == Qt::LocalDate ) { //can't use LocalFormat
#if defined(QT_CHECK_RANGE)
		qWarning( "QDate::fromString: Parameter out of range" );
#endif
		return dt;
	}

	switch( f ) {
		case Qt::ISODate :
		{
			int year( s.mid( 0, 4 ).toInt() );
			int month( s.mid( 5, 2 ).toInt() );
			int day( s.mid( 8, 2 ).toInt() );

			if ( year && month && day )
				return ExtDate( year, month, day );
		}
		break;

		default :
#ifndef QT_NO_TEXTDATE
		case Qt::TextDate :
		{
			//Three possible date formats:
			//dd mth yyyy; mth dd yyyy; wkd mth dd yyyy
			//"mth" is the word for the month (long or short form)
			QStringList ss = QStringList::split( " ", s );
			bool ok = false;
			int month = -1;
			uint imonth = 0;
			uint iyear = 0;

			//If neither of the first two words is a number, then we'll assume
			//the first word is a superfluous "weekday" string
			int day = ss[0].toInt( &ok );
			if ( ! ok ) {
				day = ss[1].toInt( &ok );
				if ( ! ok ) {
						day = ss[2].toInt( &ok );
						if ( !ok ) return dt;  //could not find a valid day number in first three words
						imonth = 1;  //the month must be the second word
						iyear = 3;  //the year must be the fourth word
				} else {
					//the month is either the first word, or the third.
					imonth = 0;
					iyear = 2;
				}
			} else {
				//month is the second word
				imonth = 1;
				iyear = 2;
			}

			for ( uint i = 0; i < 12; i++ ) {
				if ( ss[imonth] == shortMonthName( i+1 ) || ss[imonth] == longMonthName( i+1 ) ) {
						month = i + 1;
						break;
				}
			}

			if ( month == -1 && imonth == 0 ) { //try the third word
				imonth = 2;
				iyear = 3;
				for ( uint i = 0; i < 12; i++ ) {
					if ( ss[imonth] == shortMonthName( i+1 ) || ss[imonth] == longMonthName( i+1 ) ) {
							month = i + 1;
							break;
					}
				}
			}

			if ( month > -1 ) ok = true;
			if ( ! ok ) return dt; //could not parse month; return invalid

			int year = ss[iyear].toInt( &ok );
			if ( ! ok ) return dt; //could not parse year; return invalid

			return ExtDate( year, month, day );

			break;
		}
#else
		break;
#endif  //ifndef QT_NO_TEXTDATE
	}

	return dt;
}
#endif  //ifndef QT_NO_DATESTRING

bool ExtDate::isValid( int y, int m, int d )
{
	if (m < 1 || m > 12) return false;
	if (d < 1) return false;
	if (m != 2 && d > (int) m_monthLength[m-1]) return false;
	if (m == 2 && d > ( (int) m_monthLength[1] + (leapYear(y) ? 1 : 0))) return false;
	return true;
}

QDate ExtDate::qdate() const {
	QDate q( year(), month(), day() );

	if ( q.isValid() )
		return q;
	else
		return QDate();
}

bool	ExtDate::leapYear( int year )
{
	// year is the year-number where JC birth is 0
	if ((year % 4) != 0) return false;
	// multiple of 4 : can be a leap year
	// centennial years are NOT leap, but quadri-centennial ARE.
	if ((year % 400) == 0) return true;
	if ((year % 100) == 0) return false;
	// year is multiple of 4 but not centennial so leap year !
	return true;
}

int ExtDate::dayOfYear(int y, int m, int d)
{
	return m_monthOrigin[m-1] + d + ((m > 1) ? (leapYear(y) ? 1 : 0) : 0);
}

/*****************************************************************************
  ExtDateTime member functions
 *****************************************************************************/

/*!
    \class ExtDateTime extdatetime.h
    \brief The ExtDateTime class provides date and time functions.

    \ingroup time

    A ExtDateTime object contains a calendar date and a clock time (a
    "datetime"). It is a combination of the ExtDate and QTime classes.
    It can read the current datetime from the system clock. It
    provides functions for comparing datetimes and for manipulating a
    datetime by adding a number of seconds, days, months or years.

    A ExtDateTime object is typically created either by giving a date
    and time explicitly in the constructor, or by using the static
    function currentDateTime(), which returns a ExtDateTime object set
    to the system clock's time. The date and time can be changed with
    setDate() and setTime(). A datetime can also be set using the
    setTime_t() function, which takes a POSIX-standard "number of
    seconds since 00:00:00 on January 1, 1970" value. The fromString()
    function returns a ExtDateTime given a string and a date format
    which is used to interpret the date within the string.

    The date() and time() functions provide access to the date and
    time parts of the datetime. The same information is provided in
    textual format by the toString() function.

    ExtDateTime provides a full set of operators to compare two
    ExtDateTime objects where smaller means earlier and larger means
    later.

    You can increment (or decrement) a datetime by a given number of
    seconds using addSecs() or days using addDays(). Similarly you can
    use addMonths() and addYears(). The daysTo() function returns the
    number of days between two datetimes, and secsTo() returns the
    number of seconds between two datetimes.

    The range of a datetime object is constrained to the ranges of the
    ExtDate and QTime objects which it embodies.

    Methods in this class are reentrant.

    \sa ExtDate QTime ExtDateTimeEdit
*/


/*!
    \fn ExtDateTime::ExtDateTime()

    Constructs a null datetime (i.e. null date and null time). A null
    datetime is invalid, since the date is invalid.

    \sa isValid()
*/


/*!
    Constructs a datetime with date \a date and null (but valid) time
    (00:00:00.000).
*/

ExtDateTime::ExtDateTime( const ExtDate &date )
    : d(date)
{
}

/*!
    Constructs a datetime with date \a date and time \a time.
*/

ExtDateTime::ExtDateTime( const ExtDate &date, const QTime &time )
    : d(date), t(time)
{
}


/*!
    \fn bool ExtDateTime::isNull() const

    Returns TRUE if both the date and the time are null; otherwise
    returns FALSE. A null datetime is invalid.

    \sa ExtDate::isNull(), QTime::isNull()
*/

/*!
    \fn bool ExtDateTime::isValid() const

    Returns TRUE if both the date and the time are valid; otherwise
    returns FALSE.

    \sa ExtDate::isValid(), QTime::isValid()
*/

/*!
    \fn ExtDate ExtDateTime::date() const

    Returns the date part of the datetime.

    \sa setDate(), time()
*/

/*!
    \fn QTime ExtDateTime::time() const

    Returns the time part of the datetime.

    \sa setTime(), date()
*/

/*!
    \fn void ExtDateTime::setDate( const ExtDate &date )

    Sets the date part of this datetime to \a date.

    \sa date(), setTime()
*/

/*!
    \fn void ExtDateTime::setTime( const QTime &time )

    Sets the time part of this datetime to \a time.

    \sa time(), setDate()
*/


/*!
    Returns the datetime as the number of seconds that have passed
    since 1970-01-01T00:00:00, Coordinated Universal Time (UTC).

    On systems that do not support timezones, this function will
    behave as if local time were UTC.

    \sa setTime_t()
*/

uint ExtDateTime::toTime_t() const
{
    tm brokenDown;
    brokenDown.tm_sec = t.second();
    brokenDown.tm_min = t.minute();
    brokenDown.tm_hour = t.hour();
    brokenDown.tm_mday = d.day();
    brokenDown.tm_mon = d.month() - 1;
    brokenDown.tm_year = d.year() - 1900;
    brokenDown.tm_isdst = -1;
    int secsSince1Jan1970UTC = (int) mktime( &brokenDown );
    if ( secsSince1Jan1970UTC < -1 )
	secsSince1Jan1970UTC = -1;
    return (uint) secsSince1Jan1970UTC;
}

/*!
    \overload

    Convenience function that sets the date and time to local time
    based on the given UTC time.
*/

void ExtDateTime::setTime_t( uint secsSince1Jan1970UTC )
{
    setTime_t( secsSince1Jan1970UTC, Qt::LocalTime );
}

/*!
    Sets the date and time to \a ts time (\c Qt::LocalTime or \c
    Qt::UTC) given the number of seconds that have passed since
    1970-01-01T00:00:00, Coordinated Universal Time (UTC). On systems
    that do not support timezones this function will behave as if
    local time were UTC.

    On Windows, only a subset of \a secsSince1Jan1970UTC values are
    supported, as Windows starts counting from 1980.

    \sa toTime_t()
*/
void ExtDateTime::setTime_t( uint secsSince1Jan1970UTC, Qt::TimeSpec ts )
{
    time_t tmp = (time_t) secsSince1Jan1970UTC;
    tm *brokenDown = 0;

#if defined(Q_OS_UNIX) && defined(QT_THREAD_SUPPORT) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // posix compliant system
    // use the reentrant versions of localtime() and gmtime() where available
    tm res;
    if ( ts == Qt::LocalTime )
	brokenDown = localtime_r( &tmp, &res );
    if ( !brokenDown ) {
	brokenDown = gmtime_r( &tmp, &res );
	if ( !brokenDown ) {
	    d.setJD( ExtDate::GregorianToJD( 1970, 1, 1 ) );
	    t.setHMS(0,0,0);
			//	    t.ds = 0;
	    return;
	}
    }
#else
    if ( ts == Qt::LocalTime )
	brokenDown = localtime( &tmp );
    if ( !brokenDown ) {
	brokenDown = gmtime( &tmp );
	if ( !brokenDown ) {
	    d.setJD( ExtDate::GregorianToJD( 1970, 1, 1 ) );
//	    t.ds = 0;
	    t.setHMS(0,0,0);
			return;
	}
    }
#endif

    d.setJD( ExtDate::GregorianToJD( brokenDown->tm_year + 1900,
				     brokenDown->tm_mon + 1,
				     brokenDown->tm_mday ) );
    t.setHMS( brokenDown->tm_hour, brokenDown->tm_min, brokenDown->tm_sec );
//		t.ds = MSECS_PER_HOUR * brokenDown->tm_hour +
//	   MSECS_PER_MIN * brokenDown->tm_min +
//	   1000 * brokenDown->tm_sec;
}
#ifndef QT_NO_DATESTRING
#ifndef QT_NO_SPRINTF
/*!
    \overload

    Returns the datetime as a string. The \a f parameter determines
    the format of the string.

    If \a f is \c Qt::TextDate, the string format is "Wed May 20
    03:40:13 1998" (using ExtDate::shortDayName(), ExtDate::shortMonthName(),
    and QTime::toString() to generate the string, so the day and month
    names will have localized names).

    If \a f is \c Qt::ISODate, the string format corresponds to the
    ISO 8601 extended specification for representations of dates and
    times, which is YYYY-MM-DDTHH:MM:SS.

    If \a f is \c Qt::LocalDate, the string format depends on the
    locale settings of the system.

    If the format \a f is invalid or the datetime is invalid, toString()
    returns a null string.

    \sa ExtDate::toString() QTime::toString()
*/

QString ExtDateTime::toString( Qt::DateFormat f ) const
{
	if ( !isValid() )
		return QString::null;

	if ( f == Qt::ISODate ) {
		return d.toString( Qt::ISODate ) + "T" + t.toString( Qt::ISODate );
	}
#ifndef QT_NO_TEXTDATE
	else if ( f == Qt::TextDate ) {
		return toString( "%a %b %e %Y %H:%M:%S" );
	}
#endif
	else if ( f == Qt::LocalDate ) {
		return toString( KGlobal::locale()->dateFormat()
						+ " " + KGlobal::locale()->timeFormat() );
	}

	return QString::null;
}
#endif

QString ExtDateTime::toString( const QString& format ) const
{
	if ( !isValid() )
		return QString::null;

	//Parse the date portion of the format string
	QString result = date().toString( format );

	//For the time format, use the following KDE format specs:
	//Replace occurences of the following tokens with their
	//corresponding values:
	//
	// %H Hour in 24h format, 2 digits
	// %k Hour in 24h format, 1-2 digits
	// %I Hour in 12h format, 2 digits
	// %l Hour in 12h format, 1-2 digits
	// %M Minute, 2 digits
	// %S Seconds, 2 digits
	// %p pm/am

	int h = time().hour();

	result.replace( "%H", QString().sprintf( "%02d", h ) );
	result.replace( "%k", QString().sprintf( "%d", h ) );
	result.replace( "%I", QString().sprintf( "%02d", ( h > 12 ) ? h-12 : h ) );
	result.replace( "%l", QString().sprintf( "%d", ( h > 12 ) ? h-12 : h ) );
	result.replace( "%M", QString().sprintf( "%02d", time().minute() ) );
	result.replace( "%S", QString().sprintf( "%02d", time().second() ) );
	result.replace( "%p", QString().sprintf( "%s", ( h > 12 ) ? "pm" : "am" ) );

	return result;
}
#endif //QT_NO_DATESTRING

/*!
    Returns a ExtDateTime object containing a datetime \a ndays days
    later than the datetime of this object (or earlier if \a ndays is
    negative).

    \sa daysTo(), addMonths(), addYears(), addSecs()
*/

ExtDateTime ExtDateTime::addDays( int ndays ) const
{
    return ExtDateTime( d.addDays(ndays), t );
}

/*!
    Returns a ExtDateTime object containing a datetime \a nmonths months
    later than the datetime of this object (or earlier if \a nmonths
    is negative).

    \sa daysTo(), addDays(), addYears(), addSecs()
*/

ExtDateTime ExtDateTime::addMonths( int nmonths ) const
{
    return ExtDateTime( d.addMonths(nmonths), t );
}

/*!
    Returns a ExtDateTime object containing a datetime \a nyears years
    later than the datetime of this object (or earlier if \a nyears is
    negative).

    \sa daysTo(), addDays(), addMonths(), addSecs()
*/

ExtDateTime ExtDateTime::addYears( int nyears ) const
{
    return ExtDateTime( d.addYears(nyears), t );
}

/*!
    Returns a ExtDateTime object containing a datetime \a nsecs seconds
    later than the datetime of this object (or earlier if \a nsecs is
    negative).

    \sa secsTo(), addDays(), addMonths(), addYears()
*/

ExtDateTime ExtDateTime::addSecs( int nsecs ) const
{
	long int dd = d.jd();
	int tt = MSECS_PER_HOUR*t.hour() + MSECS_PER_MIN*t.minute() + 1000*t.second() + t.msec();
	tt += nsecs*1000;

	while ( tt < 0 ) {
		tt += MSECS_PER_DAY;
		--dd;
	}

	while ( tt > int(MSECS_PER_DAY) ) {
		tt -= MSECS_PER_DAY;
		++dd;
	}

	ExtDateTime ret;
	ret.setTime( QTime().addMSecs( tt ) );
	ret.setDate( ExtDate( dd ) );

	return ret;
}

/*!
    Returns the number of days from this datetime to \a dt (which is
    negative if \a dt is earlier than this datetime).

    \sa addDays(), secsTo()
*/

int ExtDateTime::daysTo( const ExtDateTime &dt ) const
{
    return d.daysTo( dt.d );
}

/*!
    Returns the number of seconds from this datetime to \a dt (which
    is negative if \a dt is earlier than this datetime).

    Example:
    \code
    ExtDateTime dt = ExtDateTime::currentDateTime();
    ExtDateTime xmas( ExtDate(dt.date().year(),12,24), QTime(17,00) );
    kdDebug( ) << "There are " << dt.secsTo(xmas) << " seconds to Christmas" << endl;
    \endcode

    \sa addSecs(), daysTo(), QTime::secsTo()
*/

int ExtDateTime::secsTo( const ExtDateTime &dt ) const
{
    return t.secsTo(dt.t) + d.daysTo(dt.d)*SECS_PER_DAY;
}


/*!
    Returns TRUE if this datetime is equal to \a dt; otherwise returns FALSE.

    \sa operator!=()
*/

bool ExtDateTime::operator==( const ExtDateTime &dt ) const
{
    return  t == dt.t && d == dt.d;
}

/*!
    Returns TRUE if this datetime is different from \a dt; otherwise
    returns FALSE.

    \sa operator==()
*/

bool ExtDateTime::operator!=( const ExtDateTime &dt ) const
{
    return  t != dt.t || d != dt.d;
}

/*!
    Returns TRUE if this datetime is earlier than \a dt; otherwise
    returns FALSE.
*/

bool ExtDateTime::operator<( const ExtDateTime &dt ) const
{
    if ( d < dt.d )
	return true;
    return d == dt.d ? t < dt.t : false;
}

/*!
    Returns TRUE if this datetime is earlier than or equal to \a dt;
    otherwise returns FALSE.
*/

bool ExtDateTime::operator<=( const ExtDateTime &dt ) const
{
    if ( d < dt.d )
	return true;
    return d == dt.d ? t <= dt.t : false;
}

/*!
    Returns TRUE if this datetime is later than \a dt; otherwise
    returns FALSE.
*/

bool ExtDateTime::operator>( const ExtDateTime &dt ) const
{
    if ( d > dt.d )
	return true;
    return d == dt.d ? t > dt.t : false;
}

/*!
    Returns TRUE if this datetime is later than or equal to \a dt;
    otherwise returns FALSE.
*/

bool ExtDateTime::operator>=( const ExtDateTime &dt ) const
{
    if ( d > dt.d )
	return true;
    return d == dt.d ? t >= dt.t : false;
}

/*!
    \overload

    Returns the current datetime, as reported by the system clock.

    \sa ExtDate::currentDate(), QTime::currentTime()
*/

ExtDateTime ExtDateTime::currentDateTime()
{
    return currentDateTime( Qt::LocalTime );
}

/*!
  Returns the current datetime, as reported by the system clock, for the
  TimeSpec \a ts. The default TimeSpec is LocalTime.

  \sa ExtDate::currentDate(), QTime::currentTime(), Qt::TimeSpec
*/

ExtDateTime ExtDateTime::currentDateTime( Qt::TimeSpec ts )
{
    ExtDateTime dt;
    dt.setDate( ExtDate::currentDate(ts) );
    QTime t = t.currentTime(ts);
    if ( t.hour()==0 && t.minute()==0 )         // midnight or right after?
        dt.setDate( ExtDate::currentDate(ts) ); // fetch date again
    dt.setTime( t );
    return dt;
}

#ifndef QT_NO_DATESTRING
/*!
    Returns the ExtDateTime represented by the string \a s, using the
    format \a f, or an invalid datetime if this is not possible.

    Note for \c Qt::TextDate: It is recommended that you use the
    English short month names (e.g. "Jan"). Although localized month
    names can also be used, they depend on the user's locale settings.

    \warning Note that \c Qt::LocalDate cannot be used here.
*/
ExtDateTime ExtDateTime::fromString( const QString& s )
{
	ExtDateTime dtResult = ExtDateTime::fromString( s, Qt::TextDate );
	if ( dtResult.isValid() ) return dtResult;

	dtResult = ExtDateTime::fromString( s, Qt::ISODate );

	if ( dtResult.isValid() ) return dtResult;
	else return ExtDateTime(); //invalid
}

ExtDateTime ExtDateTime::fromString( const QString& s, Qt::DateFormat f )
{
	ExtDateTime dt;

	if ( ( s.isEmpty() ) || ( f == Qt::LocalDate ) ) {
#if defined(QT_CHECK_RANGE)
		qWarning( "ExtDateTime::fromString: Parameter out of range" );
#endif
		dt.d.setJD( INVALID_DAY );
		return dt;
	}

	if ( f == Qt::ISODate ) {
		if ( s.length() <= 10 || ! s.contains( ':' )  ) { //no time specified
			QTime t = QTime(0,0,0);
			return ExtDateTime( ExtDate::fromString( s.mid(0,10), Qt::ISODate ) );
		} else {
			return ExtDateTime( ExtDate::fromString( s.mid(0,10), Qt::ISODate ),
					QTime::fromString( s.mid(11), Qt::ISODate ) );
		}
	}
#if !defined(QT_NO_REGEXP) && !defined(QT_NO_TEXTDATE)
	else if ( f == Qt::TextDate ) {

		//parse the time, if it exists.
		QTime time;
		QString sd = s;
		int hour, minute, second;
		int pivot = s.find( QRegExp(QString::fromLatin1("[0-9][0-9]:[0-9][0-9]:[0-9][0-9]")) );
		if ( pivot != -1 ) {
			hour = s.mid( pivot, 2 ).toInt();
			minute = s.mid( pivot+3, 2 ).toInt();
			second = s.mid( pivot+6, 2 ).toInt();
			time.setHMS( hour, minute, second );

			sd = s.left( pivot - 1 );
		}

		//sd is now just the date string.
		ExtDate date = ExtDate::fromString( s, Qt::TextDate );
		return ExtDateTime( date, time );
	}

#endif //QT_NO_REGEXP
	return ExtDateTime();
}
#endif //QT_NO_DATESTRING


#ifndef QT_NO_DATASTREAM
QDataStream &operator<<( QDataStream & ostream, const ExtDate & date)
{
	return ostream << (Q_UINT32)(date.jd());
}

QDataStream &operator>>( QDataStream & ostream, ExtDate & date)
{
	Q_UINT32 julday;
	ostream >> julday;
	date.setJD( julday );
	return ostream;
}

QDataStream &operator<<( QDataStream & ostream, const ExtDateTime & dt)
{
	return ostream << dt.d << dt.t;
}

QDataStream &operator>>( QDataStream & ostream, ExtDateTime & dt)
{
	ostream >> dt.d >> dt.t;
	return ostream;
}

}
#endif // QT_NO_DATASTREAM
