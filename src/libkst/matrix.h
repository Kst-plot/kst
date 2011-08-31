/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2004  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include "scalar.h"
#include "vector.h"
#include "primitive.h"

class QXmlStreamWriter;

namespace Kst {

class Matrix;
typedef SharedPtr<Matrix> MatrixPtr;

class KSTCORE_EXPORT Matrix : public Primitive
{
  Q_OBJECT

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;

    virtual QString descriptionTip() const;

  protected:
    Matrix(ObjectStore *store);
    virtual ~Matrix();

    friend class ObjectStore;
    virtual void _initializeShortName();

  public:
    void change(uint nX, uint nY, double minX=0, double minY=0,
        double stepX=1, double stepY=1);

    void change(QByteArray& data, uint nX, uint nY, double minX=0, double minY=0,
        double stepX=1, double stepY=1);

    // Return the sample count (x times y) of the matrix
    virtual int sampleCount() const;

    // return the z value of the rectangle in which the specified point lies
    // ok is false if the point is out of bounds
    double value(double x, double y, bool *ok = 0L) const;

    // set the z value of the rectangle in which the specified point lies
    // return false if the point is out of bounds
    bool setValue(double x, double y, double z);

    // return the value of the specified rectangle
    // ok is false if the rectangle does not exist
    double valueRaw(int x, int y, bool *ok = 0L) const;

    // set the value of the specified rectangle
    // return false if the rectangle does not exist
    bool setValueRaw(int x, int y, double z);

    // return some stats on the z values
    double minValue() const;
    double maxValue() const;

    // spike insensitive values
    void calcNoSpikeRange(double per = 0.005);
    double maxValueNoSpike() const;
    double minValueNoSpike() const;

    // return mean of the z values
    double meanValue() const;

    // return least positive z value
    double minValuePositive() const;

    // number of new samples in the matrix since last resetNumNew()
    int numNew() const;

    // reset numNew to 0
    void resetNumNew();

    // labels for plots
    virtual void setXLabelInfo(const LabelInfo &label_info);
    virtual void setYLabelInfo(const LabelInfo &label_info);
    virtual void setTitleInfo(const LabelInfo &label_info);
    virtual LabelInfo xLabelInfo() const;
    virtual LabelInfo yLabelInfo() const;
    virtual LabelInfo titleInfo() const;

    void zero();

    // clear out the matrix
    void blank();

    // get usage of this matrix by other objects
    virtual int getUsage() const;

    // save the matrix
    virtual void save(QXmlStreamWriter &s);

    // set the labels for this matrix
    void setLabel(const QString& newLabel);

    // whether or not this matrix can be edited
    bool editable() const;

    void setEditable(bool editable);

    // whether or not this matrix can be saved
    bool saveable() const;

    // matrix dimensions
    int xNumSteps() const { return _nX; }
    int yNumSteps() const { return _nY; }
    double xStepSize() const { return _stepX; }
    double yStepSize() const { return _stepY; }
    virtual bool invertXHint() const {return _invertXHint; }
    virtual bool invertYHint() const {return _invertYHint; }

    double minX() const { return _minX; }
    double minY() const { return _minY; }

    virtual bool resize(int xSize, int ySize, bool reinit = true);

    virtual void deleteDependents();

    virtual QString sizeString() const;

    virtual void internalUpdate();

    double Z(int i) const {return _z[i];}

    // output primitives: statistics scalars, etc.
    VectorMap vectors() const {return _vectors;}
    ScalarMap scalars() const {return _scalars;}
    StringMap strings() const {return _strings;}

    virtual PrimitiveMap metas() const;

    virtual ObjectList<Primitive> outputPrimitives() const;

    QByteArray getBinaryArray() const;

  protected:
    int _NS;
    int _NRealS; // number of samples with real values
    int _nX;  //this can be 0
    int _nY;  //this should never be 0
    double _minX;
    double _minY;
    double _stepX;
    double _stepY;
    bool _invertXHint;
    bool _invertYHint;

    int _numNew; // number of new samples

    bool _editable : 1;
    bool _saveable : 1;

    double _minNoSpike;
    double _maxNoSpike;

    // labels for this matrix
    LabelInfo _xLabelInfo;
    LabelInfo _yLabelInfo;
    LabelInfo _titleInfo;

    void createScalars(ObjectStore *store);
    void renameScalars();
    void updateScalars();

    // the flat-packed array in row-major order
    double *_z;
    int _zSize; // internally keep track of real _z size

    // for resizing the internal array _z only
    virtual bool resizeZ(int sz, bool reinit = true);

    // returns -1 if (x,y) is out of bounds
    int zIndex(int x, int y) const;

    ObjectMap<Scalar> _scalars;
    ObjectMap<Vector> _vectors;
    ObjectMap<String> _strings;


};

typedef ObjectList<Matrix> MatrixList;
typedef ObjectMap<Matrix> MatrixMap;

}

Q_DECLARE_METATYPE(Kst::Matrix*)

#endif
// vim: ts=2 sw=2 et
