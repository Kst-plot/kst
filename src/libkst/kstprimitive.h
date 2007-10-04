/***************************************************************************
                               kstprimitive.h
                             -------------------
    begin                : Tue Jun 20 2006
    copyright            : Copyright (C) 2006, The University of Toronto
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

#ifndef KSTPRIMITIVE_H
#define KSTPRIMITIVE_H

#include "kst_export.h"
#include "object.h"
#include "objectmap.h"
#include "objectlist.h"
#include <qpointer.h>

class KST_EXPORT KstPrimitive : public Kst::Object {
  public:
    KstPrimitive(Kst::Object* provider = 0L);

  protected:
    ~KstPrimitive();

  public:
    // Must not be a Kst::ObjectPtr!
//    virtual void setProvider(Kst::Object* obj);
    inline Kst::ObjectPtr provider() const { return Kst::ObjectPtr(_provider); }

    /** Update the primitive via the provider and/or internalUpdate().
        Return true if there was new data. */
    UpdateType update(int update_counter = -1);

  protected:
    virtual Kst::Object::UpdateType internalUpdate(Kst::Object::UpdateType providerRC);

    /** Possibly null.  Be careful, this is non-standard usage of a KstShared.
     * The purpose of this is to trigger hierarchical updates properly.
     */
    QPointer<Kst::Object> _provider;
};

typedef KstSharedPtr<KstPrimitive> KstPrimitivePtr;
typedef Kst::ObjectList<KstPrimitivePtr> KstPrimitiveList;
typedef Kst::ObjectMap<KstPrimitivePtr> KstPrimitiveMap;

#endif
// vim: ts=2 sw=2 et
