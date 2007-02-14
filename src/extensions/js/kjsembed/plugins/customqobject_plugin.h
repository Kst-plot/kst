// -*- c++ -*-

/*
 *  Copyright (C) 2003, Richard J. Moore <rich@kde.org>
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

#ifndef KJSEMBED_CUSTOMQOBJECT_PLUGIN_H
#define KJSEMBED_CUSTOMQOBJECT_PLUGIN_H

#include <kjsembed/jsbindingplugin.h>
#include <kjsembed/jsproxy_imp.h>
#include <qobject.h>

namespace KJSEmbed {
namespace Bindings {

class MyCustomQObjectLoader : public JSBindingPlugin
{
public:
    MyCustomQObjectLoader( QObject *parent, const char *name, const QStringList &args );
    virtual ~MyCustomQObjectLoader(){;}
    KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
};


class MyCustomQObjectImp : public QObject {

Q_OBJECT
	
    /** Identifiers for the methods provided by this class. */
    enum Mode { On, Off };
	Q_PROPERTY(Mode mode READ mode WRITE setMode)
	Q_PROPERTY(QString thing READ thing WRITE setThing)
	Q_ENUMS(Mode)
public:
    MyCustomQObjectImp( QObject *parent, const char *name);
    virtual ~MyCustomQObjectImp();

public slots:
	Mode mode() const;
	void setMode( Mode md);
	QString thing() const;
	void setThing( const QString &t);
private:
	Mode m_mode;
	QString m_thing;
	
};

} // namespace
} // namespace

#endif // KJSEMBED_IMAGE_IMP_H
