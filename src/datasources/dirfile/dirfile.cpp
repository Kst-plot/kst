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

#include "dirfile.h"
#include "getdata.h"
#include "getdata_struct.h"

#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDir>

class DirFileSource::Config {
  public:
    Config() {
    }

    void read(QSettings *cfg, const QString& fileName = QString::null) {
      Q_UNUSED(fileName);
      cfg->beginGroup("Directory of Binary Files");
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
: Kst::DataSource(store, cfg, filename, type, None), _rowIndex(0L), _config(0L), _tmpBuf(0L), _tmpBufSize(0) {
  _valid = false;
  _haveHeader = false;
  _fieldListComplete = false;
  if (!type.isEmpty() && type != "Directory of Binary Files") {
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
  update();

  // In testing using the automatic creator provided by Barth, the FileSystemWatcher is not properly picking up 
  // modifications to files in the directory.  Temporarily, it's going to check one of the files for updates.
  QFileSystemWatcher *watcher = new QFileSystemWatcher();
  // Proper way.  See above
  // watcher->addPath(_directoryName);

  // Alternate method.
  if (_fieldList.count() > 1) {
    QString filePath = _directoryName + "/" + _fieldList[1];
    watcher->addPath(filePath);
  }

  connect(watcher, SIGNAL(fileChanged ( const QString & )), this, SLOT(checkUpdate()));
  connect(watcher, SIGNAL(directoryChanged ( const QString & )), this, SLOT(checkUpdate()));

}



DirFileSource::~DirFileSource() {
}


bool DirFileSource::reset() {
  _fieldList.clear();
  init();
  return true;
}


bool DirFileSource::init() {
  int err = 0;

  _frameCount = 0;
  FormatType *ft = GetFormat(_directoryName.toLatin1(), &err);

  if (err == GD_E_OK) {
    _fieldList.append("INDEX");
    for (int i = 0; i < ft->n_lincom; i++) {
      _fieldList.append(ft->lincomEntries[i].field);
    }

    for (int i = 0; i < ft->n_multiply; i++) {
      _fieldList.append(ft->multiplyEntries[i].field);
    }

    for (int i = 0; i < ft->n_linterp; i++) {
      _fieldList.append(ft->linterpEntries[i].field);
    }

    for (int i = 0; i < ft->n_bit; i++) {
      _fieldList.append(ft->bitEntries[i].field);
    }

    for (int i = 0; i < ft->n_phase; i++) {
      _fieldList.append(ft->phaseEntries[i].field);
    }

    for (int i = 0; i < ft->n_raw; i++) {
      _fieldList.append(ft->rawEntries[i].field);
    }

    _writable = true;
  }
  return update() == Kst::Object::UPDATE;
}


Kst::Object::UpdateType DirFileSource::update() {
  int err = 0;
  int newNF = GetNFrames(_directoryName.toLatin1(), &err, 0L);
  bool isnew = newNF != _frameCount;

  _frameCount = newNF;

  updateNumFramesScalar();
  return (isnew ? Kst::Object::UPDATE : Kst::Object::NO_CHANGE);
}


int DirFileSource::readField(double *v, const QString& field, int s, int n) {
  int err = 0;

  if (n < 0) {
    return GetData(_directoryName.toLatin1(), field.left(FIELD_LENGTH).toLatin1(),
                   s, 0, /* 1st sframe, 1st samp */
                   0, 1, /* num sframes, num samps */
                   'd', (void*)v,
                   &err);
  } else {
    return GetData(_directoryName.toLatin1(), field.left(FIELD_LENGTH).toLatin1(),
                   s, 0, /* 1st sframe, 1st samp */
                   n, 0, /* num sframes, num samps */
                   'd', (void*)v,
                   &err);
  }
}


int DirFileSource::writeField(const double *v, const QString& field, int s, int n) {
  int err = 0;

  return PutData(_directoryName.toLatin1(), field.left(FIELD_LENGTH).toLatin1(),
      s, 0, /* 1st sframe, 1st samp */
      n, 0, /* num sframes, num samps */
      'd', (void*)v,
      &err);
}


bool DirFileSource::isValidField(const QString& field) const {
  int err = 0;
  GetSamplesPerFrame(_directoryName.toLatin1(), field.left(FIELD_LENGTH).toLatin1(), &err);
  return err == 0;
}


int DirFileSource::samplesPerFrame(const QString &field) {
  int err = 0;
  return GetSamplesPerFrame(_directoryName.toLatin1(), field.left(FIELD_LENGTH).toLatin1(), &err);
}


int DirFileSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _frameCount;
}


bool DirFileSource::isEmpty() const {
  return _frameCount < 1;
}


QString DirFileSource::fileType() const {
  return "Directory of Binary Files";
}


void DirFileSource::save(QXmlStreamWriter &streamWriter) {
  Kst::DataSource::save(streamWriter);
}


QString DirFilePlugin::pluginName() const { return "DirFile Reader"; }


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
    *typeSuggestion = "Directory of Binary Files";
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


QStringList DirFilePlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  Q_UNUSED(cfg);
  Q_UNUSED(type)
  int err = 0;
  struct FormatType *ft = GetFormat(getDirectory(filename).toLatin1(), &err);
  QStringList fieldList;

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "Directory of Binary Files";
  }

  if (err == GD_E_OK) {
    fieldList.append("INDEX");
    for (int i = 0; i < ft->n_lincom; i++) {
      fieldList.append(ft->lincomEntries[i].field);
    }

    for (int i = 0; i < ft->n_multiply; i++) {
      fieldList.append(ft->multiplyEntries[i].field);
    }

    for (int i = 0; i < ft->n_linterp; i++) {
      fieldList.append(ft->linterpEntries[i].field);
    }

    for (int i = 0; i < ft->n_bit; i++) {
      fieldList.append(ft->bitEntries[i].field);
    }

    for (int i = 0; i < ft->n_phase; i++) {
      fieldList.append(ft->phaseEntries[i].field);
    }

    for (int i = 0; i < ft->n_raw; i++) {
      fieldList.append(ft->rawEntries[i].field);
    }
  }
  return fieldList;

}


QString DirFilePlugin::getDirectory(QString filepath) {
  QString properDirPath = QFileInfo(filepath).path();
  QFile file(filepath);
  if (file.open(QFile::ReadOnly)) {
    QTextStream stream(&file);
    QString directoryName = stream.readLine();
    properDirPath += "/";
    properDirPath += directoryName;
  }
  return properDirPath;
}


int DirFilePlugin::understands(QSettings *cfg, const QString& filename) const {
  // FIXME: GetNFrames causes a memory error here.  I think it is due to
  // the lfilename parameter.
  Q_UNUSED(cfg);
  int err = 0;

  int frameCount = GetNFrames(getDirectory(filename).toLatin1(), &err, 0L);
  if (frameCount > 0 && err == GD_E_OK) {
    return 98;
  }

  // qDebug() << "Don't understand.  filename = [" << filename << "] FrameCount=" << frameCount << " err=" << err << endl;
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
  rc += "Directory of Binary Files";
  return rc;
}


Kst::DataSourceConfigWidget *DirFilePlugin::configWidget(QSettings *cfg, const QString& filename) const {

  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return 0;;

}

Q_EXPORT_PLUGIN2(kstdata_dirfile, DirFilePlugin)


// vim: ts=2 sw=2 et
