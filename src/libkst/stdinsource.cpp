/***************************************************************************
                    stdinsource.cpp  -  data source for stdin
                             -------------------
    begin                : Fri Oct 31 2003
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

#include "config.h"
#include "stdinsource.h"

#include <k3tempfile.h>

#include <stdlib.h>
#include <unistd.h>

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

KstStdinSource::KstStdinSource(KConfig *cfg)
: KstDataSource(cfg, "stdin", "stdin") {
  _file = new K3TempFile;
  _filename = _file->name();
  // Unfortunately we have to update here.  stdin is a special case.
  update();
  _src = KstDataSource::loadSource(_filename, "ASCII");
  if (_src && _src->isValid()) {
    _valid = true;
  }
}


KstStdinSource::~KstStdinSource() {
  _file->close();
  _file->unlink();
  delete _file;
  _file = 0L;
}


KstObject::UpdateType KstStdinSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  if (!_valid) {
    _src = KstDataSource::loadSource(_filename, "ASCII");
    if (_src && _src->isValid()) {
      _valid = true;
    } else {
      return setLastUpdateResult(KstObject::NO_CHANGE);
    }
  }

  fd_set rfds;
  struct timeval tv;
  int retval;
  char instr[4097];
  int i = 0;
  bool new_data = false;
  bool got_some = false;

  FILE *fp = _file->fstream();

  if (!fp) {
    return setLastUpdateResult(KstObject::NO_CHANGE);
  }

  do {
    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    /* Wait up to 0 seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);

    new_data = false;
    if (retval > 0) {
      char *fgs = fgets(instr, 4096, stdin);
      if (fgs && fp) {
        got_some = true;
        fputs(instr, fp);
        new_data = true;
      }
    }
  } while (++i < 100000 && new_data);

  fflush(fp);

  if (got_some && _src) {
    return setLastUpdateResult(_src->update(u));
  }
  return setLastUpdateResult(KstObject::NO_CHANGE);
}


int KstStdinSource::readField(double *v, const QString& field, int s, int n) {
  if (isValid()) {
    return _src->readField(v, field, s, n);
  }
  return -1;
}


bool KstStdinSource::isValidField(const QString& field) const {
  if (isValid()) {
    return _src->isValidField(field);
  }
  return false;
}


int KstStdinSource::samplesPerFrame(const QString& field) {
  if (isValid()) {
    return _src->samplesPerFrame(field);
  }
  return 0;
}


int KstStdinSource::frameCount(const QString& field) const {
  if (isValid()) {
    return _src->frameCount(field);
  }
  return 0;
}


QString KstStdinSource::fileType() const {
  if (isValid()) {
    return _src->fileType();
  }
  return QString::null;
}


void KstStdinSource::save(QTextStream &ts, const QString& indent) {
  if (isValid()) {
    return _src->save(ts, indent);
  }
  KstDataSource::save(ts, indent);
}


bool KstStdinSource::isValid() const {
  return _valid && _src && _src->isValid();
}


bool KstStdinSource::isEmpty() const {
  return !isValid() || _src->isEmpty();
}

// vim: ts=2 sw=2 et
