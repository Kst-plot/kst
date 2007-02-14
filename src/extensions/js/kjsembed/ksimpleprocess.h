// -*- c++ -*-

/*
 *  Copyright (C) 2004, Ian Reinhart Geiser <geiseri@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KSIMPLE_PROCESS_H
#define KSIMPLE_PROCESS_H

#include <qobject.h>
#include <qprocess.h>

/**
 * Very simple class that allows one to run a command and get the
 * standard output back as a string value.
 *
 * @author Ian Reinhart Geiser, geiseri@kde.org
 */
class KSimpleProcess : public QObject
{
    Q_OBJECT

public:
    /**
     * Executes a string as if it where at the current shell.  This will block until
     * execution is finished.  Once its finished the standard output will be returned.
     * if the addStdErr is true, the standard error will be combined with the standard
     * in value.
     * Example:
     *\code
     * QString myProc = KSimpleProcess::exec( "ps -uh" );
     * kdDebug() << "Running Processes\n" << myProc << endl;
     *\endcode
     */
    static QString exec( const QString &args, bool addStdErr = false );

private:
    KSimpleProcess();
    ~KSimpleProcess();
    void enter_loop();
    QString execInternal( const QString &args, bool addStdError);
    
    QString m_currBuffer;
    bool m_stdErrOn;
    QProcess *m_proc;

public slots:
    void slotProcessExited();
    void slotReceivedStdout();
    void slotReceivedStderr();
    
    friend class Im_lonely;  // yet another reason why c++ is braindead...
};

#endif // KSIMPLE_PROCESS_H
