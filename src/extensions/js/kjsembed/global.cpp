/***************************************************************************
 *   Copyright (C) 2004 by Richard Moore                                   *
 *   rich@kde.org                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

# include "global.h"

#ifdef QT_ONLY
# include <qobject.h>
# include <cstdio>
# ifdef _WIN32
#  include <windows.h>
#  include <fcntl.h>
#  include <io.h>
#  include <iostream.h>
#  include <qfile.h>
# endif
#endif

static QTextStream *kjsembed_err = 0L;
static QTextStream *kjsembed_in = 0L;
static QTextStream *kjsembed_out = 0L;

#ifndef _WIN32
char *itoa(int num, char *str, int radix)
{
   int k;
   char c, flag, *ostr;

   if (num < 0) {
      num = -num;
      *str++ = '-';
   }
   k = 10000;
   ostr = str;
   flag = 0;
   while (k) {
      c = num / k;
      if (c || k == 1 || flag) {
         num %= k;
         c += '0';
         *str++ = c;
         flag = 1;
      }
      k /= 10;
   }
   *str = '\0';
   return ostr;
}

#endif

#ifdef _WIN32
static QFile win32_stdin;
static QFile win32_stdout;
static QFile win32_stderr;

static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole() {
   int hConHandle;
   long lStdHandle;
   CONSOLE_SCREEN_BUFFER_INFO coninfo;
   AllocConsole(); 
   GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
   coninfo.dwSize.Y = MAX_CONSOLE_LINES;
   SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
   
   lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
   hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
   win32_stdin.open(IO_ReadOnly,hConHandle);

   lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
   hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
   win32_stdout.open(IO_WriteOnly,hConHandle );

   lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
   hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
   win32_stderr.open(IO_WriteOnly,hConHandle);

   ios::sync_with_stdio();

}



#endif


QTextStream &consoleOut(  )
{
   return *KJSEmbed::conout();
}

QTextStream &consoleError( )
{
   return *KJSEmbed::conerr();
}

QTextStream &consoleIn( )
{
   return *KJSEmbed::conin();
}

#ifdef QT_ONLY
QTextStream &kdDebug( int area )
{
#ifndef QT_DEBUG
   return consoleError() << "DEBUG: (" << area << ") ";
#else
   return consoleOut();
#endif

}

QTextStream &kdWarning( int area )
{
   return consoleOut() << "WARNING: (" << area << ") ";
}

QString i18n( const char *string )
{
	return QObject::tr( string, "qjsembed string");
}

#endif

QTextStream *KJSEmbed::conin()
{
   if ( !kjsembed_in ) {
#ifdef _WIN32
	   kjsembed_in = new QTextStream( &win32_stdin );
#else
	   kjsembed_in = new QTextStream( stdin, IO_ReadOnly );
#endif
   }
   return kjsembed_in;
}

QTextStream *KJSEmbed::conout()
{
   if ( !kjsembed_out ) {
#ifdef _WIN32
	   kjsembed_out = new QTextStream( &win32_stdout  );
#else
	   kjsembed_out = new QTextStream( stdout, IO_WriteOnly );
#endif
   }
   return kjsembed_out;

}

QTextStream *KJSEmbed::conerr()
{
   if ( !kjsembed_err ) {
#ifdef _WIN32
	   kjsembed_err = new QTextStream( &win32_stderr  );
#else
	   kjsembed_err = new QTextStream( stderr, IO_WriteOnly );
#endif
   }
   return kjsembed_err;
}
