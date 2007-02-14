// -*- c++ -*-

/*
 *  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
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

#ifndef KJSEMBED_QPROCESS_PLUGIN_H
#define KJSEMBED_QPROCESS_PLUGIN_H

#include <kjsembed/jsbindingplugin.h>
#include <kjsembed/jsproxy_imp.h>
#include <qprocess.h>
#include <qdir.h>

namespace KJSEmbed {
namespace Bindings {

class ProcessLoader : public JSBindingPlugin
{
public:
    ProcessLoader( QObject *parent, const char *name, const QStringList &args );
    virtual ~ProcessLoader(){;}
    KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
};


class ProcessImp : public QProcess {

Q_OBJECT

    /** Identifiers for the methods provided by this class. */
	enum Communication { Stdin = 0x01, Stdout = 0x02, Stderr = 0x04, DupStderr = 0x08 };

	Q_PROPERTY(QStringList arguments READ arguments WRITE setArguments )
	Q_PROPERTY(Communication communication READ communication WRITE setCommunication)
	Q_PROPERTY(bool canReadLineStdout READ canReadLineStdout )
	Q_PROPERTY(bool canReadLineStderr READ canReadLineStderr )
	Q_PROPERTY(bool isRunning READ isRunning )
	Q_PROPERTY(bool normalExit READ normalExit )
	Q_PROPERTY(int exitStatus READ exitStatus )
	Q_ENUMS(Communication)

public:
    ProcessImp( QObject *parent = 0, const char *name = 0);
    virtual ~ProcessImp();

    QStringList arguments() const;
    void setArguments( const QStringList args);

    Communication communication() const;
    void setCommunication( Communication comm);

    bool canReadLineStdout() const;
    bool canReadLineStderr() const;
    bool isRunning() const;
    bool normalExit() const;
    int exitStatus() const;

public slots:
	bool start();
	virtual QString readLineStdout ();
	virtual QString readLineStderr ();
	QDir workingDirectory () const;
	void setWorkingDirectory ( const QDir & dir );
	void addArgument ( const QString & arg );
};

} // namespace
} // namespace

#endif // KJSEMBED_QPROCESS_PLUGIN_H
