/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATAMATRIX_H
#define DATAMATRIX_H

#include "kstcore_export.h"
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
        xStart - starting x column
        yStart - starting y row
        xNumSteps - number of columns to read in x direction;
        yNumSteps - number of rows to read in y direction;
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
      int frame; // only used for image streams
    };


    struct KSTCORE_EXPORT DataInfo
    {
      DataInfo();

      int xSize;
      int ySize;
      bool invertXHint;
      bool invertYHint;
      int frameCount; // only used for image streams
    };


    virtual QString typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    // save DataMatrix
    virtual void save(QXmlStreamWriter &xml);

    // change properties of DataMatrix
    void change(DataSourcePtr file, const QString &field,
                int xStart, int yStart,
                int xNumSteps, int yNumSteps,
                bool doAve, bool doSkip, int skip, int frame,
                double minX, double minY, double stepX, double stepY);
    void changeFrames(int xStart, int yStart,
                int xNumSteps, int yNumSteps,
                bool doAve, bool doSkip, int skip, int frame,
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

    // for image streams
    int frame() const {return _frame;}
    void setFrame(int f) {_frame = f;}
    bool isStream();

    // labels for this matrix
    virtual QString label() const;

    // returns true if the file and field is valid; false otherwise
    bool isValid() const;

    // reload contents of DataMatrix from file
    void reload();
    virtual void reset();

    // change the datasource for this matrix
    void changeFile(DataSourcePtr file);

    virtual QString descriptionTip() const;

    virtual QString propertyString() const;
    virtual void internalUpdate();

    virtual LabelInfo xLabelInfo() const;
    virtual LabelInfo yLabelInfo() const;
    virtual LabelInfo titleInfo() const;

    virtual ScriptInterface* createScriptInterface();

    int fileLength() const;

  protected:
    DataMatrix(ObjectStore *store);
    virtual ~DataMatrix();

    // update DataMatrix
    virtual qint64 minInputSerial() const;
    virtual qint64 maxInputSerialOfLastChange() const;

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    virtual void _resetFieldMetadata();

  private:
    void commonConstructor(DataSourcePtr file, const QString &field,
                           int reqXStart, int reqYStart, int reqNX, int reqNY,
                           bool doAve, bool doSkip, int skip, int frame,
                           double minX, double minY, double stepX, double stepY);

    void doUpdateSkip(int realXStart, int realYStart, int frame);
    void doUpdateNoSkip(int realXStart, int realYStart, int frame);

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
    int _frame;

    int readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps, int skip, int frame);

    QHash<QString, ScalarPtr> _fieldScalars;
    QHash<QString, StringPtr> _fieldStrings;

    // make a "copy" of this DataMatrix
    virtual PrimitivePtr makeDuplicate() const;
    virtual bool checkValidity(const DataSourcePtr& ds) const;


};

typedef SharedPtr<DataMatrix> DataMatrixPtr;
typedef ObjectList<DataMatrix> DataMatrixList;

}
#endif
// vim: ts=2 sw=2 et
