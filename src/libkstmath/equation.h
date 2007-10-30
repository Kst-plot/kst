/***************************************************************************
                          equation.h: Equation for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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

/** A class for handling equations for kst
 *@author C. Barth Netterfield
 */
#ifndef EQUATION_H
#define EQUATION_H

#include "dataobject.h"
#include "objectfactory.h"
#include "kst_export.h"

#define MAX_DIV_REG 100

class QXmlStreamWriter;

namespace Equations {
  class Node;
}

namespace Kst {

class ObjectStore;

class KST_EXPORT Equation : public DataObject {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }

    void attach();
    UpdateType update(int update_counter = -1);

    void save(QXmlStreamWriter &s);
    QString propertyString() const;

    /** equations used to edit the vector */
    void setEquation(const QString &Equation);
    void setExistingXVector(VectorPtr xvector, bool do_interp);

    const QString& equation() const { return _equation; }
    VectorPtr vXIn() const { return _xInVector; }
    VectorPtr vX() const { return _xOutVector; }
    VectorPtr vY() const { return _yOutVector; }

    bool doInterp() const { return _doInterp; }

    bool isValid() const;

    void showNewDialog();
    void showEditDialog();

    ObjectTag xVTag() const { return _xOutVector->tag(); }
    ObjectTag yVTag() const { return _yOutVector->tag(); }

    const CurveHintList *curveHints() const;

    DataObjectPtr makeDuplicate(DataObjectDataObjectMap& duplicatedMap);

    void replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject);

    void replaceDependency(VectorPtr oldVector, VectorPtr newVector);
    void replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix);

    bool uses(ObjectPtr p) const;

  protected:
    Equation(ObjectStore *store, const ObjectTag& in_tag, const QString& equation, double x0, double x1, int nx);
    Equation(ObjectStore *store, const ObjectTag& in_tag, const QString& equation = QString::null, VectorPtr xvector = 0L, bool do_interp = false);
    ~Equation();

    friend class EquationFactory;
    friend class ObjectStore; // FIXME: remove this when factory is working

  private:
    QString _equation;

    VectorMap VectorsUsed;
    ScalarMap ScalarsUsed;

    void commonConstructor(ObjectStore *store, const QString& equation);

    bool FillY(bool force = false);
    bool _isValid : 1;
    bool _doInterp : 1;

    int _numNew, _numShifted, _interp, _ns;

    static const QString XINVECTOR;
    static const QString XOUTVECTOR;
    static const QString YOUTVECTOR;
    VectorPtr _xInVector, _xOutVector, _yOutVector;
    Equations::Node *_pe;
};

typedef SharedPtr<Equation> EquationPtr;
typedef ObjectList<Equation> EquationList;

}
#endif
// vim: ts=2 sw=2 et
