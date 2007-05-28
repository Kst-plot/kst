/***************************************************************************
                          kstvector.h  -  description
                             -------------------
    begin                : Fri Sep 22 2000
    copyright            : (C) 2000 by cbn
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

#ifndef KSTVECTOR_H
#define KSTVECTOR_H

#include <math.h>
#include <qhash.h>
#include <qpointer.h>
#include "kstobjectcollection.h"
#include "kstprimitive.h"
#include "kstscalar.h"
#include "kst_export.h"

#define IS_POINT(x) (!isnan(x))
#define NOT_POINT(x) (isnan(x))

namespace KST {
  // Do not compare against this, only assign it and use the helpers above.
  extern KST_EXPORT const double NOPOINT;
}

namespace Equation {
  class Node;
}

class KstDataObject;

class KstVector;
typedef KstSharedPtr<KstVector> KstVectorPtr;
// KST::interpolate is still too polluting
extern double kstInterpolate(double *v, int _size, int in_i, int ns_i) KST_EXPORT;
extern double kstInterpolateNoHoles(double *v, int _size, int in_i, int ns_i) KST_EXPORT;

/**A class for handling data vectors for kst.
 *@author cbn
 */
class KstVector : public KstPrimitive {
  Q_OBJECT
  public:
    /**
     * Vectors do not automatically add themselves to the global vector list
     */
    KstVector(KstObjectTag in_tag = KstObjectTag::invalidTag, int size = 0,
        KstObject *provider = 0L, bool bIsScalarList = false);
    KstVector(const QDomElement& e);

  protected:
    virtual ~KstVector();

  public:
    inline int length() const { return _size; }

    /** Return V[i], interpolated/decimated to have ns_i total samples */
    double interpolate(int i, int ns_i) const;

    /** Return V[i], interpolated/decimated to have ns_i total samples, without any holes */
    double interpolateNoHoles(int i, int ns_i) const;

    /** Return V[i] uninterpolated */
    double value(int i) const;

    /** Return Minimum value in Vector */
    inline double min() const { return _min; }

    /** Return max value in Vector */
    inline double max() const { return _max; }

    /** Return SpikeInsensitive max value in vector **/
    inline double ns_max() const { return _ns_max; }

    /** Return SpikeInsensitive min value in vector **/
    inline double ns_min() const { return _ns_min; }

    /** Return Mean value in Vector */
    inline double mean() const { return _mean; }

    /** Return Least Positive value in Vector */
    inline double minPos() const { return _minPos; }

    /** Number of new samples in the vector since last newSync */
    inline int numNew() const { return NumNew; }

    /** Number of samples  shifted since last newSync */
    inline int numShift() const { return NumShifted; }

    inline bool isRising() const { return _is_rising; }

    /** reset New Samples and Shifted samples */
    void newSync();

    /** return a sensible top label.... */
    virtual QString fileLabel() const;

    virtual bool resize(int sz, bool reinit = true);

    virtual void setNewAndShift(int inNew, int inShift);

    /** Clear out the vector by setting everything to 0.0 */
    void zero();

    /** Make the vector truly empty.  All values set to NOPOINT (NaN). */
    void blank();

    /* Generally you don't need to call this */
    void updateScalars();

    /** return a sensible label for this vector */
    virtual QString label() const;

    virtual int getUsage() const;

    /** Save vector information */
    virtual void save(QTextStream &ts, const QString& indent = QString::null, bool saveAbsolutePosition = false);

    /** Generate a new vector [x0..x1] with n total points */
    // #### Remove
    static KstVectorPtr generateVector(double x0, double x1, int n, const KstObjectTag& tag);

    virtual void setTagName(const KstObjectTag& newTag);

    /** Return a pointer to the raw vector */
    double *const value() const;

    /** access functions for _isScalarList */
    bool isScalarList() const { return _isScalarList; }
    
    const QHash<QString, KstScalar*>& scalars() const;

    void setLabel(const QString& label_in);

    bool saveable() const;

    bool editable() const;
    void setEditable(bool editable);

    bool saveData() const;
    virtual void setSaveData(bool save);

  protected:
    /** current number of samples */
    int _size;

    /** number of valid points */
    int _nsum;

    /** variables for SpikeInsensitiveAutoscale **/
    double _ns_max;
    double _ns_min;

    /** Where the vector is held */
    double *_v;

    /** number of samples shifted since last newSync */
    int NumShifted;

    /** number of new samples since last newSync */
    int NumNew;

    /** Statistics Scalars */
    QHash<QString, KstScalar*> _scalars;

    /** is the vector monotonically rising */
    bool _is_rising : 1;

    /** if true then we have a scalar list and do not want to be able to
    use it in a curve, display statistics for it, etc. */
    bool _isScalarList : 1;

    /** The user should not be permitted to edit the contents of the vector if
    this is false. */
    bool _editable : 1;

    /** can/should the vector be saved */
    bool _saveable : 1;

    /** should the vector data be saved? */
    bool _saveData : 1;

    double _min, _max, _mean, _minPos;

    /** Scalar Maintenance methods */
    void CreateScalars();
    void RenameScalars();

    QString _label;
    
    friend class KstDataObject;
    virtual double* realloced(double *memptr, int newSize);
    KstObject::UpdateType internalUpdate(KstObject::UpdateType providerRC);
} KST_EXPORT;

typedef KstObjectList<KstVectorPtr> KstVectorList;
typedef KstObjectMap<KstVectorPtr> KstVectorMap;
typedef KstObjectCollection<KstVector> KstVectorCollection;

#endif
// vim: ts=2 sw=2 et
