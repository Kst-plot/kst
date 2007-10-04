/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005  University of British Columbia                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GENERATEDMATRIX_H
#define GENERATEDMATRIX_H

#include "matrix.h"
#include "kst_export.h"

namespace Kst {

class KST_EXPORT GeneratedMatrix : public Matrix {
  public:
    GeneratedMatrix(const QDomElement &e);
    GeneratedMatrix(ObjectTag tag, uint nX, uint nY,
               double minX, double minY, double stepX, double stepY,
               double gradZMin, double gradZMax, bool xDirection);

    virtual void save(QTextStream &ts, const QString& indent = QString::null);

    void change(ObjectTag tag, uint nX, uint nY,
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

typedef KstSharedPtr<GeneratedMatrix> GeneratedMatrixPtr;
typedef ObjectList<GeneratedMatrixPtr> GeneratedMatrixList;

}

#endif
// vim: ts=2 sw=2 et
