/***************************************************************************
                          datavector.h  -  description
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

#ifndef DATAVECTOR_H
#define DATAVECTOR_H

#include "kst_export.h"
#include "dataprimitive.h"
#include "vector.h"



namespace Kst {

/**A class for handling data vectors for kst.
 *@author cbn
 */

class KSTCORE_EXPORT DataVector : public Vector, public DataPrimitive
{
    Q_OBJECT

  public:

    /** Parameters for reading vectors a field from a file.  Data is returned in the data;
      data points to aloocated array which should be filled
      startingFrame is the starting frame
      numberOfFrames is the number of frames to read
        if numberOfFrames is -1, it means to read 1 -sample- from startingFrame.
      skipFrame skip this frame      
      returns the number of samples read.
      If it returns -9999, use the skipFrame= -1. 
     */

    struct KSTCORE_EXPORT ReadInfo {
      double*  data;
      int startingFrame;
      int numberOfFrames;
      int skipFrame;
      int *lastFrameRead;
    };


    struct KSTCORE_EXPORT DataInfo
    {
      DataInfo();
      DataInfo(int frameCount, int samplesPerFrame);

      int frameCount;
      int samplesPerFrame;
    };



    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    virtual void reset(); // must be called with a lock

    /** change the properties of a DataVector */
    void change(DataSourcePtr file, const QString &field,
                int f0, int n, int skip,
                bool in_doSkip, bool in_doAve);

    void changeFile(DataSourcePtr file);

    void changeFrames(int f0, int n, int skip,
                      bool in_doSkip, bool in_doAve);           //si

    /** Return frames held in Vector */
    int numFrames() const;                                      //si

    /** Return the requested number of frames in the vector */
    int reqNumFrames() const;

    /** Return Starting Frame of Vector */
    int startFrame() const;                                     //si

    /** Return the requested starting frame of the Vector */
    int reqStartFrame() const;

    /** Return frames to skip per read */
    bool doSkip() const;                                        //si^3
    bool doAve() const;
    int skip() const;

    /** Reload the contents of the vector */
    void reload();                                              //si

    /** Returns intrinsic samples per frame */
    int samplesPerFrame() const;                                //si

    /** Save vector information */
    virtual void save(QXmlStreamWriter &s);

    /** return a sensible label for this vector */
    virtual LabelInfo labelInfo() const;

    /** return the length of the file */
    int fileLength() const;                                     //si

    /** return whether the vector is suppose to read to end of file */
    bool readToEOF() const;                                     //si

    /** read whether the vector is suppose to count back from end of file */
    bool countFromEOF() const;                                  //si

    /** Read from end */
    void setFromEnd();

    virtual QString descriptionTip() const;                     //si

    virtual QString propertyString() const;

    bool isValid() const;                                       //si
    virtual void internalUpdate();

    //implemented in Vector too but must not be virtual.
    QByteArray scriptInterface(QList<QByteArray> &command);

  protected:
    DataVector(ObjectStore *store);
    virtual ~DataVector();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    virtual void _resetFieldMetadata();

    virtual qint64 minInputSerial() const;
    virtual qint64 maxInputSerialOfLastChange() const;

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

    void checkIntegrity(); // must be called with a lock

    bool _dontUseSkipAccel;

    // wrappers around DataSource interface functions
    int readField(double *v, const QString& field, int s, int n, int skip = -1, int *lastFrameRead = 0L);
    const DataInfo dataInfo(const QString& field) const;

    QHash<QString, ScalarPtr> _fieldScalars;
    QHash<QString, StringPtr> _fieldStrings;

    /** make a copy of the DataVector */
    virtual PrimitivePtr makeDuplicate() const;
    virtual bool checkValidity(const DataSourcePtr& ds) const;

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
