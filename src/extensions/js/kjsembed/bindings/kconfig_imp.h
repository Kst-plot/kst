// -*- c++ -*-

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

#ifndef KJSEMBED_CONFIG_IMP_H
#define KJSEMBED_CONFIG_IMP_H

#include <qdatetime.h>
#include <qfont.h>
#include <qvariant.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qstringlist.h>

#include "bindingobject.h"

#ifndef QT_ONLY
class KConfig;
#else
class QSettings;
#endif

namespace KJSEmbed {
namespace Bindings {

class Config : public BindingObject
{
    Q_OBJECT
    Q_PROPERTY( bool forceGlobal READ forceGlobal WRITE setForceGlobal )
    Q_PROPERTY( bool dollarExpansion READ isDollarExpansion WRITE setDollarExpansion )
    Q_PROPERTY( bool readOnly READ isReadOnly WRITE setReadOnly )
    Q_PROPERTY( bool readDefaults READ readDefaults WRITE setReadDefaults )

public:
    Config( QObject *parent=0, const char *name=0);
    /*
    Config( QObject *parent, const char *name,  const QString& confName );
    */
    virtual ~Config();

    void setForceGlobal( bool force );
    bool forceGlobal() const;
    void setDollarExpansion( bool _bExpand  );
    bool isDollarExpansion() const;
    void setReadOnly(bool _ro);
    bool isReadOnly() const;
    void setReadDefaults(bool b);
    bool readDefaults() const;

public slots:

	// Group
    void setDesktopGroup();
    void setGroup(const QString&);
    QString group();
    QStringList groupList();
    QString locale();

    void rollback( bool bDeep  );
    void sync();
    bool hasKey( const QString& key ) const;
    bool entryIsImmutable(const QString &key) const;
    void revertToDefault(const QString &key);
    bool hasDefault(const QString &key) const;

    void setFileWriteMode(int mode);
    QString readEntry(const QString& pKey, const QString& aDefault ) const;
    QVariant readPropertyEntry( const QString& pKey, const QVariant &var) const;
    QStringList readListEntry( const QString& pKey ) const;
    QString readPathEntry( const QString& pKey, const QString & aDefault) const;
    QStringList readPathListEntry( const QString& pKey ) const;
    int readNumEntry( const QString& pKey, int nDefault  ) const;
    uint readUnsignedNumEntry( const QString& pKey, uint nDefault  ) const;
    long readLongNumEntry( const QString& pKey, long nDefault  ) const;
    double readDoubleNumEntry( const QString& pKey, double nDefault  ) const;
    QFont readFontEntry( const QString& pKey, const QFont* pDefault) const;
    bool readBoolEntry( const QString& pKey, const bool bDefault  ) const;
    QRect readRectEntry( const QString& pKey, const QRect* pDefault) const;
    QPoint readPointEntry( const QString& pKey, const QPoint* pDefault) const;
    QSize readSizeEntry( const QString& pKey, const QSize* pDefault) const;
    QColor readColorEntry( const QString& pKey, const QColor* pDefault) const;
    QDateTime readDateTimeEntry( const QString& pKey, const QDateTime* pDefault) const;
    QString readEntryUntranslated( const QString& pKey, const QString& aDefault  ) const;

    void writeEntry( const QString& pKey, const QString& pValue );
    void writePropertyEntry( const QString& pKey, const QVariant& pValue );
    void writeListEntry( const QString& pKey, const QStringList &rValue );
    void writeNumEntry( const QString& pKey, int nValue );
    void writeUnsignedNumEntry( const QString& pKey, uint nValue );
    void writeLongNumEntry( const QString& pKey, unsigned long nValue );
    void writeDoubleNumEntry( const QString& pKey, double nValue );
    void writeBoolEntry( const QString& pKey, bool bValue );
    void writeFontEntry( const QString& pKey, const QFont& rFont );
    void writeColorEntry( const QString& pKey, const QColor& rColor );
    void writeDateTimeEntry( const QString& pKey, const QDateTime& rDateTime  );
    void writeRectEntry( const QString& pKey, const QRect& rValue );
    void writePointEntry( const QString& pKey, const QPoint& rValue );
    void writeSizeEntry( const QString& pKey, const QSize& rValue );
    void writePathEntry( const QString& pKey, const QString & path );
    void writePathListEntry( const QString& pKey, const QStringList &rValue);
    void deleteEntry( const QString& pKey, bool bNLS , bool bGlobal );
    bool deleteGroup( const QString& group, bool bDeep , bool bGlobal  );

private:
#ifndef QT_ONLY
    KConfig *m_config;
#else
    QSettings *m_config;
    QString m_name;
    bool m_forceGlobal;
#endif
};
}
}

#endif
