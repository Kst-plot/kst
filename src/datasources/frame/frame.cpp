/***************************************************************************
                 frame.cpp  -  data source for frames
                             -------------------
    begin                : Tue Oct 21 2003
    copyright            : (C) 2003 The University of Toronto
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

#include "creaddata.h"
#include "frame.h"
#include "readdata.h"

#include <kdebug.h>
#include <klocale.h>

#include <qfile.h>

#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


FrameSource::FrameSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type) {
  _valid = init();
}


FrameSource::~FrameSource() {
}


bool FrameSource::reset() {
  _fieldList.clear();
  return _valid = init();
}


bool FrameSource::init() {
  int buf[4], err = 0;

  _fieldList.append("INDEX");
  ReadData(_filename.toLatin1(), "FFINFO",
           0, 0, /* 1st sframe, 1st samp */
           0, 2, /* num sframes, num samps */
           'i', (void*)buf,
           &err);

  if (err != E_OK) {
    return false;
  }

  _bytesPerFrame = buf[0];
  _framesPerFile = buf[1];
  _frameCount = 0;

  /* split out file name and extension */
  int len = _filename.length();
  char ext[3];
  ext[0] = _filename.toLatin1()[len - 2];
  ext[1] = _filename.toLatin1()[len - 1];
  ext[2] = '\0';

  _rootFileName = _filename;

  if (isxdigit(ext[0]) && isxdigit(ext[1])) {
    char *tmpstr = 0L;
    _rootFileName.truncate(_rootFileName.length() - 2);
    _rootExt = strtol(ext, &tmpstr, 16);
    _maxExt = _rootExt;
  } else {
    _maxExt = _rootExt = -1;
  }
  return update() == KstObject::UPDATE;
}


KstObject::UpdateType FrameSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  QString tmpfilename;
  struct stat stat_buf;
  int done = 0;
  int dec = 0;
  int newN;

  if (_maxExt < 0) { // no hex number extension: only this file
    if (stat(_filename.toLatin1(), &stat_buf) != 0) { // file is gone
      newN = 0;
    } else {                               // file exists
      newN = stat_buf.st_size/_bytesPerFrame;
    }
  } else {
    do {
      tmpfilename.sprintf("%s%2.2x", _rootFileName.toLatin1(), _maxExt);
      if (stat(QFile::encodeName(tmpfilename).data(), &stat_buf) != 0) {
          if (_maxExt > _rootExt) {  // deleted (?) check the next one down
            _maxExt--;
            dec = 1;
          } else {                      // All files have been deleted
            stat_buf.st_size = 0;
            done = 1;
          }
      } else {
        if (stat_buf.st_size == _bytesPerFrame*_framesPerFile) { // Full file
          if (dec) { // already checked next one up: it is empty
            done = 1;
          } else {
            _maxExt++;
          }
        } else {
          done = 1;
        }
      }
    } while (!done);
    newN = (_maxExt - _rootExt)*_framesPerFile + stat_buf.st_size/_bytesPerFrame;
  }

  bool isnew = _frameCount != newN;
  _frameCount = newN;

  updateNumFramesScalar();
  return setLastUpdateResult(isnew ? KstObject::UPDATE : KstObject::NO_CHANGE);
}


int FrameSource::readField(double *v, const QString& field, int s, int n) {
  int err = 0;

  if (n < 0) {
    return CReadData(_filename.toLatin1(), field.left(8).toLatin1(),
                     s, 0, /* 1st sframe, 1st samp */
                     0, 1, /* num sframes, num samps */
                     'd', (void*)v,
                     &err);
  } else {
    return CReadData(_filename.toLatin1(), field.left(8).toLatin1(),
                     s, 0, /* 1st sframe, 1st samp */
                     n, 0, /* num sframes, num samps */
                     'd', (void*)v,
                     &err);
  }
}


bool FrameSource::isValidField(const QString& field) const {
  int err = 0;

  CReadData(_filename.toLatin1(), field.left(8).toLatin1(),
            0, 0, /* 1st sframe, 1st samp */
            1, 0, /* num sframes, num samps */
            'n', (void*)0L,
            &err);

  return err == 0;
}


int FrameSource::samplesPerFrame(const QString &field) {
  int err = 0;

  return CReadData(_filename.toLatin1(), field.left(8).toLatin1(),
                   0, 0, /* 1st sframe, 1st samp */
                   1, 0, /* num sframes, num samps */
                   'n', (void*)0L,
                   &err);
}


int FrameSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


QString FrameSource::fileType() const {
  return "Binary Frame";
}


bool FrameSource::isEmpty() const {
  return _frameCount < 1;
}


void FrameSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}


bool FrameSource::fieldListIsComplete() const {
  return false;
}


extern "C" {
KstDataSource *create_frame(KConfig *cfg, const QString& filename, const QString& type) {
  return new FrameSource(cfg, filename, type);
}

QStringList provides_frame() {
  QStringList rc;
  rc += "Binary Frame";
  return rc;
}

int understands_frame(KConfig*, const QString& filename) {
  int err = 0;
  CReadData(filename.toLatin1(), "INDEX",
            0, 0, /* 1st sframe, 1st samp */
            1, 0, /* num sframes, num samps */
            'n', (void*)0L,
            &err);
  if (err == E_OK) {
    return 98;
  }

  return 0;
}

}

KST_KEY_DATASOURCE_PLUGIN(frame)

// vim: ts=2 sw=2 et
