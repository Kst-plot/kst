/*
   This file is part of the KDE libraries
   Copyright (c) 2005 S.R.Haque <srhaque@iee.org>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _TIMEZONES_H
#define _TIMEZONES_H

#include <qdatetime.h>
#include <qnamespace.h>
#include <qmap.h>
#include <qstring.h>
#include "sharedptr.h"
#include <kst_export.h>

namespace Kst {

class TimezoneDetails;
class TimezoneDetailsPrivate;
class TimezonePrivate;
class TimezonesPrivate;

/**
 * The KTimezoneSource class contains information source-dependent functions
 * related to a timezone. Create subclasses to implement custom sources of
 * timezone information.
 *
 * For example, to be able to create {@link KTimezone } objects from libical's
 * VTIMEZONE objects:
 *<ul>
 *      <li>Subclass this class with a custom {@link parse() } routine.
 *      <li>Create one or more instances of this class.
 *      <li>Use the instance(s) to create {@link KTimezone } objects.
 *      <li>If required, add the objects to a {@link KTimezones } database.
 *</ul>
 * @since 3.5
 * @author S.R.Haque <srhaque@iee.org>.
 */
class KSTCORE_EXPORT TimezoneSource :
    public Shared
{
public:
    TimezoneSource(const QString &db);
    virtual ~TimezoneSource();

    /**
     * Location of system timezone information.
     * @return value which can be combined with zone name to retrieve timezone info.
     */
    virtual QString db();

    /**
     * Extract timezone detail information. The default implementation consists
     * of a parser for zoneinfo files in tzfile(5).
     * @return true if the parse encountered no errors.
     */
    virtual bool parse(const QString &zone, TimezoneDetails &dataReceiver) const;

private:
    QString m_db;
};

/**
 * The Timezone class contains core functions related to a timezone. Instances
 * are created in the context of a {@link TimezoneSource } which provides
 * extended functionality via {@link TimezoneDetails }.
 *
 * @see TimezoneSource
 * @see TimezoneDetails
 * @since 3.5
 * @author S.R.Haque <srhaque@iee.org>.
 */
class KSTCORE_EXPORT Timezone
{
public:
    /**
     * A representation for unknown locations; this is a float
     * that does not represent a real latitude or longitude.
     */
    static const float UNKNOWN;

    /**
     * A test for a valid latitude. The valid range is +90.0 (North Pole)
     * to -90.0 (South Pole).
     */
    static bool isValidLatitude(float latitude);

    /**
     * A test for a valid longitude. The valid range is +180.0 (east of
     * Greenwich) to -180.0 (west of Greenwich).
     */
    static bool isValidLongitude(float longitude);

    /**
     * Create a timezone.
     *
     * @param db database of timezones.
     * @param name in system-dependent format.
     * @param countryCode ISO 3166 2-character country code, empty if unknown.
     * @param latitude in degrees, UNKNOWN if not known.
     * @param longitude in degrees, UNKNOWN if not known.
     * @param comment description of the timezone, if any.
     */
    Timezone(
        SharedPtr<TimezoneSource> db, const QString &name,
        const QString &countryCode = QString(), float latitude = UNKNOWN, float longitude = UNKNOWN,
        const QString &comment = QString());
    ~Timezone();

    /**
     * Returns the name of the timezone.
     *
     * @return name in system-dependent format.
     */
    QString name() const;

    /**
     * Returns the two-letter country code of the timezone.
     *
     * @return ISO 3166 2-character country code, empty if unknown.
     */
    QString countryCode() const;

    /**
     * Returns the latitude of the timezone.
     *
     * @return latitude in degrees, UNKNOWN if not known.
     */
    float latitude() const;

    /**
     * Returns the latitude of the timezone.
     *
     * @return latitude in degrees, UNKNOWN if not known.
     */
    float longitude() const;

    /**
     * Returns the current offset of this timezone to UTC or the local
     * timezone in seconds.
     *
     * Take care if you cache the results of this routine; that would
     * break if the result were stored across a daylight savings change.
     *
     * @return offset in seconds.
     */
    int offset(Qt::TimeSpec basisSpec = Qt::UTC) const;

    /**
     * Returns the offset of the given timezone to UTC at the given
     * date/time (which is interpreted as being UTC).
     *
     * @return offset in seconds.
     */
    int offset(const QDateTime &dateTime) const;

    /**
     * Convert a date/time (which is interpreted as being localtime in this
     * timezone) into localtime in the given timezone.
     *
     * @return converted date/time.
     */
    QDateTime convert(const Timezone *newZone, const QDateTime &dateTime) const;

    /**
     * Returns any comment for the timezone.
     *
     * @return comment, may be empty.
     */
    QString comment() const;

    /**
     * Extract timezone detail information.
     * @return true if the parse encountered no errors.
     */
    bool parse(TimezoneDetails &dataReceiver) const;

private:
    SharedPtr<TimezoneSource> m_db;
    QString m_name;
    QString m_countryCode;
    float m_latitude;
    float m_longitude;
    QString m_comment;
    TimezonePrivate *d;
};

/**
 * The TimezoneDetails class contains extended functions related to a
 * timezone.
 *
 * The parser must be customised by overriding the given virtual callbacks:
 *<ul>
 *    <li>{@link parseEnded() }
 *    <li>{@link parseStarted() }
 *    <li>{@link gotHeader() }
 *    <li>{@link gotTransitionTime() }
 *    <li>{@link gotLocalTimeIndex() }
 *    <li>{@link gotLocalTime() }
 *    <li>{@link gotAbbreviation() }
 *    <li>{@link gotLeapAdjustment() }
 *    <li>{@link gotIsStandard() }
 *    <li>{@link gotIsUTC() }
 *</ul>
 *
 * @see Timezone
 * @since 3.5
 * @author S.R.Haque <srhaque@iee.org>.
 */
class KSTCORE_EXPORT TimezoneDetails
{
public:
    TimezoneDetails();
    virtual ~TimezoneDetails();

    /**
     * Always called after all other callbacks.
     */
    virtual void parseEnded();

    /**
     * Always called before any other callback.
     */
    virtual void parseStarted();

    /**
     * Called when the header is seen.
     */
    virtual void gotHeader(
        unsigned ttIsGmtCnt, unsigned ttIsStdCnt, unsigned leapCnt,
        unsigned timeCnt, unsigned typeCnt, unsigned charCnt);

    /**
     * Called when a transition time is seen.
     */
    virtual void gotTransitionTime(int index, unsigned transitionTime);

    /**
     * Called when a local time index is seen.
     */
    virtual void gotLocalTimeIndex(int index, unsigned localTimeIndex);

    /**
     * Called when a local time is seen.
     */
    virtual void gotLocalTime(int index, int gmtOff, bool isDst, unsigned abbrIndex);

    /**
     * Called when a timezone abbreviation is seen. Note that the index here
     * is NOT a simple incrementing integer, rather it matches the sequence
     * of abbrIndex values from {@link gotLocalTime() }.
     */
    virtual void gotAbbreviation(int index, const QString &abbr);

    /**
     * Called when a leap second adjustment is seen.
     */
    virtual void gotLeapAdjustment(int index, unsigned leapTime, unsigned leapSeconds);

    /**
     * Called when a standard/wall time indicator is seen.
     */
    virtual void gotIsStandard(int index, bool isStandard);

    /**
     * Called when a UTC/local time indicator is seen.
     */
    virtual void gotIsUTC(int index, bool isUTC);

private:
    TimezoneDetailsPrivate *d;
};

/**
 * The Timezones class models a timezone database. It supports system
 * timezones, and also has support for private timezone entries.
 *
 * @since 3.5
 * @author S.R.Haque <srhaque@iee.org>.
 */
class KSTCORE_EXPORT Timezones
{
public:
    Timezones();
    ~Timezones();

    /**
     * Returns the local timezone. The idea of this routine is to provide a
     * robust lookup of the local timezone.
     *
     * The problem is that on Unix systems, there are a variety of mechanisms
     * for setting this information, and no real way of getting it. For example,
     * if you set your timezone to "Europe/London", then the tzname[]
     * maintained by tzset() typically returns { "GMT", "BST" }. The point of
     * this routine is to actually return "Europe/London" (or rather, the
     * corresponding Timezone).
     *
     * @return local timezone. If necessary, we will use a series of heuristics
     *         which end by returning UTC. We will never return NULL.
     */
    const Timezone *local();

    /**
     * Returns the given timezone.
     *
     * @param name Name of timezone. Empty is equivalent to UTC.
     * @return named timezone, NULL on error.
     */
    const Timezone *zone(const QString &name);

    typedef QMap<QString, Timezone *> ZoneMap;

    /**
     * Return timezone database.
     * @return known timezones.
     */
    const ZoneMap allZones();

    /**
     * Add user-defined timezone to database.
     */
    void add(Timezone *zone);

private:
    float convertCoordinate(const QString &coordinate);

    QString m_zoneinfoDir;
    ZoneMap *m_zones;
    Timezone *m_UTC;
    TimezonesPrivate *d;
};

}

#endif
