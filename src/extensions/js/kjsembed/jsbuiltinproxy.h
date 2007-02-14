// -*- c++ -*-

/*
 *  Copyright (C) 2001-2004, Ian Reinhart Geiser <geiseri@kde.org>
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

#ifndef BUILTINJSPROXY_H
#define BUILTINJSPROXY_H

#include <kjsembed/jsproxy.h>
#include <kjsembed/global.h>

namespace KJSEmbed {

/**
 * Class for builtin global objects objects.
 *
 * @author Ian Reinhart Geiser <geiseri@kde.org>
 */
class KJSEMBED_EXPORT JSBuiltinProxy : public JSProxy
{
public:
    JSBuiltinProxy( const QString &name );
    virtual ~JSBuiltinProxy();


    virtual QString typeName() const;

    virtual bool inherits( const char *clazz );
    
    bool implementsCall() const { return true; }
    
    bool implementsConstruct() const { return false; }
    
    /** Reimplemented to return the name and class of the target. */
    virtual KJS::UString toString( KJS::ExecState *exec ) const;
private:
    QString m_name;
};

} // namespace KJSEmbed
#endif 
