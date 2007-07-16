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
#include "kstobject.h"
#include "kstobjectmap.h"
#include "kstobjectlist.h"
#include <qpointer.h>

class KST_EXPORT KstPrimitive : public KstObject {
  public:
    KstPrimitive(KstObject* provider = 0L);

  protected:
    ~KstPrimitive();

  public:
    // Must not be a KstObjectPtr!
//    virtual void setProvider(KstObject* obj);
    inline KstObjectPtr provider() const { return KstObjectPtr(_provider); }

    /** Update the primitive via the provider and/or internalUpdate().
        Return true if there was new data. */
    UpdateType update(int update_counter = -1);

  protected:
    virtual KstObject::UpdateType internalUpdate(KstObject::UpdateType providerRC);

    /** Possibly null.  Be careful, this is non-standard usage of a KstShared.
     * The purpose of this is to trigger hierarchical updates properly.
     */
    QPointer<KstObject> _provider;
};

typedef KstSharedPtr<KstPrimitive> KstPrimitivePtr;
typedef KstObjectList<KstPrimitivePtr> KstPrimitiveList;
typedef KstObjectMap<KstPrimitivePtr> KstPrimitiveMap;

#endif
// vim: ts=2 sw=2 et
