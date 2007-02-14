/***************************************************************************
                 kstmatrix.h - a matrix with nX*nY editable points
                             -------------------
    begin                : July 2005
    copyright            : (C) 2005 by University of British Columbia
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
#ifndef KSTAMATRIX_H
#define KSTAMATRIX_H

#include "kstmatrix.h"
#include "kst_export.h"

class KST_EXPORT KstAMatrix : public KstMatrix {
  public:
    KstAMatrix(const QDomElement &e);
    KstAMatrix(KstObjectTag in_tag, uint nX, uint nY, double minX, double minY, double stepX, double stepY);

    virtual void save(QTextStream &ts, const QString& indent = QString::null);
};

typedef KstSharedPtr<KstAMatrix> KstAMatrixPtr;
typedef KstObjectList<KstAMatrixPtr> KstAMatrixList;



#endif
// vim: ts=2 sw=2 et
