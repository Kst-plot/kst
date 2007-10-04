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
#ifndef EDITABLEVECTOR_H
#define EDITABLEVECTOR_H

#include "vector.h"
#include "kst_export.h"

/**A vector  with n editable pts
 *@author cbn
 */

namespace Kst {

class EditableVector : public Vector {
 public:
  KST_EXPORT EditableVector(const QString &tag, const QByteArray &data);
  KST_EXPORT EditableVector(int n, ObjectTag tag);

  Object::UpdateType update(int update_counter);

  void setSaveData(bool save);
};

typedef KstSharedPtr<EditableVector> EditableVectorPtr;
typedef ObjectList<EditableVectorPtr> EditableVectorList;

}

#endif
// vim: ts=2 sw=2 et
