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

class KST_EXPORT Equation : public DataObject {
  public:
    Equation(const QString& in_tag, const QString& equation, double x0, double x1, int nx);
    Equation(const QString& in_tag, const QString& equation, VectorPtr xvector, bool do_interp);
    ~Equation();

    void attach();
    UpdateType update(int update_counter = -1);

    void save(QXmlStreamWriter &s);
    QString propertyString() const;

    /** equations used to edit the vector */
    void setEquation(const QString &Equation);
    void setExistingXVector(VectorPtr xvector, bool do_interp);

    const QString& equation() const { return _equation; }
    VectorPtr vXIn() const { return *_xInVector; }
    VectorPtr vX() const { return *_xOutVector; }
    VectorPtr vY() const { return *_yOutVector; }

    bool doInterp() const { return _doInterp; }

    bool isValid() const;

    void setTagName(const QString& tag);

    void showNewDialog();
    void showEditDialog();

    QString xVTag() const { return (*_xOutVector)->tagName(); }
    QString yVTag() const { return (*_yOutVector)->tagName(); }

    const CurveHintList *curveHints() const;

    DataObjectPtr makeDuplicate(DataObjectDataObjectMap& duplicatedMap);
    
    void replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject);

    void replaceDependency(VectorPtr oldVector, VectorPtr newVector);
    void replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix);

    bool uses(ObjectPtr p) const;

  private:
    QString _equation;

    VectorMap VectorsUsed;
    ScalarMap ScalarsUsed;

    void commonConstructor(const QString& in_tag, const QString& equation);

    bool FillY(bool force = false);
    bool _isValid : 1;
    bool _doInterp : 1;

    int _numNew, _numShifted, _interp, _ns;

    static const QString XINVECTOR;
    static const QString XOUTVECTOR;
    static const QString YOUTVECTOR;
    VectorMap::Iterator _xInVector, _xOutVector, _yOutVector;
    Equations::Node *_pe;
};

typedef SharedPtr<Equation> EquationPtr;
typedef ObjectList<EquationPtr> EquationList;

}
#endif
// vim: ts=2 sw=2 et
