#include "bindingobject.h"
#ifndef QT_ONLY
#include "bindingobject.moc"
#endif
namespace KJSEmbed {
namespace Bindings {

BindingObject::BindingObject( QObject *parent, const char *name )
    : QObject( parent, name )
{
}

BindingObject::~BindingObject()
{
}

}
}
