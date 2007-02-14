/***************************************************************************
                               kstgvector.h 
                             -------------------
    begin                : Sep 25, 2005
    copyright            : (C) 2005 The University of Toronto
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
#ifndef KSTGVECTOR_H
#define KSTGVECTOR_H

#include "kstvector.h"
#include <qtimer.h>

class KstGVector : public KstVector {
  Q_OBJECT
  public:
    KstGVector(const QString& tag);

    void save(QTextStream& ts, const QString& indent = QString::null, bool saveAbsolutePosition = false);

    void changeRange(double x0, double x1, int n);
    KstObject::UpdateType update(int update_counter);

    void setFrequency(int f);

  private slots:
    void cycle();

  protected:
    friend class KstPulse;
    int _cycles;
    QTimer _t;
};

typedef KstSharedPtr<KstGVector> KstGVectorPtr;
typedef KstObjectList<KstGVectorPtr> KstGVectorList;

#endif
// vim: ts=2 sw=2 et
