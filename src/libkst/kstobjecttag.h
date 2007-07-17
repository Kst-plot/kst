/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTOBJECTTAG_H
#define KSTOBJECTTAG_H

#include <qstring.h>
#include <qstringlist.h>

#include "kst_export.h"

class KstObjectTag {
  public:
    static const KstObjectTag invalidTag;

    static const QChar tagSeparator;
    static const QChar tagSeparatorReplacement;

    static const QStringList globalTagContext;
    static const QStringList constantTagContext;
    static const QStringList orphanTagContext;

    // construct a null tag
    KstObjectTag();
    // construct a tag in a given context
    KstObjectTag(const QString& tag, const QStringList& context, unsigned int minDisplayComponents = 1);
    // construct a tag in the context of another tag
    KstObjectTag(const QString& tag, const KstObjectTag& contextTag, bool alwaysShowContext = true);
    // construct a tag from a fullTag representation
    KstObjectTag(const QStringList &fullTag);

    QString tag() const;
    QStringList fullTag() const;
    QStringList context() const;

    unsigned int components() const;

    // change the tag, maintaining context
    void setTag(const QString& tag);

    // change the context
    void setContext(const QStringList& context);

    // change the tag and context
    void setTag(const QString& tag, const QStringList& context);

    bool isValid() const;

    QString tagString() const;

    // display methods
    void setUniqueDisplayComponents(unsigned int n);

    unsigned int uniqueDisplayComponents() const;

    void setMinDisplayComponents(unsigned int n);

    QStringList displayFullTag() const;

    QString displayString() const;

    // factory for String representation
    static KstObjectTag fromString(const QString& str);

    bool operator==(const KstObjectTag& tag) const;

    bool operator!=(const KstObjectTag& tag) const;

    static QString cleanTag(const QString& in_tag);

  private:
    QString _tag;
    QStringList _context;
    unsigned int _minDisplayComponents; // minimum number of components to use in display tag
    unsigned int _uniqueDisplayComponents;  // number of components necessary for unique display tag
} KST_EXPORT;

#endif

// vim: ts=2 sw=2 et

