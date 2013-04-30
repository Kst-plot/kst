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

#include "asciiconfigwidget.h"

#include "kst_atof.h"

#include <QFile>
#include <QFileInfo>
#include <QButtonGroup>
#include <QPlainTextEdit>
#include <QMessageBox>

//
// AsciiConfigWidgetInternal
//



AsciiConfigWidgetInternal::AsciiConfigWidgetInternal(QWidget *parent) : 
    QWidget(parent), 
    Ui_AsciiConfig(),
    _index_offset(1)
{
  setupUi(this);

  QButtonGroup* bgroup = new QButtonGroup(this);
  bgroup->addButton(_whitespace, AsciiSourceConfig::Whitespace);
  bgroup->addButton(_custom, AsciiSourceConfig::Custom);
  bgroup->addButton(_fixed, AsciiSourceConfig::Fixed);

  _showBeginning->setFont(  QFont("Courier"));
  _showBeginning->setReadOnly(true);
  _showBeginning->setLineWrapMode(QPlainTextEdit::NoWrap);
  _showBeginning->setMinimumSize(640, 100);

  QObject::connect(_ctime, SIGNAL(toggled(bool)), this, SLOT(interpretationChanged(bool)));
  QObject::connect(_seconds, SIGNAL(toggled(bool)), this, SLOT(interpretationChanged(bool)));
  QObject::connect(_indexFreq, SIGNAL(toggled(bool)), this, SLOT(interpretationChanged(bool)));
  QObject::connect(_formattedString, SIGNAL(toggled(bool)), this, SLOT(interpretationChanged(bool)));

}


QString AsciiConfigWidgetInternal::readLine(QTextStream& in, int maxLength)
{
  const QString line = in.readLine();
  if (line.size() > maxLength) {
    // very log line, don't show it complete
    return line.mid(0, maxLength) + " ...";
  }
  return line;
}

void AsciiConfigWidgetInternal::showBeginning()
{
  QFile file(_filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }

  int lines_read = 1;
  QTextStream in(&file);
  QStringList lines;
  while (!in.atEnd() && lines_read <= 100) {
    lines << QString("%1: ").arg(lines_read, 3) + readLine(in, 1000);
    lines_read++;
  }

  _showBeginning->setPlainText(lines.join("\n"));
  _showBeginning->moveCursor(QTextCursor::Start);

  _labelBeginning->setText(QString("First lines of file '%1'").arg(QFileInfo(_filename).fileName()));
}

void AsciiConfigWidgetInternal::interpretationChanged(bool enabled) {
  if (enabled) {
    if (_ctime->isChecked()) {
      _offsetDateTime->setEnabled(false);
      _offsetFileDate->setEnabled(false);
      _offsetRelative->setEnabled(true);
      _offsetRelative->setChecked(true);
    } else if (_formattedString->isChecked()) {
      _offsetDateTime->setEnabled(true);
      _offsetFileDate->setEnabled(true);
      _offsetRelative->setEnabled(true);
    } else {
      _offsetDateTime->setEnabled(true);
      _offsetFileDate->setEnabled(true);
      _offsetRelative->setEnabled(false);
      if (_offsetRelative->isChecked()) {
        _offsetDateTime->setChecked(true);
      }
    }
  }
}

AsciiSourceConfig AsciiConfigWidgetInternal::config()
{
  AsciiSourceConfig config;
  config._fileNamePattern = _fileNamePattern->text();
  config._indexVector = _indexVector->currentText();

  if (_interpret->isChecked()) {
    if (_ctime->isChecked()) {
      config._indexInterpretation = AsciiSourceConfig::CTime;
    } else if (_seconds->isChecked()) {
      config._indexInterpretation = AsciiSourceConfig::Seconds;
    } else if (_formattedString->isChecked()) {
      config._indexInterpretation = AsciiSourceConfig::FormattedTime;
    } else if (_indexFreq->isChecked()) {
      config._indexInterpretation = AsciiSourceConfig::FixedRate;
    } else {
      config._indexInterpretation = AsciiSourceConfig::NoInterpretation;
    }
  } else {
    config._indexInterpretation = AsciiSourceConfig::NoInterpretation;
  }

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
  config._columnWidthIsConst = _colWidthConst->isChecked();
  config._readFields = _readFields->isChecked();
  config._readUnits = _readUnits->isChecked();
  config._useDot = _useDot->isChecked();

  config._dataLine = _startLine->value() - _index_offset;
  config._fieldsLine = _fieldsLine->value() - _index_offset;
  config._unitsLine = _unitsLine->value() - _index_offset;

  config._limitFileBuffer = _limitFileBuffer->isChecked();
  bool ok;
  int size = _limitFileBufferSize->text().toInt(&ok);
  if (ok) {
    config._limitFileBufferSize = size * 1024 * 1024;
  }

  config._useThreads =_useThreads->isChecked();
  config._timeAsciiFormatString = _timeAsciiFormatString->text();
  config._dataRate = _dataRate->value();
  config._offsetDateTime = _offsetDateTime->isChecked();
  config._offsetFileDate = _offsetFileDate->isChecked();
  config._offsetRelative = _offsetRelative->isChecked();
  config._dateTimeOffset = _dateTimeOffset->dateTime();
  config._relativeOffset = _relativeOffset->value();
  return config;
}

void AsciiConfigWidgetInternal::setFilename(const QString& filename)
{
  _filename = filename;
  //_dateTimeOffset->setDateTime(QFileInfo(_filename).lastModified());
  showBeginning();
}


void AsciiConfigWidgetInternal::setConfig(const AsciiSourceConfig& config)
{
  _delimiters->setText(config._delimiters);
  _fileNamePattern->setText(config._fileNamePattern);
  _columnDelimiter->setText(config._columnDelimiter);
  _columnWidth->setValue(config._columnWidth);
  _colWidthConst->setChecked(config._columnWidthIsConst);
  _readFields->setChecked(config._readFields);
  _readUnits->setChecked(config._readUnits);
  _useDot->setChecked(config._useDot);
  _useComma->setChecked(!config._useDot);
  
  _startLine->setValue(config._dataLine + _index_offset);
  _fieldsLine->setValue(config._fieldsLine + _index_offset);
  _unitsLine->setValue(config._unitsLine + _index_offset);

  AsciiSourceConfig::ColumnType ct = (AsciiSourceConfig::ColumnType) config._columnType.value();
  if (ct == AsciiSourceConfig::Fixed) {
    _fixed->setChecked(true);
  } else if (ct == AsciiSourceConfig::Custom) {
    _custom->setChecked(true);
  } else {
    _whitespace->setChecked(true);
  }

  _limitFileBuffer->setChecked(config._limitFileBuffer);
  _limitFileBufferSize->setText(QString::number(config._limitFileBufferSize / 1024 / 1024));

  _useThreads->setChecked(config._useThreads);
  _timeAsciiFormatString->setText(config._timeAsciiFormatString);
  _dataRate->setValue(config._dataRate.value());
  _offsetDateTime->setChecked(config._offsetDateTime.value());
  _offsetFileDate->setChecked(config._offsetFileDate.value());
  _offsetRelative->setChecked(config._offsetRelative.value());
  _dateTimeOffset->setDateTime(config._dateTimeOffset.value());
  _relativeOffset->setValue(config._relativeOffset.value());

}


AsciiConfigWidget::AsciiConfigWidget(QSettings& s) : Kst::DataSourceConfigWidget(s) {
  QGridLayout *layout = new QGridLayout(this);
  _ac = new AsciiConfigWidgetInternal(this);
  layout->addWidget(_ac, 0, 0);
  layout->activate();
  _oldConfig = _ac->config();
}


AsciiConfigWidget::~AsciiConfigWidget() {
}


void AsciiConfigWidget::setFilename(const QString& filename)
{
  _ac->setFilename(filename);
}


void AsciiConfigWidget::load() {
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

    if (src->_config._indexInterpretation == AsciiSourceConfig::CTime) {
      _ac->_interpret->setChecked(true);
      _ac->_ctime->setChecked(true);
    } else if (src->_config._indexInterpretation == AsciiSourceConfig::Seconds) {
      _ac->_interpret->setChecked(true);
      _ac->_seconds->setChecked(true);
    } else if (src->_config._indexInterpretation == AsciiSourceConfig::FormattedTime) {
      _ac->_interpret->setChecked(true);
      _ac->_formattedString->setChecked(true);
    } else if (src->_config._indexInterpretation == AsciiSourceConfig::FixedRate) {
      _ac->_interpret->setChecked(true);
      _ac->_indexFreq->setChecked(true);
    } else if (src->_config._indexInterpretation == AsciiSourceConfig::NoInterpretation) {
      _ac->_interpret->setChecked(false);
    }

    if (src->vector().list().contains(src->_config._indexVector)) {
      int idx = _ac->_indexVector->findText(src->_config._indexVector);
      if (idx == -1)
        idx = _ac->_indexVector->findText("INDEX");
      _ac->_indexVector->setCurrentIndex(idx == -1 ? 0 : idx);
    }
  } else {
    _ac->_indexVector->addItem("INDEX");

    if (config._indexInterpretation == AsciiSourceConfig::CTime) {
      _ac->_interpret->setChecked(true);
      _ac->_ctime->setChecked(true);
    } else if (config._indexInterpretation == AsciiSourceConfig::Seconds) {
      _ac->_interpret->setChecked(true);
      _ac->_seconds->setChecked(true);
    } else if (config._indexInterpretation == AsciiSourceConfig::FormattedTime) {
      _ac->_interpret->setChecked(true);
      _ac->_formattedString->setChecked(true);
    } else if (config._indexInterpretation == AsciiSourceConfig::FixedRate) {
      _ac->_interpret->setChecked(true);
      _ac->_indexFreq->setChecked(true);
    } else if (config._indexInterpretation == AsciiSourceConfig::NoInterpretation) {
      _ac->_interpret->setChecked(false);
    }

  }
  if (_ac->_interpret->isChecked()) {
    _ac->_indexVector->setEnabled(hasInstance());
  }
  _oldConfig = _ac->config();
}


void AsciiConfigWidget::save() {
  if (hasInstance()) {
    Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(instance());
    if (_ac->_applyDefault->isChecked()) {
      _ac->config().saveGroup(settings());
    }
    _ac->config().saveGroup(settings(), src->fileName());

    // Update the instance from our new settings
    if (src->reusable()) {
      src->_config.readGroup(settings(), src->fileName());
      if (_ac->config().isUdateNecessary(_oldConfig)) {
        src->reset();
        src->internalDataSourceUpdate();
      }
    }
  }
}

bool AsciiConfigWidget::isOkAcceptabe() const {
  AsciiSourceConfig config = _ac->config();
  QString msg;
  if (config._readFields) {
    if (config._fieldsLine == config._dataLine) {
      msg = QString("Line %1 can not list field names AND values!").arg(config._fieldsLine + 1);
    }
    if (config._readUnits) {
      if (config._unitsLine == config._dataLine) {
        msg = QString("Line %1 can not list units AND values!").arg(config._unitsLine + 1);
      }
      if (config._unitsLine == config._fieldsLine) {
        msg = QString("Line %1 can not list field names AND units!").arg(config._unitsLine + 1);
      }
    }
  }
  if (!msg.isEmpty()) {
    QMessageBox::critical(0, "Inconsistent parameters", msg);
    return false;
  }
  return true;
}


// vim: ts=2 sw=2 et
