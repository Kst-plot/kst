// -*- c++ -*-

/*
 *  Copyright (C) 2004 Ian Reinhart Geiser <geiseri@kde.org>
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
#include "point_imp.h"
#include <qpoint.h>
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>

namespace KJSEmbed {
namespace Bindings {

  Point::Point( KJS::ExecState *exec, int id )
  : JSProxyImp(exec), mid(id)
  {
  }

  Point::~Point()
  {
  }
  void Point::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    if( !JSProxy::checkType(object, JSProxy::ValueProxy, "QPoint") ) return;
    
    JSProxy::MethodTable methods[] = {
      { Methodx, "x"},
      { MethodsetX, "setX"},
      { Methody, "y"},
      { MethodsetY, "setY"},
      { MethodmanhattanLength, "manhattanLength"},
      { 0, 0 }
    };
    JSProxy::addMethods<Point>(exec, methods, object);

  }

  KJS::Value Point::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
    if( !JSProxy::checkType(self, JSProxy::ValueProxy, "QPoint") ) return KJS::Value();
    JSValueProxy *vp = JSProxy::toValueProxy( self.imp() );
    KJS::Value retValue = KJS::Value();
    QPoint val = vp->toVariant().toPoint();

    switch ( mid ) { 
      case Methodx:
	retValue = KJS::Number(val.x());
       break;
      case MethodsetX:
	val.setX(extractInt(exec,args,0));
	break;
      case  Methody:
	retValue = KJS::Number(val.y());
	break;
      case  MethodsetY:
	val.setY(extractInt(exec,args,0));
	break;
      case  MethodmanhattanLength:
	retValue = KJS::Number(val.manhattanLength());
	break;
      default:
	QString msg = i18n( "Point has no method %1" ).arg(mid);
  return throwError(exec, msg);
	break;
    }

    vp->setValue(val);
    return retValue;
  }

} // namespace Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
