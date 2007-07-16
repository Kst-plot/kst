/***************************************************************************
                     kstmatrix.h: 2D matrix type for kst
                             -------------------
    begin                : Mon July 19 2004
    copyright            : (C) 2004 by University of British Columbia
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

#ifndef KSTMATRIX_H
#define KSTMATRIX_H

#include <qhash.h>
#include "kstscalar.h"
#include "kstprimitive.h"

class KstMatrix;
class KstDataObject;
class QXmlStreamWriter;
typedef KstSharedPtr<KstMatrix> KstMatrixPtr;

class KST_EXPORT KstMatrix : public KstPrimitive {
  Q_OBJECT
  public:
    // Matrices do not automatically add themselves to the global matrix list
    KstMatrix(KstObjectTag in_tag = KstObjectTag::invalidTag,
              KstObject *provider = 0L, uint nX = 1, uint nY = 0,
              double minX = 0, double minY = 0,
              double stepX = 1, double stepY = 1);

  protected:
    ~KstMatrix();
    
  public:
    void change(const KstObjectTag& tag, uint nX, uint nY, double minX, double minY,
        double stepX, double stepY);

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

    // return a label for this matrix
    virtual QString label() const;
    virtual QString xLabel() const;
    virtual QString yLabel() const;

    void zero();

    // clear out the matrix
    void blank();

    // get usage of this matrix by other objects
    virtual int getUsage() const;

    // save the matrix
    virtual void save(QXmlStreamWriter &s);

    // set tag name of the matrix
    virtual void setTagName(const KstObjectTag& tag);

    // the statistics scalars for this matrix
    const QHash<QString, KstScalar*>& scalars() const;

    // set the labels for this matrix
    void setLabel(const QString& newLabel);
    void setXLabel(const QString& newLabel);
    void setYLabel(const QString& newLabel);

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
    double minX() const { return _minX; }
    double minY() const { return _minY; }
    
    virtual bool resize(int xSize, int ySize, bool reinit = true);

  protected:
    int _NS;
    int _NRealS; // number of samples with real values
    int _nX;  //this can be 0
    int _nY;  //this should never be 0
    double _minX;
    double _minY;
    double _stepX;
    double _stepY;
    int _numNew; // number of new samples
    QHash<QString, KstScalar*> _statScalars; // statistics scalars
    bool _editable : 1;
    bool _saveable : 1;

    double _minNoSpike;
    double _maxNoSpike;

    // labels for this matrix
    QString _label; 
    QString _xLabel;
    QString _yLabel;

    void createScalars();
    void renameScalars();
    void updateScalars();

    // the flat-packed array in row-major order
    double *_z;
    int _zSize; // internally keep track of real _z size
    
    // for resizing the internal array _z only
    virtual bool resizeZ(int sz, bool reinit = true);

    // returns -1 if (x,y) is out of bounds
    int zIndex(int x, int y) const;

    KstObject::UpdateType internalUpdate(KstObject::UpdateType providerUpdateType);
};

typedef KstObjectList<KstMatrixPtr> KstMatrixList;
typedef KstObjectMap<KstMatrixPtr> KstMatrixMap;
typedef KstObjectCollection<KstMatrix> KstMatrixCollection;

#endif
// vim: ts=2 sw=2 et
