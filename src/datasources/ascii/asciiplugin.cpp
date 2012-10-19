/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2003 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "asciiplugin.h"
#include "asciiconfigwidget.h"
#include "asciisourceconfig.h"
#include "kst_atof.h"

#include <QFile>
#include <QFileInfo>
#include <QButtonGroup>
#include <QPlainTextEdit>
#include <QMessageBox>
QString AsciiPlugin::pluginName() const { return "ASCII File Reader"; }
QString AsciiPlugin::pluginDescription() const { return "ASCII File Reader"; }


Kst::DataSource *AsciiPlugin::create(Kst::ObjectStore *store, QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new AsciiSource(store, cfg, filename, type, element);
}


QStringList AsciiPlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {

  if (typeSuggestion) {
    *typeSuggestion = AsciiSource::asciiTypeKey();
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


QStringList AsciiPlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = AsciiSource::asciiTypeKey();
  }

  AsciiSourceConfig config;
  config.readGroup(*cfg, filename);
  QStringList rc = AsciiSource::fieldListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}


QStringList AsciiPlugin::scalarList(QSettings *cfg,
                                    const QString& filename,
                                    const QString& type,
                                    QString *typeSuggestion,
                                    bool *complete) const {

  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = AsciiSource::asciiTypeKey();
  }

  AsciiSourceConfig config;
  config.readGroup(*cfg, filename);
  QStringList rc = AsciiSource::scalarListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}


QStringList AsciiPlugin::stringList(QSettings *cfg,
                                    const QString& filename,
                                    const QString& type,
                                    QString *typeSuggestion,
                                    bool *complete) const {

  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = AsciiSource::asciiTypeKey();
  }

  AsciiSourceConfig config;
  config.readGroup(*cfg, filename);
  QStringList rc = AsciiSource::stringListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}


int AsciiPlugin::understands(QSettings *cfg, const QString& filename) const {
  AsciiSourceConfig config;
  config.readGroup(*cfg, filename);

  if (!QFile::exists(filename) || QFileInfo(filename).isDir()) {
    return 0;
  }

  if (!config._fileNamePattern.value().isEmpty()) {
    QRegExp filenamePattern(config._fileNamePattern);
    filenamePattern.setPatternSyntax(QRegExp::Wildcard);
    if (filenamePattern.exactMatch(filename)) {
      return 100;
    }
  }

  QFile f(filename);
  if (f.open(QIODevice::ReadOnly)) {    

    QRegExp commentRE;
    QRegExp dataRE;
    if (config._columnType == AsciiSourceConfig::Custom && !config._columnDelimiter.value().isEmpty()) {
      commentRE.setPattern(QString("^[%1]*[%2].*").arg(QRegExp::escape(config._columnDelimiter)).arg(config._delimiters));
      dataRE.setPattern(QString("^[%1]*(([Nn][Aa][Nn]|(\\-\\+)?[Ii][Nn][Ff]|[0-9\\+\\-\\.eE]+)[\\s]*)+").arg(QRegExp::escape(config._columnDelimiter)));
    } else {
      commentRE.setPattern(QString("^\\s*[%1].*").arg(config._delimiters));
      dataRE.setPattern(QString("^[\\s]*(([Nn][Aa][Nn]|(\\-\\+)?[Ii][Nn][Ff]|[0-9\\+\\-\\.eE]+)[\\s]*)+"));
    }

    int skip = config._dataLine;
    bool done = false;
    while (!done) {
      const QByteArray line = f.readLine();
      const qint64 rc = line.size();
      if (skip > 0) {
        --skip;
        if (rc <= 0) {
          done = true;
        }
        continue;
      }
      if (rc <= 0) {
        done = true;
      } else if (rc == 1) {
        // empty line; do nothing
      } else if (commentRE.exactMatch(line)) {
        // comment; do nothing
      } else if (dataRE.exactMatch(line)) {
        // a number - this may be an ascii file - assume that it is
        // This line checks for an indirect file and gives that a chance too.
        // Indirect files look like ascii files.
        return 75;
        //return QFile::exists(line.trimmed()) ? 49 : 75;
      } else {
        return 20;
      }
    }
  } else {
    return 0;
  }

  return 1; // still might be ascii - ex: header with no data yet.
}


bool AsciiPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList AsciiPlugin::provides() const {
  return QStringList() <<AsciiSource::asciiTypeKey();
}


Kst::DataSourceConfigWidget *AsciiPlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(filename)
  ConfigWidgetAscii *config = new ConfigWidgetAscii(*cfg);
  config->load();
  config->setFilename(filename);
  return config;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstdata_ascii, AsciiPlugin)
#endif

// vim: ts=2 sw=2 et
