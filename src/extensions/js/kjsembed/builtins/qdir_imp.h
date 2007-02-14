
  

#ifndef KJSEMBED_QDir_H
#define KJSEMBED_QDir_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>
#include <kjsembed/jsbindingbase.h>

class QDir;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {
    class QDirLoader : public JSBindingBase
    {
        public:
            KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
            void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
    };
}
/**
 * Wrapper class for QDir methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QDirImp : public JSProxyImp
{
public:
   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_setPath_7,
       Method_path_8,
       Method_absPath_9,
       Method_canonicalPath_10,
       Method_dirName_11,
       Method_filePath_12,
       Method_absFilePath_13,
       Method_cd_14,
       Method_cdUp_15,
       Method_nameFilter_16,
       Method_setNameFilter_17,
       Method_filter_18,
       Method_setFilter_19,
       Method_sorting_20,
       Method_setSorting_21,
       Method_matchAllDirs_22,
       Method_setMatchAllDirs_23,
       Method_count_24,
       Method_encodedEntryList_26,
       Method_encodedEntryList_27,
       Method_entryList_28,
       Method_entryList_29,
       Method_entryInfoList_30,
       Method_entryInfoList_31,
       Method_mkdir_32,
       Method_rmdir_33,
       Method_isReadable_34,
       Method_exists_35,
       Method_isRoot_36,
       Method_isRelative_37,
       Method_convertToAbs_38,
       Method_remove_41,
       Method_rename_42,
       Method_exists_43,
       Method_refresh_44,
       Method_convertSeparators_45,
       Method_drives_46,
       Method_separator_47,
       Method_setCurrent_48,
       Method_current_49,
       Method_home_50,
       Method_root_51,
       Method_currentDirPath_52,
       Method_homeDirPath_53,
       Method_rootDirPath_54,
       Method_match_55,
       Method_match_56,
       Method_cleanDirPath_57,
       Method_isRelativePath_58,
       Method_Last = -1
   };

   QDirImp( KJS::ExecState *exec, int id );
   ~QDirImp();

   static void addBindings( KJS::ExecState *exec, KJS::Object &object );

   //
   // Methods implemented by this class.
   //

   KJS::Value setPath_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value path_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value absPath_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value canonicalPath_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value dirName_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value filePath_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value absFilePath_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value cd_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value cdUp_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value nameFilter_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setNameFilter_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value filter_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setFilter_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sorting_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSorting_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value matchAllDirs_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setMatchAllDirs_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value count_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value encodedEntryList_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value encodedEntryList_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value entryList_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value entryList_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value entryInfoList_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value entryInfoList_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value mkdir_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value rmdir_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isReadable_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value exists_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isRoot_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isRelative_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value convertToAbs_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value remove_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value rename_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value exists_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value refresh_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value convertSeparators_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value drives_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value separator_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setCurrent_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value current_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value home_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value root_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value currentDirPath_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value homeDirPath_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value rootDirPath_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value match_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value match_56( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value cleanDirPath_57( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isRelativePath_58( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

   //
   // JS binding code.
   //

   /** Returns true iff this object implements the call function. */
   virtual bool implementsCall() const { return true; }

   /** Invokes the call function. */
   virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
   QDir *instance; // Temp
   int mid;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QDir_H

// Local Variables:
// c-basic-offset: 4
// End:


