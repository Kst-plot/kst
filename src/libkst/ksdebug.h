/* This file is part of the KDE libraries
    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
                  2000-2002 Stephan Kulow (coolo@kde.org)
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

#ifndef _KSDEBUG_H_
#define _KSDEBUG_H_

#include <qstring.h>
#include "kst_export.h"

class QWidget;
class QDateTime;
class QDate;
class QTime;
class QPoint;
class QSize;
class QRect;
class QRegion;
class KURL;
class QStringList;
class QColor;
class QPen;
class QBrush;
class QVariant;
template <class T>
class QValueList;

class kstdbgstream;
class kstndbgstream;

/**
 * \addtogroup kdebug Debug message generators
 *  @{
 * KDE debug message streams let you and the user control just how many debug
 * messages you see.
 */

typedef kstdbgstream & (*KstDBGFUNC)(kstdbgstream &); // manipulator function
typedef kstndbgstream & (*KstNDBGFUNC)(kstndbgstream &); // manipulator function

#ifdef k_funcinfo
#undef k_funcinfo
#endif

#ifdef k_lineinfo
#undef k_lineinfo
#endif

#ifdef __GNUC__
#define k_funcinfo "[" << __PRETTY_FUNCTION__ << "] "
#else
#define k_funcinfo "[" << __FILE__ << ":" << __LINE__ << "] "
#endif

#define k_lineinfo "[" << __FILE__ << ":" << __LINE__ << "] "

class kstdbgstreamprivate;
/**
 * kstdbgstream is a text stream that allows you to print debug messages.
 * Using the overloaded "<<" operator you can send messages. Usually
 * you do not create the kstdbgstream yourself, but use kstdDebug()
 * kdWarning(), kdError() or kdFatal to obtain one.
 *
 * Example:
 * \code
 *    int i = 5;
 *    kstdDebug() << "The value of i is " << i << endl;
 * \endcode
 * @see kstndbgstream
 */
class KST_EXPORT kstdbgstream {
 public:
  /**
   * @internal
   */
    kstdbgstream(unsigned int _area, unsigned int _level, bool _print = true) :
      area(_area), level(_level),  print(_print) { }
    kstdbgstream(const char * initialString, unsigned int _area, unsigned int _level, bool _print = true) :
      output(QString::fromLatin1(initialString)), area(_area), level(_level),  print(_print) { }
    /// Copy constructor
    kstdbgstream(kstdbgstream &str);
    kstdbgstream(const kstdbgstream &str) :
      output(str.output), area(str.area), level(str.level), print(str.print) {}
    ~kstdbgstream();
    /**
     * Prints the given value.
     * @param i the boolean to print (as "true" or "false")
     * @return this stream
     */
    kstdbgstream &operator<<(bool i)  {
	if (!print) return *this;
	output += QString::fromLatin1(i ? "true" : "false");
	return *this;
    }
    /**
     * Prints the given value.
     * @param i the short to print
     * @return this stream
     */
    kstdbgstream &operator<<(short i)  {
	if (!print) return *this;
	QString tmp; tmp.setNum(i); output += tmp;
	return *this;
    }
    /**
     * Prints the given value.
     * @param i the unsigned short to print
     * @return this stream
     */
    kstdbgstream &operator<<(unsigned short i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    /**
     * Prints the given value.
     * @param ch the char to print
     * @return this stream
     */
    kstdbgstream &operator<<(char ch);
    /**
     * Prints the given value.
     * @param ch the unsigned char to print
     * @return this stream
     */
    kstdbgstream &operator<<(unsigned char ch) {
        return operator<<( static_cast<char>( ch ) );
    }
    /**
     * Prints the given value.
     * @param i the int to print
     * @return this stream
     */
    kstdbgstream &operator<<(int i)  {
	if (!print) return *this;
	QString tmp; tmp.setNum(i); output += tmp;
	return *this;
    }
    /**
     * Prints the given value.
     * @param i the unsigned int to print
     * @return this stream
     */
    kstdbgstream &operator<<(unsigned int i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    /**
     * Prints the given value.
     * @param i the long to print
     * @return this stream
     */
    kstdbgstream &operator<<(long i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    /**
     * Prints the given value.
     * @param i the unsigned long to print
     * @return this stream
     */
    kstdbgstream &operator<<(unsigned long i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    /**
     * Prints the given value.
     * @param i the long long to print
     * @return this stream
     */
    kstdbgstream &operator<<(Q_LLONG i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    /**
     * Prints the given value.
     * @param i the unsigned long long to print
     * @return this stream
     */
    kstdbgstream &operator<<(Q_ULLONG i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }

    /**
     * Flushes the output.
     */
    void flush(); //AB: maybe this should be virtual! would save some trouble for some 3rd party projects

    /**
     * Prints the given value.
     * @param ch the char to print
     * @return this stream
     * @since 3.3
     */
    kstdbgstream &operator<<(QChar ch);
    /**
     * Prints the given value.
     * @param string the string to print
     * @return this stream
     */
    kstdbgstream &operator<<(const QString& string) {
	if (!print) return *this;
	output += string;
	if (output.at(output.length() -1 ) == '\n')
	    flush();
	return *this;
    }
    /**
     * Prints the given value.
     * @param string the string to print
     * @return this stream
     */
    kstdbgstream &operator<<(const char *string) {
	if (!print) return *this;
	output += QString::fromUtf8(string);
	if (output.at(output.length() - 1) == '\n')
	    flush();
	return *this;
    }
    /**
     * Prints the given value.
     * @param string the string to print
     * @return this stream
     */
    kstdbgstream &operator<<(const QCString& string) {
        *this << string.data();
        return *this;
    }
    /**
     * Prints the given value.
     * @param p a pointer to print (in number form)
     * @return this stream
     */
    kstdbgstream& operator<<(const void * p) {
        form("%p", p);
        return *this;
    }
    /**
     * Invokes the given function.
     * @param f the function to invoke
     * @return the return value of @p f
     */
    kstdbgstream& operator<<(KstDBGFUNC f) {
	if (!print) return *this;
	return (*f)(*this);
    }
    /**
     * Prints the given value.
     * @param d the double to print
     * @return this stream
     */
    kstdbgstream& operator<<(double d) {
      QString tmp; tmp.setNum(d); output += tmp;
      return *this;
    }
    /**
     * Prints the string @p format which can contain
     * printf-style formatted values.
     * @param format the printf-style format
     * @return this stream
     */
    kstdbgstream &form(const char *format, ...)
#ifdef __GNUC__
      __attribute__ ( ( format ( printf, 2, 3 ) ) )
#endif
     ;

    /** Operator to print out basic information about a QWidget.
     *  Output of class names only works if the class is moc'ified.
     * @param widget the widget to print
     * @return this stream
     */
    kstdbgstream& operator << (const QWidget* widget);
    kstdbgstream& operator << (QWidget* widget); // KDE4 merge

    /**
     * Prints the given value.
     * @param dateTime the datetime to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QDateTime& dateTime );

    /**
     * Prints the given value.
     * @param date the date to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QDate& date );

    /**
     * Prints the given value.
     * @param time the time to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QTime& time );

    /**
     * Prints the given value.
     * @param point the point to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QPoint& point );

    /**
     * Prints the given value.
     * @param size the QSize to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QSize& size );

    /**
     * Prints the given value.
     * @param rect the QRect to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QRect& rect);

    /**
     * Prints the given value.
     * @param region the QRegion to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QRegion& region);

    /**
     * Prints the given value.
     * @param url the url to print
     * @return this stream
     */
    kstdbgstream& operator << ( const KURL& url );

    /**
     * Prints the given value.
     * @param list the stringlist to print
     * @return this stream
     */
    // ### KDE4: Remove in favor of template operator for QValueList<T> below
    kstdbgstream& operator << ( const QStringList& list);

    /**
     * Prints the given value.
     * @param color the color to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QColor& color);

    /**
     * Prints the given value.
     * @param pen the pen to print
     * @return this stream
     * @since 3.2
     */
    kstdbgstream& operator << ( const QPen& pen );

    /**
     * Prints the given value.
     * @param brush the brush to print
     * @return this stream
     */
    kstdbgstream& operator << ( const QBrush& brush );

    /**
     * Prints the given value.
     * @param variant the variant to print
     * @return this stream
     * @since 3.3
     */
    kstdbgstream& operator << ( const QVariant& variant );

    /**
     * Prints the given value.
     * @param data the byte array to print
     * @return this stream
     * @since 3.3
     */
    kstdbgstream& operator << ( const QByteArray& data );

    /**
     * Prints the given value
     * @param list the list to print
     * @return this stream
     * @since 3.3
     */
    template <class T>
    kstdbgstream& operator << ( const QValueList<T> &list );

 private:
    QString output;
    unsigned int area, level;
    bool print;
    kstdbgstreamprivate* d;
};

template <class T>
kstdbgstream &kstdbgstream::operator<<( const QValueList<T> &list )
{
    *this << "(";
    typename QValueList<T>::ConstIterator it = list.begin();
    if ( !list.isEmpty() ) {
      *this << *it++;
    }
    for ( ; it != list.end(); ++it ) {
      *this << "," << *it;
    }
    *this << ")";
    return *this;
}

/**
 * \relates KGlobal
 * Prints an "\n".
 * @param s the debug stream to write to
 * @return the debug stream (@p s)
 */
inline kstdbgstream &endl( kstdbgstream &s) { s << "\n"; return s; }

/**
 * \relates KGlobal
 * Flushes the stream.
 * @param s the debug stream to write to
 * @return the debug stream (@p s)
 */
inline kstdbgstream &flush( kstdbgstream &s) { s.flush(); return s; }

KST_EXPORT kstdbgstream &perror( kstdbgstream &s);

/**
 * \relates KGlobal
 * kstndbgstream is a dummy variant of kstdbgstream. All functions do
 * nothing.
 * @see kndDebug()
 */
class KST_EXPORT kstndbgstream {
 public:
    /// Default constructor.
    kstndbgstream() {}
    ~kstndbgstream() {}
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(short int )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(unsigned short int )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(char )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(unsigned char )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(int )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(unsigned int )  { return *this; }
    /**
     * Does nothing.
     */
    void flush() {}
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(QChar)  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(const QString& ) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(const QCString& ) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &operator<<(const char *) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(const void *) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(void *) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(double) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(long) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(unsigned long) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(Q_LLONG) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(Q_ULLONG) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator<<(KstNDBGFUNC) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream& operator << (const QWidget*) { return *this; }
    kstndbgstream& operator << (QWidget*) { return *this; } // KDE4 merge
    /**
     * Does nothing.
     * @return this stream
     */
    kstndbgstream &form(const char *, ...) { return *this; }

    kstndbgstream& operator<<( const QDateTime& ) { return *this; }
    kstndbgstream& operator<<( const QDate&     ) { return *this; }
    kstndbgstream& operator<<( const QTime&     ) { return *this; }
    kstndbgstream& operator<<( const QPoint & )  { return *this; }
    kstndbgstream& operator<<( const QSize & )  { return *this; }
    kstndbgstream& operator<<( const QRect & )  { return *this; }
    kstndbgstream& operator<<( const QRegion & ) { return *this; }
    kstndbgstream& operator<<( const KURL & )  { return *this; }
    kstndbgstream& operator<<( const QStringList & ) { return *this; }
    kstndbgstream& operator<<( const QColor & ) { return *this; }
    kstndbgstream& operator<<( const QPen & ) { return *this; }
    kstndbgstream& operator<<( const QBrush & ) { return *this; }
    kstndbgstream& operator<<( const QVariant & ) { return *this; }
    kstndbgstream& operator<<( const QByteArray & ) { return *this; }

    template <class T>
    kstndbgstream& operator<<( const QValueList<T> & ) { return *this; }
};

/**
 * Does nothing.
 * @param s a stream
 * @return the given @p s
 */
inline kstndbgstream &endl( kstndbgstream & s) { return s; }
/**
 * Does nothing.
 * @param s a stream
 * @return the given @p s
 */
inline kstndbgstream &flush( kstndbgstream & s) { return s; }
inline kstndbgstream &perror( kstndbgstream & s) { return s; }

/**
 * \relates KGlobal
 * Returns a debug stream. You can use it to print debug
 * information.
 * @param area an id to identify the output, 0 for default
 * @see kndDebug()
 */
KST_EXPORT kstdbgstream kstdDebug(int area = 0);
KST_EXPORT kstdbgstream kstdDebug(bool cond, int area = 0);
/**
 * \relates KGlobal
 * Returns a backtrace.
 * @return a backtrace
 */
KST_EXPORT QString kstdBacktrace();
/**
 * \relates KGlobal
 * Returns a backtrace.
 * @param levels the number of levels of the backtrace
 * @return a backtrace
 * @since 3.1
 */
KST_EXPORT QString kstdBacktrace(int levels);
/**
 * Returns a dummy debug stream. The stream does not print anything.
 * @param area an id to identify the output, 0 for default
 * @see kstdDebug()
 */
inline kstndbgstream kstndDebug(int area = 0) { Q_UNUSED(area); return kstndbgstream(); }
inline kstndbgstream kstndDebug(bool , int  = 0) { return kstndbgstream(); }
inline QString kstndBacktrace() { return QString::null; }
inline QString kstndBacktrace(int) { return QString::null; }

/**
 * \relates KGlobal
 * Returns a warning stream. You can use it to print warning
 * information.
 * @param area an id to identify the output, 0 for default
 */
KST_EXPORT kstdbgstream kstdWarning(int area = 0);
KST_EXPORT kstdbgstream kstdWarning(bool cond, int area = 0);
/**
 * \relates KGlobal
 * Returns an error stream. You can use it to print error
 * information.
 * @param area an id to identify the output, 0 for default
 */
KST_EXPORT kstdbgstream kstdError(int area = 0);
KST_EXPORT kstdbgstream kstdError(bool cond, int area = 0);
/**
 * \relates KGlobal
 * Returns a fatal error stream. You can use it to print fatal error
 * information.
 * @param area an id to identify the output, 0 for default
 */
KST_EXPORT kstdbgstream kstdFatal(int area = 0);
KST_EXPORT kstdbgstream kstdFatal(bool cond, int area = 0);

/**
 * \relates KGlobal
 * Deletes the kdebugrc cache and therefore forces KDebug to reread the
 * config file
 */
KST_EXPORT void kstdClearDebugConfig();

/** @} */

#ifdef NDEBUG
#define kstdDebug kstndDebug
#define kstdBacktrace kstndBacktrace
#endif

#endif

