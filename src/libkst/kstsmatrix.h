/***************************************************************************
        kstsmatrix.h  - a gradient from gradZMin, gradZMax that goes across
                        in x or y direction 
                             -------------------
    begin                : July, 2005
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
#ifndef KSTSMATRIX_H
#define KSTSMATRIX_H

#include "kstmatrix.h"
#include "kst_export.h"

class KST_EXPORT KstSMatrix : public KstMatrix {
  public:
    KstSMatrix(const QDomElement &e);
    KstSMatrix(KstObjectTag tag, uint nX, uint nY,
               double minX, double minY, double stepX, double stepY,
               double gradZMin, double gradZMax, bool xDirection);

    virtual void save(QTextStream &ts, const QString& indent = QString::null);

    void change(KstObjectTag tag, uint nX, uint nY,
                double minX, double minY, double stepX, double stepY,
                double gradZMin, double gradZMax, bool xDirection);
    
    // return gradient min and maxes in order
    double gradZMin() { return _gradZMin; }
    double gradZMax() { return _gradZMax; }
    bool xDirection() { return _xDirection; }
    
  private:
    double _gradZMin;
    double _gradZMax;
    bool _xDirection;
};

typedef KstSharedPtr<KstSMatrix> KstSMatrixPtr;
typedef KstObjectList<KstSMatrixPtr> KstSMatrixList;

#endif
// vim: ts=2 sw=2 et
