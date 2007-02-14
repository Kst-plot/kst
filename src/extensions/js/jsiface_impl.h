/***************************************************************************
                      jsiface_impl.h  -  Part of KST
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

#ifndef JSIFACE_IMPL_H
#define JSIFACE_IMPL_H

#include "jsiface.h"
#include <kjsembed/kjsembedpart.h>

// Warning: If you change something here, you could break existing scripts.

class JSIfaceImpl : public JSIface {
  public:
    JSIfaceImpl(KJSEmbed::KJSEmbedPart*);
    ~JSIfaceImpl();

    QString evaluate(const QString& script);
    QString evaluateFile(const QString& filename);

  protected:
    KJSEmbed::KJSEmbedPart *_jsPart;
};

#endif
// vim: ts=2 sw=2 et
