/***************************************************************************
                          generatedvector.h  - a vector from x0 to x1 with n pts
                             -------------------
    begin                : March, 2005
    copyright            : (C) 2005 by cbn
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
#ifndef GENERATEDVECTOR_H
#define GENERATEDVECTOR_H

#include "vector.h"
#include "kst_export.h"

namespace Kst {

/**A vector from x0 to x1 with n pts
 *@author cbn
 */
class KST_EXPORT GeneratedVector : public Vector {
  Q_OBJECT

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    void save(QXmlStreamWriter &s);

    void changeRange(double x0, double x1, int n);
    Object::UpdateType update();
    void setSaveData(bool save);

    virtual QString descriptionTip() const;
  protected:
    GeneratedVector(ObjectStore *store, double x0=0, double x1=0, int n=0);
    GeneratedVector(ObjectStore *store, const QByteArray &data, double x0, double x1, int n);

    friend class GeneratedVectorFactory;
    friend class ObjectStore; // FIXME: remove this when factory works
    virtual QString _automaticDescriptiveName() const;
};

typedef SharedPtr<GeneratedVector> GeneratedVectorPtr;
typedef ObjectList<GeneratedVector> GeneratedVectorList;

}

#endif
// vim: ts=2 sw=2 et
