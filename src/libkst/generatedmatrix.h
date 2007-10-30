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
  Q_OBJECT

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;

    virtual void save(QXmlStreamWriter &xml);

    void change(uint nX, uint nY,
                double minX, double minY, double stepX, double stepY,
                double gradZMin, double gradZMax, bool xDirection);

    // return gradient min and maxes in order
    double gradZMin() { return _gradZMin; }
    double gradZMax() { return _gradZMax; }
    bool xDirection() { return _xDirection; }

  protected:
    GeneratedMatrix(ObjectStore *store, const QDomElement &e);
    GeneratedMatrix(ObjectStore *store, const ObjectTag& tag, uint nX=1, uint nY=0,
               double minX=0, double minY=0, double stepX=1, double stepY=1,
               double gradZMin=0, double gradZMax=1, bool xDirection=true);

    friend class ObjectStore;

  private:
    double _gradZMin;
    double _gradZMax;
    bool _xDirection;
};

typedef SharedPtr<GeneratedMatrix> GeneratedMatrixPtr;
typedef ObjectList<GeneratedMatrix> GeneratedMatrixList;

}

#endif
// vim: ts=2 sw=2 et
