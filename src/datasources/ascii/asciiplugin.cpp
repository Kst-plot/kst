/***************************************************************************
                     asciiplugin.cpp  -  ASCII file data source
                             -------------------
    begin                : Fri Oct 17 2003
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

#include "asciiplugin.h"
#include "asciisource_p.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qtextdocument.h>


#include <math_kst.h>
#include <kst_inf.h>
#include "ui_asciiconfig.h"
#include "kst_i18n.h"
#include "measuretime.h"

//
// ConfigWidgetAsciiInternal
//

class ConfigWidgetAsciiInternal : public QWidget, public Ui_AsciiConfig {
  public:
    ConfigWidgetAsciiInternal(QWidget *parent) : QWidget(parent), Ui_AsciiConfig() {
      setupUi(this);
    }

    AsciiSource::Config config();
    void setConfig(const AsciiSource::Config&);
};


AsciiSource::Config ConfigWidgetAsciiInternal::config()
{
  AsciiSource::Config config;
  config._fileNamePattern = _fileNamePattern->text();
  config._indexInterpretation = (AsciiSource::Config::Interpretation) (1 + _indexType->currentIndex());
  config._delimiters = _delimiters->text();
  AsciiSource::Config::ColumnType ct = AsciiSource::Config::Whitespace;
  if (_fixed->isChecked()) {
    ct = AsciiSource::Config::Fixed;
  } else if (_custom->isChecked()) {
    ct = AsciiSource::Config::Custom;
  }
  config._columnType = ct;
  config._columnDelimiter = _columnDelimiter->text();
  config._columnWidth = _columnWidth->value();
  config._dataLine = _startLine->value();
  config._readFields = _readFields->isChecked();
  config._useDot = _useDot->isChecked();
  config._fieldsLine = _fieldsLine->value();

  return config;
}

void ConfigWidgetAsciiInternal::setConfig(const AsciiSource::Config& config)
{
  _delimiters->setText(config._delimiters);// settings().value("Comment Delimiters", DEFAULT_DELIMITERS).toString());
  _fileNamePattern->setText(config._fileNamePattern); // settings().value("Filename Pattern").toString());
  _columnDelimiter->setText(config._columnDelimiter); // settings().value("Column Delimiter").toString());
  _columnWidth->setValue(config._columnWidth); //settings().value("Column Width", DEFAULT_COLUMN_WIDTH).toInt());
  _startLine->setValue(config._dataLine); //settings().value("Data Start", 0).toInt());
  _readFields->setChecked(config._readFields); //settings().value("Read Fields", false).toBool());
  _useDot->setChecked(config._useDot); //settings().value("Use Dot", true).toBool());
  _fieldsLine->setValue(config._fieldsLine); //settings().value("Fields Line", 0).toInt());
  AsciiSource::Config::ColumnType ct = config._columnType; //(AsciiSource::Config::ColumnType)settings().value("Column Type", 0).toInt();
  if (ct == AsciiSource::Config::Fixed) {
    _fixed->setChecked(true);
  } else if (ct == AsciiSource::Config::Custom) {
    _custom->setChecked(true);
  } else {
    _whitespace->setChecked(true);
  }
}


//
// ConfigWidgetAscii
//

class ConfigWidgetAscii : public Kst::DataSourceConfigWidget {
  public:
    ConfigWidgetAscii(QSettings&);
    ~ConfigWidgetAscii();

    void load();
    void save();

    ConfigWidgetAsciiInternal *_ac;
};


ConfigWidgetAscii::ConfigWidgetAscii(QSettings& s) : Kst::DataSourceConfigWidget(s) {
  QGridLayout *layout = new QGridLayout(this);
  _ac = new ConfigWidgetAsciiInternal(this);
  layout->addWidget(_ac, 0, 0);
  layout->activate();
}


ConfigWidgetAscii::~ConfigWidgetAscii() {
}


void ConfigWidgetAscii::load() {

  settings().beginGroup(AsciiSource::Config::asciiTypeKey());
  AsciiSource::Config config;
  config.readGroup(settings());
  _ac->setConfig(config);

  _ac->_indexVector->clear();
  if (hasInstance()) {
    _ac->_indexVector->addItems(instance()->fieldList());
    Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(instance());
    assert(src);
    _ac->_indexType->setCurrentIndex(src->_config->_indexInterpretation - 1);
    if (instance()->fieldList().contains(src->_config->_indexVector)) {
      _ac->_indexVector->setEditText(src->_config->_indexVector);
    }

    settings().beginGroup(src->fileName());
    _ac->setConfig(config);
    settings().endGroup();

  } else {
    _ac->_indexVector->addItem("INDEX");

    int x = config._indexInterpretation; //settings().value("Default INDEX Interpretation", (int)AsciiSource::Config::INDEX).toInt();
    if (x > 0 && x <= _ac->_indexType->count()) {
      _ac->_indexType->setCurrentIndex(x - 1);
    } else {
      _ac->_indexType->setCurrentIndex(0);
    }
  }
  _ac->_indexVector->setEnabled(hasInstance());

  settings().endGroup();
}

void ConfigWidgetAscii::save() {
  settings().beginGroup(AsciiSource::Config::asciiTypeKey());
  if (_ac->_applyDefault->isChecked()) {
    _ac->config().save(settings());
  }

  // If we have an instance, save settings for that instance as well
  Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(instance());
  if (src) {
    settings().beginGroup(src->fileName());
    _ac->config().save(settings());
    settings().endGroup();
  }
  settings().endGroup();

  // Update the instance from our new settings
  if (src && src->reusable()) {
    src->_config->readGroup(settings(), src->fileName());
    src->reset();
  }
}



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
    *typeSuggestion = AsciiSource::Config::asciiTypeKey();
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
    *typeSuggestion = AsciiSource::Config::asciiTypeKey();
  }

  AsciiSource::Config config;
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
    *typeSuggestion = AsciiSource::Config::asciiTypeKey();
  }

  AsciiSource::Config config;
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
    *typeSuggestion = AsciiSource::Config::asciiTypeKey();
  }

  AsciiSource::Config config;
  config.readGroup(*cfg, filename);
  QStringList rc = AsciiSource::stringListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}

int AsciiPlugin::understands(QSettings *cfg, const QString& filename) const {
  AsciiSource::Config config;
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
    QByteArray s;
    qint64 rc = 0;
    bool done = false;

    QRegExp commentRE, dataRE;
    if (config._columnType == AsciiSource::Config::Custom && !config._columnDelimiter.value().isEmpty()) {
      commentRE.setPattern(QString("^[%1]*[%2].*").arg(QRegExp::escape(config._columnDelimiter)).arg(config._delimiters));
      dataRE.setPattern(QString("^[%1]*(([Nn][Aa][Nn]|(\\-\\+)?[Ii][Nn][Ff]|[0-9\\+\\-\\.eE]+)[\\s]*)+").arg(QRegExp::escape(config._columnDelimiter)));
    } else {
      commentRE.setPattern(QString("^\\s*[%1].*").arg(config._delimiters));
      dataRE.setPattern(QString("^[\\s]*(([Nn][Aa][Nn]|(\\-\\+)?[Ii][Nn][Ff]|[0-9\\+\\-\\.eE]+)[\\s]*)+"));
    }

    int skip = config._dataLine;

    while (!done) {
      rc = AsciiSource::readFullLine(f, s);
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
      } else if (commentRE.exactMatch(s)) {
        // comment; do nothing
      } else if (dataRE.exactMatch(s)) {
        // a number - this may be an ascii file - assume that it is
        // This line checks for an indirect file and gives that a chance too.
        // Indirect files look like ascii files.
        return 75;
        //return QFile::exists(s.trimmed()) ? 49 : 75;
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
  QStringList rc;
  rc += AsciiSource::Config::asciiTypeKey();
  return rc;
}


Kst::DataSourceConfigWidget *AsciiPlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(filename)
  ConfigWidgetAscii *config = new ConfigWidgetAscii(*cfg);
  config->load();
  return config;
}

Q_EXPORT_PLUGIN2(kstdata_ascii, AsciiPlugin)

// vim: ts=2 sw=2 et
