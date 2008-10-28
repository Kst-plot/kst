/***************************************************************************
                    nadsource.cpp  -  NAD data source plugin
                             -------------------
    begin                : May 02 2006
    copyright            : (C) 2006 Staikos Computing Services Inc.
    email                : info@staikos.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "nadsource.h"

#include <qapplication.h>
#include <qevent.h>
#include <qwaitcondition.h>
#include <ksdebug.h>
#include <kurl.h>

using namespace NAD;

#ifdef NAN
double NOPOINT = NAN;
#else
double NOPOINT = 0.0/0.0; /* NaN */
#endif
 
NADSource::NADSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type), _filename(filename), _firstTime(true) {
  kstdDebug() << "NADSource constructor (filename=" + filename + ")" << endl;

  _cache = new NADCache();

  _cacheConn = new NADCacheAgentConnection();
  _cacheConn->connectToCacheAgent();

  _serverConn = new NADConnection();
  _serverConn->setUrl(filename);

  if (init()) {
    _valid = true;
  }
}


NADSource::~NADSource() {
  kstdDebug() << "NADSource destructor\n";
  delete _cache;
  delete _cacheConn;
  delete _serverConn;
}


bool NADSource::init() {
  kstdDebug() << "NADSource:init()\n";

  KstObject::UpdateType u = update();

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->lock(_filename, "INDEX", NADCacheAgentConnection::READLOCK);
  }

  _fieldList = _cache->getFields(_filename);

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->unlock(_filename, "INDEX");
  }

  if (!_fieldList.empty()) {
    _fieldList.prepend("INDEX");
  }

  return u == KstObject::UPDATE;
}


KstObject::UpdateType NADSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  if (_firstTime) {
    _firstTime = false;
    updateNumFramesScalar();
    return setLastUpdateResult(KstObject::UPDATE);
  }

  int updated = 0;
  if (_cacheConn->isConnected()) {
    _cacheConn->lock(_filename, "", NADCacheAgentConnection::WRITELOCK);
    updated = _cacheConn->update(_filename, "");
    _cacheConn->unlock(_filename, "");
  }

  if (updated > 0) {
    updateNumFramesScalar();
    kstdDebug() << "NADSource:update(" << QString::number(u) << ") = UPDATE\n";
    return setLastUpdateResult(KstObject::UPDATE);
  } else if (updated == 0) {
    kstdDebug() << "NADSource:update(" << QString::number(u) << ") = NO_CHANGE\n";
    return setLastUpdateResult(KstObject::NO_CHANGE);
  } else {
    kstdDebug() << "NADSource:update(" << QString::number(u) << ") = ERROR (" << QString::number(updated) << ")\n";
    return setLastUpdateResult(KstObject::NO_CHANGE);
  }
}


// if n > 0, read n FRAMES starting at frame s for field into buffer v
// if n < 0, read 1 SAMPLE from frame s for field into buffer v
int NADSource::readField(double *v, const QString& field, int s, int n) {
  kstdDebug() << "NADSource:readField(" << field << ", s=" + QString::number(s) + ", n=" + QString::number(n) + ")" << endl;

  if (field == "INDEX") {
    if (n < 0) {
      v[0] = double(s);
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i);
    }
    return n > 0 ? n : 0;
  }

  if (!_valid) {
    kstdDebug() << "tried to read from an invalid NAD source" << endl;
    return -1;
  }

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->lock(_filename, field, NADCacheAgentConnection::READLOCK);
  }

  NADFieldInfo info = _cache->getFieldInfo(_filename, field);
  FrameRanges cacheFR = _cache->getFrameRanges(_filename, field);

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->unlock(_filename, field);
  }

  FrameRange fullRange = cacheFR.fullRange();

  if (s + fullRange.first > fullRange.second) {
    kstdDebug() << "Nothing to read: (" << fullRange.first << "," << fullRange.second << ") " << s << endl;
    return 0;
  }

  FrameRanges serverFR;
  if (n < 0) {
    // use skip=1 for one sample
    cacheFR = cacheFR.intersect(fullRange.first + s, fullRange.first + s, 1);
    serverFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s, 1);
  } else {
    cacheFR = cacheFR.intersect(fullRange.first + s, fullRange.first + s + n - 1);
    serverFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s + n - 1);
  }
  
  int samples = 0;
  int fetchRet;

  if (!serverFR.isEmpty()) {
    // fetch the data into the cache
    if (_cacheConn && _cacheConn->isConnected()) {
      _cacheConn->lock(_filename, field, NADCacheAgentConnection::WRITELOCK);
      fetchRet = _cacheConn->fetchData(_filename, field, serverFR);
      _cacheConn->unlock(_filename, field);
      if (fetchRet < 0) {
        kstdDebug() << "NADSource: error fetching frames " << serverFR.toString() << " for field " << field << " into NAD cache: " << fetchRet << endl;
      } else if (fetchRet == 0) {
        // data in cache
      } else if (fetchRet == 1) {
        // fetch directly from server
      }
    } else {
      // cache not connected so fetch directly from server
      fetchRet = 1;
    }
  }

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->lock(_filename, field, NADCacheAgentConnection::READLOCK);
  }

  FrameRanges missingFR;
  if (n < 0) {
    // use skip=1 for one sample
    cacheFR = _cache->getFrameRanges(_filename, field).intersect(fullRange.first + s, fullRange.first + s, 1);
    missingFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s, 1);
  } else {
    cacheFR = _cache->getFrameRanges(_filename, field).intersect(fullRange.first + s, fullRange.first + s + n - 1);
    missingFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s + n - 1);
  }

  if (!cacheFR.isEmpty()) {
    kstdDebug() << "NADSource: getting field " << field << " frames " << cacheFR.toString() << " from cache" << endl;

    int ret = _cache->getDataFromCache(_filename, field, 'd', v, cacheFR, cacheFR.firstFrame() - s);

    if (ret < 0) {
      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->unlock(_filename, field);
      }
      return ret;
    } else {
      samples += ret;
    }
  }

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->unlock(_filename, field);
  }

  if (!missingFR.isEmpty()) {
    int getDataRet;
    if (fetchRet == 1) {
      // fetch directly from server
      char *tempBuf = new char[(serverFR.lastFrame() - serverFR.firstFrame() + 1) * _serverConn->samplesPerFrame(field) * sizeForType(_serverConn->type(field))];
      if (_serverConn && _serverConn->isValid()) {
        // fetch data in native format and convert to doubles
        int getDataRet = _serverConn->getData(field, tempBuf, serverFR);
        if (getDataRet > 0) {
          convertType(tempBuf, _serverConn->type(field), v + ((serverFR.firstFrame() - s) * _serverConn->samplesPerFrame(field)), 'd', getDataRet);
          samples += getDataRet;
        }
      }
      delete[] tempBuf;
    }

    if (fetchRet < 0 || getDataRet < 0) {
      // fill in NaNs for missing data if NADCache::fetchData() or NADConnection::getData() fails
      QValueList<FrameRange> missing = missingFR.getRanges();
      QValueList<FrameRange>::const_iterator it;

      for (it = missing.begin(); it != missing.end(); ++it) {
        for (uint32_t i = (*it).first; i <= (*it).second/((missingFR.getSkip()) ? missingFR.getSkip() : 1); ++i) {
          if (missingFR.getSkip() == 0) {
            for (uint32_t j=0; j<info.spf; ++j) {
              v[(i-s)*info.spf + j] = NOPOINT;
              samples++;
            }
          } else {
            v[(i-s)*info.spf] = NOPOINT;
            samples++;
          }
        }
      }
    }
  }

  return samples;
}


/** Reads a field from the file.  Data is returned in the
 *  double Array v[].  Will skip according to the parameter, but it may not
 *  be implemented.  If it returns -9999, use the non-skip version instead. */
// if n > 0, read 1 SAMPLE from each of n FRAMES starting at frame s skipping by 'skip' for field into buffer v
// if n < 0, read 1 SAMPLE from frame s for field into buffer v
int NADSource::readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead) {
  if (skip == 0) {
    int ret = readField(v, field, s, n);
    // FIXME: deal with lastFrameRead
    return ret;
  }

  kstdDebug() << "NADSource:readField(" << field << ", s=" + QString::number(s) + ", n=" + QString::number(n) + ", skip=" + QString::number(skip) + ")" << endl;
  if (lastFrameRead) *lastFrameRead = -1;

  if (field == "INDEX") {
    if (n < 0) {
      v[0] = double(s);
      if (lastFrameRead) *lastFrameRead = s;
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i*skip);
    }
    if (lastFrameRead) *lastFrameRead = s + (n - 1) * skip;
    return n > 0 ? n : 0;
  }

  if (!_valid) {
    kstdDebug() << "tried to read from an invalid NAD source" << endl;
    return -1;
  }

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->lock(_filename, field, NADCacheAgentConnection::READLOCK);
  }

  NADFieldInfo info = _cache->getFieldInfo(_filename, field);
  FrameRanges cacheFR = _cache->getFrameRanges(_filename, field);

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->unlock(_filename, field);
  }

  FrameRange fullRange = cacheFR.fullRange();

  if (s + fullRange.first > fullRange.second) {
    kstdDebug() << "Nothing to read: (" << fullRange.first << "," << fullRange.second << ") " << s << endl;
    return 0;
  }

  FrameRanges serverFR;
  if (n < 0) {
    // use skip=1 for one sample
    cacheFR = cacheFR.intersect(fullRange.first + s, fullRange.first + s, 1);
    serverFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s, 1);
  } else {
    cacheFR = cacheFR.intersect(fullRange.first + s, fullRange.first + s + (n - 1)*skip, skip);
    serverFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s + (n - 1)*skip, skip);
  }
  
  int samples = 0;
  int fetchRet;

  if (!serverFR.isEmpty()) {
    // fetch the data into the cache
    if (_cacheConn && _cacheConn->isConnected()) {
      _cacheConn->lock(_filename, field, NADCacheAgentConnection::WRITELOCK);
      fetchRet = _cacheConn->fetchData(_filename, field, serverFR);
      _cacheConn->unlock(_filename, field);
      if (fetchRet < 0) {
        kstdDebug() << "NADSource: error fetching frames " << serverFR.toString() << " for field " << field << " into NAD cache: " << fetchRet << endl;
      } else if (fetchRet == 0) {
        // data in cache
      } else if (fetchRet == 1) {
        // fetch directly from server
      }
    } else {
      // cache not connected so fetch directly from server
      fetchRet = 1;
    }
  }

  FrameRanges missingFR;
  if (n < 0) {
    // use skip=1 for one sample
    cacheFR = _cache->getFrameRanges(_filename, field).intersect(fullRange.first + s, fullRange.first + s, 1);
    missingFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s, 1);
  } else {
    cacheFR = _cache->getFrameRanges(_filename, field).intersect(fullRange.first + s, fullRange.first + s + (n - 1)*skip, skip);
    missingFR = cacheFR.getMissingFrames(fullRange.first + s, fullRange.first + s + (n - 1)*skip, skip);
  }

  if (!cacheFR.isEmpty()) {
    kstdDebug() << "NADSource: getting field " << field << " frames " << cacheFR.toString() << " from cache" << endl;

    if (_cacheConn && _cacheConn->isConnected()) {
      _cacheConn->lock(_filename, field, NADCacheAgentConnection::READLOCK);
    }

    int ret = _cache->getDataFromCache(_filename, field, 'd', v, cacheFR, (cacheFR.firstFrame() - s) / skip);

    if (_cacheConn && _cacheConn->isConnected()) {
      _cacheConn->unlock(_filename, field);
    }

    if (ret < 0) {
      return ret;
    } else {
      samples += ret;
    }
  }

  if (!missingFR.isEmpty()) {
    int getDataRet;
    if (fetchRet == 1) {
      // fetch directly from server
      char *tempBuf = new char[(((serverFR.lastFrame() - serverFR.firstFrame()) / skip) + 1) * sizeForType(_serverConn->type(field))];
      if (_serverConn && _serverConn->isValid()) {
        // fetch data in native format and convert to doubles
        int getDataRet = _serverConn->getData(field, tempBuf, serverFR);
        if (getDataRet > 0) {
          QValueList<FrameRange> ranges = serverFR.getRanges();
          for (QValueList<FrameRange>::const_iterator it = ranges.begin(); it != ranges.end(); ++it) {
            convertType(tempBuf + (((*it).first - serverFR.firstFrame()) / skip) * sizeForType(_serverConn->type(field)), _serverConn->type(field), v + ((*it).first - s) / skip, 'd', ((*it).second - (*it).first)/skip + 1);
          }
          samples += getDataRet;
        }
      }
      delete[] tempBuf;
    }

    if (fetchRet < 0 || getDataRet < 0) {
      // fill in NaNs for missing data if NADCache::fetchData() or NADConnection::getData() fails
      QValueList<FrameRange> missing = missingFR.getRanges();
      QValueList<FrameRange>::const_iterator it;
      uint32_t skip = missingFR.getSkip();

      for (it = missing.begin(); it != missing.end(); ++it) {
        if (skip) {
          for (uint32_t i = (*it).first; i <= (*it).second; i += skip) {
            v[(i-s)/skip] = NOPOINT;
            samples++;
          }
        } else {
          for (uint32_t i = (*it).first; i <= (*it).second; ++i) {
            for (uint32_t j=0; j<info.spf; ++j) {
              v[(i-s)*info.spf + j] = NOPOINT;
              samples++;
            }
          }
        }
      }
    }
  }

  if (lastFrameRead) {
    *lastFrameRead = QMAX(cacheFR.lastFrame(), missingFR.lastFrame());
  }

  return samples;
}


bool NADSource::isValidField(const QString& field) const {
  bool valid = _fieldList.contains(field);
  kstdDebug() << "NADSource:isValidField(" << field << ") = " << valid << endl;
  return valid;
}


int NADSource::samplesPerFrame(const QString &field) {
  if (!_valid) {
    return 0;
  }

  if (field == "INDEX") {
    kstdDebug() << "NADSource:samplesPerFrame(" << field << ") = 1\n";
    return 1;
  }

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->lock(_filename, field, NADCacheAgentConnection::READLOCK);
  }

  int spf = _cache->getFieldInfo(_filename, field).spf;

  if (_cacheConn && _cacheConn->isConnected()) {
    _cacheConn->unlock(_filename, field);
  }

  kstdDebug() << "NADSource:samplesPerFrame(" << field << ") = " << spf << endl;
  return spf;
}


int NADSource::frameCount(const QString& field) const {
  if (!_valid) {
    return 0;
  }

  if (field == "INDEX") {
    int maxLen = 0;
    for (QStringList::ConstIterator i = _fieldList.begin(); i != _fieldList.end(); ++i) {
      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->lock(_filename, *i, NADCacheAgentConnection::READLOCK);
      }

      FrameRanges fr = _cache->getFrameRanges(_filename, *i);

      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->unlock(_filename, *i);
      }

      FrameRange range = fr.fullRange();
      maxLen = QMAX(range.second - range.first + 1, (uint32_t)maxLen);
    }
    kstdDebug() << "NADSource:frameCount(" << field << ") = maxLen = " << maxLen << endl;
    return maxLen;
  } else {
      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->lock(_filename, field, NADCacheAgentConnection::READLOCK);
      }

      FrameRanges fr = _cache->getFrameRanges(_filename, field);

      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->unlock(_filename, field);
      }

      FrameRange range = fr.fullRange();
      return (range.second - range.first + 1);
  }
}


QString NADSource::fileType() const {
  return "NAD Connection";
}


void NADSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}


bool NADSource::isEmpty() const {
  kstdDebug() << "NADSource:isEmpty()" << endl;

  if (!_valid) {
    return true;
  }

  for (QStringList::ConstIterator i = _fieldList.begin(); i != _fieldList.end(); ++i) {
    if (*i != "INDEX") {
      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->lock(_filename, *i, NADCacheAgentConnection::READLOCK);
      }

      FrameRanges fr = _cache->getFrameRanges(_filename, *i);

      if (_cacheConn && _cacheConn->isConnected()) {
        _cacheConn->unlock(_filename, *i);
      }

      if (!fr.isEmpty()) {
        return false;
      }
    }
  }

  return true;
}


extern "C" {
KstDataSource *create_nad(KConfig *cfg, const QString& filename, const QString& type) {
  kstdDebug() << "in create_nad:" << filename << endl;

  return new NADSource(cfg, filename, type);
}


QStringList provides_nad() {
  kstdDebug() << "in provides_nad" << endl;

  QStringList rc;
  rc += "NAD Connection";
  return rc;
}


int understands_nad(KConfig *cfg, const QString& filename) {
  Q_UNUSED(cfg)

  kstdDebug() << "in understands_nad:" << filename << endl;

  KURL url(filename);
  if (url.protocol().lower() == "nad") {
    return 100;
  } else {
    return 0;
  }
}


QStringList fieldList_nad(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(cfg)

  kstdDebug() << "in fieldList_nad:" << filename << endl;

  if (!type.isEmpty() && !provides_nad().contains(type)) {
    return QStringList();
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "NAD Connection";
  }

  NAD::NADCache cache;
  NAD::NADCacheAgentConnection cacheConn;
  cacheConn.connectToCacheAgent();
  if (cacheConn.isConnected()) {
    cacheConn.lock(filename, "", NADCacheAgentConnection::WRITELOCK);
    cacheConn.update(filename, "");
  }

  QStringList rc = cache.getFields(filename);
  
  if (cacheConn.isConnected()) {
    cacheConn.unlock(filename, "");
  }
  
  if (!rc.empty()) {
    rc.prepend("INDEX");
  }

  return rc;
}


QWidget* widget_nad(const QString& filename) {
  Q_UNUSED(filename)
  // Return 0L, or a widget that inherits from KstDataSourceConfigWidget
  return 0L;
}

}

KST_KEY_DATASOURCE_PLUGIN(nad)

// vim: ts=2 sw=2 et
