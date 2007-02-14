/***************************************************************************
                          jsiface.h  -  Part of KST
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
#ifndef JSIFACE_H
#define JSIFACE_H

#include <dcopobject.h>
#include <dcopref.h>
#include <qstringlist.h>  //Qt 3.1.2 KDE 3.1.4

// Warning: If you change something here, you could break existing scripts.

class JSIface : virtual public DCOPObject {
  K_DCOP
  k_dcop:
    virtual QString evaluate(const QString& script) = 0;
    virtual QString evaluateFile(const QString& filename) = 0;
};

#endif
// vim: ts=2 sw=2 et
