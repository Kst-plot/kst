// -*- c++ -*-

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

#ifndef KJSEMBEDJSCONSOLEWIDGET_H
#define KJSEMBEDJSCONSOLEWIDGET_H

#include <qframe.h>

class QPushButton;
class QHBox;

class KLineEdit;
class KPopupTitle;
class KProcess;
class KShellProcess;
class KTextEdit;

namespace KJS {
    class Value;
}

namespace KJSEmbed {

class KJSEmbedPart;

/**
 * A QWidget that provides a console for executing Javascript commands. Creating
 * a JS console is easy, as you can see below:
 *
 * <pre>
 *   KJS::Object global( new KJS::ObjectImp() );
 *   KJS::Interpreter *js = new KJS::Interpreter( global );
 *   KJSEmbed::JSConsoleWidget *win = new KJSEmbed::JSConsoleWidget( js );
 *   win->addBindings( js->globalExec(), global );
 * </pre>
 *
 * This example creates a console for a JS interpreter and adds a print function
 * to the interpreter.
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 */
class JSConsoleWidget : public QFrame
{
    Q_OBJECT

public:
    JSConsoleWidget( KJSEmbedPart *js, QWidget *parent=0, const char *name=0 );
    virtual ~JSConsoleWidget();

public slots:
    /** Returns the KJSEmbedPart the console is using. */
    KJSEmbed::KJSEmbedPart *jscript() const { return js; }

    /** Returns the message widget. */
    KTextEdit *messages() const { return log; }

    /** Returns the title widget. */
    KPopupTitle *title() const { return ttl; }

    /**
     * Returns the QHBox used to layout the entry part of the console. This
     * can be used by clients to hide and show the interactive parts of the
     * console, or to add new buttons etc.
     */
    QHBox *commandBox() const { return cmdBox; }

    /** Invokes the content of the command entry field. */
    void invoke();

    /** Invokes the specified command string. */
    virtual bool execute( const QString &cmd );

    bool execute( const QString &cmd, const KJS::Value &self );

    /** Prints the specified string to the console. */
    virtual void println( const QString &text );

    /**
     * Prints the specified string to the console as a warning, the default
     * implementation prints the text in red.
     */
    virtual void warn( const QString &text );

    /**
     * Runs the specified command using KShellProcess. The output of the
     * command is displayed in the console. Any output sent to stdout is
     * displayed as normal text, anything sent to stderr is displayed as
     * a warning. Once the command has finished the exit status is printed
     * if it is non-zero.
     *
     * Note that there is no extra layer of security to prevent this method
     * being called from a script because in general a script that can
     * access this object can also write content to a file then execute the
     * file. If you intend to run untrusted scripts, it is your responsibility
     * to ensure that you only expose safe objects and methods to the
     * interpreter.
     */
    virtual bool run( const QString &shellCmd );

protected:
    /** Creates the console view. */
    void createView();

protected slots:
    /** Called when the process exits. */
    void childExited();

    /** Called when the process sends message to stdout. */
    void receivedStdOutput(KProcess *, char *, int);

    /** Called when the process sends message to stderr. */
    void receivedStdError(KProcess *, char *, int);

private:
    KJSEmbedPart *js;
    KShellProcess *proc;

    KTextEdit *log;
    QHBox *cmdBox;
    KLineEdit *cmd;
    QPushButton *go;
    KPopupTitle *ttl;

    class JSConsoleWidgetPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSCONSOLEWIDGET_H

// Local Variables:
// c-basic-offset: 4
// End:
