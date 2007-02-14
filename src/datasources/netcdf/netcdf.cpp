/***************************************************************************
                netcdf.cpp  -  netCDF file data source reader
                             -------------------
    begin                : 17/06/2004
    copyright            : (C) 2004 Nicolas Brisset <nicodev@users.sourceforge.net>
    email                : kst@kde.org
    modified             : 03/14/05 by K. Scott
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "netcdf_source.h" // Local header for the kst netCDF datasource

#include <ksdebug.h>

#include <qfile.h>
#include <qfileinfo.h>

#include <ctype.h>
#include <stdlib.h>

#include "kststring.h"


NetcdfSource::NetcdfSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type), _ncfile(0L) {
  if (!type.isEmpty() && type != "netCDF") {
    return;
  }

  _valid = false;
  _maxFrameCount = 0;

  _filename = filename;
  _valid = initFile();
}



NetcdfSource::~NetcdfSource() {
  delete _ncfile;
  _ncfile = 0L;
}


bool NetcdfSource::reset() {
  delete _ncfile;
  _ncfile = 0L;
  _maxFrameCount = 0;
  return _valid = initFile();
}


bool NetcdfSource::initFile() {
  _ncfile = new NcFile(_filename.latin1(), NcFile::ReadOnly);
  if (!_ncfile->is_valid()) {
      kstdDebug() << _filename << ": failed to open in initFile()" << endl;
      return false;
    }

  // kstdDebug() << _filename << ": building field list" << endl;
  _fieldList.clear();
  _fieldList += "INDEX";

  int nb_vars = _ncfile->num_vars();
  // kstdDebug() << nb_vars << " vars found in total" << endl;

  _maxFrameCount = 0;

  for (int i = 0; i < nb_vars; i++) {
    NcVar *var = _ncfile->get_var(i);
    _fieldList += var->name();
    int fc = var->num_vals() / var->rec_size();
    _maxFrameCount = QMAX(_maxFrameCount, fc);
    _frameCounts[var->name()] = fc;
  }

  // Get metadata
  int globalAttributesNb = _ncfile->num_atts();
  for (int i = 0; i < globalAttributesNb; ++i) {
    // Get only first value, should be enough for a start especially as strings are complete
    NcAtt *att = _ncfile->get_att(i);
    if (att) {
      QString attrName = QString(att->name());
      char *attString = att->as_string(0);
      QString attrValue = QString(att->as_string(0));
      delete[] attString;
      KstString *ms = new KstString(KstObjectTag(attrName, tag()), this, attrValue);
      _metaData.insert(attrName, ms);
    }
    delete att;
  }
  
  update(); // necessary?  slows down initial loading
  return true;
}



KstObject::UpdateType NetcdfSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  _ncfile->sync();

  bool updated = false;
  /* Update member variables _ncfile, _maxFrameCount, and _frameCounts
     and indicate that an update is needed */
  int nb_vars = _ncfile->num_vars();
  for (int j = 0; j < nb_vars; j++) {
    NcVar *var = _ncfile->get_var(j);
    int fc = var->num_vals() / var->rec_size();
    _maxFrameCount = QMAX(_maxFrameCount, fc);
    updated = updated || (_frameCounts[var->name()] != fc);
    _frameCounts[var->name()] = fc;
  }
  return setLastUpdateResult(updated ? KstObject::UPDATE : KstObject::NO_CHANGE);
}



int NetcdfSource::readField(double *v, const QString& field, int s, int n) {
  NcType dataType = ncNoType; /* netCDF data type */
  /* Values for one record */
  NcValues *record = 0;// = new NcValues(dataType,numFrameVals);

  // kstdDebug() << "Entering NetcdfSource::readField with params: " << field << ", from " << s << " for " << n << " frames" << endl;

  /* For INDEX field */
  if (field.lower() == "index") {
    if (n < 0) {
      v[0] = double(s);
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i);
    }
    return n;
  }

  /* For a variable from the netCDF file */
  NcVar *var = _ncfile->get_var(field.latin1());  // var is owned by _ncfile
  if (!var) {
    kstdDebug() << "Queried field " << field << " which can't be read" << endl;
    return -1;
  }

  dataType = var->type();

  if (s >= var->num_vals() / var->rec_size()) {
    return 0;
  }

  bool oneSample = n < 0;
  int recSize = var->rec_size();

  switch (dataType) {
    case ncShort:
      {
        if (oneSample) {
          record = var->get_rec(s);
          v[0] = record->as_short(0);
          delete record;
        } else {
          for (int i = 0; i < n; i++) {
            record = var->get_rec(i+s);
            for (int j = 0; j < recSize; j++) {
              v[i*recSize + j] = record->as_short(j);
            }
            delete record;
          }
        }
      }	
      break;

    case ncInt:
      {
        if (oneSample) {
          record = var->get_rec(s);
          v[0] = record->as_int(0);
          delete record;
        } else {
          for (int i = 0; i < n; i++) {
            record = var->get_rec(i+s);
            // kstdDebug() << "Read record " << i+s << endl;
            for (int j = 0; j < recSize; j++) {
              v[i*recSize + j] = record->as_int(j);
            }
            delete record;
          }
        }
      }
      break;

    case ncFloat:
      {
        if (oneSample) {
          record = var->get_rec(s);
          v[0] = record->as_float(0);
          delete record;
        } else {
          for (int i = 0; i < n; i++) {
            record = var->get_rec(i+s);
            for (int j = 0; j < recSize; j++) {
              v[i*recSize + j] = record->as_float(j);
            }
            delete record;
          }
        }
      } 
      break;

    case ncDouble: 
      {
        if (oneSample) {
          record = var->get_rec(s);
          v[0] = record->as_double(0);
          delete record;
        } else {
          for (int i = 0; i < n; i++) {
            record = var->get_rec(i+s);
            for (int j = 0; j < recSize; j++) {
              v[i*recSize + j] = record->as_double(j);
            }
            delete record;
          }
        }
      }
      break;

    default:
      kstdDebug() << field << ": wrong datatype for kst, no values read" << endl;
      return -1;
      break;

  }

  // kstdDebug() << "Finished reading " << field << endl;

  return oneSample ? 1 : n * recSize;
}



bool NetcdfSource::isValidField(const QString& field) const {  
  return _fieldList.contains(field);
}



int NetcdfSource::samplesPerFrame(const QString& field) {
  if (field.lower() == "index") {
    return 1;
  }
  NcVar *var = _ncfile->get_var(field.latin1());
  if (!var) {
    return 0;
  }
  return var->rec_size();
}



int NetcdfSource::frameCount(const QString& field) const {
  if (field.isEmpty() || field.lower() == "index") {
    return _maxFrameCount;
  } else { 
    return _frameCounts[field];
  }
}



QString NetcdfSource::fileType() const {
  return "netCDF";
}



void NetcdfSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}



bool NetcdfSource::isEmpty() const {
  return frameCount() < 1;
}



extern "C" {
  KstDataSource *create_netcdf(KConfig *cfg, const QString& filename, const QString& type) {
    return new NetcdfSource(cfg, filename, type);
  }


  QStringList provides_netcdf() {
    QStringList rc;
    rc += "netCDF";
    return rc;
  }


  /** understands_netcdf: returns true if:
    - the file is readable (!)
    - the file can be opened by the netcdf library **/
  int understands_netcdf(KConfig*, const QString& filename) {
    QFile f(filename);

    if (!f.open(IO_ReadOnly)) {
      kstdDebug() << "Unable to read file !" << endl;
      return 0;
    }

    NcFile *ncfile = new NcFile(filename.latin1());
    if (ncfile->is_valid()) {
      // kstdDebug() << filename << " looks like netCDF !" << endl;
      delete ncfile;
      return 80;
    } else {
      delete ncfile;
      return 0;
    }
  }

}

KST_KEY_DATASOURCE_PLUGIN(netcdf)

// vim: ts=2 sw=2 et
