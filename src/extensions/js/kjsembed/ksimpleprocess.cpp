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
#include "ksimpleprocess.h"
#include <qapplication.h>
#include <klocale.h>
static QStringList  splitArgList( const QString &args)
{
	QStringList returnArgs = QStringList::split( ' ', args);
	return returnArgs;
}

QString KSimpleProcess::exec( const QString &args, bool addStdErr )
{
	KSimpleProcess proc;
	return proc.execInternal( args, addStdErr);
}

QString KSimpleProcess::execInternal( const QString &args, bool addStdError)
{
	m_proc->setArguments( splitArgList( args ));
	connect(m_proc, SIGNAL(processExited()), this, SLOT(slotProcessExited()));
	connect(m_proc, SIGNAL(readyReadStdout()), this, SLOT(slotReceivedStdout()));
	if( addStdError )
		connect(m_proc, SIGNAL(readyReadStderr()), this, SLOT(slotReceivedStderr()));

	if ( !m_proc->start()  )
		return i18n("Could not run command '%1'.").arg( args.latin1() );
	enter_loop();
	return m_currBuffer;
}

KSimpleProcess::KSimpleProcess() : QObject(0,0)
{
	m_currBuffer = "";
	m_stdErrOn = false;
	m_proc = new QProcess(this);
}

KSimpleProcess::~KSimpleProcess()
{
	delete m_proc;
}


// If a troll sees this, he kills david not me
void qt_enter_modal( QWidget *widget );
void qt_leave_modal( QWidget *widget );

void KSimpleProcess::enter_loop()
{
  QWidget dummy(0,0,WType_Dialog | WShowModal);
  dummy.setFocusPolicy( QWidget::NoFocus );
  qt_enter_modal(&dummy);
  qApp->enter_loop();
  qt_leave_modal(&dummy);
}

void KSimpleProcess::slotProcessExited()
{
	while( m_proc->canReadLineStdout() )
		m_currBuffer += m_proc->readLineStdout() + '\n';
	qApp->exit_loop();
}

void KSimpleProcess::slotReceivedStdout()
{
	m_currBuffer += m_proc->readLineStdout() + '\n';
}

void KSimpleProcess::slotReceivedStderr()
{
	m_currBuffer += m_proc->readLineStderr() + '\n';
}

#include "ksimpleprocess.moc"
