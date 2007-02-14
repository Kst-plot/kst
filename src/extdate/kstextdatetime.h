/*************************************************************************
** Definition of extended range date class
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

#ifndef KSTEXTDATETIME_H
#define KSTEXTDATETIME_H

#include <limits.h>
#include "qstring.h"
#include "qnamespace.h"
#include "qdatetime.h"
#include <kdeversion.h>
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
#include <kdemacros.h>
#endif
#include "kst_export.h"


#define INVALID_DAY LONG_MIN

namespace KST {
class ExtDateTime;

/*****************************************************************************
  ExtDate class
 *****************************************************************************/

extern	void test2_unit(int y, int m, int d);

class ExtDateTime;

class KST_EXPORT ExtDate
{
public:
	ExtDate() : m_jd(INVALID_DAY), m_year(0), m_month(0), m_day(0) {}
	ExtDate( int y, int m, int d );
	ExtDate( const QDate &q ) { ExtDate( q.year(), q.month(), q.day() ); }
	ExtDate( long int jd );

	bool isNull() const { return m_jd == INVALID_DAY; }
	bool isValid() const;

	QDate qdate() const;

	int year() const { return m_year; }
	int month() const { return m_month; }
	int day() const { return m_day; }
	int dayOfWeek() const;
	int dayOfYear() const;
	int daysInMonth() const;
	int daysInYear() const;
	int weekNumber( int *yearNum = 0 ) const;
	long int jd() const { return m_jd; }

#ifndef QT_NO_TEXTDATE
#ifndef QT_NO_COMPAT
	static QString monthName( int month ) { return shortMonthName( month ); }
	static QString dayName( int weekday ) { return shortDayName( weekday ); }
#endif
	static QString shortMonthName( int month );
	static QString shortDayName( int weekday );
	static QString longMonthName( int month );
	static QString longDayName( int weekday );
#endif //QT_NO_TEXTDATE
#ifndef QT_NO_TEXTSTRING
#if !defined(QT_NO_SPRINTF)
	QString toString( Qt::DateFormat f = Qt::TextDate ) const;
#endif
	QString toString( const QString& format ) const;
#endif
	bool setYMD( int y, int m, int d );
	bool setJD( long int _jd );

	ExtDate addDays( int days ) const;
	ExtDate addMonths( int months ) const;
	ExtDate addYears( int years ) const;
	int daysTo( const ExtDate & ) const;

	bool operator==( const ExtDate &d ) const { return m_jd == d.jd(); };
	bool operator!=( const ExtDate &d ) const { return m_jd != d.jd(); };
	bool operator<( const ExtDate &d )  const { return m_jd  < d.jd(); };
	bool operator<=( const ExtDate &d ) const { return m_jd <= d.jd(); };
	bool operator>( const ExtDate &d )  const { return m_jd  > d.jd(); };
	bool operator>=( const ExtDate &d ) const { return m_jd >= d.jd(); };

	static ExtDate currentDate( Qt::TimeSpec ts = Qt::LocalTime );
#ifndef QT_NO_DATESTRING
	static ExtDate fromString( const QString &s );
	static ExtDate fromString( const QString &s, Qt::DateFormat f );
#endif
	static bool isValid( int y, int m, int d );
	static bool leapYear( int year );

	static long int GregorianToJD( int y, int m, int d );
	static void JDToGregorian( long int jd, int &y, int &m, int &d );

private:
	static int dayOfYear(int y, int m, int d);

	long int m_jd;
	int m_year, m_month, m_day;
	static uint m_monthLength[12];
	static uint m_monthOrigin[12];
	static const char* const m_shortMonthNames[12];
	static const char* const m_shortDayNames[7];
	static const char* const m_longMonthNames[12];
	static const char* const m_longDayNames[7];

	friend class ExtDateTime;

	#ifndef QT_NO_DATASTREAM
	friend Q_EXPORT QDataStream &operator<<( QDataStream &, const ExtDate & );
	friend Q_EXPORT QDataStream &operator>>( QDataStream &, ExtDate & );
	friend Q_EXPORT QDataStream &operator<<( QDataStream &, const ExtDateTime & );
	friend Q_EXPORT QDataStream &operator>>( QDataStream &, ExtDateTime & );
#endif
};

/*****************************************************************************
  ExtDateTime class
 *****************************************************************************/

class KST_EXPORT ExtDateTime
{
public:
    ExtDateTime() {}				// set null date and null time
    ExtDateTime( const ExtDate & );
    ExtDateTime( const ExtDate &, const QTime & );

    bool   isNull()	const		{ return d.isNull() && t.isNull(); }
    bool   isValid()	const		{ return d.isValid() && t.isValid(); }

    ExtDate  date()	const		{ return d; }
    QTime  time()	const		{ return t; }
    uint   toTime_t()	const;
    void   setDate( const ExtDate &date ) { d = date; }
    void   setTime( const QTime &time ) { t = time; }
    void   setTime_t( uint secsSince1Jan1970UTC );
    void   setTime_t( uint secsSince1Jan1970UTC, Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
#ifndef QT_NO_SPRINTF
    QString toString( Qt::DateFormat f = Qt::TextDate )	const;
#endif
    QString toString( const QString& format ) const;
#endif
    ExtDateTime addDays( int days )	const;
    ExtDateTime addMonths( int months )   const;
    ExtDateTime addYears( int years )     const;
    ExtDateTime addSecs( int secs )	const;
    int	   daysTo( const ExtDateTime & )	const;
    int	   secsTo( const ExtDateTime & )	const;

    bool   operator==( const ExtDateTime &dt ) const;
    bool   operator!=( const ExtDateTime &dt ) const;
    bool   operator<( const ExtDateTime &dt )  const;
    bool   operator<=( const ExtDateTime &dt ) const;
    bool   operator>( const ExtDateTime &dt )  const;
    bool   operator>=( const ExtDateTime &dt ) const;

    static ExtDateTime currentDateTime();
    static ExtDateTime currentDateTime( Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
    static ExtDateTime fromString( const QString &s );
    static ExtDateTime fromString( const QString &s, Qt::DateFormat f );
#endif
private:
    ExtDate  d;
    QTime  t;
#ifndef QT_NO_DATASTREAM
    friend Q_EXPORT QDataStream &operator<<( QDataStream &, const ExtDateTime &);
    friend Q_EXPORT QDataStream &operator>>( QDataStream &, ExtDateTime & );
#endif
};

/*****************************************************************************
  Date and time stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream &, const ExtDate & );
Q_EXPORT QDataStream &operator>>( QDataStream &, ExtDate & );
#endif // QT_NO_DATASTREAM
}

#endif // EXTDATE_H

