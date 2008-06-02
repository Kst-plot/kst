/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef EDITABLEVECTOR_H
#define EDITABLEVECTOR_H

#include "vector.h"
#include "kst_export.h"

/**A vector  with n editable pts
 *@author cbn
 */

namespace Kst {

class EditableVector : public Vector {
  Q_OBJECT

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    void save(QXmlStreamWriter &s);

    Object::UpdateType update();

    void setSaveData(bool save);

    virtual QString description();

  protected:
    KST_EXPORT EditableVector(ObjectStore *store, const QByteArray& data);
    KST_EXPORT EditableVector(ObjectStore *store, int n=0);

    friend class EditableVectorFactory;
    friend class ObjectStore; // FIXME: remove this when factory works
    virtual QString _automaticDescriptiveName() const;
};

typedef SharedPtr<EditableVector> EditableVectorPtr;
typedef ObjectList<EditableVector> EditableVectorList;

}

#endif
// vim: ts=2 sw=2 et
