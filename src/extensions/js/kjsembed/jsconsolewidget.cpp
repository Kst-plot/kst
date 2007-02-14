/*
 *  Copyright (C) 2001-2003, Richard J. Moore <rich@kde.org>
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

#include <qfile.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kprocess.h>
#include <ktextedit.h>
#include <kwin.h>

#include <kjs/interpreter.h>
#include <kjs/ustring.h>
#include <kjs/types.h>

#include "jsbinding.h"
#include "kjsembedpart.h"

#include "jsconsolewidget.h"
#include "jsconsolewidget.moc"

namespace KJSEmbed {

class JSConsoleWidgetPrivate
{
};

JSConsoleWidget::JSConsoleWidget( KJSEmbedPart *jspart, QWidget *parent, const char *name )
    : QFrame( parent, name ? name : "jsconsole_widget" ),
      js(jspart), proc(0), d(0)
{
    setFocusPolicy( QWidget::StrongFocus );
    createView();
}

JSConsoleWidget::~JSConsoleWidget()
{
}

void JSConsoleWidget::createView()
{
    QPixmap px( KGlobal::iconLoader()->loadIcon("konsole", KIcon::NoGroup, KIcon::SizeSmall) );
    QPixmap pxl( KGlobal::iconLoader()->loadIcon("konsole", KIcon::NoGroup, KIcon::SizeLarge) );
    setIcon( px );
    KWin::setIcons( winId(), pxl, px );

    ttl = new KPopupTitle( this, "title" );
    ttl->setText( i18n("JavaScript Console") );
    ttl->setIcon( px );

    log = new KTextEdit( this, "log_widget" );
    log->setReadOnly( true );
    log->setUndoRedoEnabled( false );
    log->setTextFormat( Qt::RichText );
    log->setWrapPolicy( QTextEdit::Anywhere );
    log->setText( "<qt><pre>" );
    log->setFocusPolicy( QWidget::NoFocus );

    // Command entry section
    cmdBox = new QHBox( this, "cmd_box" );
    cmdBox->setSpacing( KDialog::spacingHint() );

    QLabel *prompt = new QLabel( i18n("&KJS>"), cmdBox, "prompt" );
    cmd = new KLineEdit( cmdBox, "cmd_edit" );
    cmd->setFocusPolicy( QWidget::StrongFocus );
    cmd->setFocus();
    prompt->setBuddy( cmd );

    go = new QPushButton( i18n("&Run"), cmdBox, "run_button" );
    go->setFixedSize( go->sizeHint() );

    connect( cmd, SIGNAL(returnPressed(const QString&)), go, SLOT( animateClick() ) );
    connect( go, SIGNAL( clicked() ), SLOT( invoke() ) );

    // Setup completion
    KCompletion *comp = cmd->completionObject();
    connect( cmd, SIGNAL(returnPressed(const QString&)), comp, SLOT(addItem(const QString&)) );

    // Layout
    QVBoxLayout *vert = new QVBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );
    vert->addWidget( ttl );
    vert->addWidget( log );
    vert->addWidget( cmdBox );
}

void JSConsoleWidget::invoke()
{
    QString code( cmd->text() );
    println( QString( "<b><font color=\"#888888\">KJS&gt;</font> %1</b>" ).arg( code ) );
    execute( code );
}

bool JSConsoleWidget::execute( const QString &cmd )
{
    return execute( cmd, KJS::Null() );
}

bool JSConsoleWidget::execute( const QString &cmd, const KJS::Value &self )
{
    KJS::Completion jsres;
    bool ok = js->execute( jsres, cmd, self );

    // Executed ok
    if ( ok ) {

	// No return value
	if ( !jsres.isValueCompletion() )
	    return ok;

	// Return value
	KJS::Value ret = jsres.value();
	KJS::UString s = ret.toString( js->globalExec() );

	if ( s.isNull() ) {
	    warn( i18n("Success, but return value cannot be displayed") );
	    return ok;
	}

	QString txt = s.qstring();
	txt = txt.replace( QChar('\n'), "<br>" );
	println( txt );

	return ok;
    }

    // Handle errors
    KJS::ComplType ct = jsres.complType();
    if ( (ct == KJS::Throw) || (ct == KJS::Break) || ct == KJS::Continue ) {

	KJS::UString s = jsres.value().toString( js->globalExec() );
	if ( !s.isNull() )
	    warn( s.qstring() );
	else
	    warn( i18n("Unspecified error") );
    }
    else {
	kdDebug(80001) << "jsconsolewidget: Unknown completion error, " << ct << endl;
	warn( i18n("Unknown error returned, completion type %1").arg(ct) );
    }

    return ok;
}

void JSConsoleWidget::println( const QString &msg )
{
    log->append( msg );
    log->scrollToBottom();
}

void JSConsoleWidget::warn( const QString &msg )
{
    QString err( "<font color=\"red\"><b>%1</b></font>" );
    println( err.arg(msg) );
}

//
// Process Handling
//

bool JSConsoleWidget::run( const QString &cmd )
{
    kdDebug(80001) << "JSConsoleWidget::run(" << cmd << ")" << endl;

    if ( proc )
	return false;

    proc = new KShellProcess("/bin/sh");
    *proc << cmd;

    connect( proc, SIGNAL( processExited(KProcess *) ), SLOT( childExited() ) );
    connect( proc, SIGNAL( receivedStdout(KProcess *, char *, int) ),
	     this, SLOT( receivedStdOutput(KProcess *, char *, int) ) );
    connect( proc, SIGNAL( receivedStderr(KProcess *, char *, int) ),
	     this, SLOT( receivedStdError(KProcess *, char *, int) ) );

    return proc->start( KProcess::NotifyOnExit,
			KProcess::Communication( KProcess::Stdout|KProcess::Stderr ));
}


void JSConsoleWidget::childExited()
{
    QString s;
    if ( proc->normalExit() ) {
	if ( proc->exitStatus() )
	    s = i18n( "<b>[Exited with status %1]</b>\n" ).arg( proc->exitStatus() );
	else
	    s = i18n( "<b>[Finished]</b>\n" );
	println( s );
    }
    else {
        s = i18n("[Aborted]\n");
	warn( s );
    }

    delete proc;
    proc = 0;
}

void JSConsoleWidget::receivedStdOutput( KProcess *, char *buffer, int buflen )
{
    QCString buf = QCString( buffer, buflen+1 );
    println( QString(buf) );
}

void JSConsoleWidget::receivedStdError( KProcess *, char *buffer, int buflen )
{
    QCString buf = QCString( buffer, buflen+1 );
    warn( QString(buf) );
}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
