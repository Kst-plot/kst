// -*- c++ -*-

/*
 *  Copyright (C) 2002-2003, Richard J. Moore <rich@kde.org>
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

#ifndef XMLACTIONCLIENT_H
#define XMLACTIONCLIENT_H

#include <qobject.h>
#include <qxml.h>
#include <qmap.h>

class KActionCollection;
class KAction;

namespace KJSEmbed {

class XMLActionHandler;
class XMLActionRunner;
class XMLActionScript;

/**
 * Loads actions from an XML file using SAX.
 *
 * @author Richard Moore, rich@kde.org
 */
class XMLActionClient : public QObject
{
    Q_OBJECT

public:
    /**
     * Represents a script.
     */
    struct XMLActionScript
    {
	XMLActionScript() {}
	XMLActionScript( const XMLActionScript &self ) 
	    : src(self.src), type(self.type), text(self.text) {}
	~XMLActionScript() {}

	/** Returns true iff this instance holds a valid script. */
	bool isValid() const { return !type.isEmpty(); }

	/**
	 * Clears this script object. After this method has been called,
	 * isValid() will return false.
	 */
	void clear() { src = type = text = QString::null; }

	QString src;
	QString type;
	QString text;
    };

    /** Creates an XMLActionClient. */
    XMLActionClient( QObject *parent=0, const char *name=0 );

    /** 
     * Cleans up. When the object is deleted, any actions it has created will
     * also be deleted.
     */
    virtual ~XMLActionClient();

    /** Returns the KActionCollection that the actions will be added to. */
    KActionCollection *actionCollection() const { return ac; }

    /** Sets the KActionCollection that the actions will be added to. */
    void setActionCollection( KActionCollection *acts ) { ac = acts; }

    /** Returns the runner for this XMLActionClient. */
    XMLActionRunner *runner() const { return actrun; }

    /** Sets the runner for this XMLActionClient. */
    void setRunner( XMLActionRunner *r ) { actrun = r; }

    /** Loads actions from the named XML file. Returns true on success. */
    bool load( const QString &filename );

    /** Loads actions from the named XML file. Returns true on success. */
    bool load( XMLActionHandler *handler, const QString &filename );

    /** Runs the named script. */
    bool run( const QString &name );

    /** Returns the named script. */
    XMLActionScript script( const QString &name ) const { return scripts[name]; }

    /** Calls XMLActionRunner::run(). */
    bool run( const XMLActionScript &script );

    /** Binds a name to a script. */
    virtual bool bind( const QString &name, const XMLActionScript &script );

    /** Binds an action to a script. */
    virtual bool bind( KAction *act, const XMLActionScript &script );

protected slots:
    /**
     * Called when a bound action is activated to invoke the script with the
     * sender's name.
     */
    void action_activated();

private:
    KActionCollection *ac;
    XMLActionRunner *actrun;
    QMap<QString, XMLActionScript> scripts;
    class XMLActionClientPrivate *d;
};

/**
 * Abstract class implemented by classes that can run scripts.
 *
 * @see XMLActionClient
 * @author Richard Moore, rich@kde.org
 */
class XMLActionRunner
{
public:
    /**
     * This method should be reimplemented to execute the specified script.
     *
     * @return true if the script was executed successfully, false otherwise.
     */
    virtual bool run( XMLActionClient *client, const XMLActionClient::XMLActionScript &script );
};

/**
 * SAX handler for loading actions from XML.
 *
 * The following tags are supported:
 * <pre>
 *    actionset ( header? action* )
 *    header ( name | label | icons | script )
 *    action ( (header | name | label | icons | shortcut | group | 
 *              whatsthis | statustext | type | script | data)+ )
 *    type ( #CDATA )
 *    label ( #CDATA | text )
 *    icons ( #CDATA )
 *    shortcut ( #CDATA | text )
 *    whatsthis ( #CDATA | text )
 *    group ( #CDATA )
 *        exclusive    defaults to false
 *    name ( #CDATA )
 *    text ( #CDATA )
 *    data ( item+ )
 *    item ( #CDATA | text )
 *    script ( #CDATA )
 *        type    type of script
 *        src     url of script file (optional)
 * </pre>
 *
 * Unknown tags are ignored, so subclasses can define new ones if they
 * want to store additional data.
 *
 * @author Richard Moore, rich@kde.org
 */
class XMLActionHandler : public QXmlDefaultHandler
{
public:
    XMLActionHandler( XMLActionClient *actclient );

    virtual bool startElement( const QString &ns, const QString &ln, const QString &qn,
			       const QXmlAttributes &attrs );
    virtual bool endElement( const QString &ns, const QString &ln, const QString &qn );
    virtual bool characters( const QString &chars );

    /** Called when an action tag is closed. */
    void defineAction();

    XMLActionClient *client() const { return actclient; }

    /** Creates a KAction based on the values read from the XML. */
    virtual KAction *createAction( KActionCollection *parent );

private:
    /**
     * Structure containing information gathered about an action.
     */
    struct XMLActionData {
	XMLActionData() { clear(); }

	void clear() {
	    text = icons = keys = name = group = whatsthis = status = QString::null;
	    exclusive = false;
	    script.clear();
	}

	QString type;
	QString text;
	QString icons;
	QString keys;
	QString name;
	QString group;
	bool exclusive;
	QString status;
	QString whatsthis;
	XMLActionClient::XMLActionScript script;
	QStringList items;
    };

    XMLActionData *actionData() { return &ad; }

private:
    XMLActionClient *actclient;
    QString cdata;
    bool inAction;
    XMLActionData ad;
    class XMLActionHandlerPrivate *d;
};

} // namespace KJSEmbed

#endif // XMLACTIONCLIENT_H

