/***************************************************************************
 *   Copyright (C) 2004 by ian reinhart geiser                             *
 *   geiseri@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KFILEITEMLOADER_H
#define KFILEITEMLOADER_H

#include <kjsembed/jsbindingplugin.h>
#include <kjsembed/jsproxy_imp.h>

/**
Loader for the KFileItem wrapper.  This is automaticly invoked from the jsfactory.

@author ian reinhart geiser
*/
namespace KJSEmbed {
namespace Bindings {

class KFileItemLoader : public JSBindingPlugin
{
public:
    KFileItemLoader(QObject *parent, const char *name, const QStringList &args);

    ~KFileItemLoader();

    KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
};

class KFileItemImp : public JSProxyImp 
{
    /** Identifiers for the methods provided by this class. */
    enum MethodId { Methodrefresh, MethodrefreshMimeType, Methodurl, MethodsetUrl, MethodsetName, MethodpermissionsString, Methoduser, Methodgroup, MethodisLink, MethodisDir, MethodisFile, MethodisReadable, MethodlinkDest, MethodtimeString, MethodisLocalFile, Methodtext, Methodname,  MethodmimeType, MethodisMimeTypeKnown, MethodmimeComment, MethodiconName, Methodpixmap, Methodoverlays, MethodgetStatusBarInfo, MethodgetToolTipText, Methodrun};
public:
    KFileItemImp( KJS::ExecState *exec, int id );
    virtual ~KFileItemImp();

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const 
    {
        return true;
    }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
private:
	int mid;
};

} // Bindings
} // KJSEmbed

#endif
