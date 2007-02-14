/***************************************************************************
                                   elogeventthreadsubmit.cpp
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

#include <klocale.h>

#include "elogeventthreadsubmit.h"

ElogEventThreadSubmit::ElogEventThreadSubmit(KstELOG* elog,
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
                                            bool bSuppressEmail)
: ElogThreadSubmit( elog,
                    bIncludeCapture,
                    bIncludeConfiguration,
                    bIncludeDebugInfo,
                    pByteArrayCapture,
                    strMessage,
                    strUserName,
                    strUserPassword,
                    strWritePassword,
                    strLogbook,
                    strAttributes,
                    bSubmitAsHTML,
                    bSuppressEmail) {
  _strType = i18n("Elog event entry");
}

#include "elogeventthreadsubmit.moc"

// vim: ts=2 sw=2 et
