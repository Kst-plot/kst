/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2006 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "kst_export.h"
#include "object.h"
#include "objectmap.h"
#include "objectlist.h"
#include <qpointer.h>

namespace Kst {

class KST_EXPORT Primitive : public Object {
  public:
    Primitive(Object* provider = 0L);

  protected:
    ~Primitive();

  public:
    // Must not be a ObjectPtr!
//    virtual void setProvider(Object* obj);
    inline ObjectPtr provider() const { return ObjectPtr(_provider); }

    /** Update the primitive via the provider and/or internalUpdate().
        Return true if there was new data. */
    UpdateType update(int update_counter = -1);

  protected:
    virtual Object::UpdateType internalUpdate(Object::UpdateType providerRC);

    /** Possibly null.  Be careful, this is non-standard usage of a KstShared.
     * The purpose of this is to trigger hierarchical updates properly.
     */
    QPointer<Object> _provider;
};

typedef SharedPtr<Primitive> PrimitivePtr;
typedef ObjectList<PrimitivePtr> PrimitiveList;
typedef ObjectMap<PrimitivePtr> PrimitiveMap;

}
#endif
// vim: ts=2 sw=2 et
