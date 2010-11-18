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
//#define KST_DEBUG_SHARED
#include "sharedptr.h"

#include "kstnetcdf.h"
#include "kst_i18n.h"
#include "debug.h"

#include <QFile>
#include <QFileInfo>

#include <ctype.h>
#include <stdlib.h>


using namespace Kst;

static const QString netCdfTypeString = I18N_NOOP("netCDF Files");


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
  const DataScalar::DataInfo dataInfo(const QString&) const { return DataScalar::DataInfo(); }
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
  QStringList list() const { return netcdf._stringList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataString::DataInfo dataInfo(const QString&) const { return DataString::DataInfo(); }
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  NetcdfSource& netcdf;
};


int DataInterfaceNetCdfString::read(const QString& string, DataString::ReadInfo& p)
{
  return netcdf.readString(p.value, string);
}


bool DataInterfaceNetCdfString::isValid(const QString& string) const
{
  return  netcdf._stringList.contains( string );
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
  const DataVector::DataInfo dataInfo(const QString&) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  NetcdfSource& netcdf;
};


const DataVector::DataInfo DataInterfaceNetCdfVector::dataInfo(const QString &field) const
{
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
  const DataMatrix::DataInfo dataInfo	(const QString&) const;
  void setDataInfo(const QString&, const DataMatrix::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  NetcdfSource& netcdf;
};


const DataMatrix::DataInfo DataInterfaceNetCdfMatrix::dataInfo(const QString& matrix) const
{
  if (!netcdf._matrixList.contains( matrix ) ) {
    return DataMatrix::DataInfo();
  }

  QByteArray bytes = matrix.toLatin1();
  NcVar *var = netcdf._ncfile->get_var(bytes.constData());  // var is owned by _ncfile

  if (var->num_dims() != 2) {
    return DataMatrix::DataInfo();
  }

  DataMatrix::DataInfo info;
  info.samplesPerFrame = 1;
  // TODO is this right?
  info.xSize = var->get_dim(0)->size();
  info.ySize = var->get_dim(1)->size();

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
  _ncfile = new NcFile(_filename.toUtf8().data(), NcFile::ReadOnly);
  if (!_ncfile->is_valid()) {
      qDebug() << _filename << ": failed to open in initFile()" << endl;
      return false;
    }

  KST_DBG qDebug() << _filename << ": building field list" << endl;
  _fieldList.clear();
  _fieldList += "INDEX";

  int nb_vars = _ncfile->num_vars();
  KST_DBG qDebug() << nb_vars << " vars found in total" << endl;

  _maxFrameCount = 0;

  for (int i = 0; i < nb_vars; i++) {
    NcVar *var = _ncfile->get_var(i);
    if (var->num_dims() == 0) {
      _scalarList += var->name();
    } else if (var->num_dims() == 1) {
      _fieldList += var->name();
      int fc = var->num_vals() / var->rec_size();
      _maxFrameCount = qMax(_maxFrameCount, fc);
      _frameCounts[var->name()] = fc;
    } else if (var->num_dims() == 2) {
      _matrixList += var->name();
    }
  }

  // Get strings
  int globalAttributesNb = _ncfile->num_atts();
  for (int i = 0; i < globalAttributesNb; ++i) {
    // Get only first value, should be enough for a start especially as strings are complete
    NcAtt *att = _ncfile->get_att(i);
    if (att) {
      QString attrName = QString(att->name());
      char *attString = att->as_string(0);
      QString attrValue = QString(att->as_string(0));
      delete[] attString;
      //TODO port
      //KstString *ms = new KstString(KstObjectTag(attrName, tag()), this, attrValue);
      _stringList += attrName;
    }
    delete att;
  }

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
  int nb_vars = _ncfile->num_vars();
  for (int j = 0; j < nb_vars; j++) {
    NcVar *var = _ncfile->get_var(j);
    int fc = var->num_vals() / var->rec_size();
    _maxFrameCount = qMax(_maxFrameCount, fc);
    updated = updated || (_frameCounts[var->name()] != fc);
    _frameCounts[var->name()] = fc;
  }
  return updated ? Object::Updated : Object::NoChange;
}


int NetcdfSource::readScalar(double *v, const QString& field)
{
  // TODO error handling
  QByteArray bytes = field.toLatin1();
  NcVar *var = _ncfile->get_var(bytes.constData());  // var is owned by _ncfile
  var->get(v);
  return 1;
}

int NetcdfSource::readString(QString *stringValue, const QString& stringName)
{
  // TODO more error handling?
  NcAtt *att = _ncfile->get_att((NcToken) stringName.toLatin1().data());
  if (att) {
    *stringValue = QString(att->as_string(0));
    return 1;
  }
  delete att;
  return 0;
}

int NetcdfSource::readField(double *v, const QString& field, int s, int n) {
  NcType dataType = ncNoType; /* netCDF data type */
  /* Values for one record */
  NcValues *record = 0;// = new NcValues(dataType,numFrameVals);

  KST_DBG qDebug() << "Entering NetcdfSource::readField with params: " << field << ", from " << s << " for " << n << " frames" << endl;

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
  QByteArray bytes = field.toLatin1();
  NcVar *var = _ncfile->get_var(bytes.constData());  // var is owned by _ncfile
  if (!var) {
    KST_DBG qDebug() << "Queried field " << field << " which can't be read" << endl;
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
            KST_DBG qDebug() << "Read record " << i+s << endl;
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
      KST_DBG qDebug() << field << ": wrong datatype for kst, no values read" << endl;
      return -1;
      break;

  }

  KST_DBG qDebug() << "Finished reading " << field << endl;

  return oneSample ? 1 : n * recSize;
}





int NetcdfSource::readMatrix(double *v, const QString& field) 
{
  /* For a variable from the netCDF file */
  QByteArray bytes = field.toLatin1();
  NcVar *var = _ncfile->get_var(bytes.constData());  // var is owned by _ncfile
  if (!var) {
    KST_DBG qDebug() << "Queried field " << field << " which can't be read" << endl;
    return -1;
  }

  int xSize = var->get_dim(0)->size();
  int ySize = var->get_dim(1)->size();

  var->get(v, xSize, ySize);

 
  return  xSize * ySize;
}








int NetcdfSource::samplesPerFrame(const QString& field) {
  if (field.toLower() == "index") {
    return 1;
  }
  QByteArray bytes = field.toLatin1();
  NcVar *var = _ncfile->get_var(bytes.constData());
  if (!var) {
    return 0;
  }
  return var->rec_size();
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




//
// NetCdfPlugin
//

QString NetCdfPlugin::pluginName() const { return "netCDF Reader"; }
QString NetCdfPlugin::pluginDescription() const { return "netCDF Reader"; }


Kst::DataSource *NetCdfPlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const
{
  return new NetcdfSource(store, cfg, filename, type, element);
}



QStringList NetCdfPlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const
{
  return QStringList();
}


const QString& NetcdfSource::typeString() const
{
  return netCdfTypeString;
}



QStringList NetCdfPlugin::scalarList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const
{

  Q_UNUSED(cfg);
  Q_UNUSED(type)
  QStringList scalarList;
  return scalarList;
}

QStringList NetCdfPlugin::stringList(QSettings *cfg,
                                      const QString& filename,
                                      const QString& type,
                                      QString *typeSuggestion,
                                      bool *complete) const {
  Q_UNUSED(cfg);
  Q_UNUSED(type)
  QStringList stringList;

  return stringList;
}

QStringList NetCdfPlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(cfg);
  Q_UNUSED(type)

  QStringList fieldList;

  return fieldList;
}





bool NetCdfPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList NetCdfPlugin::provides() const
{
  return QStringList() << netCdfTypeString;
}


Kst::DataSourceConfigWidget *NetCdfPlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}




  /** understands_netcdf: returns true if:
    - the file is readable (!)
    - the file can be opened by the netcdf library **/
int NetCdfPlugin::understands(QSettings *cfg, const QString& filename) const
{
    QFile f(filename);

    if (!f.open(QFile::ReadOnly)) {
      KST_DBG qDebug() << "Unable to read file !" << endl;
      return 0;
    }

    NcFile *ncfile = new NcFile(filename.toUtf8().data());
    if (ncfile->is_valid()) {
      KST_DBG qDebug() << filename << " looks like netCDF !" << endl;
      delete ncfile;
      return 80;
    } else {
      delete ncfile;
      return 0;
    }
  }


Q_EXPORT_PLUGIN2(kstdata_netcdfsource, NetCdfPlugin)
