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
#include "kst_export.h"

namespace Kst {

namespace Equation {
  class Node;
}

/**A class for handling data vectors for kst.
 *@author cbn
 */

class KST_EXPORT DataVector : public Vector {
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

    /** Update the vector.  Return true if there was new data. */
    virtual UpdateType update(int update_counter = -1);

    /** Reload the contents of the vector */
    void reload();

    /** Returns intrinsic samples per frame */
    int samplesPerFrame() const;

    /** Save vector information */
    virtual void save(QXmlStreamWriter &s);

    /** return the name of the file if a RVector - otherwise return "" */
    QString filename() const;

    /** return the field name if an RVector, other wise return "" */
    const QString& field() const;

    /** return a sensible label for this vector */
    virtual QString label() const;
    virtual QString fileLabel() const { return filename(); }

    /** return the length of the file */
    int fileLength() const;

    /** return whether the vector is suppose to read to end of file */
    bool readToEOF() const;

    /** read whether the vector is suppose to count back from end of file */
    bool countFromEOF() const;

    /** return true if it has a valid file and field, or false otherwise */
    bool isValid() const;

    /** Read from end */
    void setFromEnd();

    // make a copy
    SharedPtr<DataVector> makeDuplicate() const;

    /** the data source */
    DataSourcePtr dataSource() const;

  public Q_SLOTS:
    void sourceUpdated(ObjectPtr object, int version);

  protected:
    /** Create an RVECTOR */
    DataVector(ObjectStore *store, const ObjectTag& tag,
               DataSourcePtr file=0L, const QString &field=QString::null,
               int f0=-1, int n=-1,
               int skip=-1, bool in_doSkip=false,
               bool in_doAve=false);

    DataVector(ObjectStore *store, const ObjectTag& tag, const QByteArray& data,
               const QString& providerName = QString(), const QString& file = QString(),
               const QString& field = QString(), int start = -1, int num = -1,
               int skip = -1, bool doAve = false,
               const QString &o_file = "|",
               int o_n = -2, int o_f = -2,
               int o_s = -1, bool o_ave = false);

    virtual ~DataVector();

    friend class DataVectorFactory;
    friend class ObjectStore; // FIXME: remove this when factory is working

    virtual QString _automaticDescriptiveName();

  private:
    Object::UpdateType doUpdate(bool force = false);

    bool _dirty; // different from the Object dirty flag

    /** Common contructor for an RVector */
    void commonRVConstructor(DataSourcePtr file,
        const QString &field,
        int f0, int n,
        int skip, bool in_doSkip,
        bool in_doAve);

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

    /** file to read for rvectors */
    DataSourcePtr _file;

    /** For rvector, the field.  For fvector, the equation. */
    QString _field;

    /** Number of Samples allocated to the vector */
    int _numSamples;

    int N_AveReadBuf;
    double *AveReadBuf;

    void reset(); // must be called with a lock

    void checkIntegrity(); // must be called with a lock

    bool _dontUseSkipAccel;
};

typedef SharedPtr<DataVector> DataVectorPtr;
typedef ObjectList<DataVector> DataVectorList;

}
#endif
// vim: ts=2 sw=2 et
