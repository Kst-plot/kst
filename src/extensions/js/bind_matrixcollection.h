/***************************************************************************
                           bind_matrixcollection.h
                             -------------------
    begin                : Jul 27 2005
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

#ifndef BIND_MATRIXCOLLECTION_H
#define BIND_MATRIXCOLLECTION_H

#include "bind_collection.h"

#include <kstmatrix.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class MatrixCollection
   @inherits Collection
   @description An array of Matrices.
*/
class KstBindMatrixCollection : public KstBindCollection {
  public:
    KstBindMatrixCollection(KJS::ExecState *exec, const KstMatrixList& matrixs);
    KstBindMatrixCollection(KJS::ExecState *exec);
    ~KstBindMatrixCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;

  protected:
    QStringList _matrices;
    bool _isGlobal;
};


#endif

// vim: ts=2 sw=2 et
