/***************************************************************************
                 dirfile.cpp  -  data source for dirfiles
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

#include "dirfilesource.h"
#include "kst_i18n.h"

#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDir>

static const QString dirfileTypeString = I18N_NOOP("Directory of Binary Files");

class DirFileSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString::null) {
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


DirFileSource::DirFileSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type, None), _config(0L) {

  _valid = false;
  if (!type.isEmpty() && type != dirfileTypeString) {
    return;
  }

  _config = new DirFileSource::Config;
  _config->read(cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  _valid = true;
  _directoryName = DirFilePlugin::getDirectory(_filename);

  init();
  registerChange();

}



DirFileSource::~DirFileSource() {
  if (_watcher) {
    disconnect(_watcher, SIGNAL(fileChanged ( const QString & )), this, SLOT(checkUpdate()));
    disconnect(_watcher, SIGNAL(directoryChanged ( const QString & )), this, SLOT(checkUpdate()));
    delete _watcher;
    _watcher = 0L;
  }
  delete _config;
  _config = 0L;
  delete _dirfile;
  _dirfile = 0L;
}


bool DirFileSource::reset() {
  disconnect(_watcher, SIGNAL(fileChanged ( const QString & )), this, SLOT(checkUpdate()));
  disconnect(_watcher, SIGNAL(directoryChanged ( const QString & )), this, SLOT(checkUpdate()));
  if (_dirfile) {
    delete _dirfile;
  }

  delete _watcher;
  _watcher = 0L;

  init();
  return true;
}


bool DirFileSource::init() {
  _fieldList.clear();
  _scalarList.clear();
  _matrixList.clear();
  _stringList.clear();

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

    _writable = true;
    _frameCount = _dirfile->NFrames();
  }

  _watcher = new QFileSystemWatcher();
  if (_fieldList.count() > 1) {
    QString filePath = _dirfile->ReferenceFilename();
    _watcher->addPath(filePath);
  }
  connect(_watcher, SIGNAL(fileChanged ( const QString & )), this, SLOT(checkUpdate()));
  connect(_watcher, SIGNAL(directoryChanged ( const QString & )), this, SLOT(checkUpdate()));

  registerChange();
  return true;
}


Kst::Object::UpdateType DirFileSource::internalDataSourceUpdate() {
  int newNF = _dirfile->NFrames();
  bool isnew = newNF != _frameCount;
  _frameCount = newNF;

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


QString DirFilePlugin::pluginName() const { return "DirFile Reader"; }
QString DirFilePlugin::pluginDescription() const { return "DirFile Reader"; }


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
      QString directoryName = stream.readLine();
      properDirPath += "/";
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

Q_EXPORT_PLUGIN2(kstdata_dirfile, DirFilePlugin)


// vim: ts=2 sw=2 et
