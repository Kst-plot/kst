/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
                  2002 Holger Freyther (freyther@kde.org)

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

//#define THREADDEBUG
//#define HAVE_BACKTRACE

#include "ksdebug.h"

#ifdef NDEBUG
#undef kstdDebug
#undef kstdBacktrace
#endif

#include <kapplication.h>
#include <kglobal.h>
#include <kinstance.h>
#include <kstandarddirs.h>

#include <klocale.h>
#include <qbrush.h>
#include <qdatetime.h>
#include <qfile.h>
#include <q3intdict.h>
#include <qmessagebox.h>
#include <qmutex.h>
#include <qpen.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qthread.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3MemArray>

#include <kurl.h>

#include <stdlib.h>	// abort
#include <unistd.h>	// getpid
#include <stdarg.h>	// vararg stuff
#include <ctype.h>      // isprint
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <kconfig.h>
#include "kstaticdeleter.h"
#include <config.h>

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif

class KstDebugEntry;

class KstDebugEntry
{
public:
    KstDebugEntry (int n, const Q3CString& d) {number=n; descr=d;}
    unsigned int number;
    Q3CString descr;
};

static Q3IntDict<KstDebugEntry> *KstDebugCache;

static KStaticDeleter< Q3IntDict<KstDebugEntry> > kstdd;

static QMutex kstDebugMutex;

static Q3CString getDescrFromNum(unsigned int _num)
{
  QMutexLocker ml(&kstDebugMutex);
  if (!KstDebugCache) {
    kstdd.setObject(KstDebugCache, new Q3IntDict<KstDebugEntry>( 601 ));
    // Do not call this deleter from ~KApplication
    KGlobal::unregisterStaticDeleter(&kstdd);
    KstDebugCache->setAutoDelete(true);
  }

  KstDebugEntry *ent = KstDebugCache->find( _num );
  if ( ent )
    return ent->descr;

  if ( !KstDebugCache->isEmpty() ) // areas already loaded
    return Q3CString();

  QString filename(locate("config","kdebug.areas"));
  if (filename.isEmpty())
      return Q3CString();

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't open %s", filename.local8Bit().data());
    file.close();
    return Q3CString();
  }

  uint lineNumber=0;
  Q3CString line(1024);
  int len;

  while (( len = file.readLine(line.data(),line.size()-1) ) > 0) {
      int i=0;
      ++lineNumber;

      while (line[i] && line[i] <= ' ')
        i++;

      unsigned char ch=line[i];

      if ( !ch || ch =='#' || ch =='\n')
          continue; // We have an eof, a comment or an empty line

      if (ch < '0' && ch > '9') {
          qWarning("Syntax error: no number (line %u)",lineNumber);
          continue;
      }

      const int numStart=i;
      do {
          ch=line[++i];
      } while ( ch >= '0' && ch <= '9');

      const Q_ULONG number =line.mid(numStart,i).toULong();

      while (line[i] && line[i] <= ' ')
        i++;

      KstDebugCache->insert(number, new KstDebugEntry(number, line.mid(i, len-i-1)));
  }
  file.close();

  ent = KstDebugCache->find( _num );
  if ( ent )
      return ent->descr;

  return Q3CString();
}

enum DebugLevels {
    KDEBUG_INFO=    0,
    KDEBUG_WARN=    1,
    KDEBUG_ERROR=   2,
    KDEBUG_FATAL=   3
};


struct kstDebugPrivate {
  kstDebugPrivate() :
  	oldarea(0), config(0) { }

  ~kstDebugPrivate() { delete config; }

  Q3CString aAreaName;
  unsigned int oldarea;
  KConfig *config;
};

static kstDebugPrivate *kstDebug_data = 0;
static KStaticDeleter<kstDebugPrivate> pcd;

static void kstDebugBackend( unsigned short nLevel, unsigned int nArea, const char *data)
{
  QMutexLocker ml(&kstDebugMutex);
  if ( !kstDebug_data )
  {
      pcd.setObject(kstDebug_data, new kstDebugPrivate());
      // Do not call this deleter from ~KApplication
      KGlobal::unregisterStaticDeleter(&pcd);
  }

  if (!kstDebug_data->config && KGlobal::_instance )
  {
      kstDebug_data->config = new KConfig("kdebugrc", false, false);
      kstDebug_data->config->setGroup("0");

      //AB: this is necessary here, otherwise all output with area 0 won't be
      //prefixed with anything, unless something with area != 0 is called before
      if ( KGlobal::_instance )
        kstDebug_data->aAreaName = KGlobal::instance()->instanceName();
  }

  if (kstDebug_data->config && kstDebug_data->oldarea != nArea) {
    kstDebug_data->config->setGroup( QString::number(static_cast<int>(nArea)) );
    kstDebug_data->oldarea = nArea;
    if ( nArea > 0 && KGlobal::_instance )
      kstDebug_data->aAreaName = getDescrFromNum(nArea);
    if ((nArea == 0) || kstDebug_data->aAreaName.isEmpty())
      if ( KGlobal::_instance )
        kstDebug_data->aAreaName = KGlobal::instance()->instanceName();
  }

  int nPriority = 0;
  QString aCaption;

    /* Determine output */

  QString key;
  switch( nLevel )
  {
  case KDEBUG_INFO:
      key = "InfoOutput";
      aCaption = "Info";
      nPriority = LOG_INFO;
      break;
  case KDEBUG_WARN:
      key = "WarnOutput";
      aCaption = "Warning";
      nPriority = LOG_WARNING;
	break;
  case KDEBUG_FATAL:
      key = "FatalOutput";
      aCaption = "Fatal Error";
      nPriority = LOG_CRIT;
      break;
  case KDEBUG_ERROR:
  default:
      /* Programmer error, use "Error" as default */
      key = "ErrorOutput";
      aCaption = "Error";
      nPriority = LOG_ERR;
      break;
  }

  short nOutput = kstDebug_data->config ? kstDebug_data->config->readNumEntry(key, 2) : 2;

  // If the application doesn't have a QApplication object it can't use
  // a messagebox.
  if (!kapp && (nOutput == 1))
    nOutput = 2;
  else if ( nOutput == 4 && nLevel != KDEBUG_FATAL )
      return;

  const int BUFSIZE = 4096;
  char buf[BUFSIZE];
  if ( !kstDebug_data->aAreaName.isEmpty() ) {
      strlcpy( buf, kstDebug_data->aAreaName.data(), BUFSIZE );
#ifdef THREADDEBUG
      char tid[22];
      snprintf(tid, 22, " %d", (int)QThread::currentThread());
      strlcat( buf, tid, BUFSIZE );
#endif
      strlcat( buf, ": ", BUFSIZE );
      strlcat( buf, data, BUFSIZE );
  }
  else
      strlcpy( buf, data, BUFSIZE );


  // Output
  switch( nOutput )
  {
  case 0: // File
  {
      const char* aKey;
      switch( nLevel )
      {
      case KDEBUG_INFO:
          aKey = "InfoFilename";
          break;
      case KDEBUG_WARN:
          aKey = "WarnFilename";
          break;
      case KDEBUG_FATAL:
          aKey = "FatalFilename";
          break;
      case KDEBUG_ERROR:
      default:
          aKey = "ErrorFilename";
          break;
      }
      QFile aOutputFile( kstDebug_data->config->readPathEntry(aKey, "kdebug.dbg") );
      aOutputFile.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered );
      aOutputFile.writeBlock( buf, strlen( buf ) );
      aOutputFile.close();
      break;
  }
  case 1: // Message Box
  {
      // Since we are in kdecore here, we cannot use KMsgBox and use
      // QMessageBox instead
      if ( !kstDebug_data->aAreaName.isEmpty() )
          aCaption += QString("(%1)").arg( kstDebug_data->aAreaName );
      QMessageBox::warning( 0L, aCaption, data, i18n("&OK") );
      break;
  }
  case 2: // Shell
  {
      write( 2, buf, strlen( buf ) );  //fputs( buf, stderr );
      break;
  }
  case 3: // syslog
  {
      syslog( nPriority, "%s", buf);
      break;
  }
  }

  // check if we should abort
  if( ( nLevel == KDEBUG_FATAL )
      && ( !kstDebug_data->config || kstDebug_data->config->readNumEntry( "AbortFatal", 1 ) ) )
        abort();
}

kstdbgstream &perror( kstdbgstream &s) { return s << QString::fromLocal8Bit(strerror(errno)); }
kstdbgstream kstdDebug(int area) { return kstdbgstream(area, KDEBUG_INFO); }
kstdbgstream kstdDebug(bool cond, int area) { if (cond) return kstdbgstream(area, KDEBUG_INFO); else return kstdbgstream(0, 0, false); }

kstdbgstream kstdError(int area) { return kstdbgstream("ERROR: ", area, KDEBUG_ERROR); }
kstdbgstream kstdError(bool cond, int area) { if (cond) return kstdbgstream("ERROR: ", area, KDEBUG_ERROR); else return kstdbgstream(0,0,false); }
kstdbgstream kstdWarning(int area) { return kstdbgstream("WARNING: ", area, KDEBUG_WARN); }
kstdbgstream kstdWarning(bool cond, int area) { if (cond) return kstdbgstream("WARNING: ", area, KDEBUG_WARN); else return kstdbgstream(0,0,false); }
kstdbgstream kstdFatal(int area) { return kstdbgstream("FATAL: ", area, KDEBUG_FATAL); }
kstdbgstream kstdFatal(bool cond, int area) { if (cond) return kstdbgstream("FATAL: ", area, KDEBUG_FATAL); else return kstdbgstream(0,0,false); }

kstdbgstream::kstdbgstream(kstdbgstream &str)
 : output(str.output), area(str.area), level(str.level), print(str.print) 
{ 
    str.output.truncate(0); 
}

void kstdbgstream::flush() {
    if (output.isEmpty() || !print)
	return;
    kstDebugBackend( level, area, output.local8Bit().data() );
    output = QString::null;
}

kstdbgstream &kstdbgstream::form(const char *format, ...)
{
    char buf[4096];
    va_list arguments;
    va_start( arguments, format );
    vsnprintf( buf, sizeof(buf), format, arguments );
    va_end(arguments);
    *this << buf;
    return *this;
}

kstdbgstream::~kstdbgstream() {
    if (!output.isEmpty()) {
	fprintf(stderr, "ASSERT: debug output not ended with \\n\n");
        fprintf(stderr, "%s", kstdBacktrace().latin1());
	*this << "\n";
    }
}

kstdbgstream& kstdbgstream::operator << (char ch)
{
  if (!print) return *this;
  if (!isprint(ch))
    output += "\\x" + QString::number( static_cast<uint>( ch ), 16 ).rightJustify(2, '0');
  else {
    output += ch;
    if (ch == '\n') flush();
  }
  return *this;
}

kstdbgstream& kstdbgstream::operator << (QChar ch)
{
  if (!print) return *this;
  if (!ch.isPrint())
    output += "\\x" + QString::number( ch.unicode(), 16 ).rightJustify(2, '0');
  else {
    output += ch;
    if (ch == '\n') flush();
  }
  return *this;
}

kstdbgstream& kstdbgstream::operator << (QWidget* widget)
{
    return *this << const_cast< const QWidget* >( widget );
}

kstdbgstream& kstdbgstream::operator << (const QWidget* widget)
{
  QString string, temp;
  // -----
  if(widget==0)
    {
      string=(QString)"[Null pointer]";
    } else {
      temp.setNum((ulong)widget, 16);
      string=(QString)"["+widget->className()+" pointer "
	+ "(0x" + temp + ")";
      if(widget->name(0)==0)
	{
	  string += " to unnamed widget, ";
	} else {
	  string += (QString)" to widget " + widget->name() + ", ";
	}
      string += "geometry="
	+ QString().setNum(widget->width())
	+ "x"+QString().setNum(widget->height())
	+ "+"+QString().setNum(widget->x())
	+ "+"+QString().setNum(widget->y())
	+ "]";
    }
  if (!print)
    {
      return *this;
    }
  output += string;
  if (output.at(output.length() -1 ) == '\n')
    {
      flush();
    }
  return *this;
}
/*
 * either use 'output' directly and do the flush if needed
 * or use the QString operator which calls the char* operator
 *
 */
kstdbgstream& kstdbgstream::operator<<( const QDateTime& time) {
    *this << time.toString();
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QDate& date) {
    *this << date.toString();

    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QTime& time ) {
    *this << time.toString();
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QPoint& p ) {
    *this << "(" << p.x() << ", " << p.y() << ")";
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QSize& s ) {
    *this << "[" << s.width() << "x" << s.height() << "]";
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QRect& r ) {
    *this << "[" << r.x() << "," << r.y() << " - " << r.width() << "x" << r.height() << "]";
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QRegion& reg ) {
    *this<< "[ ";

    Q3MemArray<QRect>rs=reg.rects();
    for (uint i=0;i<rs.size();++i)
        *this << QString("[%1,%2 - %3x%4] ").arg(rs[i].x()).arg(rs[i].y()).arg(rs[i].width()).arg(rs[i].height() ) ;

    *this <<"]";
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const KUrl& u ) {
    *this << u.prettyURL();
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QStringList& l ) {
    *this << "(";
    *this << l.join(",");
    *this << ")";

    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QColor& c ) {
    if ( c.isValid() )
        *this <<c.name();
    else
        *this << "(invalid/default)";
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QPen& p ) {
    static const char* const s_penStyles[] = {
        "NoPen", "SolidLine", "DashLine", "DotLine", "DashDotLine",
        "DashDotDotLine" };
    static const char* const s_capStyles[] = {
        "FlatCap", "SquareCap", "RoundCap" };
    *this << "[ style:";
    *this << s_penStyles[ p.style() ];
    *this << " width:";
    *this << p.width();
    *this << " color:";
    if ( p.color().isValid() )
        *this << p.color().name();
    else
        *this <<"(invalid/default)";
    if ( p.width() > 0 ) // cap style doesn't matter, otherwise
    {
        *this << " capstyle:";
        *this << s_capStyles[ p.capStyle() >> 4 ];
        // join style omitted
    }
    *this <<" ]";
    return *this;
}
kstdbgstream& kstdbgstream::operator<<( const QBrush& b) {
    static const char* const s_brushStyles[] = {
        "NoBrush", "SolidPattern", "Dense1Pattern", "Dense2Pattern", "Dense3Pattern",
        "Dense4Pattern", "Dense5Pattern", "Dense6Pattern", "Dense7Pattern",
        "HorPattern", "VerPattern", "CrossPattern", "BDiagPattern", "FDiagPattern",
        "DiagCrossPattern" };

    *this <<"[ style: ";
    *this <<s_brushStyles[ b.style() ];
    *this <<" color: ";
    // can't use operator<<(str, b.color()) because that terminates a kstdbgstream (flushes)
    if ( b.color().isValid() )
        *this <<b.color().name() ;
    else
        *this <<"(invalid/default)";
    if ( b.pixmap() )
        *this <<" has a pixmap";
    *this <<" ]";
    return *this;
}

kstdbgstream& kstdbgstream::operator<<( const QVariant& v) {
    *this << "[variant: ";
    *this << v.typeName();
    // For now we just attempt a conversion to string.
    // Feel free to switch(v.type()) and improve the output.
    *this << " toString=";
    *this << v.toString();
    *this << "]";
    return *this;
}

kstdbgstream& kstdbgstream::operator<<( const QByteArray& data) {
    if (!print) return *this;
    output += '[';
    unsigned int i = 0;
    unsigned int sz = kMin( data.size(), uint(64) );
    for ( ; i < sz ; ++i ) {
        output += QString::number( (unsigned char) data[i], 16 ).rightJustify(2, '0');
        if ( i < sz )
            output += ' ';
    }
    if ( sz < data.size() )
        output += "...";
    output += ']';
    return *this;
}

QString kstdBacktrace(int levels)
{
  QMutexLocker ml(&kstDebugMutex);
    QString s;
#ifdef HAVE_BACKTRACE
    void* trace[256];
    int n = backtrace(trace, 256);
    if (!n)
        return s;
    char** strings = backtrace_symbols (trace, n);

    if ( levels != -1 )
        n = kMin( n, levels );
    s = "[\n";

    for (int i = 0; i < n; ++i)
        s += QString::number(i) +
             QString::fromLatin1(": ") +
             QString::fromLatin1(strings[i]) + QString::fromLatin1("\n");
    s += "]\n";
    if (strings)
        free (strings);
#else
    Q_UNUSED(levels)
#endif
    return s;
}

QString kstdBacktrace()
{
    return kstdBacktrace(-1 /*all*/);
}

void kstdClearDebugConfig()
{
  QMutexLocker ml(&kstDebugMutex);
  delete kstDebug_data->config;
  kstDebug_data->config = 0;
}


// Needed for --enable-final
#ifdef NDEBUG
#define kstdDebug kndDebug
#endif
// vim: ts=2 sw=2 et
