/***************************************************************************
                      pluginxmlparser.h  -  Part of KST
                             -------------------
    begin                : Tue May 06 2003
    copyright            : (C) 2003 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PLUGINXMLPARSER_H
#define _PLUGINXMLPARSER_H

#include "plugin.h"
#include "kst_export.h"

#include <qdom.h>
//Added by qt3to4:
#include <Q3ValueList>

class QString;

class PluginXMLParser {
  public:
    KST_EXPORT PluginXMLParser();
    KST_EXPORT ~PluginXMLParser();

    /**
     *   Parse an XML file associated with a plugin.
     *   @returns   0 - Success
     *             -1 - file not found
     *             -2 - file can't be parsed (not XML?)
     *             -3 - XML parse error
     */
    KST_EXPORT int parseFile(const QString& filename);

    const Plugin::Data& data() const;

  private:
    int parseDOM(const QDomDocument& doc);
    int parseIntro(const QDomElement& element);
    int parseInterface(const QDomElement& element);
#if 0
    int parseParalist(const QDomElement& element);
#endif
    int parseCurveHints(const QDomElement& element);
    int parseIO(const QDomElement& element, Q3ValueList<Plugin::Data::IOValue>& collection);

    Plugin::Data _pluginData;
};

#endif

// vim: ts=2 sw=2 et
