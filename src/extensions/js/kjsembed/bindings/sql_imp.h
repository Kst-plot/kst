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

#ifndef KJSEMBED_SQL_IMP_H
#define KJSEMBED_SQL_IMP_H

#include "bindingobject.h"
#include <qsqlquery.h>
#include <qsqldatabase.h>

namespace KJSEmbed {
namespace Bindings {

/**
 * QObject Binding for QSQLQuery.
 *
 * @author Ian Reinhart Geiser, geiseri@kde.org
 * @version $Id$
 */
class SqlQuery : public BindingObject
{
	Q_OBJECT
	Q_PROPERTY( int  size READ size)
	Q_PROPERTY( int  at READ at)
	Q_PROPERTY( int  numRowsAffected READ numRowsAffected )
	Q_PROPERTY( bool  select READ isSelect)
	Q_PROPERTY( bool  valid READ isValid)
	Q_PROPERTY( bool  active READ isActive)
	Q_PROPERTY( QString lastError READ lastError)
	Q_PROPERTY( QString lastQuery READ lastQuery)

public:
	SqlQuery( QObject *parent=0, const char *name=0 );
	SqlQuery( QObject *parent, const char *name, const QSqlQuery &q );
	//SqlQuery( const SqlQuery &copy);
	virtual ~SqlQuery();

	QSqlQuery query(){ return m_query; }
	void setQuery(const QSqlQuery &q) {m_query = QSqlQuery(q); }

	int size () const;
	bool isSelect () const;
	bool isValid () const;
	bool isActive () const;
	int at () const;
	int numRowsAffected () const;
	QString lastError () const;
	QString lastQuery () const;

public slots:



	bool isNull ( int field );
	bool exec ( const QString & m_query );
	QVariant value ( int i );
	bool seek ( int i, bool relative = FALSE );
	bool next ();
	bool prev ();
	bool first ();
	bool last ();

private:
	QSqlQuery m_query;
};

/**
 * QObject Binding for QSQLDatabase.
 *
 * @author Ian Reinhart Geiser, geiseri@kde.org
 * @version $Id$
 */
class SqlDatabase : public BindingObject
{
    Q_OBJECT
    Q_PROPERTY( QString  databaseName READ databaseName WRITE setDatabaseName)
    Q_PROPERTY( QString  userName READ userName WRITE setUserName)
    Q_PROPERTY( QString  password READ password WRITE setPassword)
    Q_PROPERTY( QString  hostName READ hostName WRITE setHostName)
    Q_PROPERTY( int port READ port WRITE setPort)
    Q_PROPERTY( QString  driverName READ driverName)
    Q_PROPERTY( QString  lastError READ lastError)

public:
	SqlDatabase( QObject *parent=0, const char *name=0 );
	virtual ~SqlDatabase();


public slots:
	void setDatabaseName (const QString &name );
	void setUserName (const QString &name );
	void setPassword (const QString &password );
	void setHostName (const QString &host );
	void setPort ( int p );
	QString databaseName () const;
	QString userName () const;
	QString password () const;
	QString hostName () const;
	int port () const;
	QString driverName () const;
	
	bool addDatabase ( const QString & type, const QString & connectionName = QSqlDatabase::defaultConnection );
	QStringList drivers ();
	bool open ();
	bool open ( const QString &user, const QString &password );
	void close ();
	bool isOpen ();
	bool isOpenError ();
	QStringList tables ();
	SqlQuery *exec( const QString &query );
	QString  lastError () const;
	bool transaction ();
	bool commit ();
	bool rollback ();

private:
	QString connectionName;
	//QSqlDatabase *db;
};


} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBED_SQL_IMP_H
