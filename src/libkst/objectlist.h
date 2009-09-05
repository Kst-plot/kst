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
#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QList>
#include <QStringList>

#include "rwlock.h"
#include "sharedptr.h"

namespace Kst {

template<class T>
class ObjectList : public QList<SharedPtr<T> > {
};

}
#endif

// vim: ts=2 sw=2 et
