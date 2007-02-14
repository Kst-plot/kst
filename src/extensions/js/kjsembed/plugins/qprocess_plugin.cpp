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

#include <kdebug.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/kjsembedpart.h>
#include <kjsembed/customobject_imp.h>
#include <qvariant.h>


#include "qprocess_plugin.h"

namespace KJSEmbed {
namespace Bindings {

ProcessLoader::ProcessLoader( QObject *parent, const char *name, const QStringList &args ) :
	JSBindingPlugin(parent, name, args)
{
}

KJS::Object ProcessLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
{
	kdDebug() << "Loading a process object" << endl;

	QObject *parent = 0L;
	JSObjectProxy *proxy = JSProxy::toObjectProxy( args[0].imp() );
	if ( proxy )
		parent = proxy->object();
	ProcessImp *imp =  new ProcessImp(parent, "Process" );
	JSObjectProxy *prx = new JSObjectProxy( jspart, imp );

	KJS::Object proxyObj( prx );
	prx->addBindings( exec, proxyObj );
	CustomObjectImp::addBindings( exec, proxyObj );
	return proxyObj;
}

ProcessImp::ProcessImp(QObject *parent, const char *name )
    : QProcess(parent, name)
{
	kdDebug() << "New ProcessImp " << endl;
}

ProcessImp::~ProcessImp()
{
}

QStringList ProcessImp::arguments() const
{
	return QProcess::arguments();
}
void ProcessImp::setArguments( const QStringList args)
{
	QProcess::setArguments( args );
}

ProcessImp::Communication ProcessImp::communication() const
{
	return (ProcessImp::Communication)QProcess::communication();
}
void ProcessImp::setCommunication( Communication comm)
{
	QProcess::setCommunication( comm );
}

bool ProcessImp::canReadLineStdout() const
{
	return QProcess::canReadLineStdout();
}
bool ProcessImp::canReadLineStderr() const
{
	return QProcess::canReadLineStderr();
}
bool ProcessImp::isRunning() const
{
	return QProcess::isRunning();
}
bool ProcessImp::normalExit() const
{
	return QProcess::normalExit();
}
int ProcessImp::exitStatus() const
{
	return QProcess::exitStatus();
}

bool ProcessImp::start()
{
	return QProcess::start( );
}

QString ProcessImp::readLineStdout ()
{
	return QProcess::readLineStdout();
}

QString ProcessImp::readLineStderr ()
{
	return QProcess::readLineStderr();
}

QDir ProcessImp::workingDirectory () const
{
	return QProcess::workingDirectory();
}

void ProcessImp::setWorkingDirectory ( const QDir & dir )
{
	QProcess::setWorkingDirectory( dir );
}

void ProcessImp::addArgument ( const QString & arg )
{
	QProcess::addArgument( arg );
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed


#include <kgenericfactory.h>
typedef KGenericFactory<KJSEmbed::Bindings::ProcessLoader> ProcessLoaderFactory;
K_EXPORT_COMPONENT_FACTORY( libqprocessplugin, ProcessLoaderFactory( "ProcessLoader" ) )

#include "qprocess_plugin.moc"
