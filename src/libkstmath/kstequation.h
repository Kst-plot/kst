/***************************************************************************
                          kstequation.h: Equation for KST
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
#ifndef KSTEQUATION_H
#define KSTEQUATION_H

#include "dataobject.h"
#include "objectfactory.h"
#include "kst_export.h"

#define MAX_DIV_REG 100

namespace Equation {
  class Node;
}

class QXmlStreamWriter;

class KST_EXPORT KstEquation : public Kst::DataObject {
  public:
    KstEquation(const QString& in_tag, const QString& equation, double x0, double x1, int nx);
    KstEquation(const QString& in_tag, const QString& equation, Kst::VectorPtr xvector, bool do_interp);
    ~KstEquation();

    void attach();
    UpdateType update(int update_counter = -1);

    void save(QXmlStreamWriter &s);
    QString propertyString() const;

    /** equations used to edit the vector */
    void setEquation(const QString &Equation);
    void setExistingXVector(Kst::VectorPtr xvector, bool do_interp);

    const QString& equation() const { return _equation; }
    Kst::VectorPtr vXIn() const { return *_xInVector; }
    Kst::VectorPtr vX() const { return *_xOutVector; }
    Kst::VectorPtr vY() const { return *_yOutVector; }

    bool doInterp() const { return _doInterp; }

    bool isValid() const;

    void setTagName(const QString& tag);

    void showNewDialog();
    void showEditDialog();

    QString xVTag() const { return (*_xOutVector)->tagName(); }
    QString yVTag() const { return (*_yOutVector)->tagName(); }

    const KstCurveHintList *curveHints() const;

    Kst::DataObjectPtr makeDuplicate(Kst::DataObjectDataObjectMap& duplicatedMap);
    
    void replaceDependency(Kst::DataObjectPtr oldObject, Kst::DataObjectPtr newObject);

    void replaceDependency(Kst::VectorPtr oldVector, Kst::VectorPtr newVector);
    void replaceDependency(KstMatrixPtr oldMatrix, KstMatrixPtr newMatrix);

    bool uses(KstObjectPtr p) const;

  private:
    QString _equation;

    Kst::VectorMap VectorsUsed;
    Kst::ScalarMap ScalarsUsed;

    void commonConstructor(const QString& in_tag, const QString& equation);

    bool FillY(bool force = false);
    bool _isValid : 1;
    bool _doInterp : 1;

    int _numNew, _numShifted, _interp, _ns;

    static const QString XINVECTOR;
    static const QString XOUTVECTOR;
    static const QString YOUTVECTOR;
    Kst::VectorMap::Iterator _xInVector, _xOutVector, _yOutVector;
    Equation::Node *_pe;
};

typedef KstSharedPtr<KstEquation> KstEquationPtr;
typedef KstObjectList<KstEquationPtr> KstEquationList;

#endif
// vim: ts=2 sw=2 et
