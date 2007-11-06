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
#ifndef EDITABLEMATRIX_H
#define EDITABLEMATRIX_H

#include "matrix.h"
#include "kst_export.h"

namespace Kst {

class KST_EXPORT EditableMatrix : public Matrix {
  Q_OBJECT

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    virtual void save(QXmlStreamWriter &xml);

  protected:
    EditableMatrix(ObjectStore *store, const ObjectTag& in_tag, uint nX=1, uint nY=1, double minX=0, double minY=0, double stepX=1, double stepY=1);
    EditableMatrix(ObjectStore *store, const QDomElement &e);

    friend class ObjectStore;
};

typedef SharedPtr<EditableMatrix> EditableMatrixPtr;
typedef ObjectList<EditableMatrix> EditableMatrixList;

}

#endif
// vim: ts=2 sw=2 et
