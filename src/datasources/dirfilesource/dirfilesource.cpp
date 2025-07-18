/***************************************************************************
                 dirfile.cpp  -  data source for dirfiles
                             -------------------
    begin                : Tue Oct 21 2003
    copyright            : (C) 2003 The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dirfilesource.h"


#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDir>

using namespace Kst;

static const QString dirfileTypeString = "Directory of Binary Files";

class DirFileSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString()) {
      Q_UNUSED(fileName);
      cfg->beginGroup(dirfileTypeString);
      cfg->endGroup();
    }

    void save(QXmlStreamWriter& s) {
      Q_UNUSED(s);
    }

    void load(const QDomElement& e) {
      Q_UNUSED(e);
    }
};

//
// Scalar Interface
//
class DataInterfaceDirFileScalar : public DataSource::DataInterface<DataScalar>
{
public:
  DataInterfaceDirFileScalar(DirFileSource& d) : dir(d) {}

  // read one element
  int read(const QString&, DataScalar::ReadInfo&);

  // named elements
  QStringList list() const { return dir._scalarList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return dir._scalarList.contains( field ); }

  // T specific: not used for scalars
  const DataScalar::DataInfo dataInfo(const QString&, int frame = 0) const { Q_UNUSED(frame) return DataScalar::DataInfo(); }
  void setDataInfo(const QString&, const DataScalar::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  DirFileSource& dir;
};

int DataInterfaceDirFileScalar::read(const QString& field, DataScalar::ReadInfo& p)
{
  return dir.readScalar(*p.value, field);
}

//
// String Interface
//
class DataInterfaceDirFileString : public DataSource::DataInterface<DataString>
{
public:
  DataInterfaceDirFileString(DirFileSource& d) : dir(d) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return dir._stringList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return dir._stringList.contains( field ); }

  // T specific: not used for Strings
  virtual const DataString::DataInfo dataInfo(const QString&, int frame=0) const;
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  DirFileSource& dir;
};

int DataInterfaceDirFileString::read(const QString& field, DataString::ReadInfo& p)
{
  if (dir.isStringStream(field)) {
    return dir.readSindir(*p.value, field, p.frame);
  } else {
    return dir.readString(*p.value, field);
  }
}

const DataString::DataInfo DataInterfaceDirFileString::dataInfo(const QString &, int frame) const
{
  Q_UNUSED(frame)

  DataString::DataInfo info;
  info.frameCount = dir.frameCount();

  return info;
}


//
// Vector interface
//

class DataInterfaceDirFileVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceDirFileVector(DirFileSource& d) : dir(d) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return dir._fieldList; }
  bool isListComplete() const { return true; }
  bool isValid(const QString& field) const { return dir._fieldList.contains( field ); }

  // T specific
  const DataVector::DataInfo dataInfo(const QString&, int frame = 0) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&);


  DirFileSource& dir;
};



const DataVector::DataInfo DataInterfaceDirFileVector::dataInfo(const QString &field, int frame) const
{
  Q_UNUSED(frame)
  if (!dir._fieldList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(dir._frameCount, dir.samplesPerFrame(field));
}


int DataInterfaceDirFileVector::read(const QString& field, DataVector::ReadInfo& p)
{
  return dir.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


QMap<QString, double> DataInterfaceDirFileVector::metaScalars(const QString& field)
{
  QStringList keys = dir.fieldScalars(field);
  QList<double> values;
  int scalars = dir.readFieldScalars(values, field, true);
  QMap<QString, double> m;
  for (int i = 0; i < scalars; i++) {
    if (values.size() > i && keys.size() > i)
      m[keys.at(i)] = values.at(i);
  }
  return m;
}


QMap<QString, QString> DataInterfaceDirFileVector::metaStrings(const QString& field)
{
  QStringList keys = dir.fieldStrings(field);
  QStringList values;
  int strings = dir.readFieldStrings(values, field, true);
  QMap<QString, QString> m;
  for (int i = 0; i < strings; i++) {
    if (values.size() > i && keys.size() > i)
      m[keys.at(i)] = values.at(i);
  }
  return m;
}

//
// DirFileSource
//

DirFileSource::DirFileSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e) :
    Kst::DataSource(store, cfg, filename, type), _config(0L),
  iv(new DataInterfaceDirFileVector(*this)), ix(new DataInterfaceDirFileScalar(*this)),
  is(new DataInterfaceDirFileString(*this)), _resetNeeded(false)
{
  setInterface(iv);
  setInterface(ix);
  setInterface(is);

  setUpdateType(None);

  _valid = false;
  if (!type.isEmpty() && type != dirfileTypeString) {
    return;
  }

  _config = new DirFileSource::Config;
  _config->read(cfg, _filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  _valid = true;
  _directoryName = DirFilePlugin::getDirectory(_filename);

  init();
  registerChange();

}



DirFileSource::~DirFileSource()
{
  delete _config;
  _config = 0L;
  delete _dirfile;
  _dirfile = 0L;
}


void DirFileSource::reset()
{
  resetFileWatcher();
  if (_dirfile) {
    delete _dirfile;
  }

  init();
  Object::reset();
}


bool DirFileSource::init() {
  _fieldList.clear();
  _scalarList.clear();
  _stringList.clear();
  _sindirList.clear();

  _frameCount = 0;

  _dirfile = new Dirfile(_directoryName.toLatin1(), GD_RDONLY);

  if (_dirfile->Error() == GD_E_OK) {

    const char **vl = _dirfile->VectorList();
    for (int i = 0; vl[i]!=NULL; i++) {
      _fieldList.append(QString::fromUtf8(vl[i]));
    }

    _scalarList.append("FRAMES");
    const char **xl = _dirfile->FieldListByType(ConstEntryType);
    for (int i = 0; xl[i]!=NULL; i++) {
      _scalarList.append(QString::fromUtf8(xl[i]));
    }

    _stringList.append("FILE");
    const char **tl = _dirfile->FieldListByType(StringEntryType);
    for (int i = 0; tl[i]!=NULL; i++) {
      _stringList.append(QString::fromUtf8(tl[i]));
    }

    const char **nl = _dirfile->FieldListByType(SindirEntryType);
    for (int i = 0; nl[i]!=NULL; i++) {
      _sindirList.append(QString::fromUtf8(nl[i]));
      _stringList.append(QString::fromUtf8(nl[i])); // _stringList contains both
    }

    _writable = true;
    _frameCount = _dirfile->NFrames();
  }

  if (_fieldList.count() > 1) {
    QString filePath = _dirfile->ReferenceFilename();
  }
  setUpdateType(Timer);

  registerChange();
  return true;
}


Kst::Object::UpdateType DirFileSource::internalDataSourceUpdate() {
  int newNF = _dirfile->NFrames();
  bool isnew = newNF != _frameCount;

  _resetNeeded |= (_frameCount>newNF);

  _frameCount = newNF;

  if (_resetNeeded && newNF>0) {
    _resetNeeded = false;
    reset();
  }
  return (isnew ? Updated : NoChange);
}

int DirFileSource::readField(double *v, const QString& field, int s, int n) {

  if (n < 0) {
    return _dirfile->GetData(field.toUtf8().constData(),
                   s, 0, /* 1st sframe, 1st samp */
                   0, 1, /* num sframes, num samps */
                   Float64, (void*)v);
  } else {
    return _dirfile->GetData(field.toUtf8().constData(),
                   s, 0, /* 1st sframe, 1st samp */
                   n, 0, /* num sframes, num samps */
                   Float64, (void*)v);
  }
}


// int DirFileSource::writeField(const double *v, const QString& field, int s, int n) {
//   int err = 0;
//
//   return PutData(_directoryName.toLatin1(), field.left(FIELD_LENGTH).toLatin1(),
//       s, 0, /* 1st sframe, 1st samp */
//       n, 0, /* num sframes, num samps */
//       'd', (void*)v,
//       &err);
// }


int DirFileSource::samplesPerFrame(const QString &field) {
  return int(_dirfile->SamplesPerFrame(field.toUtf8().constData()));
}


int DirFileSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


bool DirFileSource::isEmpty() const {
  return _frameCount < 1;
}


QString DirFileSource::fileType() const {
  return dirfileTypeString;
}


void DirFileSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


int DirFileSource::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = _frameCount;
    return 1;
  } else {
    _dirfile->GetConstant(scalar.toUtf8().constData(), Float64, (void *)&S);
    if (_dirfile->Error() == GD_E_OK) {
      return 1;
    }
  }

  return 0;
}


int DirFileSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  } else {
    char tmpstr[4097];
    _dirfile->GetString(string.toUtf8().constData(), 4097, tmpstr);
    if (_dirfile->Error() == GD_E_OK) {
      S = QString::fromUtf8(tmpstr);
      return 1;
    }
  }

  return 0;
}

int DirFileSource::readSindir(QString &S, const QString &field, int frame) {
  const char *tmpstr[1];

  _dirfile->GetData(field.toUtf8().constData(), frame, 0, 0, 1, tmpstr);
  if (_dirfile->Error() == GD_E_OK) {
    S = QString::fromUtf8(tmpstr[0]);
    return 1;
  }

  return 0;
}

//QStringList fieldScalars(const QString& field);

QStringList DirFileSource::fieldScalars(const QString& field) {
  const char **mflist = _dirfile->MFieldListByType(field.toAscii(), ConstEntryType);
  if (!mflist) {
    return QStringList();
  }
  QStringList scalars;
  for (int i=0; mflist[i]; i++) {
    scalars.append(mflist[i]);
  }
  return scalars;
}

int DirFileSource::readFieldScalars(QList<double> &v, const QString& field, bool init) {
  int nc=0;
  if (init) { // only update if we need to initialize.  Otherwise preserve old values.
    v.clear();
    nc = _dirfile->NMFieldsByType(field.toAscii(),ConstEntryType);
    double *vin = (double *)_dirfile->MConstants(field.toAscii(), Float64);
    for (int i=0; i<nc; i++) {
      v.append(vin[i]);
    }
  }
  return (nc);
}


QStringList DirFileSource::fieldStrings(const QString& field) {
  const char **mflist = _dirfile->MFieldListByType(field.toAscii(), StringEntryType);
  if (!mflist) {
    return QStringList();
  }
  QStringList strings;
  for (int i=0; mflist[i]; i++) {
    strings.append(mflist[i]);
  }
  return strings;
}

int DirFileSource::readFieldStrings(QStringList &v, const QString& field, bool init) {
  int nc=0;
  if (init) { // only update if we need to initialize.  Otherwise preserve old values.
    v.clear();
    nc = _dirfile->NMFieldsByType(field.toAscii(),StringEntryType);
    char **str_in = (char **)_dirfile->MStrings(field.toAscii());
    for (int i=0; i<nc; i++) {
      v.append(str_in[i]);
    }
  }
  return (nc);
}


QString DirFilePlugin::pluginName() const { return tr("DirFile Reader"); }
QString DirFilePlugin::pluginDescription() const { return tr("DirFile Reader"); }


Kst::DataSource *DirFilePlugin::create(Kst::ObjectStore *store,
                                            QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {
  return new DirFileSource(store, cfg, filename, type, element);
}



QStringList DirFilePlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = dirfileTypeString;
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  return QStringList();
}

const QString& DirFileSource::typeString() const {
  return dirfileTypeString;
}

bool DirFileSource::isStringStream(QString field)
{
  if (_sindirList.contains(field)) {
    return true;
  } else {
    return false;
  }
}



QStringList DirFilePlugin::scalarList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  Q_UNUSED(cfg);
  Q_UNUSED(type)
  QStringList scalarList;

  Dirfile dirfile(getDirectory(filename).toLatin1(), GD_RDONLY);
  if (dirfile.Error() == GD_E_OK) {

    scalarList.append("FRAMES");
    const char **xl = dirfile.FieldListByType(ConstEntryType);
    for (int i = 0; xl[i]!=NULL; i++) {
      scalarList.append(QString::fromUtf8(xl[i]));
    }
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = dirfileTypeString;
  }

  return scalarList;
}

QStringList DirFilePlugin::stringList(QSettings *cfg,
                                      const QString& filename,
                                      const QString& type,
                                      QString *typeSuggestion,
                                      bool *complete) const {
  Q_UNUSED(cfg);
  Q_UNUSED(type)
  QStringList stringList;

  Dirfile dirfile(getDirectory(filename).toLatin1(), GD_RDONLY);

  if (dirfile.Error() == GD_E_OK) {

    stringList.append("FILE");
    const char **tl = dirfile.FieldListByType(StringEntryType);
    for (int i = 0; tl[i]!=NULL; i++) {
      stringList.append(QString::fromUtf8(tl[i]));
    }
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = dirfileTypeString;
  }

  return stringList;
}

QStringList DirFilePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {
  Q_UNUSED(cfg);
  Q_UNUSED(type)

  QStringList fieldList;

  Dirfile dirfile(getDirectory(filename).toLatin1(), GD_RDONLY);

  if (dirfile.Error() == GD_E_OK) {

    const char **vl = dirfile.VectorList();
    for (int i = 0; vl[i]!=NULL; i++) {
      fieldList.append(QString::fromUtf8(vl[i]));
    }
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = dirfileTypeString;
  }

  return fieldList;
}


QString DirFilePlugin::getDirectory(QString filepath) {
  QFileInfo info(filepath);
  QString properDirPath = info.path();
  if (info.fileName() == "format") {
    //do nothing... allows for format file to be selected.
  } else if (info.isDir()) {
    //do nothing... a directory was selected.
    properDirPath = filepath;
  } else {
    QFile file(filepath);
    if (file.open(QFile::ReadOnly)) {
      QTextStream stream(&file);
      QString directoryName = stream.readLine(1000);
      properDirPath += '/';
      properDirPath += directoryName;
    }
  }
  return properDirPath;
}


int DirFilePlugin::understands(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(cfg);

  Dirfile dirfile(getDirectory(filename).toLatin1(), GD_RDONLY);

  if (dirfile.Error() == GD_E_OK) {
    return 98;
  }
  return 0;
}



bool DirFilePlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList DirFilePlugin::provides() const {
  QStringList rc;
  rc += dirfileTypeString;
  return rc;
}


Kst::DataSourceConfigWidget *DirFilePlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

// vim: ts=2 sw=2 et
