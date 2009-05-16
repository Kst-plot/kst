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

/** Create a DataVector: raw data from a file */
DataVector::DataVector(ObjectStore *store)
: Vector(store) {

  _saveable = true;
  _dontUseSkipAccel = false;
  _numSamples = 0;
  _scalars["sum"]->setValue(0.0);
  _scalars["sumsquared"]->setValue(0.0);
  F0 = NF = 0; // nothing read yet

  N_AveReadBuf = 0;
  AveReadBuf = 0L;

  _file = 0L;
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


void DataVector::sourceUpdated(ObjectPtr object) {
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "\t\tUP - Data Source update required by Vector" << shortName() << "for update of" << object->shortName();
#endif
  writeLock();
  UpdateManager::self()->updateStarted(object, this);
  if (update()) {
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "\t\t\tUP - Vector" << shortName() << "has been updated as part of update of" << object->shortName() << "informing dependents";
#endif
    emit updated(object);
  }
  UpdateManager::self()->updateFinished(object, this);
  unlock();
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
  _file = in_file;
  ReqF0 = in_f0;
  ReqNF = in_n;
  _field = in_field;

  if (_file) {
    _file->writeLock();
  }
  reset();
  _resetFieldMetadata();
  if (_file) {
    _file->unlock();
  }

  if (ReqNF <= 0 && ReqF0 < 0) {
    ReqF0 = 0;
  }

  if (in_file) {
    connect(in_file, SIGNAL(sourceUpdated(ObjectPtr)), this, SLOT(sourceUpdated(ObjectPtr)));
  }

}


void DataVector::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for vector %1 was not opened.", Name()), Debug::Warning);
  }
  _file = in_file;
  if (_file) {
    _file->writeLock();
  }
  reset();
  _resetFieldMetadata();
  if (_file) {
    _file->unlock();
  }
}


void DataVector::changeFrames(int in_f0, int in_n,
                              int in_skip, bool in_DoSkip,
                              bool in_DoAve) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_file) {
    _file->writeLock();
  }
  reset();
  if (_file) {
    _file->unlock();
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
}


DataVector::~DataVector() {
  _file = 0;

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
  if (_file) {
    s.writeStartElement("datavector");
    _file->readLock();
    s.writeAttribute("file", _file->fileName());
    _file->unlock();
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


/** return the name of the file */
QString DataVector::filename() const {
  QString rc;
  if (_file) {
    _file->readLock();
    rc = _file->fileName();
    _file->unlock();
  }
  return rc;
}


/** return the field */
const QString& DataVector::field() const {
  return _field;
}


QString DataVector::label() const {
  bool ok;
  QString label;

  _field.toInt(&ok);
  if (ok && _file) {
    _file->readLock();
    if (_file->fileType() == "ASCII") {
      label = i18n("Column %1", _field);
    } else {
      label = _field;
    }
    _file->unlock();
  } else {
    label = _field;
  }

  label.replace('_', "\\_");
  return label;
}


void DataVector::reset() { // must be called with a lock
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _dontUseSkipAccel = false;
  if (_file) {
    SPF = _file->samplesPerFrame(_field);
  }
  F0 = NF = 0;
  resize(0);
  _numSamples = 0;
  _dirty = true;
}


void DataVector::checkIntegrity() {
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  if (_dirty) {
    reset();
  }

  // if it looks like we have a new file, reset
  if (_file && (SPF != _file->samplesPerFrame(_field) || _file->frameCount(_field) < NF)) {
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


/** Update an RVECTOR */
Object::UpdateType DataVector::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_file) {
    _file->writeLock();
  }
  Object::UpdateType rc = doUpdate(true);
  if (_file) {
    _file->unlock();
  }

  return rc;
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

Object::UpdateType DataVector::doUpdate(bool force) {
  int i, k, shift, n_read=0;
  int ave_nread;
  int new_f0, new_nf;
  bool start_past_eof = false;
  
  checkIntegrity();

  if (DoSkip && Skip < 2 && SPF == 1) {
    DoSkip = false;
  }

  if (!_file) {
    return NO_CHANGE;
  }

  // set new_nf and new_f0
  int fc = _file->frameCount(_field);
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

  if (NF == new_nf && F0 == new_f0 && !force) {
    return NO_CHANGE;
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
    //qDebug() << "new_nf = " << new_nf << " and skip = " << Skip << " so new_nf/Skip+1 = " << (new_nf / Skip + 1) << endl;
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
        rc = _file->readField(_v + _numSamples, _field, new_f0, (new_nf - NF)/Skip, Skip, &lastRead);
        if (rc != -9999) {
          //qDebug() << "USED SKIP FOR READ - " << _field << " - rc=" << rc << " for Skip=" << Skip << " s=" << new_f0 << " n=" << (new_nf - NF)/Skip << endl;
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
      //qDebug() << "NF = " << NF << " numsamples = " << _numSamples << " new_f0 = " << new_f0 << endl;
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
          ave_nread = _file->readField(AveReadBuf, _field, new_f0+i, Skip);
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
          //qDebug() << "readField " << _field << " start=" << new_f0 + i << " n=-1" << endl;
          n_read += _file->readField(t++, _field, new_f0 + i, -1);
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
    } else if (_file->samplesPerFrame(_field) > 1) {
      assert(new_nf - NF - 1 > 0 || new_nf - NF - 1 == -1 || force);
      assert(new_f0 + NF >= 0);
      assert(new_f0 + new_nf - 1 >= 0);
      n_read = _file->readField(_v+NF*SPF, _field, new_f0 + NF, new_nf - NF - 1);
      n_read += _file->readField(_v+(new_nf-1)*SPF, _field, new_f0 + new_nf - 1, -1);
    } else {
      //qDebug() << "Reading into _v=" << (void*)_v << " which has size " << _size << " and starting at offset " << NF*SPF << " for s=" << new_f0 + NF << " and n=" << new_nf - NF << endl;
      assert(new_f0 + NF >= 0);
      if (new_nf - NF > 0 || new_nf - NF == -1) {
        n_read = _file->readField(_v+NF*SPF, _field, new_f0 + NF, new_nf - NF);
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
    //qDebug() << "SET DIRTY since _numSamples = " << _numSamples << " but _size = " << _size << endl;
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
  return Vector::internalUpdate(UPDATE);
}


/** Returns intrinsic samples per frame */
int DataVector::samplesPerFrame() const {
  return SPF;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataVector::isValid() const {
  if (_file) {
    _file->readLock();
    bool rc = _file->isValidField(_field);
    _file->unlock();
    return rc;
  }
  return false;
}


int DataVector::fileLength() const {
  if (_file) {
    _file->readLock();
    int rc = _file->frameCount(_field);
    _file->unlock();

    return rc;
  }

  return 0;
}


void DataVector::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_file) {
    _file->writeLock();
    if (_file->reset()) { // try the efficient way first
      reset();
    } else { // the inefficient way
      DataSourcePtr newsrc = DataSource::loadSource(store(), _file->fileName(), _file->fileType());
      assert(newsrc != _file);
      if (newsrc) {
        _file->unlock();
        // FIXME: need to writelock store?
        if (store()) {
          store()->removeObject(_file);
        }
        _dontUseSkipAccel = false;
        _file = newsrc;
        _file->writeLock();
        reset();
      }
    }
    _resetFieldMetadata();
    _file->unlock();
  }
}

void DataVector::_resetFieldMetadata() {
  _resetFieldScalars();
  _resetFieldStrings();
}

void DataVector::_resetFieldStrings() {
  // Note: this does not necessarily preseve order if the 
  // datasource or field have been changed.  If dynamic
  // fieldScalars are ever wanted, this should be fixed.
  QStringList string_names = dataSource()->fieldStrings(field());
  QStringList string_values;
  dataSource()->readFieldStrings(string_values, field(), true);
  StringPtr sp;
  QString key;

  QStringList fieldStringKeys = _fieldStrings.keys();
  // remove field strings that no longer need to exist
  readLock();
  for (int i=0; i<fieldStringKeys.count(); i++) {
    key = fieldStringKeys.at(i);
    if (!string_names.contains(key)) {
      StringPtr sp = _fieldStrings[key];
      _strings.remove(key);
      _fieldStrings.remove(key);
      sp = 0L;
    }
  }
  // find or insert strings, to set their value
  for (int i=0; i<string_names.count(); i++) {
    key = string_names.at(i);
    if (!_fieldStrings.contains(key)) { // insert a new one
      _strings.insert(key, sp = store()->createObject<String>());
      _fieldStrings.insert(key, sp);
      sp->setProvider(this);
      sp->setSlaveName(string_names.at(i));
      sp->_KShared_ref();
    } else {  // find it
      sp = _fieldStrings[key];
    }
    sp->setValue(string_values[i]);
  }
  unlock();
}


void DataVector::_resetFieldScalars() {
  // Note: this does not necessarily preseve order if the 
  // datasource or field have been changed.  If dynamic
  // fieldScalars are ever wanted, this should be fixed.
  QStringList scalar_names = dataSource()->fieldScalars(field());
  QList<double> scalar_values;
  dataSource()->readFieldScalars(scalar_values, field(), true);

  ScalarPtr sp;
  QString key;

  QStringList fieldScalarKeys = _fieldScalars.keys();
  // remove field scalars that no longer need to exist
  readLock();
  for (int i=0; i<fieldScalarKeys.count(); i++) {
    key = fieldScalarKeys.at(i);
    if (!scalar_names.contains(key)) {
      ScalarPtr sp = _fieldScalars[key];
      _scalars.remove(key);
      _fieldScalars.remove(key);
      sp = 0L;
    }
  }
  // find or insert scalars, to set their value
  for (int i=0; i<scalar_names.count(); i++) {
    key = scalar_names.at(i);
    if (!_fieldScalars.contains(key)) { // insert a new one
      _scalars.insert(key, sp = store()->createObject<Scalar>());
      _fieldScalars.insert(key, sp);
      sp->setProvider(this);
      sp->setSlaveName(scalar_names.at(i));
      sp->_KShared_ref();
    } else {  // find it
      sp = _fieldScalars[key];
    }
    sp->setValue(scalar_values[i]);
  }
  unlock();
}

DataSourcePtr DataVector::dataSource() const {
  return _file;
}

DataVectorPtr DataVector::makeDuplicate() const {
  Q_ASSERT(store());
  DataVectorPtr vector = store()->createObject<DataVector>();

  vector->writeLock();
  vector->change(_file, _field, ReqF0, ReqNF, Skip, DoSkip, DoAve);
  if (descriptiveNameIsManual()) {
    vector->setDescriptiveName(descriptiveName());
  }

  vector->update();
  vector->unlock();

  return vector;
}

QString DataVector::_automaticDescriptiveName() const {
  QString name = field();
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

}
// vim: ts=2 sw=2 et
