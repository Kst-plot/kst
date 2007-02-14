/***************************************************************************
                                    elogeventthreadsubmit.h
                             -------------------
    begin                : Feb 09 2004
    copyright            : (C) 2004 The University of British Columbia
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

#ifndef ELOGEVENTTHREADSUBMIT_H
#define ELOGEVENTTHREADSUBMIT_H

#include "elogthreadsubmit.h"

class ElogEventThreadSubmit : public ElogThreadSubmit {
  Q_OBJECT
  public:
    ElogEventThreadSubmit(KstELOG* pELOG,
                          bool bIncludeCapture,
                          bool bIncludeConfiguration,
                          bool bIncludeDebugInfo,
                          QByteArray* pByteArrayCapture,
                          QString strMessage,
                          QString strUserName,
                          QString strUserPassword,
                          QString strWritePassword,
                          QString strLogbook,
                          QString strAttributes,
                          bool bSubmitAsHTML,
                          bool bSuppressEmail);
};

#endif

// vim: ts=2 sw=2 et
