/***************************************************************************
                          datavector.h  -  description
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

#ifndef DATAVECTOR_H
#define DATAVECTOR_H

#include "vector.h"

#include "datasource.h"
#include "dataprimitive.h"
#include "kst_export.h"

namespace Kst {

/**A class for handling data vectors for kst.
 *@author cbn
 */

class KST_EXPORT DataVector : public Vector, public DataPrimitive {
  Q_OBJECT

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    /** change the properties of a DataVector */
    void change(DataSourcePtr file, const QString &field,
                int f0, int n, int skip,
                bool in_doSkip, bool in_doAve);

    void changeFile(DataSourcePtr file);

    void changeFrames(int f0, int n, int skip,
                      bool in_doSkip, bool in_doAve);

    /** Return frames held in Vector */
    int numFrames() const;

    /** Return the requested number of frames in the vector */
    int reqNumFrames() const;

    /** Return Starting Frame of Vector */
    int startFrame() const;

    /** Return the requested starting frame of the Vector */
    int reqStartFrame() const;

    /** Return frames to skip per read */
    bool doSkip() const;
    bool doAve() const;
    int skip() const;

    /** Reload the contents of the vector */
    void reload();

    /** Returns intrinsic samples per frame */
    int samplesPerFrame() const;

    /** Save vector information */
    virtual void save(QXmlStreamWriter &s);

    /** return a sensible label for this vector */
    virtual QString label() const;
    virtual QString fileLabel() const { return filename(); }

    /** return the length of the file */
    int fileLength() const;

    /** return whether the vector is suppose to read to end of file */
    bool readToEOF() const;

    /** read whether the vector is suppose to count back from end of file */
    bool countFromEOF() const;

    /** Read from end */
    void setFromEnd();

    /** make a copy of the DataVector */
    SharedPtr<DataVector> makeDuplicate() const;

    virtual QString descriptionTip() const;

    virtual QString propertyString() const;

    bool isValid() const;
    virtual void internalUpdate();

  protected:
    DataVector(ObjectStore *store);
    virtual ~DataVector();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    virtual void _resetFieldMetadata();

    virtual qint64 minInputSerial() const;
    virtual qint64 minInputSerialOfLastChange() const;

  private:
    virtual void _resetFieldScalars();
    virtual void _resetFieldStrings();

    bool _dirty; // different from the Object dirty flag

    /** Samples Per Frame */
    int SPF;

    /** current number of frames */
    int NF;

    /** current starting frame */
    int F0;

    /** frames to skip per read */
    bool DoSkip;
    bool DoAve;
    int Skip;

    /** max number of frames */
    int ReqNF;

    /** Requested Starting Frame */
    int ReqF0;

    /** Number of Samples allocated to the vector */
    int _numSamples;

    int N_AveReadBuf;
    double *AveReadBuf;

    void reset(); // must be called with a lock

    void checkIntegrity(); // must be called with a lock

    bool _dontUseSkipAccel;

    QHash<QString, Scalar*> _fieldScalars;
    QHash<QString, String*> _fieldStrings;

    friend class TestDataSource;
    friend class TestHistogram;
    friend class TestCSD;
    friend class TestPSD;
    friend class TestScalar;
};

typedef SharedPtr<DataVector> DataVectorPtr;
typedef ObjectList<DataVector> DataVectorList;

}
#endif
// vim: ts=2 sw=2 et
