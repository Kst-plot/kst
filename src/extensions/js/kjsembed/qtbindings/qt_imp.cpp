


#include <qcstring.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>

#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qnamespace.h>
#include "qt_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QtImp::QtImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QtImp::~QtImp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void QtImp::addStaticBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QtImp *meth = new QtImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }


    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {

        // enum ButtonState
        { "NoButton", Qt::NoButton },
        { "LeftButton", Qt::LeftButton },
        { "RightButton", Qt::RightButton },
        { "MidButton", Qt::MidButton },
        { "MouseButtonMask", Qt::MouseButtonMask },
        { "ShiftButton", Qt::ShiftButton },
        { "ControlButton", Qt::ControlButton },
        { "AltButton", Qt::AltButton },
        { "MetaButton", Qt::MetaButton },
        { "KeyButtonMask", Qt::KeyButtonMask },
        { "Keypad", Qt::Keypad },
        // enum Orientation
        { "Horizontal", Qt::Horizontal },
        { "Vertical", Qt::Vertical },
        // enum SortOrder
        { "Ascending", Qt::Ascending },
        { "Descending", Qt::Descending },
        // enum AlignmentFlags
        { "AlignAuto", Qt::AlignAuto },
        { "AlignLeft", Qt::AlignLeft },
        { "AlignRight", Qt::AlignRight },
        { "AlignHCenter", Qt::AlignHCenter },
        { "AlignJustify", Qt::AlignJustify },
        { "AlignHorizontal_Mask", Qt::AlignHorizontal_Mask },
        { "AlignTop", Qt::AlignTop },
        { "AlignBottom", Qt::AlignBottom },
        { "AlignVCenter", Qt::AlignVCenter },
        { "AlignVertical_Mask", Qt::AlignVertical_Mask },
        { "AlignCenter", Qt::AlignCenter },
        // enum TextFlags
        { "SingleLine", Qt::SingleLine },
        { "DontClip", Qt::DontClip },
        { "ExpandTabs", Qt::ExpandTabs },
        { "ShowPrefix", Qt::ShowPrefix },
        { "WordBreak", Qt::WordBreak },
        { "BreakAnywhere", Qt::BreakAnywhere },
        { "DontPrint", Qt::DontPrint },
        { "Underline", Qt::Underline },
        { "Overline", Qt::Overline },
        { "StrikeOut", Qt::StrikeOut },
        { "IncludeTrailingSpaces", Qt::IncludeTrailingSpaces },
        { "NoAccel", Qt::NoAccel },
        // enum WidgetState
        { "WState_Created", Qt::WState_Created },
        { "WState_Disabled", Qt::WState_Disabled },
        { "WState_Visible", Qt::WState_Visible },
        { "WState_ForceHide", Qt::WState_ForceHide },
        { "WState_OwnCursor", Qt::WState_OwnCursor },
        { "WState_MouseTracking", Qt::WState_MouseTracking },
        { "WState_CompressKeys", Qt::WState_CompressKeys },
        { "WState_BlockUpdates", Qt::WState_BlockUpdates },
        { "WState_InPaintEvent", Qt::WState_InPaintEvent },
        { "WState_Reparented", Qt::WState_Reparented },
        { "WState_ConfigPending", Qt::WState_ConfigPending },
        { "WState_Resized", Qt::WState_Resized },
        { "WState_AutoMask", Qt::WState_AutoMask },
        { "WState_Polished", Qt::WState_Polished },
        { "WState_DND", Qt::WState_DND },
        { "WState_Reserved0", Qt::WState_Reserved0 },
        { "WState_FullScreen", Qt::WState_FullScreen },
        { "WState_OwnSizePolicy", Qt::WState_OwnSizePolicy },
        { "WState_CreatedHidden", Qt::WState_CreatedHidden },
        { "WState_Maximized", Qt::WState_Maximized },
        { "WState_Minimized", Qt::WState_Minimized },
        { "WState_ForceDisabled", Qt::WState_ForceDisabled },
        { "WState_Exposed", Qt::WState_Exposed },
        { "WState_HasMouse", Qt::WState_HasMouse },
        // enum WidgetFlags
        { "WType_TopLevel", Qt::WType_TopLevel },
        { "WType_Dialog", Qt::WType_Dialog },
        { "WType_Popup", Qt::WType_Popup },
        { "WType_Desktop", Qt::WType_Desktop },
        { "WType_Mask", Qt::WType_Mask },
        { "WStyle_Customize", Qt::WStyle_Customize },
        { "WStyle_NormalBorder", Qt::WStyle_NormalBorder },
        { "WStyle_DialogBorder", Qt::WStyle_DialogBorder },
        { "WStyle_NoBorder", Qt::WStyle_NoBorder },
        { "WStyle_Title", Qt::WStyle_Title },
        { "WStyle_SysMenu", Qt::WStyle_SysMenu },
        { "WStyle_Minimize", Qt::WStyle_Minimize },
        { "WStyle_Maximize", Qt::WStyle_Maximize },
        { "WStyle_MinMax", Qt::WStyle_MinMax },
        { "WStyle_Tool", Qt::WStyle_Tool },
        { "WStyle_StaysOnTop", Qt::WStyle_StaysOnTop },
        { "WStyle_ContextHelp", Qt::WStyle_ContextHelp },
        { "WStyle_Reserved", Qt::WStyle_Reserved },
        { "WStyle_Mask", Qt::WStyle_Mask },
        { "WDestructiveClose", Qt::WDestructiveClose },
        { "WPaintDesktop", Qt::WPaintDesktop },
        { "WPaintUnclipped", Qt::WPaintUnclipped },
        { "WPaintClever", Qt::WPaintClever },
        { "WResizeNoErase", Qt::WResizeNoErase },
        { "WMouseNoMask", Qt::WMouseNoMask },
        { "WStaticContents", Qt::WStaticContents },
        { "WRepaintNoErase", Qt::WRepaintNoErase },
        { "WX11BypassWM", Qt::WX11BypassWM },
        { "WWinOwnDC", Qt::WWinOwnDC },
        { "WMacNoSheet", Qt::WMacNoSheet },
        { "WMacDrawer", Qt::WMacDrawer },
        { "WGroupLeader", Qt::WGroupLeader },
        { "WShowModal", Qt::WShowModal },
        { "WNoMousePropagation", Qt::WNoMousePropagation },
        { "WSubWindow", Qt::WSubWindow },
        { "WStyle_Splash", Qt::WStyle_Splash },
        { "WNoAutoErase", Qt::WNoAutoErase },
#if 0
        { "WNorthWestGravity", Qt::WNorthWestGravity },
        { "WType_Modal", Qt::WType_Modal },
        { "WStyle_Dialog", Qt::WStyle_Dialog },
        { "WStyle_NoBorderEx", Qt::WStyle_NoBorderEx },
#endif
        // enum WindowState
        { "WindowNoState", Qt::WindowNoState },
        { "WindowMinimized", Qt::WindowMinimized },
        { "WindowMaximized", Qt::WindowMaximized },
        { "WindowFullScreen", Qt::WindowFullScreen },
        { "WindowActive", Qt::WindowActive },
        // enum ImageConversionFlags
        { "ColorMode_Mask", Qt::ColorMode_Mask },
        { "AutoColor", Qt::AutoColor },
        { "ColorOnly", Qt::ColorOnly },
        { "MonoOnly", Qt::MonoOnly },
        { "AlphaDither_Mask", Qt::AlphaDither_Mask },
        { "ThresholdAlphaDither", Qt::ThresholdAlphaDither },
        { "OrderedAlphaDither", Qt::OrderedAlphaDither },
        { "DiffuseAlphaDither", Qt::DiffuseAlphaDither },
        { "NoAlpha", Qt::NoAlpha },
        { "Dither_Mask", Qt::Dither_Mask },
        { "DiffuseDither", Qt::DiffuseDither },
        { "OrderedDither", Qt::OrderedDither },
        { "ThresholdDither", Qt::ThresholdDither },
        { "DitherMode_Mask", Qt::DitherMode_Mask },
        { "AutoDither", Qt::AutoDither },
        { "PreferDither", Qt::PreferDither },
        { "AvoidDither", Qt::AvoidDither },
        // enum BGMode
        { "TransparentMode", Qt::TransparentMode },
        { "OpaqueMode", Qt::OpaqueMode },
#if 0
        // enum PaintUnit
        { "PixelUnit", Qt::PixelUnit },
        { "LoMetricUnit", Qt::LoMetricUnit },
        { "HiMetricUnit", Qt::HiMetricUnit },
        { "LoEnglishUnit", Qt::LoEnglishUnit },
        { "HiEnglishUnit", Qt::HiEnglishUnit },
        { "TwipsUnit", Qt::TwipsUnit },
        // enum GUIStyle
        { "MacStyle", Qt::MacStyle },
#endif
        { "WindowsStyle", Qt::WindowsStyle },
#if 0
        { "Win3Style", Qt::Win3Style },
        { "PMStyle", Qt::PMStyle },
#endif
        { "MotifStyle", Qt::MotifStyle },
        // enum SequenceMatch
        { "NoMatch", Qt::NoMatch },
        { "PartialMatch", Qt::PartialMatch },
        { "Identical", Qt::Identical },
        // enum Modifier
        { "META", Qt::META },
        { "SHIFT", Qt::SHIFT },
        { "CTRL", Qt::CTRL },
        { "ALT", Qt::ALT },
        { "MODIFIER_MASK", Qt::MODIFIER_MASK },
        { "UNICODE_ACCEL", Qt::UNICODE_ACCEL },
        { "ASCII_ACCEL", Qt::ASCII_ACCEL },
        // enum Key
        { "Key_Escape", Qt::Key_Escape },
        { "Key_Tab", Qt::Key_Tab },
        { "Key_Backtab", Qt::Key_Backtab },
        { "Key_BackTab", Qt::Key_BackTab },
        { "Key_Backspace", Qt::Key_Backspace },
        { "Key_BackSpace", Qt::Key_BackSpace },
        { "Key_Return", Qt::Key_Return },
        { "Key_Enter", Qt::Key_Enter },
        { "Key_Insert", Qt::Key_Insert },
        { "Key_Delete", Qt::Key_Delete },
        { "Key_Pause", Qt::Key_Pause },
        { "Key_Print", Qt::Key_Print },
        { "Key_SysReq", Qt::Key_SysReq },
        { "Key_Clear", Qt::Key_Clear },
        { "Key_Home", Qt::Key_Home },
        { "Key_End", Qt::Key_End },
        { "Key_Left", Qt::Key_Left },
        { "Key_Up", Qt::Key_Up },
        { "Key_Right", Qt::Key_Right },
        { "Key_Down", Qt::Key_Down },
        { "Key_Prior", Qt::Key_Prior },
        { "Key_PageUp", Qt::Key_PageUp },
        { "Key_Next", Qt::Key_Next },
        { "Key_PageDown", Qt::Key_PageDown },
        { "Key_Shift", Qt::Key_Shift },
        { "Key_Control", Qt::Key_Control },
        { "Key_Meta", Qt::Key_Meta },
        { "Key_Alt", Qt::Key_Alt },
        { "Key_CapsLock", Qt::Key_CapsLock },
        { "Key_NumLock", Qt::Key_NumLock },
        { "Key_ScrollLock", Qt::Key_ScrollLock },
        { "Key_F1", Qt::Key_F1 },
        { "Key_F2", Qt::Key_F2 },
        { "Key_F3", Qt::Key_F3 },
        { "Key_F4", Qt::Key_F4 },
        { "Key_F5", Qt::Key_F5 },
        { "Key_F6", Qt::Key_F6 },
        { "Key_F7", Qt::Key_F7 },
        { "Key_F8", Qt::Key_F8 },
        { "Key_F9", Qt::Key_F9 },
        { "Key_F10", Qt::Key_F10 },
        { "Key_F11", Qt::Key_F11 },
        { "Key_F12", Qt::Key_F12 },
        { "Key_F13", Qt::Key_F13 },
        { "Key_F14", Qt::Key_F14 },
        { "Key_F15", Qt::Key_F15 },
        { "Key_F16", Qt::Key_F16 },
        { "Key_F17", Qt::Key_F17 },
        { "Key_F18", Qt::Key_F18 },
        { "Key_F19", Qt::Key_F19 },
        { "Key_F20", Qt::Key_F20 },
        { "Key_F21", Qt::Key_F21 },
        { "Key_F22", Qt::Key_F22 },
        { "Key_F23", Qt::Key_F23 },
        { "Key_F24", Qt::Key_F24 },
        { "Key_F25", Qt::Key_F25 },
        { "Key_F26", Qt::Key_F26 },
        { "Key_F27", Qt::Key_F27 },
        { "Key_F28", Qt::Key_F28 },
        { "Key_F29", Qt::Key_F29 },
        { "Key_F30", Qt::Key_F30 },
        { "Key_F31", Qt::Key_F31 },
        { "Key_F32", Qt::Key_F32 },
        { "Key_F33", Qt::Key_F33 },
        { "Key_F34", Qt::Key_F34 },
        { "Key_F35", Qt::Key_F35 },
        { "Key_Super_L", Qt::Key_Super_L },
        { "Key_Super_R", Qt::Key_Super_R },
        { "Key_Menu", Qt::Key_Menu },
        { "Key_Hyper_L", Qt::Key_Hyper_L },
        { "Key_Hyper_R", Qt::Key_Hyper_R },
        { "Key_Help", Qt::Key_Help },
        { "Key_Direction_L", Qt::Key_Direction_L },
        { "Key_Direction_R", Qt::Key_Direction_R },
        { "Key_Space", Qt::Key_Space },
        { "Key_Any", Qt::Key_Any },
        { "Key_Exclam", Qt::Key_Exclam },
        { "Key_QuoteDbl", Qt::Key_QuoteDbl },
        { "Key_NumberSign", Qt::Key_NumberSign },
        { "Key_Dollar", Qt::Key_Dollar },
        { "Key_Percent", Qt::Key_Percent },
        { "Key_Ampersand", Qt::Key_Ampersand },
        { "Key_Apostrophe", Qt::Key_Apostrophe },
        { "Key_ParenLeft", Qt::Key_ParenLeft },
        { "Key_ParenRight", Qt::Key_ParenRight },
        { "Key_Asterisk", Qt::Key_Asterisk },
        { "Key_Plus", Qt::Key_Plus },
        { "Key_Comma", Qt::Key_Comma },
        { "Key_Minus", Qt::Key_Minus },
        { "Key_Period", Qt::Key_Period },
        { "Key_Slash", Qt::Key_Slash },
        { "Key_0", Qt::Key_0 },
        { "Key_1", Qt::Key_1 },
        { "Key_2", Qt::Key_2 },
        { "Key_3", Qt::Key_3 },
        { "Key_4", Qt::Key_4 },
        { "Key_5", Qt::Key_5 },
        { "Key_6", Qt::Key_6 },
        { "Key_7", Qt::Key_7 },
        { "Key_8", Qt::Key_8 },
        { "Key_9", Qt::Key_9 },
        { "Key_Colon", Qt::Key_Colon },
        { "Key_Semicolon", Qt::Key_Semicolon },
        { "Key_Less", Qt::Key_Less },
        { "Key_Equal", Qt::Key_Equal },
        { "Key_Greater", Qt::Key_Greater },
        { "Key_Question", Qt::Key_Question },
        { "Key_At", Qt::Key_At },
        { "Key_A", Qt::Key_A },
        { "Key_B", Qt::Key_B },
        { "Key_C", Qt::Key_C },
        { "Key_D", Qt::Key_D },
        { "Key_E", Qt::Key_E },
        { "Key_F", Qt::Key_F },
        { "Key_G", Qt::Key_G },
        { "Key_H", Qt::Key_H },
        { "Key_I", Qt::Key_I },
        { "Key_J", Qt::Key_J },
        { "Key_K", Qt::Key_K },
        { "Key_L", Qt::Key_L },
        { "Key_M", Qt::Key_M },
        { "Key_N", Qt::Key_N },
        { "Key_O", Qt::Key_O },
        { "Key_P", Qt::Key_P },
        { "Key_Q", Qt::Key_Q },
        { "Key_R", Qt::Key_R },
        { "Key_S", Qt::Key_S },
        { "Key_T", Qt::Key_T },
        { "Key_U", Qt::Key_U },
        { "Key_V", Qt::Key_V },
        { "Key_W", Qt::Key_W },
        { "Key_X", Qt::Key_X },
        { "Key_Y", Qt::Key_Y },
        { "Key_Z", Qt::Key_Z },
        { "Key_BracketLeft", Qt::Key_BracketLeft },
        { "Key_Backslash", Qt::Key_Backslash },
        { "Key_BracketRight", Qt::Key_BracketRight },
        { "Key_AsciiCircum", Qt::Key_AsciiCircum },
        { "Key_Underscore", Qt::Key_Underscore },
        { "Key_QuoteLeft", Qt::Key_QuoteLeft },
        { "Key_BraceLeft", Qt::Key_BraceLeft },
        { "Key_Bar", Qt::Key_Bar },
        { "Key_BraceRight", Qt::Key_BraceRight },
        { "Key_AsciiTilde", Qt::Key_AsciiTilde },
        { "Key_nobreakspace", Qt::Key_nobreakspace },
        { "Key_exclamdown", Qt::Key_exclamdown },
        { "Key_cent", Qt::Key_cent },
        { "Key_sterling", Qt::Key_sterling },
        { "Key_currency", Qt::Key_currency },
        { "Key_yen", Qt::Key_yen },
        { "Key_brokenbar", Qt::Key_brokenbar },
        { "Key_section", Qt::Key_section },
        { "Key_diaeresis", Qt::Key_diaeresis },
        { "Key_copyright", Qt::Key_copyright },
        { "Key_ordfeminine", Qt::Key_ordfeminine },
        { "Key_guillemotleft", Qt::Key_guillemotleft },
        { "Key_notsign", Qt::Key_notsign },
        { "Key_hyphen", Qt::Key_hyphen },
        { "Key_registered", Qt::Key_registered },
        { "Key_macron", Qt::Key_macron },
        { "Key_degree", Qt::Key_degree },
        { "Key_plusminus", Qt::Key_plusminus },
        { "Key_twosuperior", Qt::Key_twosuperior },
        { "Key_threesuperior", Qt::Key_threesuperior },
        { "Key_acute", Qt::Key_acute },
        { "Key_mu", Qt::Key_mu },
        { "Key_paragraph", Qt::Key_paragraph },
        { "Key_periodcentered", Qt::Key_periodcentered },
        { "Key_cedilla", Qt::Key_cedilla },
        { "Key_onesuperior", Qt::Key_onesuperior },
        { "Key_masculine", Qt::Key_masculine },
        { "Key_guillemotright", Qt::Key_guillemotright },
        { "Key_onequarter", Qt::Key_onequarter },
        { "Key_onehalf", Qt::Key_onehalf },
        { "Key_threequarters", Qt::Key_threequarters },
        { "Key_questiondown", Qt::Key_questiondown },
        { "Key_Agrave", Qt::Key_Agrave },
        { "Key_Aacute", Qt::Key_Aacute },
        { "Key_Acircumflex", Qt::Key_Acircumflex },
        { "Key_Atilde", Qt::Key_Atilde },
        { "Key_Adiaeresis", Qt::Key_Adiaeresis },
        { "Key_Aring", Qt::Key_Aring },
        { "Key_AE", Qt::Key_AE },
        { "Key_Ccedilla", Qt::Key_Ccedilla },
        { "Key_Egrave", Qt::Key_Egrave },
        { "Key_Eacute", Qt::Key_Eacute },
        { "Key_Ecircumflex", Qt::Key_Ecircumflex },
        { "Key_Ediaeresis", Qt::Key_Ediaeresis },
        { "Key_Igrave", Qt::Key_Igrave },
        { "Key_Iacute", Qt::Key_Iacute },
        { "Key_Icircumflex", Qt::Key_Icircumflex },
        { "Key_Idiaeresis", Qt::Key_Idiaeresis },
        { "Key_ETH", Qt::Key_ETH },
        { "Key_Ntilde", Qt::Key_Ntilde },
        { "Key_Ograve", Qt::Key_Ograve },
        { "Key_Oacute", Qt::Key_Oacute },
        { "Key_Ocircumflex", Qt::Key_Ocircumflex },
        { "Key_Otilde", Qt::Key_Otilde },
        { "Key_Odiaeresis", Qt::Key_Odiaeresis },
        { "Key_multiply", Qt::Key_multiply },
        { "Key_Ooblique", Qt::Key_Ooblique },
        { "Key_Ugrave", Qt::Key_Ugrave },
        { "Key_Uacute", Qt::Key_Uacute },
        { "Key_Ucircumflex", Qt::Key_Ucircumflex },
        { "Key_Udiaeresis", Qt::Key_Udiaeresis },
        { "Key_Yacute", Qt::Key_Yacute },
        { "Key_THORN", Qt::Key_THORN },
        { "Key_ssharp", Qt::Key_ssharp },
        { "Key_agrave", Qt::Key_agrave },
        { "Key_aacute", Qt::Key_aacute },
        { "Key_acircumflex", Qt::Key_acircumflex },
        { "Key_atilde", Qt::Key_atilde },
        { "Key_adiaeresis", Qt::Key_adiaeresis },
        { "Key_aring", Qt::Key_aring },
        { "Key_ae", Qt::Key_ae },
        { "Key_ccedilla", Qt::Key_ccedilla },
        { "Key_egrave", Qt::Key_egrave },
        { "Key_eacute", Qt::Key_eacute },
        { "Key_ecircumflex", Qt::Key_ecircumflex },
        { "Key_ediaeresis", Qt::Key_ediaeresis },
        { "Key_igrave", Qt::Key_igrave },
        { "Key_iacute", Qt::Key_iacute },
        { "Key_icircumflex", Qt::Key_icircumflex },
        { "Key_idiaeresis", Qt::Key_idiaeresis },
        { "Key_eth", Qt::Key_eth },
        { "Key_ntilde", Qt::Key_ntilde },
        { "Key_ograve", Qt::Key_ograve },
        { "Key_oacute", Qt::Key_oacute },
        { "Key_ocircumflex", Qt::Key_ocircumflex },
        { "Key_otilde", Qt::Key_otilde },
        { "Key_odiaeresis", Qt::Key_odiaeresis },
        { "Key_division", Qt::Key_division },
        { "Key_oslash", Qt::Key_oslash },
        { "Key_ugrave", Qt::Key_ugrave },
        { "Key_uacute", Qt::Key_uacute },
        { "Key_ucircumflex", Qt::Key_ucircumflex },
        { "Key_udiaeresis", Qt::Key_udiaeresis },
        { "Key_yacute", Qt::Key_yacute },
        { "Key_thorn", Qt::Key_thorn },
        { "Key_ydiaeresis", Qt::Key_ydiaeresis },
        { "Key_Back", Qt::Key_Back },
        { "Key_Forward", Qt::Key_Forward },
        { "Key_Stop", Qt::Key_Stop },
        { "Key_Refresh", Qt::Key_Refresh },
        { "Key_VolumeDown", Qt::Key_VolumeDown },
        { "Key_VolumeMute", Qt::Key_VolumeMute },
        { "Key_VolumeUp", Qt::Key_VolumeUp },
        { "Key_BassBoost", Qt::Key_BassBoost },
        { "Key_BassUp", Qt::Key_BassUp },
        { "Key_BassDown", Qt::Key_BassDown },
        { "Key_TrebleUp", Qt::Key_TrebleUp },
        { "Key_TrebleDown", Qt::Key_TrebleDown },
        { "Key_MediaPlay", Qt::Key_MediaPlay },
        { "Key_MediaStop", Qt::Key_MediaStop },
        { "Key_MediaPrev", Qt::Key_MediaPrev },
        { "Key_MediaNext", Qt::Key_MediaNext },
        { "Key_MediaRecord", Qt::Key_MediaRecord },
        { "Key_HomePage", Qt::Key_HomePage },
        { "Key_Favorites", Qt::Key_Favorites },
        { "Key_Search", Qt::Key_Search },
        { "Key_Standby", Qt::Key_Standby },
        { "Key_OpenUrl", Qt::Key_OpenUrl },
        { "Key_LaunchMail", Qt::Key_LaunchMail },
        { "Key_LaunchMedia", Qt::Key_LaunchMedia },
        { "Key_Launch0", Qt::Key_Launch0 },
        { "Key_Launch1", Qt::Key_Launch1 },
        { "Key_Launch2", Qt::Key_Launch2 },
        { "Key_Launch3", Qt::Key_Launch3 },
        { "Key_Launch4", Qt::Key_Launch4 },
        { "Key_Launch5", Qt::Key_Launch5 },
        { "Key_Launch6", Qt::Key_Launch6 },
        { "Key_Launch7", Qt::Key_Launch7 },
        { "Key_Launch8", Qt::Key_Launch8 },
        { "Key_Launch9", Qt::Key_Launch9 },
        { "Key_LaunchA", Qt::Key_LaunchA },
        { "Key_LaunchB", Qt::Key_LaunchB },
        { "Key_LaunchC", Qt::Key_LaunchC },
        { "Key_LaunchD", Qt::Key_LaunchD },
        { "Key_LaunchE", Qt::Key_LaunchE },
        { "Key_LaunchF", Qt::Key_LaunchF },
        { "Key_MediaLast", Qt::Key_MediaLast },
        { "Key_unknown", Qt::Key_unknown },
        // enum ArrowType
        { "UpArrow", Qt::UpArrow },
        { "DownArrow", Qt::DownArrow },
        { "LeftArrow", Qt::LeftArrow },
        { "RightArrow", Qt::RightArrow },
        // enum RasterOp
        { "CopyROP", Qt::CopyROP },
        { "OrROP", Qt::OrROP },
        { "XorROP", Qt::XorROP },
        { "NotAndROP", Qt::NotAndROP },
        { "EraseROP", Qt::EraseROP },
        { "NotCopyROP", Qt::NotCopyROP },
        { "NotOrROP", Qt::NotOrROP },
        { "NotXorROP", Qt::NotXorROP },
        { "AndROP", Qt::AndROP },
        { "NotEraseROP", Qt::NotEraseROP },
        { "NotROP", Qt::NotROP },
        { "ClearROP", Qt::ClearROP },
        { "SetROP", Qt::SetROP },
        { "NopROP", Qt::NopROP },
        { "AndNotROP", Qt::AndNotROP },
        { "OrNotROP", Qt::OrNotROP },
        { "NandROP", Qt::NandROP },
        { "NorROP", Qt::NorROP },
        { "LastROP", Qt::LastROP },
        // enum PenStyle
        { "NoPen", Qt::NoPen },
        { "SolidLine", Qt::SolidLine },
        { "DashLine", Qt::DashLine },
        { "DotLine", Qt::DotLine },
        { "DashDotLine", Qt::DashDotLine },
        { "DashDotDotLine", Qt::DashDotDotLine },
        { "MPenStyle", Qt::MPenStyle },
        // enum PenCapStyle
        { "FlatCap", Qt::FlatCap },
        { "SquareCap", Qt::SquareCap },
        { "RoundCap", Qt::RoundCap },
        { "MPenCapStyle", Qt::MPenCapStyle },
        // enum PenJoinStyle
        { "MiterJoin", Qt::MiterJoin },
        { "BevelJoin", Qt::BevelJoin },
        { "RoundJoin", Qt::RoundJoin },
        { "MPenJoinStyle", Qt::MPenJoinStyle },
        // enum BrushStyle
        { "NoBrush", Qt::NoBrush },
        { "SolidPattern", Qt::SolidPattern },
        { "Dense1Pattern", Qt::Dense1Pattern },
        { "Dense2Pattern", Qt::Dense2Pattern },
        { "Dense3Pattern", Qt::Dense3Pattern },
        { "Dense4Pattern", Qt::Dense4Pattern },
        { "Dense5Pattern", Qt::Dense5Pattern },
        { "Dense6Pattern", Qt::Dense6Pattern },
        { "Dense7Pattern", Qt::Dense7Pattern },
        { "HorPattern", Qt::HorPattern },
        { "VerPattern", Qt::VerPattern },
        { "CrossPattern", Qt::CrossPattern },
        { "BDiagPattern", Qt::BDiagPattern },
        { "FDiagPattern", Qt::FDiagPattern },
        { "DiagCrossPattern", Qt::DiagCrossPattern },
        { "CustomPattern", Qt::CustomPattern },
        // enum MacintoshVersion
        { "MV_Unknown", Qt::MV_Unknown },
        { "MV_9", Qt::MV_9 },
        { "MV_10_DOT_0", Qt::MV_10_DOT_0 },
        { "MV_10_DOT_1", Qt::MV_10_DOT_1 },
        { "MV_10_DOT_2", Qt::MV_10_DOT_2 },
        { "MV_10_DOT_3", Qt::MV_10_DOT_3 },
        { "MV_CHEETAH", Qt::MV_CHEETAH },
        { "MV_PUMA", Qt::MV_PUMA },
        { "MV_JAGUAR", Qt::MV_JAGUAR },
        { "MV_PANTHER", Qt::MV_PANTHER },
        // enum WindowsVersion
        { "WV_32s", Qt::WV_32s },
        { "WV_95", Qt::WV_95 },
        { "WV_98", Qt::WV_98 },
        { "WV_Me", Qt::WV_Me },
        { "WV_DOS_based", Qt::WV_DOS_based },
        { "WV_NT", Qt::WV_NT },
        { "WV_2000", Qt::WV_2000 },
        { "WV_XP", Qt::WV_XP },
        { "WV_2003", Qt::WV_2003 },
        { "WV_NT_based", Qt::WV_NT_based },
        { "WV_CE", Qt::WV_CE },
        { "WV_CENET", Qt::WV_CENET },
        { "WV_CE_based", Qt::WV_CE_based },
        // enum UIEffect
        { "UI_General", Qt::UI_General },
        { "UI_AnimateMenu", Qt::UI_AnimateMenu },
        { "UI_FadeMenu", Qt::UI_FadeMenu },
        { "UI_AnimateCombo", Qt::UI_AnimateCombo },
        { "UI_AnimateTooltip", Qt::UI_AnimateTooltip },
        { "UI_FadeTooltip", Qt::UI_FadeTooltip },
        { "UI_AnimateToolBox", Qt::UI_AnimateToolBox },
        // enum CursorShape
        { "ArrowCursor", Qt::ArrowCursor },
        { "UpArrowCursor", Qt::UpArrowCursor },
        { "CrossCursor", Qt::CrossCursor },
        { "WaitCursor", Qt::WaitCursor },
        { "IbeamCursor", Qt::IbeamCursor },
        { "SizeVerCursor", Qt::SizeVerCursor },
        { "SizeHorCursor", Qt::SizeHorCursor },
        { "SizeBDiagCursor", Qt::SizeBDiagCursor },
        { "SizeFDiagCursor", Qt::SizeFDiagCursor },
        { "SizeAllCursor", Qt::SizeAllCursor },
        { "BlankCursor", Qt::BlankCursor },
        { "SplitVCursor", Qt::SplitVCursor },
        { "SplitHCursor", Qt::SplitHCursor },
        { "PointingHandCursor", Qt::PointingHandCursor },
        { "ForbiddenCursor", Qt::ForbiddenCursor },
        { "WhatsThisCursor", Qt::WhatsThisCursor },
        { "BusyCursor", Qt::BusyCursor },
        { "LastCursor", Qt::LastCursor },
        { "BitmapCursor", Qt::BitmapCursor },
        // enum TextFormat
        { "PlainText", Qt::PlainText },
        { "RichText", Qt::RichText },
        { "AutoText", Qt::AutoText },
        { "LogText", Qt::LogText },
        // enum AnchorAttribute
        { "AnchorName", Qt::AnchorName },
        { "AnchorHref", Qt::AnchorHref },
        // enum Dock
        { "DockUnmanaged", Qt::DockUnmanaged },
        { "DockTornOff", Qt::DockTornOff },
        { "DockTop", Qt::DockTop },
        { "DockBottom", Qt::DockBottom },
        { "DockRight", Qt::DockRight },
        { "DockLeft", Qt::DockLeft },
        { "DockMinimized", Qt::DockMinimized },
#if 0
        { "Unmanaged", Qt::Unmanaged },
        { "TornOff", Qt::TornOff },
        { "Top", Qt::Top },
        { "Bottom", Qt::Bottom },
        { "Right", Qt::Right },
        { "Left", Qt::Left },
        { "Minimized", Qt::Minimized },
#endif
        // enum DateFormat
        { "TextDate", Qt::TextDate },
        { "ISODate", Qt::ISODate },
        { "LocalDate", Qt::LocalDate },
        // enum TimeSpec
        { "LocalTime", Qt::LocalTime },
        { "UTC", Qt::UTC },
        // enum BackgroundMode
        { "FixedColor", Qt::FixedColor },
        { "FixedPixmap", Qt::FixedPixmap },
        { "NoBackground", Qt::NoBackground },
        { "PaletteForeground", Qt::PaletteForeground },
        { "PaletteButton", Qt::PaletteButton },
        { "PaletteLight", Qt::PaletteLight },
        { "PaletteMidlight", Qt::PaletteMidlight },
        { "PaletteDark", Qt::PaletteDark },
        { "PaletteMid", Qt::PaletteMid },
        { "PaletteText", Qt::PaletteText },
        { "PaletteBrightText", Qt::PaletteBrightText },
        { "PaletteBase", Qt::PaletteBase },
        { "PaletteBackground", Qt::PaletteBackground },
        { "PaletteShadow", Qt::PaletteShadow },
        { "PaletteHighlight", Qt::PaletteHighlight },
        { "PaletteHighlightedText", Qt::PaletteHighlightedText },
        { "PaletteButtonText", Qt::PaletteButtonText },
        { "PaletteLink", Qt::PaletteLink },
        { "PaletteLinkVisited", Qt::PaletteLinkVisited },
        { "X11ParentRelative", Qt::X11ParentRelative },
        // enum StringComparisonMode
        { "CaseSensitive", Qt::CaseSensitive },
        { "BeginsWith", Qt::BeginsWith },
        { "EndsWith", Qt::EndsWith },
        { "Contains", Qt::Contains },
        { "ExactMatch", Qt::ExactMatch },
        // enum Corner
        { "TopLeft", Qt::TopLeft },
        { "TopRight", Qt::TopRight },
        { "BottomLeft", Qt::BottomLeft },
        { "BottomRight", Qt::BottomRight },
	{ 0, 0 }
    };

    int enumidx = 0;
    while( enums[enumidx].id ) {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    }

}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void QtImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QtImp *meth = new QtImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }
}

/**
 * Extract a Qt pointer from an Object.
 */
Qt *QtImp::toQt( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<Qt *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "Qt" )
        return 0;

    return op->toNative<Qt>();
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object QtImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         default:
             break;
    }

    QString msg = i18n("QtCons has no constructor with id '%1'.").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QtImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QtImp::toQt( self );

    switch( id ) {

    default:
        break;
    }

    QString msg = i18n( "QtImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}



} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


