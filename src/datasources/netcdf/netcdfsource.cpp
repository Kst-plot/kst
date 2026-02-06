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

// TODO move
#include "sharedptr.h"

#include "netcdfsource.h"

#include "debug.h"

#include <QFile>
#include <QFileInfo>

#include <ctype.h>
#include <stdlib.h>

//#define NETCDF_DEBUG_SHARED
#ifdef NETCDF_DEBUG_SHARED
#define NETCDF_DBG if (true)
#else
#define NETCDF_DBG if (false)
#endif


using namespace Kst;
using namespace netCDF;

static const QString netCdfTypeString = "netCDF Files";


//
// Scalar interface
//

class DataInterfaceNetCdfScalar : public DataSource::DataInterface<DataScalar>
{
public:
  DataInterfaceNetCdfScalar(NetcdfSource& s) : netcdf(s) {}

  // read one element
  int read(const QString&, DataScalar::ReadInfo&);

  // named elements
  QStringList list() const { return netcdf._scalarList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataScalar::DataInfo dataInfo(const QString&, int frame = 0) const { Q_UNUSED(frame) return DataScalar::DataInfo(); }
  void setDataInfo(const QString&, const DataScalar::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  NetcdfSource& netcdf;
};


int DataInterfaceNetCdfScalar::read(const QString& scalar, DataScalar::ReadInfo& p)
{
  return netcdf.readScalar(p.value, scalar);
}


bool DataInterfaceNetCdfScalar::isValid(const QString& scalar) const
{
  return  netcdf._scalarList.contains( scalar );
}


//
// String interface
//

class DataInterfaceNetCdfString : public DataSource::DataInterface<DataString>
{
public:
  DataInterfaceNetCdfString(NetcdfSource& s) : netcdf(s) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return netcdf._strings.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataString::DataInfo dataInfo(const QString&, int frame=0) const { Q_UNUSED(frame) return DataString::DataInfo(); }
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  NetcdfSource& netcdf;
};


//-------------------------------------------------------------------------------------------
int DataInterfaceNetCdfString::read(const QString& string, DataString::ReadInfo& p)
{
  //return netcdf.readString(p.value, string);
  if (isValid(string) && p.value) {
    *p.value = netcdf._strings[string];
    return 1;
  }
  return 0;
}


bool DataInterfaceNetCdfString::isValid(const QString& string) const
{
  return netcdf._strings.contains( string );
}





//
// Vector interface
//

class DataInterfaceNetCdfVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceNetCdfVector(NetcdfSource& s) : netcdf(s) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return netcdf._fieldList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataVector::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&);


private:
  NetcdfSource& netcdf;
};


const DataVector::DataInfo DataInterfaceNetCdfVector::dataInfo(const QString &field, int frame) const
{
  Q_UNUSED(frame)
  if (!netcdf._fieldList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(netcdf.frameCount(field), netcdf.samplesPerFrame(field));
}



int DataInterfaceNetCdfVector::read(const QString& field, DataVector::ReadInfo& p)
{
  return netcdf.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


bool DataInterfaceNetCdfVector::isValid(const QString& field) const
{
  return  netcdf._fieldList.contains( field );
}

QMap<QString, double> DataInterfaceNetCdfVector::metaScalars(const QString& field)
{
  NcVar var;
  if (field != "INDEX") {
    var = netcdf._ncfile->getVar(field.toLatin1().constData());
  }
  if (var.isNull()) {
    NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read";
    return QMap<QString, double>();
  }
  QMap<QString, double> fieldScalars;
  fieldScalars["NbAttributes"] = var.getAttCount();
  for (auto att: var.getAtts()) {
    size_t n = att.second.getAttLength();
    std::vector<double> data(n);
    // Only handle char attributes as fieldStrings, the others as fieldScalars
    switch (att.second.getType().getTypeClass()) {
    case NC_BYTE:
    case NC_UBYTE:
    case NC_SHORT:
    case NC_USHORT:
    case NC_INT:
    case NC_UINT:
    case NC_INT64:
    case NC_UINT64:
    case NC_FLOAT:
    case NC_DOUBLE:
      att.second.getValues((double *)&data[0]);
      break;
    default:
      break;
    }
    // Some attributes may have multiple values => load the first as is, and for the others
    // add a -2, -3, etc... suffix as obviously we can have only one value per scalar.
    // Do it in two steps to avoid a test in the loop while keeping a "clean" name for the first one
    QString name(att.first.c_str());
    fieldScalars[name] = data[0];
    for (size_t j=1; j<n; j++)
      fieldScalars[name + QString("-") + QString::number(j+1)] = data[j];
  }
  return fieldScalars;
}

QMap<QString, QString> DataInterfaceNetCdfVector::metaStrings(const QString& field)
{
  NcVar var;
  if (field != "INDEX") {
    var = netcdf._ncfile->getVar(field.toLatin1().constData());
  }
  if (var.isNull()) {
    NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read";
    return QMap<QString, QString>();
  }
  QMap<QString, QString> fieldStrings;
  QString tmpString;
  for (auto att: var.getAtts()) {
    // Only handle char/unspecified attributes as fieldStrings, the others as fieldScalars
    switch (att.second.getType().getTypeClass()) {
    case NC_CHAR:
    case NC_STRING:
      {
	std::string s;
	att.second.getValues(s);
	fieldStrings[QString(att.first.c_str())] = QString(s.c_str());
      }
      break;
    default:
      break;
    }
    // qDebug() << att->name() << ": " << att->values()->num() << Qt::endl;
  }
  return fieldStrings;
}


//
// Matrix interface
//

class DataInterfaceNetCdfMatrix : public DataSource::DataInterface<DataMatrix>
{
public:

  DataInterfaceNetCdfMatrix(NetcdfSource& s) : netcdf(s) {}

  // read one element
  int read(const QString&, DataMatrix::ReadInfo&);

  // named elements
  QStringList list() const { return netcdf._matrixList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataMatrix::DataInfo dataInfo	(const QString&, int frame) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  NetcdfSource& netcdf;
};


const DataMatrix::DataInfo DataInterfaceNetCdfMatrix::dataInfo(const QString& matrix, int frame) const
{
  Q_UNUSED(frame)
  if (!netcdf._matrixList.contains( matrix ) ) {
    return DataMatrix::DataInfo();
  }

  NcVar var = netcdf._ncfile->getVar(matrix.toLatin1().constData());  // var is owned by _ncfile
  if (var.isNull()) {
    return DataMatrix::DataInfo();
  }

  if (var.getDimCount() != 2) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;
  // TODO is this right?
  info.xSize = var.getDim(0).getSize();
  info.ySize = var.getDim(1).getSize();

  return info;
}


int DataInterfaceNetCdfMatrix::read(const QString& field, DataMatrix::ReadInfo& p)
{
  int count = netcdf.readMatrix(p.data->z, field);

  p.data->xMin = 0;
  p.data->yMin = 0;
  p.data->xStepSize = 1;
  p.data->yStepSize = 1;

  return count;
}


bool DataInterfaceNetCdfMatrix::isValid(const QString& field) const {
  return  netcdf._matrixList.contains( field );
}


//
// NetcdfSource
//

NetcdfSource::NetcdfSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement &element) :
  Kst::DataSource(store, cfg, filename, type),
  _ncfile(0L),
  is(new DataInterfaceNetCdfScalar(*this)),
  it(new DataInterfaceNetCdfString(*this)),
  iv(new DataInterfaceNetCdfVector(*this)),
  im(new DataInterfaceNetCdfMatrix(*this))
  {
  setInterface(is);
  setInterface(it);
  setInterface(iv);
  setInterface(im);

  setUpdateType(None);

  if (!type.isEmpty() && type != "netCDF") {
    return;
  }

  _valid = false;
  _maxFrameCount = 0;

  _filename = filename;
  _strings = fileMetas();
  _valid = initFile();
}


NetcdfSource::~NetcdfSource() {
  delete _ncfile;
  _ncfile = 0L;
}


void NetcdfSource::reset() {
  delete _ncfile;
  _ncfile = 0L;
  _maxFrameCount = 0;
  _valid = initFile();
}


bool NetcdfSource::initFile() {
  _ncfile = new NcFile(_filename.toUtf8().data(), NcFile::read);
  if (_ncfile->isNull()) {
      qDebug() << _filename << ": failed to open in initFile()";
      return false;
    }

  NETCDF_DBG qDebug() << _filename << ": building field list" << Qt::endl;
  _fieldList.clear();
  _fieldList += "INDEX";

  NETCDF_DBG qDebug() << _ncfile->getVarCount() << " vars found in total";

  _maxFrameCount = 0;

  for (auto var: _ncfile->getVars()) {
    if (var.second.isNull()) {
      continue;
    }
    if (var.second.getDimCount() == 0) {
      _scalarList += var.first.c_str();
    } else if (var.second.getDimCount() == 1) {
      _fieldList += var.first.c_str();
      int fc = var.second.getDim(0).getSize();
      _maxFrameCount = qMax(_maxFrameCount, fc);
      _frameCounts[QString(var.first.c_str())] = fc;
    } else if (var.second.getDimCount() == 2) {
      _matrixList += var.first.c_str();
    }
  }

  // Get strings
  for (auto att: _ncfile->getAtts()) {
    // Get only first value, should be enough for a start especially as strings are complete
    if (!att.second.isNull()) {
      QString attrName = QString(att.first.c_str());
      std::string s;
      att.second.getValues(s);
      QString attrValue = QString(s.c_str());
      //TODO port
      //KstString *ms = new KstString(KstObjectTag(attrName, tag()), this, attrValue);
      _strings[attrName] = attrValue;
    }
  }
  setUpdateType(Timer);

  NETCDF_DBG qDebug() << "netcdf file initialized";
  // TODO update(); // necessary?  slows down initial loading
  return true;
}



Kst::Object::UpdateType NetcdfSource::internalDataSourceUpdate() {
  //TODO port
  /*
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }
  */

  _ncfile->sync();

  bool updated = false;
  /* Update member variables _ncfile, _maxFrameCount, and _frameCounts
     and indicate that an update is needed */
  for (auto var: _ncfile->getVars()) {
    if (var.second.isNull()) {
      continue;
    }
    if (var.second.getDimCount() == 1) {
      int fc = var.second.getDim(0).getSize();
      _maxFrameCount = qMax(_maxFrameCount, fc);
      QString name(var.first.c_str());
      updated = updated || (_frameCounts[name] != fc);
      _frameCounts[name] = fc;
    }
  }
  return updated ? Object::Updated : Object::NoChange;
}


int NetcdfSource::readScalar(double *v, const QString& field)
{
  // TODO error handling
  NcVar var = _ncfile->getVar(field.toLatin1().constData());  // var is owned by _ncfile
  if (!var.isNull()) {
    var.getVar(v);
    return 1;
  }
  return 0;
}

int NetcdfSource::readString(QString *stringValue, const QString& stringName)
{
  // TODO more error handling?
  NcGroupAtt att = _ncfile->getAtt(stringName.toLatin1().constData());
  if (!att.isNull()) {
    std::string s;
    att.getValues(s);
    *stringValue = QString(s.c_str());
    return 1;
  }
  return 0;
}

int NetcdfSource::readField(double *v, const QString& field, int s, int n) {
// <<<<<<< HEAD
//   NcType dataType = ncNoType; /* netCDF data type */
//   /* Values for one record */
//   NcValues *record = 0;// = new NcValues(dataType,numFrameVals);

//   NETCDF_DBG qDebug() << "Entering NetcdfSource::readField with params: " << field << ", from " << s << " for " << n << " frames" << Qt::endl;
// =======
  NETCDF_DBG qDebug() << "Entering NetcdfSource::readField with params: " << field << ", from " << s << " for " << n << " frames";
// >>>>>>> c2b585fb (Update netCDF datasource plugin to work with modern netCDF4 C++ bindings)

  /* For INDEX field */
  if (field.toLower() == "index") {
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
// <<<<<<< HEAD
//   QByteArray bytes = field.toLatin1();
//   NcVar *var = _ncfile->get_var(bytes.constData());  // var is owned by _ncfile
//   if (!var) {
//     NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read" << Qt::endl;
// =======
  NcVar var = _ncfile->getVar(field.toLatin1().constData());  // var is owned by _ncfile
  if (var.isNull()) {
    NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read";
// >>>>>>> c2b585fb (Update netCDF datasource plugin to work with modern netCDF4 C++ bindings)
    return -1;
  }

  if (s >= (int) var.getDim(0).getSize()) {
    return 0;
  }

  bool oneSample = n < 0;
  double add_offset = 1.0, scale_factor = 1.0;
  std::vector<size_t> sv(1);
  sv[0] = s;
  std::vector<size_t> nv(1);
  nv[0] = n;
  switch (var.getType().getTypeClass()) {
    case NC_SHORT:
      {
        // Check for special attributes add_offset and scale_factor indicating the use of the convention described in
        // <http://www.unidata.ucar.edu/software/netcdf/docs/netcdf/Attribute-Conventions.html>
        bool packed = iv->metaScalars(field).contains("add_offset") && iv->metaScalars(field).contains("scale_factor");
        if (packed) {
          // Get the values into local vars
            add_offset = iv->metaScalars(field)["add_offset"];
            scale_factor = iv->metaScalars(field)["scale_factor"];
        }
        if (oneSample) {
	  short record;
	  var.getVar(sv, &record);
          v[0] = packed ? record*scale_factor+add_offset : record;
        } else {
	  std::vector<short> record(n);
	  var.getVar(sv, nv, (short *)&record[0]);
	  for (int i = 0; i < n; i++) {
	    if (packed) {
	      v[i] = record[i]*scale_factor+add_offset;
	    } else {
	      v[i] = record[i];
	    }
          }
        }
      }
      break;

    case NC_INT:
    case NC_UINT:
    case NC_INT64:
    case NC_UINT64:
    case NC_FLOAT:
    case NC_DOUBLE:
      {
        if (oneSample) {
	  var.getVar(sv, v);
        } else {
// <<<<<<< HEAD
//           for (int i = 0; i < n; i++) {
//             record = var->get_rec(i+s);
//             NETCDF_DBG qDebug() << "Read record " << i+s << Qt::endl;
//             for (int j = 0; j < recSize; j++) {
//               v[i*recSize + j] = record->as_int(j);
//             }
//             delete record;
//           }
// =======
	  var.getVar(sv, nv, v);
// >>>>>>> c2b585fb (Update netCDF datasource plugin to work with modern netCDF4 C++ bindings)
        }
      }
      break;
    default:
      NETCDF_DBG qDebug() << field << ": wrong datatype for kst, no values read" << Qt::endl;
      return -1;
      break;

  }

  NETCDF_DBG qDebug() << "Finished reading " << field << Qt::endl;

  return oneSample ? 1 : n;
}





int NetcdfSource::readMatrix(double *v, const QString& field) 
{
  /* For a variable from the netCDF file */
// <<<<<<< HEAD
//   QByteArray bytes = field.toLatin1();
//   NcVar *var = _ncfile->get_var(bytes.constData());  // var is owned by _ncfile
//   if (!var) {
//     NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read" << Qt::endl;
// =======
  NcVar var = _ncfile->getVar(field.toLatin1().constData());  // var is owned by _ncfile
  if (var.isNull()) {
    NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read";
// >>>>>>> c2b585fb (Update netCDF datasource plugin to work with modern netCDF4 C++ bindings)
    return -1;
  }

  int xSize = var.getDim(0).getSize();
  int ySize = var.getDim(1).getSize();

  var.getVar(v);

  return  xSize * ySize;
}








int NetcdfSource::samplesPerFrame(const QString& field) {
  if (field.toLower() == "index") {
    return 1;
  }
// <<<<<<< HEAD
//   QByteArray bytes = field.toLatin1();
//   NcVar *var = _ncfile->get_var(bytes.constData());
//   if (!var) {
//     NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read" << Qt::endl;
// =======
  NcVar var = _ncfile->getVar(field.toLatin1().constData());
  if (var.isNull()) {
    NETCDF_DBG qDebug() << "Queried field " << field << " which can't be read";
// >>>>>>> c2b585fb (Update netCDF datasource plugin to work with modern netCDF4 C++ bindings)
    return 0;
  }
  return 1;
}



int NetcdfSource::frameCount(const QString& field) const {
  if (field.isEmpty() || field.toLower() == "index") {
    return _maxFrameCount;
  } else {
    return _frameCounts[field];
  }
}



QString NetcdfSource::fileType() const {
  return "netCDF";
}



bool NetcdfSource::isEmpty() const {
  return frameCount() < 1;
}



QString NetcdfSource::typeString() const
{
  return netCdfTypeString;
}


const QString NetcdfSource::netcdfTypeKey()
{
  return ::netCdfTypeString;
}
