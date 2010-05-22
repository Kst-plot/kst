/***************************************************************************
                          datavector.cpp  -  description
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
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <QDebug>
#include <QXmlStreamWriter>

#include "kst_i18n.h"

#include "datacollection.h"
#include "debug.h"
#include "datavector.h"
#include "datasource.h"
#include "math_kst.h"
#include "objectstore.h"
#include "updatemanager.h"

// ReqNF <=0 means read from ReqF0 to end of File
// ReqF0 < means start at EndOfFile-ReqNF.
//
// ReqNF      ReqF0      Action
//  < 1        >=0       read from ReqF0 to end of file
//  < 1        < 0       illegal: fixed in checkIntegrity
//    1         ??       illegal: fixed in checkIntegrity
//  > 1        < 0       read the last ReqNF frames from the file
//  > 1        >=0       Read ReqNF frames starting at frame ReqF0

namespace Kst {

const QString DataVector::staticTypeString = I18N_NOOP("Data Vector");
const QString DataVector::staticTypeTag = I18N_NOOP("datavector");


DataVector::Optional::Optional() :
    frameCount(-1),
    samplesPerFrame(-1)
{
}


DataVector::Optional::Optional(int fc, int spf) :
    frameCount(fc),
    samplesPerFrame(spf)
{
}

/** Create a DataVector: raw data from a file */
DataVector::DataVector(ObjectStore *store)
: Vector(store), DataPrimitive() {

  _saveable = true;
  _dontUseSkipAccel = false;
  _numSamples = 0;
  _scalars["sum"]->setValue(0.0);
  _scalars["sumsquared"]->setValue(0.0);
  F0 = NF = 0; // nothing read yet

  N_AveReadBuf = 0;
  AveReadBuf = 0L;

  ReqF0 = 0;
  ReqNF = -1;
  Skip = 1;
  DoSkip = false;
  DoAve = false;
  _field = QString::null;
}


const QString& DataVector::typeString() const {
  return staticTypeString;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataVector::isValid() const {
  if (file()) {
    file()->readLock();
    bool rc = file()->vector().isValid(_field);
    file()->unlock();
    return rc;
  }
  return false;
}


void DataVector::change(DataSourcePtr in_file, const QString &in_field,
                        int in_f0, int in_n,
                        int in_skip, bool in_DoSkip,
                        bool in_DoAve) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  Skip = in_skip;
  DoSkip = in_DoSkip;
  DoAve = in_DoAve;
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  _dontUseSkipAccel = false;
  file() = in_file;
  ReqF0 = in_f0;
  ReqNF = in_n;
  _field = in_field;

  if (file()) {
    file()->writeLock();
  }
  reset();
  _resetFieldMetadata();
  if (file()) {
    file()->unlock();
  }

  if (ReqNF <= 0 && ReqF0 < 0) {
    ReqF0 = 0;
  }
  registerChange();
}

qint64 DataVector::minInputSerial() const {
  if (file()) {
    return (file()->serial());
  }
  return LLONG_MAX;
}

qint64 DataVector::minInputSerialOfLastChange() const {
  if (file()) {
    return (file()->serialOfLastChange());
  }
  return LLONG_MAX;
}


void DataVector::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for vector %1 was not opened.", Name()), Debug::Warning);
  }
  file() = in_file;
  if (file()) {
    file()->writeLock();
  }
  reset();
  if (file()) {
    file()->unlock();
  }
  registerChange();
}


void DataVector::changeFrames(int in_f0, int in_n,
                              int in_skip, bool in_DoSkip,
                              bool in_DoAve) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (file()) {
    file()->writeLock();
  }
  reset();
  if (file()) {
    file()->unlock();
  }
  Skip = in_skip;
  DoSkip = in_DoSkip;
  DoAve = in_DoAve;
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  ReqF0 = in_f0;
  ReqNF = in_n;

  if (ReqNF <= 0 && ReqF0 < 0) {
    ReqF0 = 0;
  }
  registerChange();
}


void DataVector::setFromEnd() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  ReqF0 = -1;
  if (ReqNF < 2) {
    ReqNF = numFrames();
    if (ReqNF < 2) {
      ReqF0 = 0;
    }
  }
  registerChange();
}


DataVector::~DataVector() {
  file() = 0;

  if (AveReadBuf) {
    free(AveReadBuf);
    AveReadBuf = 0L;
  }
}


bool DataVector::readToEOF() const {
  return ReqNF <= 0;
}


bool DataVector::countFromEOF() const {
  return ReqF0 < 0;
}


/** Return Starting Frame of Vector */
int DataVector::startFrame() const {
  return F0;
}


/** Return frames per skip to read */
int DataVector::skip() const {
  return DoSkip ? Skip : 0;
}


bool DataVector::doSkip() const {
  return DoSkip;
}


bool DataVector::doAve() const {
  return DoAve;
}


/** Return frames held in Vector */
int DataVector::numFrames() const {
  return NF;
}


int DataVector::reqNumFrames() const {
  return ReqNF;
}


int DataVector::reqStartFrame() const {
  return ReqF0;
}


/** Save vector information */
void DataVector::save(QXmlStreamWriter &s) {
  if (file()) {
    s.writeStartElement("datavector");
    file()->readLock();
    s.writeAttribute("file", file()->fileName());
    file()->unlock();
    s.writeAttribute("field", _field);

    s.writeAttribute("start", QString::number(ReqF0));
    s.writeAttribute("count", QString::number(ReqNF));

    if (doSkip()) {
      s.writeAttribute("skip", QString::number(Skip));
      if (doAve()) {
        s.writeAttribute("doAve", "true");
      }
    } else {
      s.writeAttribute("skip", QString::number(-1));
      s.writeAttribute("doAve", "false");
    }

    saveNameInfo(s, VNUM|XNUM);
    s.writeEndElement();
  }
}


QString DataVector::label() const {
  QString label;

  if (_fieldStrings.contains("quantity")) {
    label = _fieldStrings.value("quantity")->value();
  }

  if (!label.isEmpty()) {
    if (_fieldStrings.contains("units")) {
      QString units = _fieldStrings.value("units")->value();
      if (!units.isEmpty()) {
        label += "\\[" + units + "\\]";
      }
    }
  } else {
    label = _field;
    label.replace('_', "\\_");
  }

  return label;
}


void DataVector::reset() { // must be called with a lock
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _dontUseSkipAccel = false;
  if (file()) {
    SPF = opt(_field).samplesPerFrame;
  }
  F0 = NF = 0;
  resize(0);
  _numSamples = 0;
  _dirty = true;
  _resetFieldMetadata();

  Object::reset();
}


void DataVector::checkIntegrity() {
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  if (_dirty) {
    reset();
  }

  // if it looks like we have a new file, reset
  const Optional op = opt(_field);
  if (file() && (SPF != op.samplesPerFrame || op.frameCount < NF)) {
    reset();
  }

  // check for illegal NF and F0 values
  if (ReqNF < 1 && ReqF0 < 0) {
    ReqF0 = 0; // for this illegal case, read the whole file
  }

  if (ReqNF == 1) {
    ReqNF = 2;
  }
}

// Some things to consider about the following routine...
// Frames:
//    Some data sources have data devided into frames.  Each field
//    has a fixed number of samples per frame.  For some (eg, ascii files)
//    each frame has 1 sample.  For others (eg, dirfiles) you may have more.
//    Different fields in the same data source may have different samples per frame.
//    Within a data source, it is assumed that the first sample of each frame is
//    simultaneous between fields.
// Last Frame Read:
//    Only read the first sample of the last frame read, in cases where there are more
//    than one sample per frame.   This allows for more sensible association of vectors
//    into curves, when the X and Y vectors have different numbers of samples per frame.
//    The rule is that we assume is that the first sample of each frame is simultaneous.
// Skip reading:  
//    -'Skip' means read 1 sample each 'Skip' frames (not read one sample,
//     then skip 'Skip' samples or something else).
//    -In order that the data are not re-drawn each time a new sample arrives, and to
//     ensure the re-usability (via shifting) of previously read data, and to make a
//     region of data look the same regardless of the chouse of f0, all samples
//     read with skip enabled are read on 'skip boundries'... ie, the first samples of
//     frame 0, Skip, 2*Skip... N*skip, and never M*Skip+1.

void DataVector::internalUpdate() {
  int i, k, shift, n_read=0;
  int ave_nread;
  int new_f0, new_nf;
  bool start_past_eof = false;

  if (file()) {
    file()->writeLock();
  } else {
    return;
  }

  const Optional op = opt(_field);
  checkIntegrity();

  if (DoSkip && Skip < 2 && SPF == 1) {
    DoSkip = false;
  }


  // set new_nf and new_f0
  int fc = op.frameCount;
  if (ReqNF < 1) { // read to end of file
    new_f0 = ReqF0;
    new_nf = fc - new_f0;
  } else if (ReqF0 < 0) { // count back from end of file
    new_nf = fc;
    if (new_nf > ReqNF) {
      new_nf = ReqNF;
    }
    new_f0 = fc - new_nf;
  } else {
    new_f0 = ReqF0;
    new_nf = ReqNF;
    if (new_f0 + new_nf > fc) {
      new_nf = fc - new_f0;
    }
    if (new_nf <= 0) {
      // Tried to read starting past the end.
      new_f0 = 0;
      new_nf = 1;
      start_past_eof = true;
    }
  }

  if (DoSkip) {
    // change new_f0 and new_nf so they both lie on skip boundaries
    if (new_f0 != 0) {
      new_f0 = ((new_f0-1)/Skip+1)*Skip;
    }
    new_nf = (new_nf/Skip)*Skip;
  }

  // shift vector if necessary
  if (new_f0 < F0 || new_f0 >= F0 + NF) { // No useful data around.
    reset();
  } else { // shift stuff rather than re-read
    if (DoSkip) {
      shift = (new_f0 - F0)/Skip;
      NF -= (new_f0 - F0);
      _numSamples = NF/Skip;
    } else {
      shift = SPF*(new_f0 - F0);
      NF -= (new_f0 - F0);
      _numSamples = (NF-1)*SPF+1;
    }

    // FIXME: use memmove()
    for (i = 0; i < _numSamples; i++) {
      _v[i] = _v[i+shift];
    }
  }

  if (DoSkip) {
    // reallocate V if necessary
    if (new_nf / Skip != _size) {
      bool rc = resize(new_nf/Skip);
      if (!rc) {
        // FIXME: handle failed resize
      }
    }
    // for debugging: _dontUseSkipAccel = true;
    if (!_dontUseSkipAccel) {
      int rc;
      int lastRead = -1;
      if (DoAve) {
        // We don't support boxcar inside data sources yet.
        _dontUseSkipAccel = true;
      } else {
        rc = readField(_v + _numSamples, _field, new_f0, (new_nf - NF)/Skip, Skip, &lastRead);
        if (rc != -9999) {
          if (rc >= 0) {
            n_read = rc;
          } else {
            n_read = 0;
          }
        } else {
          _dontUseSkipAccel = true;
        }
      }
    }
    if (_dontUseSkipAccel) {
      n_read = 0;
      /** read each sample from the File */
      double *t = _v + _numSamples;
      int new_nf_Skip = new_nf - Skip;
      if (DoAve) {
        for (i = NF; new_nf_Skip >= i; i += Skip) {
          /* enlarge AveReadBuf if necessary */
          if (N_AveReadBuf < Skip*SPF) {
            N_AveReadBuf = Skip*SPF;
            AveReadBuf = static_cast<double*>(realloc(AveReadBuf, N_AveReadBuf*sizeof(double)));
            if (!AveReadBuf) {
              // FIXME: handle failed resize
            }
          }
          ave_nread = readField(AveReadBuf, _field, new_f0+i, Skip);
          for (k = 1; k < ave_nread; k++) {
            AveReadBuf[0] += AveReadBuf[k];
          }
          if (ave_nread > 0) {
            *t = AveReadBuf[0]/double(ave_nread);
            n_read++;
          }
          ++t;
        }
      } else {
        for (i = NF; new_nf_Skip >= i; i += Skip) {
          n_read += readField(t++, _field, new_f0 + i, -1);
        }
      }
    }
  } else {
    // reallocate V if necessary
    if ((new_nf - 1)*SPF + 1 != _size) {
      bool rc = resize((new_nf - 1)*SPF + 1);
      if (!rc) {
        // FIXME: handle failed resize
        abort();
      }
    }

    if (NF > 0) {
      NF--; /* last frame read was only partially read... */
    }

    // read the new data from file
    if (start_past_eof) {
      _v[0] = NOPOINT;
      n_read = 1;
    } else if (op.samplesPerFrame > 1) {
      assert(new_f0 + NF >= 0);
      assert(new_f0 + new_nf - 1 >= 0);
      n_read = readField(_v+NF*SPF, _field, new_f0 + NF, new_nf - NF - 1);
      n_read += readField(_v+(new_nf-1)*SPF, _field, new_f0 + new_nf - 1, -1);
    } else {
      assert(new_f0 + NF >= 0);
      if (new_nf - NF > 0 || new_nf - NF == -1) {
        n_read = readField(_v+NF*SPF, _field, new_f0 + NF, new_nf - NF);
      }
    }
  }

  NumNew = _size - _numSamples;
  NF = new_nf;
  F0 = new_f0;
  _numSamples += n_read;

  // if for some reason (eg, realtime reading an nfs mounted
  // dirfile) not all of the data was read, the data will never
  // be read; the samples will be filled in with the last data
  // point read, and a complete reset will be requested.
  // This is bad - I think it will be worthwhile
  // to add blocking w/ timeout to KstFile.
  // As a first fix, mount all nsf mounted dirfiles with "-o noac"
  _dirty = false;
  if (_numSamples != _size && !(_numSamples == 0 && _size == 1)) {
    _dirty = true;
    for (i = _numSamples; i < _size; i++) {
      _v[i] = _v[0];
    }
  }

  if (NumNew > _size) {
    NumNew = _size;
  }
  if (NumShifted > _size) {
    NumShifted = _size;
  }

  if (file()) {
    file()->unlock();
  }

  Vector::internalUpdate();
}


/** Returns intrinsic samples per frame */
int DataVector::samplesPerFrame() const {
  return SPF;
}


int DataVector::fileLength() const {

  if (file()) {    
    int rc = opt(_field).frameCount;

    return rc;
  }

  return 0;
}


void DataVector::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (file()) {
    file()->writeLock();
    file()->reset();
    file()->unlock();
    reset();
    _resetFieldMetadata();
    registerChange();
  }
}

void DataVector::_resetFieldMetadata() {
  _resetFieldScalars();
  _resetFieldStrings();
}

void DataVector::_resetFieldStrings() {
  // Note: this does not necessarily preserve order if the
  // datasource or field have been changed.  If dynamic
  // fieldScalars are ever wanted, this should be fixed.
  const QMap<QString, QString> meta_strings = file()->vector().metaStrings(_field);  
  

  QStringList fieldStringKeys = _fieldStrings.keys();
  // remove field strings that no longer need to exist
  readLock();
  for (int i=0; i<fieldStringKeys.count(); i++) {
    QString key = fieldStringKeys.at(i);
    if (!meta_strings.contains(key)) {
      StringPtr sp = _fieldStrings[key];
      _strings.remove(key);
      _fieldStrings.remove(key);
      sp = 0L;
    }
  }
  // find or insert strings, to set their value
  QMapIterator<QString, QString> it(meta_strings);
  while (it.hasNext()) {
    it.next();
    QString key = it.key();
    StringPtr sp;
    if (!_fieldStrings.contains(key)) { // insert a new one
      _strings.insert(key, sp = store()->createObject<String>());
      _fieldStrings.insert(key, sp);
      sp->setProvider(this);
      sp->setSlaveName(key);
      sp->_KShared_ref();
    } else {  // find it
      sp = _fieldStrings[key];
    }
    sp->setValue(it.value());
  }
  unlock();
}


void DataVector::_resetFieldScalars() {
  // Note: this does not necessarily preseve order if the 
  // datasource or field have been changed.  If dynamic
  // fieldScalars are ever wanted, this should be fixed.
  const QMap<QString, double> meta_scalars = file()->vector().metaScalars(_field);


  QStringList fieldScalarKeys = _fieldScalars.keys();
  // remove field scalars that no longer need to exist
  readLock();
  for (int i=0; i<fieldScalarKeys.count(); i++) {
    QString key = fieldScalarKeys.at(i);
    if (!meta_scalars.contains(key)) {
      ScalarPtr sp = _fieldScalars[key];
      _scalars.remove(key);
      _fieldScalars.remove(key);
      sp = 0L;
    }
  }
  // find or insert scalars, to set their value
  QMapIterator<QString, double> it(meta_scalars);
  while (it.hasNext()) {
    it.next();
    QString key = it.key();
    ScalarPtr sp;
    if (!_fieldScalars.contains(key)) { // insert a new one
      _scalars.insert(key, sp = store()->createObject<Scalar>());
      _fieldScalars.insert(key, sp);
      sp->setProvider(this);
      sp->setSlaveName(key);
      sp->_KShared_ref();
    } else {  // find it
      sp = _fieldScalars[key];
    }
    sp->setValue(it.value());
  }
  unlock();
}


DataVectorPtr DataVector::makeDuplicate() const {
  Q_ASSERT(store());
  DataVectorPtr vector = store()->createObject<DataVector>();

  vector->writeLock();
  vector->change(file(), _field, ReqF0, ReqNF, Skip, DoSkip, DoAve);
  if (descriptiveNameIsManual()) {
    vector->setDescriptiveName(descriptiveName());
  }

  vector->registerChange();
  vector->unlock();

  return vector;
}

QString DataVector::_automaticDescriptiveName() const {
  QString name;
  name = _field;
  return name.replace("_", "\\_");
}

QString DataVector::descriptionTip() const {
  QString IDstring;
  //QString range_string;

  IDstring = i18n(
      "Data Vector: %1\n"
      "  %2\n"
      "  Field: %3"
      ).arg(Name()).arg(dataSource()->fileName()).arg(field());

  if (countFromEOF()) {
    IDstring += i18n("\n  Last %1 frames.").arg(numFrames());
  } else if (readToEOF()) {
    IDstring += i18n("\n  Frame %1 to end.").arg(startFrame());
  } else {
    IDstring += i18n("\n  %1 Frames starting at %2.").arg(numFrames()).arg(startFrame());
  }
  if (skip()) {
    if (!doAve()) {
      IDstring+=i18n("\n  Read 1 sample per %1 frames.").arg(skip());
    } else {
      IDstring+=i18n("\n  Average each %1 frames.").arg(skip());
    }
  }
  return IDstring;
}

QString DataVector::propertyString() const {
  return i18n("%2 F0: %3 N: %4 of %1").arg(dataSource()->fileName()).arg(field()).arg(startFrame()).arg(numFrames());
}


int DataVector::readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead)
{
  const Param par = {v, s, n, skip, lastFrameRead};
  return file()->vector().read(field, par);
}

const DataVector::Optional DataVector::opt(const QString& field) const
{
  file()->readLock();
  const Optional op = file()->vector().optional(field);
  file()->unlock();
  return op;
}

}
// vim: ts=2 sw=2 et
