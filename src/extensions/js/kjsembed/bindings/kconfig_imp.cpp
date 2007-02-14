/*
*  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
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

#include "kconfig_imp.h"

#include <kjsembed/global.h>

#ifndef QT_ONLY
#include <kconfig.h>
#include <kstddirs.h>
#include <kapplication.h>
#include "kconfig_imp.moc"
#else
#include <qsettings.h>
#endif
namespace KJSEmbed {
namespace Bindings {

Config::Config( QObject *parent, const char *name)
    : BindingObject(parent, name)
{

#ifndef QT_ONLY
	if( name == 0)
		m_config = kapp->config();
	else
		m_config = new KConfig(name);
	if( !m_config->checkConfigFilesWritable(true) )
	{
	  kdWarning( 80001 ) << "Could not write to config file." << endl;
	}
#else
	m_config = new QSettings();
	if( name == 0)
	  m_name = "qjcmd";
	else
	  m_name = name;
	m_config->setPath( "QJSEmbed",m_name, QSettings::User );
	m_forceGlobal = false;
#endif
}

/*
Config::Config( QObject *parent, const char *name , const QString& confName)
        : BindingObject(parent, name)
{
    QString fileName = ::locate("kde_config", confName);
    m_config = new KConfig(fileName);
}
*/
Config::~Config()
{
#ifndef QT_ONLY
	if( kapp->config() != m_config )
#endif
    delete m_config;
}

void Config::setDesktopGroup()
{
#ifndef QT_ONLY
    m_config->setDesktopGroup();
#else
    m_config->resetGroup();
    m_config->beginGroup("/Desktop Entry");
#endif 
}

void Config::setGroup(const QString& group)
{
#ifndef QT_ONLY
    m_config->setGroup(group);
#else
   m_config->resetGroup();
   m_config->beginGroup(group);
#endif 
}

QString Config::group()
{
    return m_config->group();
}

QStringList Config::groupList()
{
#ifndef QT_ONLY
    return m_config->groupList();
#else
    m_config->resetGroup();
    return m_config->subkeyList(group());
#endif 
}

QString Config::locale()
{
#ifndef QT_ONLY
    return m_config->locale();
#else
    return "C";
#endif
}

void Config::setForceGlobal( bool force )
{
#ifndef QT_ONLY
    m_config->setForceGlobal(force);
#else
    m_forceGlobal = force; 
    m_config->setPath( "QJSEmbed",m_name, (force)?QSettings::Global:QSettings::User );
#endif
}

bool Config::forceGlobal( ) const
{
#ifndef QT_ONLY
    return m_config->forceGlobal();
#else
    return m_forceGlobal;
#endif
}

void Config::setDollarExpansion( bool _bExpand )
{
#ifndef QT_ONLY
    m_config->setDollarExpansion(_bExpand);
#else
    Q_UNUSED(_bExpand);
#endif 
}

bool Config::isDollarExpansion() const
{
#ifndef QT_ONLY
    return m_config->isDollarExpansion();
#else
    return false;
#endif 
}

void Config::setReadOnly(bool _ro)
{
#ifndef QT_ONLY
    m_config->setReadOnly(_ro);
#else
    Q_UNUSED(_ro);
#endif 
}

bool Config::isReadOnly() const
{
#ifndef QT_ONLY
    return m_config->isReadOnly();
#else
    return false;
#endif
}

void Config::setReadDefaults(bool b)
{
#ifndef QT_ONLY
    m_config->setReadDefaults(b);
#else
    Q_UNUSED(b);
#endif
}

bool Config::readDefaults() const
{
#ifndef QT_ONLY
    return m_config->readDefaults();
#else
    return false;
#endif
}

void Config::rollback( bool bDeep  )
{
#ifndef QT_ONLY
    m_config->rollback(bDeep);
#else
    Q_UNUSED(bDeep);
#endif 
}

void Config::sync()
{
    m_config->sync();
}

bool Config::hasKey( const QString& key ) const
{
#ifndef QT_ONLY
    return m_config->hasKey(key);
#else
      return false;
#endif 
}

bool Config::entryIsImmutable(const QString &key) const
{
#ifndef QT_ONLY
    return m_config->entryIsImmutable(key);
#else
    Q_UNUSED( key );
    return false; 
#endif
}

void Config::revertToDefault(const QString &key)
{
#ifndef QT_ONLY
    m_config->revertToDefault(key);
#else
    Q_UNUSED(key);
#endif 
}

bool Config::hasDefault(const QString &key) const
{
#ifndef QT_ONLY
    return m_config->hasDefault(key);
#else
    Q_UNUSED(key);
   return false;
#endif 
}

void Config::setFileWriteMode(int mode)
{
#ifndef QT_ONLY
    m_config->setFileWriteMode(mode);
#else
    Q_UNUSED(mode);
#endif  
}

QString Config::readEntry(const QString& pKey, const QString& aDefault ) const
{
    return m_config->readEntry(pKey,aDefault);
}

QVariant Config::readPropertyEntry( const QString& pKey, const QVariant &pDefault) const
{
#ifndef QT_ONLY
  return m_config->readPropertyEntry(pKey,pDefault);
#else
  QVariant returnVariant;
  returnVariant = m_config->readEntry(pKey, pDefault.toString());
  return returnVariant;
#endif 
}

QStringList Config::readListEntry( const QString& pKey  ) const
{
    return m_config->readListEntry(pKey);
}

QString Config::readPathEntry( const QString& pKey, const QString & aDefault ) const
{
#ifndef QT_ONLY
    return m_config->readPathEntry(pKey,aDefault);
#else
    return readEntry(pKey,aDefault);
#endif 
}

QStringList Config::readPathListEntry( const QString& pKey  ) const
{
#ifndef QT_ONLY
    return m_config->readPathListEntry(pKey);
#else
    return readListEntry(pKey);
#endif 
 
}

int Config::readNumEntry( const QString& pKey, int nDefault) const
{
    return m_config->readNumEntry(pKey,nDefault);
}

uint Config::readUnsignedNumEntry( const QString& pKey, uint nDefault) const
{
#ifndef QT_ONLY
    return m_config->readUnsignedNumEntry(pKey,nDefault);
#else
    return (uint)readNumEntry(pKey,nDefault);
#endif 
}

long Config::readLongNumEntry( const QString& pKey, long nDefault) const
{
#ifndef QT_ONLY
    return m_config->readLongNumEntry(pKey,nDefault);
#else
    return (long)readNumEntry(pKey,nDefault);
#endif 
}

double Config::readDoubleNumEntry( const QString& pKey, double nDefault ) const
{
#ifndef QT_ONLY
    return m_config->readDoubleNumEntry(pKey,nDefault);
#else
    return m_config->readDoubleEntry(pKey,nDefault);
#endif 
}

QFont Config::readFontEntry( const QString& pKey, const QFont* pDefault ) const
{
#ifndef QT_ONLY
    return m_config->readFontEntry(pKey,pDefault);
#else
    return readPropertyEntry(pKey,QVariant(*pDefault)).toFont();
#endif 
}

bool Config::readBoolEntry( const QString& pKey, const bool bDefault ) const
{
    return m_config->readBoolEntry(pKey,bDefault);
}

QRect Config::readRectEntry( const QString& pKey, const QRect* pDefault ) const
{
#ifndef QT_ONLY
  return m_config->readRectEntry(pKey,pDefault);
#else
    return readPropertyEntry(pKey,QVariant(*pDefault)).toRect();
#endif 
}

QPoint Config::readPointEntry( const QString& pKey, const QPoint* pDefault ) const
{
#ifndef QT_ONLY
    return m_config->readPointEntry(pKey,pDefault);
#else
    return readPropertyEntry(pKey,QVariant(*pDefault)).toPoint();
#endif 
}

QSize Config::readSizeEntry( const QString& pKey, const QSize* pDefault  ) const
{
#ifndef QT_ONLY
    return m_config->readSizeEntry(pKey,pDefault);
#else
    return readPropertyEntry(pKey,QVariant(*pDefault)).toSize();
#endif 
}

QColor Config::readColorEntry( const QString& pKey, const QColor* pDefault ) const
{
#ifndef QT_ONLY
    return m_config->readColorEntry(pKey,pDefault);
#else
    return readPropertyEntry(pKey,QVariant(*pDefault)).toColor();
#endif 
}

QDateTime Config::readDateTimeEntry( const QString& pKey, const QDateTime* pDefault) const
{
#ifndef QT_ONLY
    return m_config->readDateTimeEntry(pKey,pDefault);
#else
    return readPropertyEntry(pKey,QVariant(*pDefault)).toDateTime();
#endif 
}

QString Config::readEntryUntranslated( const QString& pKey, const QString& aDefault ) const
{
#ifndef QT_ONLY
    return m_config->readEntryUntranslated(pKey,aDefault);
#else
    return m_config->readEntry(pKey,aDefault);
#endif
}
void Config::writeEntry( const QString& pKey, const QString& pValue  )
{
    m_config->writeEntry(pKey,pValue);
}

void Config::writePropertyEntry( const QString& pKey, const QVariant& pValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, pValue   );
#else
    m_config->writeEntry(pKey,pValue.toString());
#endif 
}

void Config::writeListEntry( const QString& pKey, const QStringList &rValue   )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rValue   );
#else
    m_config->writeEntry(pKey,rValue);
#endif 
}

void Config::writeNumEntry( const QString& pKey, int nValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, nValue  );
#else 
       m_config->writeEntry(pKey,nValue); 
#endif
}
void Config::writeUnsignedNumEntry( const QString& pKey, uint nValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, nValue   );
#else
    writeNumEntry(pKey, (int)nValue );
#endif 
}
void Config::writeLongNumEntry( const QString& pKey, unsigned long nValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, nValue   );
#else
    writeNumEntry(pKey, (int)nValue);
#endif
}
void Config::writeDoubleNumEntry( const QString& pKey, double nValue )
{
    m_config->writeEntry(pKey,nValue);
}
void Config::writeBoolEntry( const QString& pKey, bool bValue  )
{
    m_config->writeEntry(pKey,bValue);
}
void Config::writeFontEntry( const QString& pKey, const QFont& rFont  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rFont   );
#else
    writePropertyEntry(pKey,rFont);
#endif 
 
}
void Config::writeColorEntry( const QString& pKey, const QColor& rColor  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rColor   );
#else
    writePropertyEntry(pKey,rColor);
#endif 
 
}
void Config::writeDateTimeEntry( const QString& pKey, const QDateTime& rDateTime  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rDateTime   );
#else
    writePropertyEntry(pKey,rDateTime);
#endif 
 
}
void Config::writeRectEntry( const QString& pKey, const QRect& rValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rValue   );
#else
    writePropertyEntry(pKey,rValue);
#endif 
 
}
void Config::writePointEntry( const QString& pKey, const QPoint& rValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rValue   );
#else
    writePropertyEntry(pKey,rValue);
#endif 
 
}
void Config::writeSizeEntry( const QString& pKey, const QSize& rValue  )
{
#ifndef QT_ONLY
    m_config->writeEntry(pKey, rValue    );
#else
    writePropertyEntry(pKey,rValue);
#endif 
}
void Config::writePathEntry( const QString& pKey, const QString & path  )
{
#ifndef QT_ONLY
    m_config->writePathEntry(pKey,path);
#else
    writeEntry(pKey,path);
#endif
}
void Config::writePathListEntry( const QString& pKey, const QStringList &rValue   )
{
#ifndef QT_ONLY
    m_config->writePathEntry(pKey,rValue);
#else
    writeListEntry(pKey,rValue);
#endif 
}
void Config::deleteEntry( const QString& pKey, bool bNLS , bool bGlobal )
{
#ifndef QT_ONLY
    m_config->deleteEntry(pKey,bNLS,bGlobal);
#else
    Q_UNUSED(bNLS);
    Q_UNUSED(bGlobal);
    m_config->removeEntry(pKey);
#endif 
}
bool Config::deleteGroup( const QString& group, bool bDeep  , bool bGlobal  )
{
#ifndef QT_ONLY
    return m_config->deleteGroup(group,bDeep,bGlobal);
#else
    return false;
#endif
}

}
}
