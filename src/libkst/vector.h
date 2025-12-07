/***************************************************************************
                          vector.h  -  description
                             -------------------
    begin                : Fri Sep 22 2000
    copyright            : (C) 2000 by cbn
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#include <QPointer>
#include <QFile>

#include "primitive.h"
#include "scalar.h"
#include "string_kst.h"
#include "labelinfo.h"
#include "kstcore_export.h"

class QXmlStreamWriter;

namespace Equation {
  class Node;
}

namespace Kst {

class KstDataObject;

// KST::interpolate is still too polluting
KSTCORE_EXPORT double kstInterpolate(double *v, int _size, int in_i, int ns_i);
//KSTCORE_EXPORT double kstInterpolateNoHoles(double *v, int _size, int in_i, int ns_i);

class Vector;
typedef SharedPtr<Vector> VectorPtr;

#define MAX_N_DESPIKE_STAT 10000

/**A class for handling data vectors for kst.
 *@author cbn
 */
class KSTCORE_EXPORT Vector : public Primitive 
{
    Q_OBJECT

  public:
    virtual QString typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;


  protected:
    Vector(ObjectStore *store);
    virtual ~Vector();

    friend class ObjectStore; 

    virtual void _initializeShortName();


  public:
    void change(QByteArray& data);
    void oldChange(QByteArray& data);

    inline int length() const { return _size; }

    /** Return V[i], interpolated/decimated to have ns_i total samples */
    double interpolate(int i, int ns_i) const;

    /** Return V[i], interpolated/decimated to have ns_i total samples, without any NaNs */
    double interpolateNoHoles(int i, int ns_i) const;

    /** Return V[i] uninterpolated */
    double value(int i) const;
    double noNanValue(int i);

    /** Return a pointer to data to be read */
    /** these might be modified for output */
    /** eg - by masking */
    double const *value() const { return _v_out;}
    double *value() { return _v_out;}
    double const *noNanValue();

    /** raw pointer for writing */
    /** reading it will not provide */
    /** mask filtering and is probably */
    /** not what you want. */
    double *raw_V_ptr() { return _v_raw;}

    /** Return Minimum value in Vector */
    inline double min() const { return _min; }

    /** Return max value in Vector */
    inline double max() const { return _max; }

    /** Return SpikeInsensitive max value in vector **/
    double ns_max(int ns_zoom_level);

    /** Return SpikeInsensitive min value in vector **/
    double ns_min(int ns_zoom_level);

    /** Return Mean value in Vector */
    inline double mean() const { return _mean; }

    /** Return Least Positive value in Vector */
    inline double minPos() const { return _minPos; }

    /** Number of new samples in the vector since last newSync */
    inline int numNew() const { return _numNew; }

    /** Number of samples  shifted since last newSync */
    inline int numShift() const { return _numShifted; }

    inline bool isRising() const { return _is_rising; }

    /** reset New Samples and Shifted samples */
    void newSync();

    virtual bool resize(int sz, bool init = true);

    /** dump the vector values to a raw binary file */
    bool saveToTmpFile(QFile &fp);

    virtual void setNewAndShift(int inNew, int inShift);

    /** Clear out the vector by setting everything to 0.0 */
    void zero();

    /** Make the vector truly empty.  All values set to NOPOINT (NaN). */
    void blank();

    /* Generally you don't need to call this */
    void updateScalars();

    /** return information for building a label for this vector */
    virtual LabelInfo labelInfo() const;
    virtual LabelInfo titleInfo() const;
    virtual void setLabelInfo(const LabelInfo &label_info);
    virtual void setTitleInfo(const LabelInfo &label_info);

    virtual int getUsage() const;

    /** Save vector information */
    virtual void save(QXmlStreamWriter &s);

    /** access functions for _isScalarList */
    bool isScalarList() const { return _isScalarList; }

    bool saveable() const;

    bool editable() const;
    void setEditable(bool editable);

    bool saveData() const;
    virtual void setSaveData(bool save);

    virtual QString descriptionTip() const;

    virtual QString sizeString() const;

    virtual void internalUpdate();

    // output primitives: statistics scalars, etc.
    ScalarMap scalars() const {return _scalars;}
    StringMap strings() const {return _strings;}

    virtual PrimitiveMap metas() const;


    virtual ObjectList<Primitive> outputPrimitives() const;

    virtual QString propertyString() const;

    // this is reimplemented but must not be virtual.
    QByteArray scriptInterface(QList<QByteArray>&command);

    /** get an binary array with a number of doubles.
      */
    QByteArray getBinaryArray() const;

    /** does the vector represent time? */
    virtual bool isTime() const {return false;}

    virtual ScriptInterface* createScriptInterface();

  protected:
    /** current number of samples */
    int _size;

    /** number of valid points */
    int _nsum;

    /** variables for SpikeInsensitiveAutoscale **/
    double _v_ns_stats[MAX_N_DESPIKE_STAT];
    int _n_ns_stats;
    bool _ns_stats_sorted;


    /** Where raw input data is held */
    double *_v_raw;
    bool _v_raw_managed; // if the vector manages the memory for _v_raw;

    /** _v_raw with flagged data replaced with NaNs */
    double *_v_flagged;

    /** points to either _v_raw or _v_flagged */
    double *_v_out;

    /** _v_out with nans removed **/
    double *_v_no_nans;
    bool _v_no_nans_dirty : 1;
    int _v_no_nans_size;

    /** number of samples shifted since last newSync */
    int _numShifted;

    /** number of new samples since last newSync */
    int _numNew;

    /** is the vector monotonically rising */
    bool _is_rising : 1;

    /** the vector has at least one NaN */
    bool _has_nan : 1;

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
    int _imax, _imin;

    /** Scalar Maintenance methods */
    void CreateScalars(ObjectStore *store);

    virtual void deleteDependents();

    LabelInfo _labelInfo;
    LabelInfo _titleInfo;

    friend class DataObject;
    friend class Matrix;
    //virtual double* realloced(double *memptr, int newSize);
    virtual void setV(double *memptr, int newSize);

    ObjectMap<Scalar> _scalars;
    ObjectMap<String> _strings;

private:
    void updateVNoNans();
};


typedef ObjectList<Vector> VectorList;
typedef ObjectMap<Vector> VectorMap;

}

Q_DECLARE_METATYPE(Kst::Vector*)

#endif
// vim: ts=2 sw=2 et
