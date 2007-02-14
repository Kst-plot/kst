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

#ifndef KJSEMBEDBINDINGPLUGIN_H
#define KJSEMBEDBINDINGPLUGIN_H
#include <qobject.h>
#include <kjs/interpreter.h>
#include <kjs/identifier.h>
#include <kjs/types.h>

#include <kjsembed/global.h>

namespace KJSEmbed {

    class KJSEmbedPart;

    namespace Bindings {
        /**
         * Interface for KJSBindings that can be added as plugins.
         *
         * @author Ian Reinhart Geiser, geiseri@kde.org
         * @version $Id$
         */
        class KJSEMBED_EXPORT JSBindingPlugin : public QObject {
            Q_OBJECT
        public:
            JSBindingPlugin( QObject *parent, const char *name, const QStringList &args );
            virtual ~JSBindingPlugin();
            /**
             * This method is called by the KJSFactory to bootstrap the plugin.
             * Plugin developers will want to use this method to create their QObject binding,
             * or call their addBindings() method in their JSOpaque proxy or JSValueProxy.
             * 
             * The value returned from ths function is the KJS::Object that represents the
             * proxy arround the added object.
             */
            virtual KJS::Object createBinding( KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args ) const = 0;

            /**
             * add bindings to an object (eg an qobject returned by a slot
             */
            virtual void addBindings( KJS::ExecState * /*exec*/, KJS::Object & /*target*/ ) const {}
            ;
        };
    } // Bindings
} // KJSEmbed

#endif
