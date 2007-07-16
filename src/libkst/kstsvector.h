/***************************************************************************
                          kstsvector.h  - a vector from x0 to x1 with n pts
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
#ifndef KSTSVECTOR_H
#define KSTSVECTOR_H

#include "kstvector.h"
#include "kst_export.h"

/**A vector from x0 to x1 with n pts
 *@author cbn
 */
class KST_EXPORT KstSVector : public KstVector {
 public:
  KstSVector(const QDomElement &e);
  KstSVector(double x0, double x1, int n, KstObjectTag tag);

  void save(QXmlStreamWriter &s);

  void changeRange(double x0, double x1, int n);
  KstObject::UpdateType update(int update_counter);
  void setSaveData(bool save);
};

typedef KstSharedPtr<KstSVector> KstSVectorPtr;
typedef KstObjectList<KstSVectorPtr> KstSVectorList;

#endif
// vim: ts=2 sw=2 et
