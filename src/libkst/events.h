/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#include <qevent.h>
#include <qstringlist.h>

class QDataStream;
class QWidget;

namespace Kst {

#define EventTypeThread     (QEvent::User + 1)
#define ELOGCaptureEvent    (QEvent::User + 2)
#define ELOGConfigureEvent  (QEvent::User + 3)
#define ELOGAttrsEvent      (QEvent::User + 4)
#define ELOGDebugInfoEvent  (QEvent::User + 5)
#define ELOGAliveEvent      (QEvent::User + 6)
#define ELOGDeathEvent      (QEvent::User + 7)
#define EventTypeLog        (QEvent::User + 8)

struct ELOGCaptureStruct {
  QDataStream* pBuffer;
  int iWidth;
  int iHeight;
};

enum ELOGAttribType {
  AttribTypeText = 0,
  AttribTypeBool,
  AttribTypeCombo,
  AttribTypeRadio,
  AttribTypeCheck
};

struct ELOGAttribStruct {
  QString attribName;
  QString comment;
  QWidget *pWidget;
  ELOGAttribType type;
  QStringList  values;
  bool bMandatory;
  int iMaxLength;
};

typedef QList<ELOGAttribStruct> ELOGAttribList;

}

#endif

// vim: ts=2 sw=2 et
