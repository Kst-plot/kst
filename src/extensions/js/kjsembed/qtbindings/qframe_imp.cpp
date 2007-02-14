
#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsbinding.h>

#include <qframe.h>
#include "qframe_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
void QFrameImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {

        // enum Shape
        { "NoFrame", QFrame::NoFrame },
        { "Box", QFrame::Box },
        { "Panel", QFrame::Panel },
        { "WinPanel", QFrame::WinPanel },
        { "HLine", QFrame::HLine },
        { "VLine", QFrame::VLine },
        { "StyledPanel", QFrame::StyledPanel },
        { "PopupPanel", QFrame::PopupPanel },
        { "MenuBarPanel", QFrame::MenuBarPanel },
        { "ToolBarPanel", QFrame::ToolBarPanel },
        { "LineEditPanel", QFrame::LineEditPanel },
        { "TabWidgetPanel", QFrame::TabWidgetPanel },
        { "GroupBoxPanel", QFrame::GroupBoxPanel },
        { "MShape", QFrame::MShape },
        // enum Shadow
        { "Plain", QFrame::Plain },
        { "Raised", QFrame::Raised },
        { "Sunken", QFrame::Sunken },
        { "MShadow", QFrame::MShadow },
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );

}

} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


