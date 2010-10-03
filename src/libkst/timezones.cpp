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

#include <config.h>

#include "timezones.h"

#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define UTC_ZONE "UTC"

#ifndef HAVE_UNSETENV
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifndef environ
extern char ** environ;
#endif

namespace Kst {

void unsetenv(const char *name)
{
  size_t len;
  char **ep;

  if (name == NULL || *name == '\0' || strchr (name, '=') != NULL) {
    errno = EINVAL;
    return;
  }

  len = strlen (name);
  ep = environ;
  while (*ep != NULL) {
    if (!strncmp (*ep, name, len) && (*ep)[len] == '=') {
      /* Found it.  Remove this pointer by moving later ones back.  */
      char **dp = ep;
      do {
        dp[0] = dp[1];
      } while (*dp++);
      /* Continue the loop in case NAME appears again.  */
    } else {
      ++ep;
    }
  }
}

#endif /* !HAVE_UNSETENV */

/**
 * Find out if the given standard (e.g. "GMT") and daylight savings time
 * (e.g. "BST", but which may be empty) abbreviated timezone names match
 * this timezone.
 *
 * Thus, this class can be used as a heuristic when trying to lookup the
 * real timezone from the abbreviated zone names.
 */
class AbbreviationsMatch :
    public TimezoneDetails
{
public:
    AbbreviationsMatch(const QString &stdZone, const QString &dstZone = "")
    {
        m_stdZone = stdZone;
        m_dstZone = dstZone;
    }

    void parseStarted()
    {
        m_foundStd = false;
        m_foundDst = m_dstZone.isEmpty();
    }

    bool test()
    {
        return (m_foundStd && m_foundDst);
    }

private:
    bool m_foundStd;
    bool m_foundDst;
    QString m_stdZone;
    QString m_dstZone;

    virtual void gotAbbreviation(int /*index*/, const QString &value)
    {
        if (m_stdZone == value)
        {
            m_foundStd = true;
        }
        if (m_dstZone == value)
        {
            m_foundDst = true;
        }
    }
};

/**
 * Internal dummy source for UTC timezone.
 */
class DummySource :
    public TimezoneSource
{
public:
    DummySource() :
        TimezoneSource("")
    {
    }

    virtual bool parse(const QString &/*zone*/, TimezoneDetails &/*dataReceiver*/) const
    {
        return true;
    }
};

/**
 * Find offset at a particular point in time.
 */
class OffsetFind :
    public TimezoneDetails
{
public:
    OffsetFind(unsigned dateTime)
    {
        m_dateTime = dateTime;
    }

    void parseStarted()
    {
        m_transitionTimeIndex = 0;
        m_localTimeIndex = -1;
        m_abbrIndex = -1;
        m_offset = 0;
        m_isDst = false;
        m_abbr = UTC_ZONE;
    }

    int offset()
    {
        return m_offset;
    }

    bool isDst()
    {
        return m_isDst;
    }

    QString abbreviation()
    {
        return m_abbr;
    }

private:
    unsigned m_dateTime;
    int m_transitionTimeIndex;
    int m_localTimeIndex;
    int m_abbrIndex;
    int m_offset;
    bool m_isDst;
    QString m_abbr;

    virtual void gotTransitionTime(int index, unsigned transitionTime)
    {
        if (transitionTime <= m_dateTime)
        {
            // Remember the index of the transition time that relates to dateTime.
            m_transitionTimeIndex = index;
        }
    }

    virtual void gotLocalTimeIndex(int index, unsigned localTimeIndex)
    {
        if (index == m_transitionTimeIndex)
        {
            // Remember the index of the local time that relates to dateTime.
            m_localTimeIndex = localTimeIndex;
        }
    }

    virtual void gotLocalTime(int index, int gmtOff, bool isDst, unsigned abbrInd)
    {
        if (index == m_localTimeIndex)
        {
            // Remember the results that relate to gmtOffset.
            m_offset = gmtOff;
            m_isDst = isDst;
            m_abbrIndex = abbrInd;
        }
    }

    virtual void gotAbbreviation(int index, const QString &value)
    {
        if (index == m_abbrIndex)
        {
            m_abbr = value;
        }
    }
};

const float Timezone::UNKNOWN = 1000.0;

bool Timezone::isValidLatitude(float latitude)
{
    return (latitude >= -90.0) && (latitude <= 90.0);
}

bool Timezone::isValidLongitude(float longitude)
{
    return (longitude >= -180.0) && (longitude <= 180.0);
}

Timezone::Timezone(
    KSharedPtr<TimezoneSource> db, const QString& name,
    const QString &countryCode, float latitude, float longitude,
    const QString &comment) :
    m_db(db),
    m_name(name),
    m_countryCode(countryCode),
    m_latitude(latitude),
    m_longitude(longitude),
    m_comment(comment),
    d(0)
{
    // Detect duff values.
    if (m_latitude * m_latitude > 90 * 90)
        m_latitude = UNKNOWN;
    if (m_longitude * m_longitude > 180 * 180)
        m_longitude = UNKNOWN;
}

Timezone::~Timezone()
{
    // FIXME when needed:
    // delete d;
}

QString Timezone::comment() const
{
    return m_comment;
}

QDateTime Timezone::convert(const Timezone *newZone, const QDateTime &dateTime) const
{
    char *originalZone = ::getenv("TZ");

    // Convert the given localtime to UTC.
    ::putenv(strdup(QString("TZ=:").append(m_name).toUtf8()));
    tzset();
    unsigned utc = dateTime.toTime_t();

    // Set the timezone and convert UTC to localtime.
    ::putenv(strdup(QString("TZ=:").append(newZone->name()).toUtf8()));
    tzset();
    QDateTime remoteTime;
    remoteTime.setTime_t(utc);
    remoteTime.setTimeSpec(Qt::LocalTime);

    // Now restore things
    if (!originalZone)
    {
        ::unsetenv("TZ");
    }
    else
    {
        ::putenv(strdup(QString("TZ=").append(originalZone).toUtf8()));
    }
    tzset();
    return remoteTime;
}

QString Timezone::countryCode() const
{
    return m_countryCode;
}

float Timezone::latitude() const
{
    return m_latitude;
}

float Timezone::longitude() const
{
    return m_longitude;
}

QString Timezone::name() const
{
    return m_name;
}

int Timezone::offset(Qt::TimeSpec basisSpec) const
{
    char *originalZone = ::getenv("TZ");

    // Get the time in the current timezone.
    QDateTime basisTime = QDateTime::currentDateTime().toTimeSpec(basisSpec);

    // Set the timezone and find out what time it is there compared to the basis.
    ::putenv(strdup(QString("TZ=:").append(m_name).toUtf8()));
    tzset();
    QDateTime remoteTime = QDateTime::currentDateTime().toLocalTime();
    int offset = remoteTime.secsTo(basisTime);

    // Now restore things
    if (!originalZone)
    {
        ::unsetenv("TZ");
    }
    else
    {
        ::putenv(strdup(QString("TZ=").append(originalZone).toUtf8()));
    }
    tzset();
    return offset;
}

int Timezone::offset(const QDateTime &dateTime) const
{
    OffsetFind finder(dateTime.toTime_t());
    int result = 0;
    if (parse(finder))
    {
        result = finder.offset();
    }
    return result;
}

bool Timezone::parse(TimezoneDetails &dataReceiver) const
{
    dataReceiver.parseStarted();
    bool result = m_db->parse(m_name, dataReceiver);
    dataReceiver.parseEnded();
    return result;
}

Timezones::Timezones() :
    m_zoneinfoDir(),
    m_zones(0),
    d(0)
{
    // Create the database (and resolve m_zoneinfoDir!).
    allZones();
    m_UTC = new Timezone(KSharedPtr<TimezoneSource>(new DummySource()), QString(UTC_ZONE));
    add(m_UTC);
}

Timezones::~Timezones()
{
    // FIXME when needed:
    // delete d;

    // Autodelete behavior.
    if (m_zones)
    {
        for (ZoneMap::ConstIterator it = m_zones->begin(); it != m_zones->end(); ++it)
        {
            delete it.value();
        }
    }
    delete m_zones;
}

void Timezones::add(Timezone *zone)
{
    m_zones->insert(zone->name(), zone);
}

const Timezones::ZoneMap Timezones::allZones()
{
    // Have we already done all the hard work? If not, create the cache.
    if (m_zones)
        return *m_zones;
    m_zones = new ZoneMap();

    // Go read the database.
    //
    // On Windows, HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Time Zones
    // is the place to look. The TZI binary value is the TIME_ZONE_INFORMATION structure.
    //
    // For Unix its all easy except knowing where to look. Try the LSB location first.
    QFile f;
    m_zoneinfoDir = "/usr/share/zoneinfo";
    f.setFileName(m_zoneinfoDir + "/zone.tab");
    if (!f.open(QIODevice::ReadOnly))
    {
        m_zoneinfoDir = "/usr/lib/zoneinfo";
        f.setFileName(m_zoneinfoDir + "/zone.tab");
        if (!f.open(QIODevice::ReadOnly))
        {
            m_zoneinfoDir = ::getenv("TZDIR");
            f.setFileName(m_zoneinfoDir + "/zone.tab");
            if (m_zoneinfoDir.isEmpty() || !f.open(QIODevice::ReadOnly))
            {
                // Solaris support. Synthesise something that looks like a zone.tab.
                //
                // /bin/grep -h ^Zone /usr/share/lib/zoneinfo/src/* | /bin/awk '{print "??\t+9999+99999\t" $2}'
                //
                // where the country code is set to "??" and the lattitude/longitude
                // values are dummies.
                m_zoneinfoDir = "/usr/share/lib/zoneinfo";
                K3TempFile temp;
                KShellProcess reader;
                reader << "/bin/grep" << "-h" << "^Zone" << m_zoneinfoDir << "/src/*" << temp.name() << "|" <<
                    "/bin/awk" << "'{print \"??\\t+9999+99999\\t\" $2}'";
                // Note the use of blocking here...it is a trivial amount of data!
                temp.close();
                reader.start(KProcess::Block);
                f.setFileName(temp.name());
                if (!temp.status() || !f.open(QIODevice::ReadOnly))
                {
                    return *m_zones;
                }
            }
        }
    }

    // Parse the zone.tab.
    QTextStream str(&f);
    QRegExp lineSeparator("[ \t]");
    QRegExp ordinateSeparator("[+-]");
    KSharedPtr<TimezoneSource> db(new TimezoneSource(m_zoneinfoDir));
    while (!str.atEnd())
    {
        QString line = str.readLine();
        if (line.isEmpty() || '#' == line[0])
            continue;
        QStringList tokens = KStringHandler::perlSplit(lineSeparator, line, 4);
        if (tokens.count() < 3)
        {
            continue;
        }

        // Got three tokens. Now check for two ordinates plus first one is "".
        QStringList ordinates = KStringHandler::perlSplit(ordinateSeparator, tokens[1], 2);
        if (ordinates.count() < 2)
        {
            continue;
        }

        float latitude = convertCoordinate(ordinates[1]);
        float longitude = convertCoordinate(ordinates[2]);

        // Add entry to list.
        if (tokens[0] == "??")
            tokens[0] = "";
        Timezone *timezone = new Timezone(db, tokens[2], tokens[0], latitude, longitude, tokens[3]);
        add(timezone);
    }
    f.close();
    return *m_zones;
}

/**
 * Convert sHHMM or sHHMMSS to a floating point number of degrees.
 */
float Timezones::convertCoordinate(const QString &coordinate)
{
    int value = coordinate.toInt();
    int degrees = 0;
    int minutes = 0;
    int seconds = 0;

    if (coordinate.length() > 11)
    {
        degrees = value / 10000;
        value -= degrees * 10000;
        minutes = value / 100;
        value -= minutes * 100;
        seconds = value;
    }
    else
    {
        degrees = value / 100;
        value -= degrees * 100;
        minutes = value;
    }
    value = degrees * 3600 + minutes * 60 + seconds;
    return value / 3600.0;
}

const Timezone *Timezones::local()
{
    const Timezone *local = 0;

    // First try the simplest solution of checking for well-formed TZ setting.
    char *envZone = ::getenv("TZ");
    if (envZone)
    {
        if (envZone[0] == '\0')
        {
            return m_UTC;
        }
        else
        if (envZone[0] == ':')
        {
            envZone++;
        }
        local = zone(envZone);
    }
    if (local)
        return local;

    // Try to match /etc/localtime against the list of zoneinfo files.
    QFile f;
    f.setFileName("/etc/localtime");
    if (f.open(QIODevice::ReadOnly))
    {
        // Compute the MD5 sum of /etc/localtime.
        KMD5 context("");
        context.reset();
        context.update(f);
        qlonglong referenceSize = f.size();
        QString referenceMd5Sum = context.hexDigest();
        f.close();
        if (!m_zoneinfoDir.isEmpty())
        {
            // Compare it with each zoneinfo file.
            for (ZoneMap::Iterator it = m_zones->begin(); it != m_zones->end(); ++it)
            {
                Timezone *zone = it.value();
                f.setFileName(m_zoneinfoDir + '/' + zone->name());
                if (f.open(QIODevice::ReadOnly))
                {
                    qlonglong candidateSize = f.size();
                    QString candidateMd5Sum;
                    if (candidateSize == referenceSize)
                    {
                        // Only do the heavy lifting for file sizes which match.
                        context.reset();
                        context.update(f);
                        candidateMd5Sum = context.hexDigest();
                    }
                    f.close();
                    if (candidateMd5Sum == referenceMd5Sum)
                    {
                        local = zone;
                        break;
                    }
                }
            }
        }
    }
    if (local)
        return local;

    // BSD support.
    QString fileZone;
    f.setFileName("/etc/timezone");
    if (!f.open(QIODevice::ReadOnly))
    {
        // Solaris support using /etc/default/init.
        f.setFileName("/etc/default/init");
        if (f.open(QIODevice::ReadOnly))
        {
            QTextStream ts(&f);
            ts.setCodec("latin1");

            // Read the last line starting "TZ=".
            while (!ts.atEnd())
            {
                fileZone = ts.readLine();
                if (fileZone.startsWith("TZ="))
                {
                    fileZone = fileZone.mid(3);

                    local = zone(fileZone);
                }
            }
            f.close();
        }
    }
    else
    {
        QTextStream ts(&f);
        ts.setCodec("latin1");

        // Read the first line.
        if (!ts.atEnd())
        {
            fileZone = ts.readLine();

            local = zone(fileZone);
        }
        f.close();
    }
    if (local)
        return local;

    // None of the deterministic stuff above has worked: try a heuristic. We
    // try to find a pair of matching timezone abbreviations...that way, we'll
    // likely return a value in the user's own country.
    if (!m_zoneinfoDir.isEmpty())
    {
        tzset();
        AbbreviationsMatch matcher(tzname[0], tzname[1]);
        int bestOffset = INT_MAX;
        for (ZoneMap::Iterator it = m_zones->begin(); it != m_zones->end(); ++it)
        {
            Timezone *zone = it.value();
            int candidateOffset = qAbs(zone->offset(Qt::LocalTime));
            if (zone->parse(matcher) && matcher.test() && (candidateOffset < bestOffset))
            {
                bestOffset = candidateOffset;
                local = zone;
            }
        }
    }
    if (local)
        return local;
    return m_UTC;
}

const Timezone *Timezones::zone(const QString &name)
{
    if (name.isEmpty())
        return m_UTC;
    ZoneMap::ConstIterator it = m_zones->find(name);
    if (it != m_zones->end())
        return it.value();

    // Error.
    return 0;
}

TimezoneDetails::TimezoneDetails()
{
}

TimezoneDetails::~TimezoneDetails()
{
}

void TimezoneDetails::gotAbbreviation(int /*index*/, const QString &)
{}

void TimezoneDetails::gotHeader(
        unsigned, unsigned, unsigned,
        unsigned, unsigned, unsigned)
{}

void TimezoneDetails::gotLeapAdjustment(int /*index*/, unsigned, unsigned)
{}

void TimezoneDetails::gotLocalTime(int /*index*/, int, bool, unsigned)
{}

void TimezoneDetails::gotLocalTimeIndex(int /*index*/, unsigned)
{}

void TimezoneDetails::gotIsStandard(int /*index*/, bool)
{}

void TimezoneDetails::gotTransitionTime(int /*index*/, unsigned)
{}

void TimezoneDetails::gotIsUTC(int /*index*/, bool)
{}

void TimezoneDetails::parseEnded()
{}

void TimezoneDetails::parseStarted()
{}

TimezoneSource::TimezoneSource(const QString &db) :
    m_db(db)
{
}

TimezoneSource::~TimezoneSource()
{
}

QString TimezoneSource::db()
{
    return m_db;
}

bool TimezoneSource::parse(const QString &zone, TimezoneDetails &dataReceiver) const
{
    QFile f(m_db + '/' + zone);
    if (!f.open(QIODevice::ReadOnly))
    {
        return false;
    }

    // Structures that represent the zoneinfo file.
    quint8 T, z, i_, f_;
    struct
    {
        quint32 ttisgmtcnt;
        quint32 ttisstdcnt;
        quint32 leapcnt;
        quint32 timecnt;
        quint32 typecnt;
        quint32 charcnt;
    } tzh;
    quint32 transitionTime;
    quint8 localTimeIndex;
    struct
    {
        qint32 gmtoff;
        qint8 isdst;
        quint8 abbrind;
    } tt;
    quint32 leapTime;
    quint32 leapSeconds;
    quint8 isStandard;
    quint8 isUTC;

    QDataStream str(&f);
    str >> T >> z >> i_ >> f_;
    unsigned i;
    for (i = 0; i < 4; i++)
        str >> tzh.ttisgmtcnt;
    str >> tzh.ttisgmtcnt >> tzh.ttisstdcnt >> tzh.leapcnt >> tzh.timecnt >> tzh.typecnt >> tzh.charcnt;
    dataReceiver.gotHeader(tzh.ttisgmtcnt, tzh.ttisstdcnt, tzh.leapcnt, tzh.timecnt, tzh.typecnt, tzh.charcnt);
    for (i = 0; i < tzh.timecnt; i++)
    {
        str >> transitionTime;
        dataReceiver.gotTransitionTime(i, transitionTime);
    }
    for (i = 0; i < tzh.timecnt; i++)
    {
        // NB: these appear to be 1-based, not zero-based!
        str >> localTimeIndex;
        dataReceiver.gotLocalTimeIndex(i, localTimeIndex);
    }
    for (i = 0; i < tzh.typecnt; i++)
    {
        str >> tt.gmtoff >> tt.isdst >> tt.abbrind;
        dataReceiver.gotLocalTime(i, tt.gmtoff, (tt.isdst != 0), tt.abbrind);
    }

    // Make sure we don't run foul of maliciously coded timezone abbreviations.
    if (tzh.charcnt > 64)
    {
        return false;
    }
    QByteArray array(tzh.charcnt, '\0');
    str.readRawData(array.data(), array.size());
    char *abbrs = array.data();
    if (abbrs[tzh.charcnt - 1] != 0)
    {
        // These abbrevations are corrupt!
        return false;
    }
    char *abbr = abbrs;
    while (abbr < abbrs + tzh.charcnt)
    {
        dataReceiver.gotAbbreviation((abbr - abbrs), abbr);
        abbr += strlen(abbr) + 1;
    }
    for (i = 0; i < tzh.leapcnt; i++)
    {
        str >> leapTime >> leapSeconds;
        dataReceiver.gotLeapAdjustment(i, leapTime, leapSeconds);
    }
    for (i = 0; i < tzh.ttisstdcnt; i++)
    {
        str >> isStandard;
        dataReceiver.gotIsStandard(i, (isStandard != 0));
    }
    for (i = 0; i < tzh.ttisgmtcnt; i++)
    {
        str >> isUTC;
        dataReceiver.gotIsUTC(i, (isUTC != 0));
    }
    return true;
}

}

