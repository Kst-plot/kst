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
  connect(bgroup, SIGNAL(buttonClicked(int)), this, SLOT(columnLayoutChanged(int)));

  _showBeginning->setFont(  QFont("Courier"));
  _showBeginning->setReadOnly(true);
  _showBeginning->setLineWrapMode(QPlainTextEdit::NoWrap);

  connect(_readFields, SIGNAL(toggled(bool)), this, SLOT(updateUnitLineEnabled(bool)));
  connect(_limitFileBuffer, SIGNAL(toggled(bool)), this, SLOT(updateFrameBuffer(bool)));
  connect(_indexType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFormatString()));
  connect(_indexVector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFormatString()));
}

void AsciiConfigWidgetInternal::updateUnitLineEnabled(bool checked)
{
  if (checked && _readUnits->isChecked()) {
    _unitsLine->setEnabled(true);
  } else {
    _unitsLine->setEnabled(false);
  }
}

void AsciiConfigWidgetInternal::updateFrameBuffer(bool checked)
{
  if (checked) {
    _limitFileBufferSize->setEnabled(true);
  } else {
    _limitFileBufferSize->setEnabled(false);
  }
}


void AsciiConfigWidgetInternal::columnLayoutChanged(int idx)
{
  if (idx == AsciiSourceConfig::Fixed) {
    widthButtonGroup->setEnabled(false);
  } else {
    widthButtonGroup->setEnabled(true);
  }
}

void AsciiConfigWidgetInternal::updateFormatString() {
  bool enable = (AsciiSourceConfig::Interpretation)(_indexType->currentIndex() + 1)
                  == AsciiSourceConfig::FormattedTime;
  _indexTimeFormat->setEnabled(enable);
  _timeFormatLabel->setEnabled(enable);
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

  _labelBeginning->setText(QString("First 100 lines in file '%1'").arg(QFileInfo(_filename).fileName()));
}


AsciiSourceConfig AsciiConfigWidgetInternal::config()
{
  AsciiSourceConfig config;
  config._fileNamePattern = _fileNamePattern->text();
  config._indexVector = _indexVector->currentText();
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

  config._limitFileBuffer = _limitFileBuffer->isChecked();
  bool ok;
  int size = _limitFileBufferSize->text().toInt(&ok);
  if (ok) {
    config._limitFileBufferSize = size * 1024 * 1024;
  }

  config._useThreads =_useThreads->isChecked();
  config._indexTimeFormat = _indexTimeFormat->text();

  return config;
}

void AsciiConfigWidgetInternal::setFilename(const QString& filename)
{
  _filename = filename;
  showBeginning();
}


void AsciiConfigWidgetInternal::setConfig(const AsciiSourceConfig& config)
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
  updateUnitLineEnabled(config._readFields);
  
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
  columnLayoutChanged(ct);

  _limitFileBuffer->setChecked(config._limitFileBuffer);
  _limitFileBufferSize->setText(QString::number(config._limitFileBufferSize / 1024 / 1024));
  updateFrameBuffer(config._limitFileBuffer);

  _useThreads->setChecked(config._useThreads);
  _indexTimeFormat->setText(config._indexTimeFormat);
  updateFormatString();
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
    _ac->_indexType->setCurrentIndex(src->_config._indexInterpretation - 1);
    if (src->vector().list().contains(src->_config._indexVector)) {
      int idx = _ac->_indexVector->findText(src->_config._indexVector);
      if (idx == -1)
        idx = _ac->_indexVector->findText("INDEX");
      _ac->_indexVector->setCurrentIndex(idx == -1 ? 0 : idx);
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
      msg = QString("Line %1 could not list field names AND values!").arg(config._fieldsLine + 1);
    }
    if (config._readUnits) {
      if (config._unitsLine == config._dataLine) {
        msg = QString("Line %1 could not list units AND values!").arg(config._unitsLine + 1);
      }
      if (config._unitsLine == config._fieldsLine) {
        msg = QString("Line %1 could not list field names AND units!").arg(config._unitsLine + 1);
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
