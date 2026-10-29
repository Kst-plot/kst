/***************************************************************************
                          datavector.cpp  -  a vector which gets its data from
                          a datasource.
                             -------------------
    begin                : Fri Sep 22 2000
    copyright            : (C) 2000-2026 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
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
#include "datavector.h"

#include <assert.h>
#include <climits>
#include <cmath>
#include <math.h>
#include <stdlib.h>

#include <QDebug>
#include <QXmlStreamWriter>

#include "datacollection.h"
#include "datasource.h"
#include "debug.h"
#include "math_kst.h"
#include "objectstore.h"
#include "updatemanager.h"
#include "vectorscriptinterface.h"

// _readToEnd  _countFromEnd   Action
//  true        false           read from ReqF0 to end of file
//  false       true            read the last ReqNF frames from end of file
//  false       false           read ReqNF frames starting at frame ReqF0
//  true        true            illegal: fixed in checkIntegrity (resets to
//  readToEnd)

namespace Kst {

const QString DataVector::staticTypeString = "Data Vector";
const QString DataVector::staticTypeTag = "datavector";

// const int INVALIDS_PER_RESET = 5;

DataVector::DataInfo::DataInfo() : frameCount(-1), samplesPerFrame(-1) {}

DataVector::DataInfo::DataInfo(double fc, int spf)
    : frameCount(fc), samplesPerFrame(spf) {}

/** Create a DataVector: raw data from a file */
DataVector::DataVector(ObjectStore *store)
    : Vector(store), DataPrimitive(this) {

  _saveable = true;
  _numSamples = 0;
  _scalars["sum"]->setValue(0.0);
  _scalars["sumsquared"]->setValue(0.0);
  F0 = NF = 0; // nothing read yet

  N_AveReadBuf = 0;
  AveReadBuf = 0L;

  ReqF0 = 0;
  ReqNF = 0;
  _countFromEnd = false;
  _readToEnd = true; // default: read to end of file
  Skip = 1;
  DoSkip = false;
  DoAve = false;
  _invalidCount = 0;
}

QString DataVector::typeString() const { return staticTypeString; }

/** return true if it has a valid file and field, or false otherwise */
bool DataVector::isValid() const {
  if (dataSource()) {
    dataSource()->readLock();
    bool rc = dataSource()->vector().isValid(_field);
    dataSource()->unlock();
    return rc;
  }
  return false;
}

ScriptInterface *DataVector::createScriptInterface() {
  return new DataVectorSI(this);
}

bool DataVector::checkValidity(const DataSourcePtr &ds) const {
  if (ds) {
    ds->readLock();
    bool rc = ds->vector().isValid(_field);
    ds->unlock();
    return rc;
  }
  return false;
}

void DataVector::change(DataSourcePtr in_file, const QString &in_field,
                        double in_f0, bool in_countFromEnd, double in_n,
                        bool in_readToEnd, int in_skip, bool in_DoSkip,
                        bool in_DoAve) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  Skip = in_skip;
  DoSkip = in_DoSkip;
  DoAve = in_DoAve;
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  setDataSource(in_file);
  ReqF0 = in_f0;
  ReqNF = in_n;
  _countFromEnd = in_countFromEnd;
  _readToEnd = in_readToEnd;
  _field = in_field;

  // Illegal: countFromEnd and readToEnd simultaneously — default to read whole
  // file
  if (_countFromEnd && _readToEnd) {
    _countFromEnd = false;
  }

  if (dataSource()) {
    dataSource()->writeLock();
  }
  reset();
  if (dataSource()) {
    dataSource()->unlock();
  }
  registerChange();
}

qint64 DataVector::minInputSerial() const {
  if (dataSource()) {
    return (dataSource()->serial());
  }
  return LLONG_MAX;
}

qint64 DataVector::maxInputSerialOfLastChange() const {
  if (dataSource()) {
    return (dataSource()->serialOfLastChange());
  }
  return NoInputs;
}

void DataVector::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(
        tr("Data file for vector %1 was not opened.").arg(Name()),
        Debug::Warning);
  }
  setDataSource(in_file);
  if (dataSource()) {
    dataSource()->writeLock();
  }
  reset();
  if (dataSource()) {
    dataSource()->unlock();
  }
  registerChange();
}

void DataVector::changeFrames(double in_f0, bool in_countFromEnd, double in_n,
                              bool in_readToEnd, int in_skip, bool in_DoSkip,
                              bool in_DoAve) {

  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (dataSource()) {
    dataSource()->writeLock();
  }
  reset();
  if (dataSource()) {
    dataSource()->unlock();
  }
  Skip = in_skip;
  DoSkip = in_DoSkip;
  DoAve = in_DoAve;
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  ReqF0 = in_f0;
  ReqNF = in_n;
  _countFromEnd = in_countFromEnd;
  _readToEnd = in_readToEnd;

  // Illegal: countFromEnd and readToEnd simultaneously — default to read whole
  // file
  if (_countFromEnd && _readToEnd) {
    _countFromEnd = false;
  }

  registerChange();
}

void DataVector::setFromEnd() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _countFromEnd = true;
  _readToEnd = false;
  ReqF0 = 0;
  if (ReqNF < 2) {
    ReqNF = numFrames();
    if (ReqNF < 2) {
      _countFromEnd = false; // fallback: too few frames, just read from start
      ReqF0 = 0;
    }
  }
  registerChange();
}

DataVector::~DataVector() {
  if (AveReadBuf) {
    free(AveReadBuf);
    AveReadBuf = 0L;
  }
}

bool DataVector::readToEOF() const { return _readToEnd; }

bool DataVector::countFromEOF() const { return _countFromEnd; }

bool DataVector::reqCountFromEnd() const { return _countFromEnd; }

bool DataVector::reqReadToEnd() const { return _readToEnd; }

/** Return Starting Frame of Vector */
double DataVector::startFrame() const { return F0; }

/** Return frames per skip to read */
int DataVector::skip() const { return DoSkip ? Skip : 0; }

bool DataVector::doSkip() const { return DoSkip; }

bool DataVector::doAve() const { return DoAve; }

/** Return frames held in Vector */
double DataVector::numFrames() const { return NF; }

double DataVector::reqNumFrames() const { return ReqNF; }

double DataVector::reqStartFrame() const { return ReqF0; }

/** Save vector information */
void DataVector::save(QXmlStreamWriter &s) {
  if (dataSource()) {
    s.writeStartElement("datavector");
    saveFilename(s);
    s.writeAttribute("field", _field);

    s.writeAttribute("start", QString::number(ReqF0, 'g', 17));
    s.writeAttribute("count", QString::number(ReqNF, 'g', 17));
    s.writeAttribute("countFromEnd", _countFromEnd ? "true" : "false");
    s.writeAttribute("readToEnd", _readToEnd ? "true" : "false");

    if (doSkip()) {
      s.writeAttribute("skip", QString::number(Skip));
      if (doAve()) {
        s.writeAttribute("doAve", "true");
      }
    } else {
      s.writeAttribute("skip", QString::number(-1));
      s.writeAttribute("doAve", "false");
    }

    s.writeAttribute("startUnits", startUnits());
    s.writeAttribute("rangeUnits", rangeUnits());

    saveNameInfo(s, VECTORNUM | SCALARNUM);
    s.writeEndElement();
  }
}

/**
 * @brief Generate default label info for axis associated with this vector.
 * Use meta-scalars "units" or "quantity" if they are defined.
 * Escape special characters in the field name.
 *
 * @return LabelInfo
 **/

LabelInfo DataVector::labelInfo() const {
  LabelInfo label_info;

  if (_fieldStrings.contains("quantity")) {
    label_info.quantity = _fieldStrings.value("quantity")->value();
    label_info.quantity.replace('[', "\\[").replace(']', "\\]");
  } else {
    label_info.quantity.clear();
  }

  if (_fieldStrings.contains("units")) {
    label_info.units = _fieldStrings.value("units")->value();
    label_info.units.replace('[', "\\[").replace(']', "\\]");
  } else {
    label_info.units.clear();
  }

  label_info.name = descriptiveName(); // _field;

  label_info.file = filename();

  return label_info;
}

void DataVector::reset() { // must be called with a lock
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (dataSource()) {
    SPF = dataInfo(_field).samplesPerFrame;
  }
  F0 = NF = 0;
  resize(0);
  _numSamples = 0;
  _dirty = true;
  _resetFieldMetadata();

  Object::reset();
}

bool DataVector::checkIntegrity() {
  if (DoSkip && Skip < 1) {
    Skip = 1;
  }

  if (_dirty) {
    reset();
  }

  // if the file seems to have shrunk/changed, return false.
  // if it has happened several times in a row, assume the
  // file has been over-written, and re-read it.
  // this is a hack to handle glitchy file system situations.
  // TODO: there has to be a better way.
  // const DataInfo info = dataInfo(_field);
  // if (dataSource() && (SPF != info.samplesPerFrame || info.frameCount < NF))
  // {
  //   _invalidCount++;
  //   if (_invalidCount>INVALIDS_PER_RESET) {

  //     reset();
  //     _invalidCount=0;
  //   }
  //   return false;
  // }

  // check for illegal mode: countFromEnd and readToEnd both set
  if (_countFromEnd && _readToEnd) {
    _countFromEnd = false; // default to read whole file from start
  }

  _invalidCount = 0;
  return true;
}

// Some things to consider about the following routine...
// Frames:
//    Some data sources have data divided into frames.  Each field
//    has a fixed number of samples per frame.  For some (eg, ascii files)
//    each frame has 1 sample.  For others (eg, dirfiles) you may have more.
//    Different fields in the same data source may have different samples per
//    frame. Within a data source, it is assumed that the first sample of each
//    frame is simultaneous between fields.
// Last Frame Read:
//    Only read the first sample of the last frame read, in cases where there
//    are more than one sample per frame.   This allows for more sensible
//    association of vectors into curves, when the X and Y vectors have
//    different numbers of samples per frame. The rule is that we assume is that
//    the first sample of each frame is simultaneous.
// Skip reading:
//    -'Skip' means read 1 sample each 'Skip' frames (not read one sample,
//     then skip 'Skip' samples or something else).
//    -In order that the data are not re-drawn each time a new sample arrives,
//    and to
//     ensure the re-usability (via shifting) of previously read data, and to
//     make a region of data look the same regardless of the chouse of f0, all
//     samples read with skip enabled are read on 'skip boundries'... ie, the
//     first samples of frame 0, Skip, 2*Skip... N*skip, and never M*Skip+1.

void DataVector::internalUpdate() {
  int i, k, shift, n_read = 0;
  int ave_nread;
  double new_f0, new_nf;
  bool start_past_eof = false;

  if (dataSource()) {
    dataSource()->writeLock();
  } else {
    return;
  }

  const DataInfo info = dataInfo(_field);
  if (!checkIntegrity()) {
    if (dataSource()) {
      dataSource()->unlock();
    }
    return;
  }

  // Prefer at least two frames when the source can provide them, but do not
  // manufacture a second frame for one-frame sources.
  if (ReqNF == 1 && info.frameCount >= 2) {
    ReqNF = 2;
  }

  if (DoSkip && Skip < 2 && SPF == 1) {
    DoSkip = false;
  }

  // set new_nf and new_f0
  double fc = info.frameCount;
  const bool ssdb = dataSource()->fileType().contains("(SSDB)");
  if (_readToEnd) { // read to end of file
    new_f0 = ReqF0;
    new_nf = fc - new_f0;
  } else if (_countFromEnd) { // count back from end of file
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

  // frame ranges always need to be integer.  Truncate.
  new_f0 = std::trunc(new_f0);
  new_nf = std::trunc(new_nf);
  if (_readToEnd) {
    new_nf = fc - new_f0;
  } else if (_countFromEnd) {
    new_f0 = fc - new_nf;
  }

  if (DoSkip) {
    // in count from end mode, change new_f0 and new_nf so they both lie on skip
    // boundaries
    if ((new_f0 != 0) && _countFromEnd) {
      new_f0 = ((new_f0 - 1) / Skip + 1) * Skip;
    }
    new_nf = (new_nf / Skip) * Skip;
  }

  // In particular an epoch-origin SSDB source can report billions of frames.
  // Reject unrepresentable/unsafe vector sizes *before* resizing or casting to
  // int.
  const long double requestedSamples =
      DoSkip ? (long double)new_nf / Skip
             : ((long double)new_nf - 1) * info.samplesPerFrame + 1;
  if (!std::isfinite(new_f0) || !std::isfinite(new_nf) || new_f0 < 0 ||
      new_nf < 0 || info.samplesPerFrame < 1 || requestedSamples < 0 ||
      requestedSamples > INT_MAX || (ssdb && requestedSamples > 1000000000)) {
    // Fixme: we should estimate max vector size based on available memory, not
    // just 1GB for SSDB. The Rule should be for everyone.  But make sure it
    // doesn't kill performance.
    dataSource()->unlock();
    return;
  }

  const int recheckFrames = dataSource()->framesToRecheck(_field);
  if (recheckFrames > 0 && NF > 0) {
    if (DoSkip) {
      // Both NF and the read loop are skip-aligned: revisit complete groups
      // so old plotted values are replaced without resetting the vector.
      const double rewind = std::ceil(double(recheckFrames) / Skip) * Skip;
      NF = qMax(0.0, NF - rewind);
      _numSamples = int(NF / Skip);
    } else {
      NF = qMax(0.0, NF - double(recheckFrames));
      // With SPF > 1 the existing read path rereads the last retained frame
      // in full, so only count the earlier frames as reusable samples.
      _numSamples = NF > 0 ? int((SPF > 1 ? NF - 1 : NF) * SPF) : 0;
    }
  }

  // shift vector if necessary
  if ((NF > new_nf) || new_f0 < F0 ||
      new_f0 >= F0 + NF) { // No useful data around.
    reset();
  } else { // shift stuff rather than re-read
    if (DoSkip) {
      shift = (int)((new_f0 - F0) / Skip);
      NF -= (new_f0 - F0);
      _numSamples = (int)(NF / Skip);
    } else {
      shift = (int)(SPF * (new_f0 - F0));
      NF -= (new_f0 - F0);
      //_numSamples = (NF-1)*SPF;
      if (shift > 0) {
        if (SPF == 1) {
          _numSamples = (int)NF;
        } else {
          _numSamples = (int)((NF - 1) * SPF);
        }
      }
    }

    memmove(_v_raw, _v_raw + shift, _numSamples * sizeof(double));
  }

  if (DoSkip) {
    // reallocate V if necessary
    if ((int)(new_nf / Skip) != _size) {
      if (!resize((int)(new_nf / Skip))) {
        // TODO: Is aborting all we can do?
        fatalError("Not enough memory for vector data");
        return;
      }
    }
    n_read = 0;
    /** read each sample from the File */
    double *t = _v_raw + _numSamples;
    int new_nf_Skip = (int)(new_nf - Skip);
    if (DoAve) {
      for (i = (int)NF; new_nf_Skip >= i; i += Skip) {
        /* enlarge AveReadBuf if necessary */
        if (N_AveReadBuf < Skip * SPF) {
          N_AveReadBuf = Skip * SPF;
          if (!kstrealloc(AveReadBuf, N_AveReadBuf * sizeof(double))) {
            qCritical() << "Vector resize failed";
          }
          if (!AveReadBuf) {
            // FIXME: handle failed resize
          }
        }
        ave_nread = readField(AveReadBuf, _field, new_f0 + i, Skip);
        for (k = 1; k < ave_nread; ++k) {
          AveReadBuf[0] += AveReadBuf[k];
        }
        if (ave_nread > 0) {
          *t = AveReadBuf[0] / double(ave_nread);
          n_read++;
        }
        ++t;
      }
    } else {
      for (i = (int)NF; new_nf_Skip >= i; i += Skip) {
        n_read += readField(t++, _field, new_f0 + i, 1, -1, true);
      }
    }
  } else {
    // reallocate V if necessary
    if ((int)((new_nf - 1) * SPF + 1) != _size) {
      if (!resize((int)((new_nf - 1) * SPF + 1))) {
        // TODO: Is aborting all we can do?
        fatalError("Not enough memory for vector data");
        return;
      }
    }

    // read the new data from file
    if (start_past_eof) {
      _v_raw[0] = NOPOINT;
      n_read = 1;
    } else if (info.samplesPerFrame > 1) {
      if (NF > 0) {
        NF--; /* last frame read was only partially read... */
      }
      int safe_nf = (int)(new_nf > 0 ? new_nf : 0);

      assert(new_f0 + NF >= 0);
      // assert(new_f0 + safe_nf - 1 >= 0);
      if ((new_f0 + safe_nf - 1 >= 0) && (safe_nf - NF > 1)) {
        n_read = readField(_v_raw + (qint64)(NF * SPF), _field, new_f0 + NF,
                           safe_nf - NF - 1);
        n_read += readField(_v_raw + (qint64)((safe_nf - 1) * SPF), _field,
                            new_f0 + safe_nf - 1, 1, -1, true);
      }
    } else {
      assert(new_f0 + NF >= 0);
      if (new_nf - NF > 0 || new_nf - NF == -1) {
        n_read = readField(_v_raw + (qint64)(NF * SPF), _field, new_f0 + NF,
                           new_nf - NF);
      }
    }
  }
  _numNew = _size - _numSamples;
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
    for (i = _numSamples; i < _size; ++i) {
      _v_raw[i] = _v_raw[0];
    }
  }

  if (_numNew > _size) {
    _numNew = _size;
  }
  if (_numShifted > _size) {
    _numShifted = _size;
  }

  if (dataSource()) {
    dataSource()->unlock();
  }

  if (n_read > 0) {
    Vector::internalUpdate();
  }
}

QByteArray DataVector::scriptInterface(QList<QByteArray> &c) {
  Q_ASSERT(c.size());
  if (c[0] == "changeFrames") {
    if (c.size() != 8) {
      return QByteArray("Bad parameter count (!=7).");
    }
    // args: f0 countFromEnd n readToEnd skip doSkip doAve
    changeFrames(c[1].toDouble(), (c[2] == "true"), c[3].toDouble(),
                 (c[4] == "true"), c[5].toInt(), (c[6] == "true") ? 1 : 0,
                 (c[7] == "true") ? 1 : 0);
    return QByteArray("Done.");
  } else if (c[0] == "numFrames") {
    return QByteArray::number(numFrames());
  } else if (c[0] == "startFrame") {
    return QByteArray::number(startFrame());
  } else if (c[0] == "doSkip") {
    return QByteArray(doSkip() ? "true" : "false");
  } else if (c[0] == "doAve") {
    return QByteArray(doAve() ? "true" : "false");
  } else if (c[0] == "skip") {
    return QByteArray::number(skip());
  } else if (c[0] == "reload") {
    reload();
    return QByteArray("Done");
  } else if (c[0] == "samplesPerFrame") {
    return QByteArray::number(samplesPerFrame());
  } else if (c[0] == "fileLength") {
    return QByteArray::number(fileLength());
  } else if (c[0] == "readToEOF") {
    return QByteArray(readToEOF() ? "true" : "false");
  } else if (c[0] == "countFromEOF") {
    return QByteArray(countFromEOF() ? "true" : "false");
  } else if (c[0] == "descriptionTip") {
    return QByteArray(descriptionTip().toLatin1());
  } else if (c[0] == "isValid") {
    return isValid() ? "true" : "false";
  }

  return "No such command...";
}

/** Returns intrinsic samples per frame */
int DataVector::samplesPerFrame() const { return SPF; }

double DataVector::fileLength() const {

  if (dataSource()) {
    double rc = dataInfo(_field).frameCount;

    return rc;
  }

  return 0;
}

void DataVector::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (dataSource()) {
    dataSource()->writeLock();
    dataSource()->reset();
    dataSource()->unlock();
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
  const QMap<QString, QString> meta_strings =
      dataSource()->vector().metaStrings(_field);

  QStringList fieldStringKeys = _fieldStrings.keys();
  // remove field strings that no longer need to exist
  readLock();
  for (int i = 0; i < fieldStringKeys.count(); ++i) {
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
    } else { // find it
      sp = _fieldStrings[key];
    }
    sp->setValue(it.value());
  }
  unlock();
}

void DataVector::_resetFieldScalars() {
  const QMap<QString, double> meta_scalars =
      dataSource()->vector().metaScalars(_field);

  QStringList fieldScalarKeys = _fieldScalars.keys();
  // remove field scalars that no longer need to exist
  readLock();
  for (int i = 0; i < fieldScalarKeys.count(); ++i) {
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
    } else { // find it
      sp = _fieldScalars[key];
    }
    sp->setValue(it.value());
  }
  unlock();
}

PrimitivePtr DataVector::makeDuplicate() const {
  Q_ASSERT(store());
  DataVectorPtr vector = store()->createObject<DataVector>();

  vector->writeLock();
  vector->change(dataSource(), _field, ReqF0, _countFromEnd, ReqNF, _readToEnd,
                 Skip, DoSkip, DoAve);
  if (descriptiveNameIsManual()) {
    vector->setDescriptiveName(descriptiveName());
  }

  vector->registerChange();
  vector->unlock();

  return kst_cast<Primitive>(vector);
}

QString DataVector::_automaticDescriptiveName() const {
  QString name;
  name = _field;
  // un-escape escaped special characters so they aren't escaped 2x.
  name.replace("\\_", "_")
      .replace("\\^", "^")
      .replace("\\[", "[")
      .replace("\\]", "]");
  // now escape the special characters.
  name.replace('_', "\\_")
      .replace('^', "\\^")
      .replace('[', "\\[")
      .replace(']', "\\]");
  return name;
}

QString DataVector::descriptionTip() const {
  QString IDstring;
  // QString range_string;

  if (dataSource()) {
    IDstring = tr("Data Vector: %1\n"
                  "  %2\n"
                  "  Field: %3")
                   .arg(Name())
                   .arg(dataSource()->fileName())
                   .arg(_field);
    if (countFromEOF()) {
      IDstring += tr("\n  Last %1 frames.").arg(numFrames());
    } else if (readToEOF()) {
      IDstring += tr("\n  Frame %1 to end.").arg(startFrame());
    } else {
      IDstring += tr("\n  %1 Frames starting at %2.")
                      .arg(numFrames())
                      .arg(startFrame());
    }
    if (skip()) {
      if (!doAve()) {
        IDstring += tr("\n  Read 1 sample per %1 frames.").arg(skip());
      } else {
        IDstring += tr("\n  Average each %1 frames.").arg(skip());
      }
    }
  } else {
    IDstring.clear();
  }

  return IDstring;
}

QString DataVector::propertyString() const {
  return tr("%2 F0: %3 N: %4 of %1",
            "%2 is a variable name.  F0 is short for the first element.  N is "
            "the number of elements")
      .arg(dataSource()->fileName())
      .arg(_field)
      .arg(startFrame())
      .arg(numFrames());
}

int DataVector::readField(double *v, const QString &field, double s, double n,
                          double skip, bool singleSample) {
  ReadInfo par;
  par.data = v;
  par.startingFrame = s;
  // Many datasource plugins use numberOfFrames < 0 as the "read exactly 1
  // sample" convention.  Preserve that contract so existing plugins keep
  // working correctly.
  par.numberOfFrames = singleSample ? -1 : n;
  par.skipFrame = skip;
  par.singleSample = singleSample;
  return dataSource()->vector().read(field, par);
}

const DataVector::DataInfo DataVector::dataInfo(const QString &field) const {
  dataSource()->readLock();
  const DataInfo info = dataSource()->vector().dataInfo(field);
  dataSource()->unlock();
  return info;
}

bool DataVector::isCTime() const {
  for (const IndexFieldProperties &ifp : dataSource()->indexFieldProperties()) {
    if (ifp.name == _field) {
      return ifp.is_ctime;
    }
  }
  return false;
}

} // namespace Kst
// vim: ts=2 sw=2 et
