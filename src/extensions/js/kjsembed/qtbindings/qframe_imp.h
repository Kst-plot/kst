
  

#ifndef KJSEMBED_QFrame_H
#define KJSEMBED_QFrame_H

#include <kjs/interpreter.h>
#include <kjs/object.h>


/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QFrame enums.
 *
 * @author Ian Reinhart Geiser, geiseri@kde.org
 */
class QFrameImp
{
public:
   static void addBindings( KJS::ExecState *exec, KJS::Object &object );
};

} // namespace KJSEmbed

#endif // KJSEMBED_QFrame_H

// Local Variables:
// c-basic-offset: 4
// End:


