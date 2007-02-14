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
#ifndef KJSEMBED_BINDINGSBINDINGBASE_H
#define KJSEMBED_BINDINGSBINDINGBASE_H

#include <kjs/interpreter.h>
#include <kjs/identifier.h>
#include <kjs/types.h>

namespace KJSEmbed {
class KJSEmbedPart;
    namespace Bindings {

        /**
        Baseclass for adding bindings at runtime.
         
        @author ian reinhart geiser <geiseri@kde.org>
        */
        class JSBindingBase {
        public:
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
             * Adds bindings to a specific object.  This is useful if you wish to add bindings to a proxy
             * that has been created elsewhere.
             */
            virtual void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const = 0;
        };

    }

}

#endif
