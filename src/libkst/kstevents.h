/***************************************************************************
                               kstevents.h
                              -------------
    begin                : Jan 06 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef KSTEVENTS_H
#define KSTEVENTS_H

#include <qevent.h>
#include <qstringlist.h>

class QDataStream;
class QWidget;

#define KstEventTypeThread     (QEvent::User + 1)
#define KstELOGCaptureEvent    (QEvent::User + 2)
#define KstELOGConfigureEvent  (QEvent::User + 3)
#define KstELOGAttrsEvent      (QEvent::User + 4)
#define KstELOGDebugInfoEvent  (QEvent::User + 5)
#define KstELOGAliveEvent      (QEvent::User + 6)
#define KstELOGDeathEvent      (QEvent::User + 7)
#define KstEventTypeLog        (QEvent::User + 8)

struct KstELOGCaptureStruct {
  QDataStream* pBuffer;
  int iWidth;
  int iHeight;
};

enum KstELOGAttribType {
  AttribTypeText = 0,
  AttribTypeBool,
  AttribTypeCombo,
  AttribTypeRadio,
  AttribTypeCheck
};

struct KstELOGAttribStruct {
  QString attribName;
  QString comment;
  QWidget *pWidget;
  KstELOGAttribType type;
  QStringList  values;
  bool bMandatory;
  int iMaxLength;
};

typedef QList<KstELOGAttribStruct> ELOGAttribList;

#endif

// vim: ts=2 sw=2 et
