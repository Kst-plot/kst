/***************************************************************************
                     asciiplugin.cpp
                             -------------------
    begin                :  Mar 16 2010
    copyright            : (C) 2010 The University of Toronto
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

#include "asciiplugin.h"
#include "asciisourceconfig.h"
#include "kst_atof.h"

#include <QFile>
#include <QFileInfo>
#include <QButtonGroup>


//
// ConfigWidgetAsciiInternal
//



ConfigWidgetAsciiInternal::ConfigWidgetAsciiInternal(QWidget *parent) : QWidget(parent), Ui_AsciiConfig() {
  setupUi(this);

  QButtonGroup* bgroup = new QButtonGroup(this);
  bgroup->addButton(_whitespace, AsciiSourceConfig::Whitespace);
  bgroup->addButton(_custom, AsciiSourceConfig::Custom);
  bgroup->addButton(_fixed, AsciiSourceConfig::Fixed);
  connect(bgroup, SIGNAL(buttonClicked(int)), this, SLOT(columnLayoutChanged(int)));

  connect(_zeroStart, SIGNAL(stateChanged(int)), this, SLOT(zeroStartChanged(int)));
}

void ConfigWidgetAsciiInternal::zeroStartChanged(int state)
{
  int adjust_index = 0;
  if (state == Qt::Checked) {
    _index_offset = 0;
    if (_startLine->minimum() == 1) {
      adjust_index = -1;
    }
  } else {
    _index_offset = 1;
    if (_startLine->minimum() == 0) {
      adjust_index = +1;
    }
  }

  _startLine->setMinimum(0);
  _fieldsLine->setMinimum(0);
  _unitsLine->setMinimum(0);

  _startLine->setValue(_startLine->value() + adjust_index);
  _fieldsLine->setValue(_fieldsLine->value() + adjust_index);
  _unitsLine->setValue(_unitsLine->value() + adjust_index);

  _startLine->setMinimum(_index_offset);
  _fieldsLine->setMinimum(_index_offset);
  _unitsLine->setMinimum(_index_offset);
}


void ConfigWidgetAsciiInternal::columnLayoutChanged(int idx)
{
  if (idx == AsciiSourceConfig::Fixed) {
    widthButtonGroup->setEnabled(false);
  } else {
    widthButtonGroup->setEnabled(true);
  }
}


AsciiSourceConfig ConfigWidgetAsciiInternal::config()
{
  AsciiSourceConfig config;
  config._fileNamePattern = _fileNamePattern->text();
  config._indexInterpretation = (AsciiSourceConfig::Interpretation) (1 + _indexType->currentIndex());
  config._delimiters = _delimiters->text();
  
  if (_whitespace->isChecked()) {
    config._columnType = AsciiSourceConfig::Whitespace;
  } else if (_custom->isChecked()) {
    config._columnType = AsciiSourceConfig::Custom;
  } else if (_fixed->isChecked()) {
    config._columnType = AsciiSourceConfig::Fixed;
  }

  config._columnDelimiter = _columnDelimiter->text();
  config._columnWidth = _columnWidth->value();
  config._columnWidthIsConst = _columnWidthIsConst->isChecked();
  config._readFields = _readFields->isChecked();
  config._readUnits = _readUnits->isChecked();
  config._useDot = _useDot->isChecked();

  config._dataLine = _startLine->value() - _index_offset;
  config._fieldsLine = _fieldsLine->value() - _index_offset;
  config._unitsLine = _unitsLine->value() - _index_offset;

  config._zeroStart = _zeroStart->isChecked();

  return config;
}


void ConfigWidgetAsciiInternal::setConfig(const AsciiSourceConfig& config)
{
  _delimiters->setText(config._delimiters);
  _fileNamePattern->setText(config._fileNamePattern);
  _columnDelimiter->setText(config._columnDelimiter);
  _columnWidth->setValue(config._columnWidth);
  _columnWidthIsConst->setChecked(config._columnWidthIsConst);
  _readFields->setChecked(config._readFields);
  _readUnits->setChecked(config._readUnits);
  _useDot->setChecked(config._useDot);
  _useComma->setChecked(!config._useDot);
  
  _index_offset = 0;
  _startLine->setMinimum(0);
  _fieldsLine->setMinimum(0);
  _unitsLine->setMinimum(0);
  _startLine->setValue(config._dataLine);
  _fieldsLine->setValue(config._fieldsLine);
  _unitsLine->setValue(config._unitsLine);

  _zeroStart->setChecked(config._zeroStart);
  zeroStartChanged(config._zeroStart ? Qt::Checked : Qt::Unchecked);

  AsciiSourceConfig::ColumnType ct = (AsciiSourceConfig::ColumnType) config._columnType.value();
  if (ct == AsciiSourceConfig::Fixed) {
    _fixed->setChecked(true);
  } else if (ct == AsciiSourceConfig::Custom) {
    _custom->setChecked(true);
  } else {
    _whitespace->setChecked(true);
  }
  columnLayoutChanged(ct);
}


//
// ConfigWidgetAscii
//

class ConfigWidgetAscii : public Kst::DataSourceConfigWidget
{
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
  AsciiSourceConfig config;
  if (hasInstance())
    config.readGroup(settings(), instance()->fileName());
  else
    config.readGroup(settings());

  _ac->setConfig(config);

  // Now handle index
  _ac->_indexVector->clear();
  if (hasInstance()) {
    Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(instance());
    _ac->_indexVector->addItems(src->vector().list());
    _ac->_indexVector->setCurrentIndex(src->_config._indexInterpretation - 1);
    if (src->vector().list().contains(src->_config._indexVector)) {
      _ac->_indexVector->setEditText(src->_config._indexVector);
    }
  } else {
    _ac->_indexVector->addItem("INDEX");
    int x = config._indexInterpretation;
    if (x > 0 && x <= _ac->_indexType->count()) {
      _ac->_indexType->setCurrentIndex(x - 1);
    } else {
      _ac->_indexType->setCurrentIndex(0);
    }
  }
  _ac->_indexVector->setEnabled(hasInstance());
}


void ConfigWidgetAscii::save() {
  if (hasInstance()) {
    Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(instance());
    if (_ac->_applyDefault->isChecked()) {
      _ac->config().saveGroup(settings());
    }
    _ac->config().saveGroup(settings(), src->fileName());

    // Update the instance from our new settings
    if (src->reusable()) {
      src->_config.readGroup(settings(), src->fileName());
      src->reset();
      src->internalDataSourceUpdate();
    }
  }
}



//
// AsciiPlugin
//

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
  return config;
}

Q_EXPORT_PLUGIN2(kstdata_ascii, AsciiPlugin)

// vim: ts=2 sw=2 et
