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
 *  but WITHOUT any WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KJSEMBEDJSOPAQUEPROXY_H
#define KJSEMBEDJSOPAQUEPROXY_H

#include <qcstring.h>
#include <qevent.h>

#include <kjs/object.h>
#include <kjsembed/jsproxy.h>

#include <algorithm>
#include <typeinfo>

class QTextStream;

namespace KJSEmbed {

/**
 * Provides a binding to an opaque pointer value. This class stores a pointer
 * without interpreting its type, it also stores the name of the type allowing
 * it to retain type-safety.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSOpaqueProxy : public JSProxy
{
public:
    /** Creates an opaque proxy. */
    JSOpaqueProxy();

    /** Creates an opaque proxy. */
    template<typename T>
    JSOpaqueProxy( T *nptr, const char *ptype )
    : JSProxy( JSProxy::OpaqueProxy ), ptrtype(ptype), ptr(new Pointer<T>(nptr))
    {

    }

    /** Creates an opaque proxy. */
    JSOpaqueProxy( QTextStream *ts );

    /** Creates an opaque proxy. */
    JSOpaqueProxy( const QEvent *ev );

    /** Cleans up. */
    virtual ~JSOpaqueProxy();

    /** Returns the type of the wrapped object.*/
    QString typeName() const;

    /** Sets the value of the proxy and its type. */
    template<typename T>
    void setValue( T *nptr, const char *ptype ) {
	  if( ptr ) {
	    if(owner() == JavaScript) ptr->cleanup();
	    delete ptr;
	  }
	  ptr = new Pointer<T>(nptr);
	  ptrtype = ptype ? ptype : "void";
    }

    /** Sets the value of the proxy to a QTextStream. */
    void setValue( QTextStream *ts );

    /** Sets the value of the proxy to a QEvent. */
    void setValue( const QEvent *ev );

    /** Returns the proxy value as a pointer. */
    template<typename T>
    T *toNative() {
      if( !ptr ) return 0L; // empty
      T *value = 0L;
      return (T*)ptr->voidStar(); // Hack for now
      return value;  // return real value
    }

    /** Returns true iff the content of this proxy inherits the specified base-class. */
    bool inherits( const char *clazz );

    /**
     * Returns the QTextStream stored in the proxy. If proxy does not contain
     * a value of type QTextStream then 0 is returned.
     */
    QTextStream *toTextStream();

    const QEvent *toEvent();

    /** Adds the bindings for the opaque proxy to the specified js object. */
    virtual void addBindings( KJS::ExecState *state, KJS::Object &object );

    /** Reimplemented to return the name and class of the target. */
    virtual KJS::UString toString( KJS::ExecState *exec ) const;

private:

    QTextStream *textstream;
    const QEvent *event;
    QCString ptrtype;

    class JSOpaqueProxyPrivate *d;

    // container objects
    class PointerBase
    {
      public:
	virtual ~PointerBase() {};
	virtual void cleanup() = 0;
	virtual const std::type_info &type() const = 0;
	virtual void *voidStar() = 0;
	//template<typename ValueType>
	//virtual void castTo(ValueType *& val) = 0L;
    };

    template<typename ValueType>
    class Pointer : public PointerBase
    {
      public:
	Pointer( ValueType *value) : ptr(value) {}
	~Pointer( ) {}
	void cleanup()
	{
	  delete ptr;
	  ptr=0L;
	}
	const std::type_info &type() const
	{
	  return typeid(ValueType);
	}
	
	void *voidStar()
	{
	  return (void*)ptr;
	}
	//template<typename type>
	//virtual void castTo(type *& val)
	//{
	//	val = dynamic_cast<type*>(ptr);
	//	kdDebug() << "Type " << val << endl;
	//}
	ValueType *ptr;
    };

    PointerBase *ptr;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSOPAQUEPROXYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
