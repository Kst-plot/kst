


#include <kjsembed/global.h>

#include <kjs/object.h>

#ifndef QT_ONLY
#include <klocale.h>
#include <kdebug.h>
#endif


#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qdir.h>
#include "qdir_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {
    KJS::Object QDirLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
    {
        JSOpaqueProxy * prx;
        if ( args.size() == 0 ) {
            prx = new JSOpaqueProxy( new QDir( QDir::current() ), "QDir" );
        } else {
            QString arg0 = ( args.size() >= 1 ) ? args[ 0 ].toString( exec ).qstring() : QString::null;
            prx = new JSOpaqueProxy( new QDir( arg0 ), "QDir" );
        }
        prx->setOwner( JSProxy::JavaScript );
        KJS::Object proxyObj( prx );
        addBindings( jspart, exec, proxyObj );
        return proxyObj;
    }

    void QDirLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
    {
        QDirImp::addBindings( exec, proxy );
    }

}

QDirImp::QDirImp( KJS::ExecState *exec, int id )
   : JSProxyImp(exec), mid(id)
{
}

QDirImp::~QDirImp()
{
}

void QDirImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

    { Method_setPath_7, "setPath" },

    { Method_path_8, "path" },

    { Method_absPath_9, "absPath" },

    { Method_canonicalPath_10, "canonicalPath" },

    { Method_dirName_11, "dirName" },

    { Method_filePath_12, "filePath" },

    { Method_absFilePath_13, "absFilePath" },

    { Method_cd_14, "cd" },

    { Method_cdUp_15, "cdUp" },

    { Method_nameFilter_16, "nameFilter" },

    { Method_setNameFilter_17, "setNameFilter" },

    { Method_filter_18, "filter" },

    { Method_setFilter_19, "setFilter" },

    { Method_sorting_20, "sorting" },

    { Method_setSorting_21, "setSorting" },

    { Method_matchAllDirs_22, "matchAllDirs" },

    { Method_setMatchAllDirs_23, "setMatchAllDirs" },

    { Method_count_24, "count" },

    { Method_encodedEntryList_26, "encodedEntryList" },

    { Method_encodedEntryList_27, "encodedEntryList" },

    { Method_entryList_28, "entryList" },

    { Method_entryList_29, "entryList" },

    { Method_entryInfoList_30, "entryInfoList" },

    { Method_entryInfoList_31, "entryInfoList" },

    { Method_mkdir_32, "mkdir" },

    { Method_rmdir_33, "rmdir" },

    { Method_isReadable_34, "isReadable" },

    { Method_exists_35, "exists" },

    { Method_isRoot_36, "isRoot" },

    { Method_isRelative_37, "isRelative" },

    { Method_convertToAbs_38, "convertToAbs" },

    { Method_remove_41, "remove" },

    { Method_rename_42, "rename" },

    { Method_exists_43, "exists" },

    { Method_refresh_44, "refresh" },

    { Method_convertSeparators_45, "convertSeparators" },

    { Method_drives_46, "drives" },

    { Method_separator_47, "separator" },

    { Method_setCurrent_48, "setCurrent" },

    { Method_current_49, "current" },

    { Method_home_50, "home" },

    { Method_root_51, "root" },

    { Method_currentDirPath_52, "currentDirPath" },

    { Method_homeDirPath_53, "homeDirPath" },

    { Method_rootDirPath_54, "rootDirPath" },

    { Method_match_55, "match" },

    { Method_match_56, "match" },

    { Method_cleanDirPath_57, "cleanDirPath" },

    { Method_isRelativePath_58, "isRelativePath" },

	{ 0, 0 }
    };

    int idx = 0;
    do {
        QDirImp *meth = new QDirImp( exec, methods[idx].id );
        object.put( exec , methods[idx].name, KJS::Object(meth) );
        ++idx;
    } while( methods[idx].id );

    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };


    EnumValue enums[] = {

        // enum FilterSpec
        { "Dirs", QDir::Dirs },
        { "Files", QDir::Files },
        { "Drives", QDir::Drives },
        { "NoSymLinks", QDir::NoSymLinks },
        { "All", QDir::All },
        { "TypeMask", QDir::TypeMask },
        { "Readable", QDir::Readable },
        { "Writable", QDir::Writable },
        { "Executable", QDir::Executable },
        { "RWEMask", QDir::RWEMask },
        { "Modified", QDir::Modified },
        { "Hidden", QDir::Hidden },
        { "System", QDir::System },
        { "AccessMask", QDir::AccessMask },
        { "DefaultFilter", QDir::DefaultFilter },
        // enum SortSpec
        { "Name", QDir::Name },
        { "Time", QDir::Time },
        { "Size", QDir::Size },
        { "Unsorted", QDir::Unsorted },
        { "SortByMask", QDir::SortByMask },
        { "DirsFirst", QDir::DirsFirst },
        { "Reversed", QDir::Reversed },
        { "IgnoreCase", QDir::IgnoreCase },
        { "DefaultSort", QDir::DefaultSort },
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );

}


KJS::Value QDirImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op ) {
        kdWarning() << "QDirImp::call() failed, not a JSOpaqueProxy" << endl;
        return KJS::Value();
    }

    if ( op->typeName() != "QDir" ) {
        kdWarning() << "QDirImp::call() failed, type is " << op->typeName() << endl;
	return KJS::Value();
    }

    instance =  op->toNative<QDir>();

    switch( mid ) {

    case Method_setPath_7:
        return setPath_7( exec, self, args );
        break;

    case Method_path_8:
        return path_8( exec, self, args );
        break;

    case Method_absPath_9:
        return absPath_9( exec, self, args );
        break;

    case Method_canonicalPath_10:
        return canonicalPath_10( exec, self, args );
        break;

    case Method_dirName_11:
        return dirName_11( exec, self, args );
        break;

    case Method_filePath_12:
        return filePath_12( exec, self, args );
        break;

    case Method_absFilePath_13:
        return absFilePath_13( exec, self, args );
        break;

    case Method_cd_14:
        return cd_14( exec, self, args );
        break;

    case Method_cdUp_15:
        return cdUp_15( exec, self, args );
        break;

    case Method_nameFilter_16:
        return nameFilter_16( exec, self, args );
        break;

    case Method_setNameFilter_17:
        return setNameFilter_17( exec, self, args );
        break;

    case Method_filter_18:
        return filter_18( exec, self, args );
        break;

    case Method_setFilter_19:
        return setFilter_19( exec, self, args );
        break;

    case Method_sorting_20:
        return sorting_20( exec, self, args );
        break;

    case Method_setSorting_21:
        return setSorting_21( exec, self, args );
        break;

    case Method_matchAllDirs_22:
        return matchAllDirs_22( exec, self, args );
        break;

    case Method_setMatchAllDirs_23:
        return setMatchAllDirs_23( exec, self, args );
        break;

    case Method_count_24:
        return count_24( exec, self, args );
        break;

    case Method_encodedEntryList_26:
        return encodedEntryList_26( exec, self, args );
        break;

    case Method_encodedEntryList_27:
        return encodedEntryList_27( exec, self, args );
        break;

    case Method_entryList_28:
        return entryList_28( exec, self, args );
        break;

    case Method_entryList_29:
        return entryList_29( exec, self, args );
        break;

    case Method_entryInfoList_30:
        return entryInfoList_30( exec, self, args );
        break;

    case Method_entryInfoList_31:
        return entryInfoList_31( exec, self, args );
        break;

    case Method_mkdir_32:
        return mkdir_32( exec, self, args );
        break;

    case Method_rmdir_33:
        return rmdir_33( exec, self, args );
        break;

    case Method_isReadable_34:
        return isReadable_34( exec, self, args );
        break;

    case Method_exists_35:
        return exists_35( exec, self, args );
        break;

    case Method_isRoot_36:
        return isRoot_36( exec, self, args );
        break;

    case Method_isRelative_37:
        return isRelative_37( exec, self, args );
        break;

    case Method_convertToAbs_38:
        return convertToAbs_38( exec, self, args );
        break;

    case Method_remove_41:
        return remove_41( exec, self, args );
        break;

    case Method_rename_42:
        return rename_42( exec, self, args );
        break;

    case Method_exists_43:
        return exists_43( exec, self, args );
        break;

    case Method_refresh_44:
        return refresh_44( exec, self, args );
        break;

    case Method_convertSeparators_45:
        return convertSeparators_45( exec, self, args );
        break;

    case Method_drives_46:
        return drives_46( exec, self, args );
        break;

    case Method_separator_47:
        return separator_47( exec, self, args );
        break;

    case Method_setCurrent_48:
        return setCurrent_48( exec, self, args );
        break;

    case Method_current_49:
        return current_49( exec, self, args );
        break;

    case Method_home_50:
        return home_50( exec, self, args );
        break;

    case Method_root_51:
        return root_51( exec, self, args );
        break;

    case Method_currentDirPath_52:
        return currentDirPath_52( exec, self, args );
        break;

    case Method_homeDirPath_53:
        return homeDirPath_53( exec, self, args );
        break;

    case Method_rootDirPath_54:
        return rootDirPath_54( exec, self, args );
        break;

    case Method_match_55:
        return match_55( exec, self, args );
        break;

    case Method_match_56:
        return match_56( exec, self, args );
        break;

    case Method_cleanDirPath_57:
        return cleanDirPath_57( exec, self, args );
        break;

    case Method_isRelativePath_58:
        return isRelativePath_58( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QDirImp has no method with id '%1'." ).arg( mid );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QDirImp::setPath_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      instance->setPath(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::path_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->path(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::absPath_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->absPath(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::canonicalPath_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->canonicalPath(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::dirName_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->dirName(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::filePath_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      QString ret;
      ret = instance->filePath(
       arg0,
       arg1 );
      return KJS::String( ret );

}

KJS::Value QDirImp::absFilePath_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      QString ret;
      ret = instance->absFilePath(
       arg0,
       arg1 );
      return KJS::String( ret );

}

KJS::Value QDirImp::cd_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      bool ret;
      ret = instance->cd(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::cdUp_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->cdUp(  );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::nameFilter_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->nameFilter(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::setNameFilter_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      instance->setNameFilter(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::filter_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->filter(  );
      return KJS::Value(); // Returns 'FilterSpec'

}

KJS::Value QDirImp::setFilter_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->setFilter(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::sorting_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->sorting(  );
      return KJS::Value(); // Returns 'SortSpec'

}

KJS::Value QDirImp::setSorting_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->setSorting(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::matchAllDirs_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->matchAllDirs(  );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::setMatchAllDirs_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setMatchAllDirs(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::count_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      uint ret;
      ret = instance->count(  );
      return KJS::Number( ret );

}

KJS::Value QDirImp::encodedEntryList_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      instance->encodedEntryList(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'QStrList'

}

KJS::Value QDirImp::encodedEntryList_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

    int arg2 = (args.size() >= 3) ? args[2].toInteger(exec) : -1;

      instance->encodedEntryList(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns 'QStrList'

}

KJS::Value QDirImp::entryList_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      QStringList ret;
      ret = instance->entryList(
       arg0,
       arg1 );

       return convertToValue( exec, ret );

}

KJS::Value QDirImp::entryList_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

    int arg2 = (args.size() >= 3) ? args[2].toInteger(exec) : -1;

      QStringList ret;
      ret = instance->entryList(
       arg0,
       arg1,
       arg2 );

       return convertToValue( exec, ret );

}

KJS::Value QDirImp::entryInfoList_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      instance->entryInfoList(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'const QFileInfoList *'

}

KJS::Value QDirImp::entryInfoList_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

    int arg2 = (args.size() >= 3) ? args[2].toInteger(exec) : -1;

      instance->entryInfoList(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns 'const QFileInfoList *'

}

KJS::Value QDirImp::mkdir_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      bool ret;
      ret = instance->mkdir(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::rmdir_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      bool ret;
      ret = instance->rmdir(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::isReadable_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isReadable(  );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::exists_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->exists(  );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::isRoot_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isRoot(  );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::isRelative_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isRelative(  );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::convertToAbs_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->convertToAbs(  );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::remove_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      bool ret;
      ret = instance->remove(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::rename_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    QString arg1 = (args.size() >= 2) ? args[1].toString(exec).qstring() : QString::null;

    bool arg2 = (args.size() >= 3) ? args[2].toBoolean(exec) : false;

      bool ret;
      ret = instance->rename(
       arg0,
       arg1,
       arg2 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::exists_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      bool ret;
      ret = instance->exists(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::refresh_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->refresh(  );
      return KJS::Value(); // Returns void

}

KJS::Value QDirImp::convertSeparators_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      QString ret;
      ret = instance->convertSeparators(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QDirImp::drives_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->drives(  );
      return KJS::Value(); // Returns 'const QFileInfoList *'

}

KJS::Value QDirImp::separator_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->separator(  );
      return KJS::Value(); // Returns 'char'

}

KJS::Value QDirImp::setCurrent_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      bool ret;
      ret = instance->setCurrent(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::current_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->current(  );
      return KJS::Value(); // Returns 'QDir'

}

KJS::Value QDirImp::home_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->home(  );
      return KJS::Value(); // Returns 'QDir'

}

KJS::Value QDirImp::root_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->root(  );
      return KJS::Value(); // Returns 'QDir'

}

KJS::Value QDirImp::currentDirPath_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->currentDirPath(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::homeDirPath_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->homeDirPath(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::rootDirPath_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->rootDirPath(  );
      return KJS::String( ret );

}

KJS::Value QDirImp::match_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QStringList arg0;
    if ( args.size() >= 1 ) {
      // TODO: populate the list
    }

    QString arg1 = (args.size() >= 2) ? args[1].toString(exec).qstring() : QString::null;

      bool ret;
      ret = instance->match(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::match_56( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    QString arg1 = (args.size() >= 2) ? args[1].toString(exec).qstring() : QString::null;

      bool ret;
      ret = instance->match(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QDirImp::cleanDirPath_57( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      QString ret;
      ret = instance->cleanDirPath(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QDirImp::isRelativePath_58( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      bool ret;
      ret = instance->isRelativePath(
       arg0 );
      return KJS::Boolean( ret );

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


