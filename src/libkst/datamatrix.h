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

#include "matrix.h"
#include "sharedptr.h"
#include "datasource.h"
#include "kst_export.h"

namespace Kst {

class KST_EXPORT DataMatrix : public Matrix {
  Q_OBJECT

  public:
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
    QString filename() const;
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
    virtual QString fileLabel() const;

    // the data source this DataMatrix is using
    DataSourcePtr dataSource() const;

    // returns true if the file and field is valid; false otherwise
    bool isValid() const;

    // update DataMatrix
    virtual UpdateType update();

    // reload contents of DataMatrix from file
    void reload();

    // change the datasource for this matrix
    void changeFile(DataSourcePtr file);

    // make a "copy" of this DataMatrix
    SharedPtr<DataMatrix> makeDuplicate() const;

  protected:
    DataMatrix(ObjectStore *store, const ObjectTag& tag);
    // constructor
    DataMatrix(ObjectStore *store, DataSourcePtr file, const QString &field,
        const ObjectTag& tag, int xStart, int yStart,
        int xNumSteps, int yNumSteps, bool doAve, bool doSkip, int skip,
        double minX = 0, double minY = 0, double stepX = 1, double stepY = 1);

    // constructor to create a saved DataMatrix
    DataMatrix(ObjectStore *store, const QDomElement &e);

    virtual ~DataMatrix();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName();

  private:
    void commonConstructor(DataSourcePtr file, const QString &field,
                           int reqXStart, int reqYStart, int reqNX, int reqNY,
                           bool doAve, bool doSkip, int skip,
                           double minX, double minY, double stepX, double stepY);

    // internal update function, called by update()
    Object::UpdateType doUpdate(bool force = false);

    bool doUpdateSkip(int realXStart, int realYStart, bool force);
    bool doUpdateNoSkip(int realXStart, int realYStart, bool force);

    void reset();

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
};

typedef SharedPtr<DataMatrix> DataMatrixPtr;
typedef ObjectList<DataMatrix> DataMatrixList;

}
#endif
// vim: ts=2 sw=2 et
