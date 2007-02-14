/***************************************************************************
                      kstgfxmousehandler.h  -  Part of KST
                             -------------------
    begin                : 2005
    copyright            : (C) 2005 The University of British Columbia
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

#ifndef KSTGFXMOUSEHANDLER_H
#define KSTGFXMOUSEHANDLER_H

#include <qguardedptr.h>
#include "kstobject.h"

class KstTopLevelView;
class KstViewObject;

typedef KstSharedPtr<KstViewObject> KstViewObjectPtr;
typedef KstSharedPtr<KstTopLevelView> KstTopLevelViewPtr;

class KstGfxMouseHandler {
  public:
    KstGfxMouseHandler();
    ~KstGfxMouseHandler();

    virtual void handlePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift);
    virtual void pressMove(KstTopLevelViewPtr view, const QPoint& pos, bool shift, const QRect& geom)  = 0;
    virtual void releasePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) = 0;
    virtual void updateFocus(KstTopLevelViewPtr view, const QPoint& pos);
    virtual void cancelMouseOperations(KstTopLevelViewPtr view);

    void saveDefaults(KstViewObjectPtr obj);

  protected:
    // copy defaults to newObj
    void copyDefaults(KstViewObjectPtr newObj);

    KstViewObjectPtr _defaultObject;
    QPoint _mouseOrigin;
    QRect _prevBand;
    bool _mouseMoved;
    bool _cancelled;
    bool _mouseDown;
};

#endif

// vim: ts=2 sw=2 et
