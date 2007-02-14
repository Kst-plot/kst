/***************************************************************************
                       jsiface_impl.cpp  -  Part of KST
                             -------------------
    begin                : Tue Feb 08 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "jsiface_impl.h"
#include <kjsembed/jsbinding.h>

JSIfaceImpl::JSIfaceImpl(KJSEmbed::KJSEmbedPart *part)
: DCOPObject("KstScript"), _jsPart(part) {
}


JSIfaceImpl::~JSIfaceImpl() {
}


QString JSIfaceImpl::evaluate(const QString& script) {
  KJS::Completion res;
  if (_jsPart->execute(res, script, KJS::Null())) {
    if (!res.isValueCompletion()) {
      return QString::null;
    }
    return res.value().toString(_jsPart->globalExec()).qstring();
  } else {
    KJS::UString s = res.value().toString(_jsPart->globalExec());
    if (s.isEmpty()) {
      return i18n("Unknown error.");
    }
    return i18n("Error: %1").arg(s.qstring());
  }
}


QString JSIfaceImpl::evaluateFile(const QString& filename) {
  if (_jsPart->runFile(filename)) {
    // FIXME
#if 0
    if (!_scripts.contains(filename)) {
      _scripts.append(filename);
    }
#endif
    KJS::Completion c = _jsPart->completion();
    if (!c.isValueCompletion()) {
      return QString::null;
    }
    return c.value().toString(_jsPart->globalExec()).qstring();
  } else {
    KJS::Completion c = _jsPart->completion();
    if (c.isNull()) {
      return i18n("Unknown error running script.");
    }
    return i18n("Error: %1").arg(c.toString(_jsPart->globalExec()).qstring());
  }
}


// vim: ts=2 sw=2 et
