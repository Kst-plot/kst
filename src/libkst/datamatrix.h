/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005  University of British Columbia                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATAMATRIX_H
#define DATAMATRIX_H

#include "kst_export.h"
#include "dataprimitive.h"
#include "matrix.h"

namespace Kst {

class MatrixData {
public:
  double xMin;
  double yMin;
  double xStepSize;
  double yStepSize;
  double *z; // the data
};

class KSTCORE_EXPORT DataMatrix : public Matrix, public DataPrimitive 
{
    Q_OBJECT

  public:

   /** Read the specified sub-range of the matrix, flat-packed in z in row-major order
        xStart - starting x *frame*
        yStart - starting y *frame*
        xNumSteps - number of *frames* to read in x direction; -1 to read 1 *sample* from xStart
        yNumSteps - number of *frames* to read in y direction; -1 to read 1 *sample* from yStart
        Will skip according to the parameter, but it may not be implemented.  If return value is -9999,
        use the non-skip version instead.
        The suggested scaling and translation is returned in xMin, yMin, xStepSize, and yStepSize
        Returns the number of *samples* read 
    **/
    struct KSTCORE_EXPORT ReadInfo {
      MatrixData* data;
      int xStart;
      int yStart;
      int xNumSteps;
      int yNumSteps;
      int skip;
    };


    struct KSTCORE_EXPORT DataInfo
    {
      DataInfo();

      int samplesPerFrame;
      int xSize;
      int ySize;
      bool invertXHint;
      bool invertYHint;
    };


    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    // save DataMatrix
    virtual void save(QXmlStreamWriter &xml);

    // change properties of DataMatrix
    void change(DataSourcePtr file, const QString &field,
                int xStart, int yStart,
                int xNumSteps, int yNumSteps,
                bool doAve, bool doSkip, int skip,
                double minX, double minY, double stepX, double stepY);
    void changeFrames(int xStart, int yStart,
                int xNumSteps, int yNumSteps,
                bool doAve, bool doSkip, int skip,
                double minX, double minY, double stepX, double stepY);

    // return properties of DataMatrix
    int reqXStart() const;
    int reqYStart() const;
    int reqXNumSteps() const;
    int reqYNumSteps() const;
    //QString filename() const;
    const QString& field() const;
    bool xReadToEnd() const;
    bool yReadToEnd() const;
    bool xCountFromEnd() const;
    bool yCountFromEnd() const;
    bool doSkip() const;
    bool doAverage() const;
    int skip() const;

    // labels for this matrix
    virtual QString label() const;

    // returns true if the file and field is valid; false otherwise
    bool isValid() const;

    // reload contents of DataMatrix from file
    void reload();
    virtual void reset();

    // change the datasource for this matrix
    void changeFile(DataSourcePtr file);

    // make a "copy" of this DataMatrix
    SharedPtr<DataMatrix> makeDuplicate() const;

    virtual QString descriptionTip() const;

    virtual QString propertyString() const;
    virtual void internalUpdate();

    virtual QString xLabel() const;
    virtual QString yLabel() const;
  protected:
    DataMatrix(ObjectStore *store);
    virtual ~DataMatrix();

    // update DataMatrix
    virtual qint64 minInputSerial() const;
    virtual qint64 minInputSerialOfLastChange() const;

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    virtual void _resetFieldMetadata();

  private:
    void commonConstructor(DataSourcePtr file, const QString &field,
                           int reqXStart, int reqYStart, int reqNX, int reqNY,
                           bool doAve, bool doSkip, int skip,
                           double minX, double minY, double stepX, double stepY);

    void doUpdateSkip(int realXStart, int realYStart);
    void doUpdateNoSkip(int realXStart, int realYStart);

    virtual void _resetFieldScalars();
    virtual void _resetFieldStrings();


    // values requested; may be different from actual matrix range
    int _reqXStart, _reqYStart, _reqNX, _reqNY;

    // matrix params since last update - used to determine if update is needed
    int _lastXStart, _lastYStart, _lastNX, _lastNY;
    bool _lastDoAve : 1;
    bool _lastDoSkip : 1;
    int _lastSkip;

    double* _aveReadBuffer; // buffer used when performing boxcar filter
    int _aveReadBufferSize;

    DataSourcePtr _file;
    QString _field; // field to read from _file
    bool _doAve : 1;
    bool _doSkip : 1;
    int _skip;
    int _samplesPerFrameCache; // cache the samples per frame of the field in datasource

    int readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps, int skip);

    QHash<QString, ScalarPtr> _fieldScalars;
    QHash<QString, StringPtr> _fieldStrings;

};

typedef SharedPtr<DataMatrix> DataMatrixPtr;
typedef ObjectList<DataMatrix> DataMatrixList;

}
#endif
// vim: ts=2 sw=2 et
