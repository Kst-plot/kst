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

#include <kjsembed/global.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include "sql_imp.h"



namespace KJSEmbed {
namespace Bindings {

SqlDatabase::SqlDatabase( QObject *parent, const char *name): BindingObject(parent, name)
{
	connectionName = "defaultConnection";
	setJSClassName( "SqlDatabase" );
}
SqlDatabase::~SqlDatabase()
{
	QSqlDatabase::removeDatabase(connectionName);
}
bool SqlDatabase::addDatabase ( const QString &type, const QString &conn )
{
	connectionName = conn;
	QSqlDatabase *db = QSqlDatabase::addDatabase(type, connectionName);
	if ( !db )
		return false;
	return true;
}
QStringList SqlDatabase::drivers ()
{
	return QSqlDatabase::drivers();
}
bool SqlDatabase::open ( )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->open();
	return false;
}
bool SqlDatabase::open ( const QString &user, const QString &password )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->open(user, password);
	return false;
}
void SqlDatabase::close ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
	{
		db->close();
	}
}
bool SqlDatabase::isOpen ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->isOpen();
	return false;
}
bool SqlDatabase::isOpenError ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->isOpenError();
	return true;
}
QStringList SqlDatabase::tables ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->tables();
	return QStringList();
}
SqlQuery *SqlDatabase::exec (const QString &query  )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
	{
		kdDebug() <<" exec query " << query << endl;
		SqlQuery *qw = new SqlQuery(this, "query",db->exec( query ));
		//JSFactory::instance()->addType( "SqlQuery" );
		kdDebug() <<" size " << qw->size() << endl;
		kdDebug() <<" valid " << qw->isValid() << endl;
		return qw;
	}
	return 0L;
}
QString  SqlDatabase::lastError () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
	{
		return db->lastError().text();
	}
	return "No Database Driver Loaded";
}
bool SqlDatabase::transaction ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->transaction();
	return false;
}
bool SqlDatabase::commit ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->commit();
	return false;
}
bool SqlDatabase::rollback ()
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->rollback();
	return false;
}
void SqlDatabase::setDatabaseName (const QString &name )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		db->setDatabaseName(name);
}
void SqlDatabase::setUserName (const QString &name )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		db->setUserName(name);
}
void SqlDatabase::setPassword (const QString &password )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		db->setPassword(password);
}
void SqlDatabase::setHostName (const QString &host )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		db->setHostName(host);
}
void SqlDatabase::setPort ( int p )
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		db->setPort(p);
}
QString SqlDatabase::databaseName () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->databaseName();
	return "";
}
QString SqlDatabase::userName () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->userName();
	return "";
}
QString SqlDatabase::password () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->password();
	return "";
}
QString SqlDatabase::hostName () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->hostName();
	return "";
}
QString SqlDatabase::driverName () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->driverName();
	return "";
}
int SqlDatabase::port () const
{
	QSqlDatabase *db = QSqlDatabase::database(connectionName,false);
	if ( db )
		return db->port();
	return 0;
}

SqlQuery::SqlQuery( QObject *parent, const char *name ): BindingObject(parent, name)
{
	kdDebug() << "New null SQL Query" << endl;
	m_query = QSqlQuery();
	setJSClassName( "SqlQuery" );
}
SqlQuery::SqlQuery(  QObject *parent, const char *name, const QSqlQuery &q ): BindingObject(parent, name)
{
	kdDebug() << "New SQL Query with argument" << endl;
	m_query = q;
}
SqlQuery::~SqlQuery()
{
	kdDebug() << "SQL Query going away..." << endl;
}

/*SqlQuery::SqlQuery(const SqlQuery &copy) : BindingObject(copy.parent(), copy.name())
{
	m_query = copy.m_query;
}
*/
bool SqlQuery::isValid () const
{
	if ( m_query.isValid())
		kdDebug() << "Query is valid" << endl;
	else
		kdDebug() << "Query is not valid" << endl;
	return m_query.isValid();
}
bool SqlQuery::isActive () const
{
	return m_query.isActive();
}
bool SqlQuery::isNull ( int field )
{
	return m_query.isNull(field);
}
int SqlQuery::at () const
{
	return m_query.at();
}
QString SqlQuery::lastQuery () const
{
	kdDebug() << "Last query error: " << m_query.lastQuery() << endl;
	return m_query.lastQuery();
}
int SqlQuery::numRowsAffected () const
{
	return m_query.numRowsAffected();
}
QString  SqlQuery::lastError () const
{
	return m_query.lastError().text();
}
bool SqlQuery::isSelect () const
{
	return m_query.isSelect();
}
int SqlQuery::size () const
{
	return m_query.size();
}
bool SqlQuery::exec ( const QString & query )
{
	return m_query.exec( query );
}
QVariant SqlQuery::value ( int i )
{
	return m_query.value(i);
}
bool SqlQuery::seek ( int i, bool relative )
{
	return m_query.seek(i,relative);
}
bool SqlQuery::next ()
{
	return m_query.next();
}
bool SqlQuery::prev ()
{
	return m_query.prev();
}
bool SqlQuery::first ()
{
	return m_query.first();
}
bool SqlQuery::last ()
{
	return m_query.last();
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#ifndef QT_ONLY
#include "sql_imp.moc"
#endif
