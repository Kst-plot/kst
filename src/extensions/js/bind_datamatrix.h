/***************************************************************************
                              bind_datamatrix.h
                             -------------------
    begin                : Jul 27 2005
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

#ifndef BIND_RMATRIX_H
#define BIND_RMATRIX_H

#include "bind_matrix.h"

#include <kstrmatrix.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DataMatrix
   @inherits Matrix
   @description A matrix object loaded from a data source.
*/
class KstBindDataMatrix : public KstBindMatrix {
  public:
    KstBindDataMatrix(KJS::ExecState *exec, KstRMatrixPtr s);
    KstBindDataMatrix(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindDataMatrix();

    /* @constructor
       @arg DataSource source The DataSource to load data from.  This may be
                              a string containing the url of the DataSource to
                              load.  If so, it will attempt to reuse an
                              existing DataSource instance, or fall back to
                              a new DataSource object.  It may also be 
                              specified as a DataSource object.
       @arg string field The field to load from the source.
       @optarg number xStart The x frame to start reading from.
       @arg number yStart The y frame to start reading from.
       @arg number xN The number of x frames to read.
       @arg number yN The number of y frames to read.
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
       description Triggers a reload of this matrix from disk.
    */
    KJS::Value reload(KJS::ExecState *exec, const KJS::List& args);
    /* @method changeFile
       @arg DataSource source The DataSource to switch to.
       @description Switches this DataMatrix to a different DataSource.
    */
    KJS::Value changeFile(KJS::ExecState *exec, const KJS::List& args);
    /* @method change
       @arg number xStart The x starting frame.
       @arg number yStart The y starting frame.
       @arg number xCount The number of x frames to read.
       @arg number yCount The number of y frames to read.
       @optarg number skip The number of frames to skip by.
       @optarg boolean ave True to boxcar filter.
       @description Changes the sequence of data that is read in by this
                    matrix.
    */
    KJS::Value change(KJS::ExecState *exec, const KJS::List& args);

    /* @property boolean valid
       @readonly
       @description True if the data matrix is valid.
    */
    KJS::Value valid(KJS::ExecState *exec) const;
    /* @property boolean skip
       @readonly
       @description True if the matrix should skip by <i>skipLength</i> samples
                    while reading from the data source.
    */
    KJS::Value skip(KJS::ExecState *exec) const;
    /* @property boolean boxcar
       @readonly
       @description True if the matrix should be boxcar filtered.
    */
    KJS::Value boxcar(KJS::ExecState *exec) const;
    /* @property boolean xReadToEnd
       @readonly
       @description If true, the matrix is read to the end of the source in the
                    x dimension.
    */
    KJS::Value xReadToEnd(KJS::ExecState *exec) const;
    /* @property boolean yReadToEnd
       @readonly
       @description If true, the matrix is read to the end of the source in the
                    y dimension.
    */
    KJS::Value yReadToEnd(KJS::ExecState *exec) const;
    /* @property boolean xCountFromEnd
       @readonly
       @description If true, the matrix reads only a maximum number of samples
                    from the end of the source in the x dimension.
    */
    KJS::Value xCountFromEnd(KJS::ExecState *exec) const;
    /* @property boolean yCountFromEnd
       @readonly
       @description If true, the matrix reads only a maximum number of samples
                    from the end of the source in the y dimension.
    */
    KJS::Value yCountFromEnd(KJS::ExecState *exec) const;
    /* @property number skipLength
       @readonly
       @description The number of samples to skip by.
    */
    KJS::Value skipLength(KJS::ExecState *exec) const;
    /* @property string field
       @readonly
       @description The field being read from the data source to create this
                    matrix.
    */
    KJS::Value field(KJS::ExecState *exec) const;
    /* @property DataSource dataSource
       @readonly
       @description The data source object used by this DataMatrix.
    */
    KJS::Value dataSource(KJS::ExecState *exec) const;

  protected:
    KstBindDataMatrix(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
