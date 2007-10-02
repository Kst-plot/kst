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

#include "kstvector.h"
#include "kst_export.h"

namespace Kst {

/**A vector from x0 to x1 with n pts
 *@author cbn
 */

class KST_EXPORT GeneratedVector : public KstVector {
 public:
  GeneratedVector(const QString &tag, const QByteArray &data, double x0, double x1, int n);
  GeneratedVector(double x0, double x1, int n, KstObjectTag tag);

  void save(QXmlStreamWriter &s);

  void changeRange(double x0, double x1, int n);
  KstObject::UpdateType update(int update_counter);
  void setSaveData(bool save);
};

typedef KstSharedPtr<GeneratedVector> GeneratedVectorPtr;
typedef KstObjectList<GeneratedVectorPtr> GeneratedVectorList;

}

#endif
// vim: ts=2 sw=2 et
