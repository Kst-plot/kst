/***************************************************************************
                              bind_datavector.h
                             -------------------
    begin                : Mar 29 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef BIND_RVECTOR_H
#define BIND_RVECTOR_H

#include "bind_vector.h"

#include <kstrvector.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DataVector
   @inherits Vector
   @description A vector object loaded from a data source.
*/
class KstBindDataVector : public KstBindVector {
  public:
    KstBindDataVector(KJS::ExecState *exec, KstRVectorPtr s);
    KstBindDataVector(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindDataVector();

    /* @constructor
       @arg DataSource source The DataSource to load data from.  This may be
                              a string containing the url of the DataSource to
                              load.  If so, it will attempt to reuse an
                              existing DataSource instance, or fall back to
                              a new DataSource object.  It may also be 
                              specified as a DataSource object.
       @arg string field The field to load from the source.
       @optarg number start The frame to start reading from.
       @arg number n The number of frames to read.
       @optarg number skip The number of samples to skip by.
       @optarg boolean ave Set to true to do boxcar filtering.
    */
    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions
    /* @method reload
       description Triggers a reload of this vector from disk.
    */
    KJS::Value reload(KJS::ExecState *exec, const KJS::List& args);
    /* @method changeFile
       @arg DataSource source The DataSource to switch to.
       @description Switches this DataVector to a different DataSource.
    */
    KJS::Value changeFile(KJS::ExecState *exec, const KJS::List& args);
    /* @method changeFrames
       @arg number start The starting frame.
       @arg number count The number of frames to read.
       @optarg number skip The number of frames to skip by.
       @optarg boolean ave True to boxcar filter.
       @description Changes the sequence of data that is read in by this
                    vector.
    */
    KJS::Value changeFrames(KJS::ExecState *exec, const KJS::List& args);

    /* @property boolean valid
       @readonly
       @description True if the data vector is valid.
    */
    KJS::Value valid(KJS::ExecState *exec) const;
    /* @property boolean skip
       @readonly
       @description True if the vector should skip by <i>skipLength</i> samples
                    while reading from the data source.
    */
    KJS::Value skip(KJS::ExecState *exec) const;
    /* @property boolean boxcar
       @readonly
       @description True if the vector should be boxcar filtered.
    */
    KJS::Value boxcar(KJS::ExecState *exec) const;
    /* @property boolean readToEnd
       @readonly
       @description If true, the vector is read to the end of the source.
    */
    KJS::Value readToEnd(KJS::ExecState *exec) const;
    /* @property boolean countFromEnd
       @readonly
       @description If true, the vector reads only a maximum number of samples
                    from the end of the source.
    */
    KJS::Value countFromEnd(KJS::ExecState *exec) const;
    /* @property number skipLength
       @readonly
       @description The number of samples to skip by.
    */
    KJS::Value skipLength(KJS::ExecState *exec) const;
    /* @property number startFrame
       @readonly
       @description The starting frame number.
    */
    KJS::Value startFrame(KJS::ExecState *exec) const;
    /* @property number startFrameRequested
       @readonly
       @description The requested starting frame number.  May not be the
                    actual <i>startFrame</i>.
    */
    KJS::Value startFrameRequested(KJS::ExecState *exec) const;
    /* @property number frames
       @readonly
       @description The number of frames read from the source.
    */
    KJS::Value frames(KJS::ExecState *exec) const;
    /* @property number framesRequested
       @readonly
       @description The number of frames requested to be read from the source.
                    May not be the actual <i>frames</i>.
    */
    KJS::Value framesRequested(KJS::ExecState *exec) const;
    /* @property number samplesPerFrame
       @readonly
       @description The number of samples per frame for the associated data
                    source.
    */
    KJS::Value samplesPerFrame(KJS::ExecState *exec) const;
    /* @property string field
       @readonly
       @description The field being read from the data source to create this
                    vector.
    */
    KJS::Value field(KJS::ExecState *exec) const;
    /* @property DataSource dataSource
       @readonly
       @description The data source object used by this DataVector.
    */
    KJS::Value dataSource(KJS::ExecState *exec) const;

  protected:
    KstBindDataVector(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
