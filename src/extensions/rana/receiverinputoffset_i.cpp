/***************************************************************************
                    receiverinputoffset_i.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2005 The University of British Columbia
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

#include <math.h>

#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtable.h>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

#include "rana.h"
#include "receiverinputoffset_i.h"

ReceiverInputOffsetI::ReceiverInputOffsetI(KstRANA* rana,
                                       QWidget* parent,
                                       const char* name,
                                       bool modal,
                                       WFlags fl) : ReceiverInputOffset(parent, name, modal, fl) {
  _rana = rana;
}

ReceiverInputOffsetI::~ReceiverInputOffsetI() {
}

void ReceiverInputOffsetI::initialize() {
  QHeader* header;

  connect( pushButtonClose, SIGNAL( clicked() ), this, SLOT( shut() ) );
  connect( pushButtonNew, SIGNAL( clicked() ), this, SLOT( newEntry() ) );
  connect( pushButtonDelete, SIGNAL( clicked() ), this, SLOT( deleteEntry() ) );
  connect( pushButtonClear, SIGNAL( clicked() ), this, SLOT( clearEntries() ) );

  connect( lineEditVLow, SIGNAL( textChanged(const QString&) ), this, SLOT( vOffsetCalculate() ) );
  connect( lineEditVHigh, SIGNAL( textChanged(const QString&) ), this, SLOT( vOffsetCalculate() ) );
  connect( lineEditVLowSigma, SIGNAL( textChanged(const QString&) ), this, SLOT( vOffsetCalculate() ) );
  connect( lineEditVHighSigma, SIGNAL( textChanged(const QString&) ), this, SLOT( vOffsetCalculate() ) );

  connect( lineEditTLow, SIGNAL( textChanged(const QString&) ), this, SLOT( tOffsetCalculate() ) );
  connect( lineEditTHigh, SIGNAL( textChanged(const QString&) ), this, SLOT( tOffsetCalculate() ) );
  connect( lineEditTLowSigma, SIGNAL( textChanged(const QString&) ), this, SLOT( tOffsetCalculate() ) );
  connect( lineEditTHighSigma, SIGNAL( textChanged(const QString&) ), this, SLOT( tOffsetCalculate() ) );
  
  connect( lineEditFrequencyGHz, SIGNAL( textChanged(const QString&) ), this, SLOT( tOffsetCalculate() ) );

  connect( table, SIGNAL( selectionChanged() ), this, SLOT( rowChanged() ) );
  connect( table, SIGNAL( valueChanged(int, int) ), this, SLOT( cellValueChanged(int, int) ) );
  
  table->setNumCols( 8 );
  table->setNumRows( 0 );
  table->setReadOnly( false );
  table->setSelectionMode( QTable::SingleRow );
  header = table->horizontalHeader();
  if (header) {
    header->setLabel( 0, i18n("T_low (K)") );
    header->setLabel( 1, i18n("sigma<T_low> (K)") );
    header->setLabel( 2, i18n("T_high (K)") );
    header->setLabel( 3, i18n("sigma<T_high> (K)") );
    header->setLabel( 4, i18n("V_low (V)") );
    header->setLabel( 5, i18n("sigma<V_low> (V)") );
    header->setLabel( 6, i18n("V_high (V)") );
    header->setLabel( 7, i18n("sigma<V_high> (V)") );
  }
  
  load( );

  if( table->numRows() == 0 ) {
    table->setNumRows( 1 );
  }
}

void ReceiverInputOffsetI::vOffsetCalculate() {
  double dVLo, dVLoSigma;
  double dVHi, dVHiSigma;
  double dVOffset, dVOffsetSigma;
  bool ok;
  
  lineEditVOffset->setText( QString("") );
  lineEditVOffsetSigma->setText( QString("") );

  dVLo = lineEditVLow->text().toDouble(&ok);
  if (ok) {
    dVLoSigma = lineEditVLowSigma->text().toDouble();

    dVHi = lineEditVHigh->text().toDouble(&ok);
    if (ok) {
      dVHiSigma = lineEditVHighSigma->text().toDouble();

      dVOffset = dVHi - dVLo;
      dVOffsetSigma = sqrt( ( dVLoSigma * dVLoSigma ) + ( dVHiSigma * dVHiSigma ) );

      lineEditVOffset->setText( QString::number(dVOffset) );
      lineEditVOffsetSigma->setText( QString::number(dVOffsetSigma) );
    }
  }

  rowUpdated();
}

void ReceiverInputOffsetI::tOffsetCalculate() {
  double dTLo, dTLoSigma;
  double dTHi, dTHiSigma;
  double dATLo, dATLoSigma;
  double dATHi, dATHiSigma;
  double dTOffset, dTOffsetSigma;
  double dFrequency;
  bool ok;
  
  lineEditTOffset->setText( QString("") );
  lineEditTOffsetSigma->setText( QString("") );

  dTLo = lineEditTLow->text().toDouble(&ok);
  if (ok) {
    dTLoSigma = lineEditTLowSigma->text().toDouble();

    dTHi = lineEditTHigh->text().toDouble(&ok);
    if (ok) {
      dTHiSigma = lineEditTHighSigma->text().toDouble();

      dFrequency = lineEditFrequencyGHz->text().toDouble();

      KstRANA::convertToAntennaTemperature(dTLo, dTLoSigma, dFrequency, dATLo, dATLoSigma);
      KstRANA::convertToAntennaTemperature(dTHi, dTHiSigma, dFrequency, dATHi, dATHiSigma);
            
      dTOffset = dATHi - dATLo;
      dTOffsetSigma = sqrt( ( dATLoSigma * dATLoSigma ) + ( dATHiSigma * dATHiSigma ) );

      lineEditTOffset->setText( QString::number(dTOffset) );
      lineEditTOffsetSigma->setText( QString::number(dTOffsetSigma) );
    }
  }

  rowUpdated();
}

void ReceiverInputOffsetI::newEntry() {
  table->insertRows(table->numRows(), 1);
}

void ReceiverInputOffsetI::deleteEntry() {
  int i;
  
  for( i=0; i<table->numRows(); i++) {
    if( table->isRowSelected(i) ) {
      table->removeRow(i);

      if( table->numRows() > 0 ) {
        if( i < table->numRows()) {
          table->selectRow(i);
        } else {
          table->selectRow(table->numRows()-1);
        }
      } else {
        table->insertRows(0, 1);
        table->selectRow(0);
      }

      break;
    }
  }
}

void ReceiverInputOffsetI::clearEntries() {
  QString empty;
  
  table->setNumRows(0);
  table->setNumRows(1);

  lineEditTLow->setText(empty);
  lineEditTLowSigma->setText(empty);
  lineEditTHigh->setText(empty);
  lineEditTHighSigma->setText(empty);

  lineEditVLow->setText(empty);
  lineEditVLowSigma->setText(empty);
  lineEditVHigh->setText(empty);
  lineEditVHighSigma->setText(empty);

  table->selectRow(0);  
}

void ReceiverInputOffsetI::rowUpdated() {
  int i;

  for( i=0; i<table->numRows(); i++) {
    if( table->isRowSelected(i) ) {
      table->setText(i, 0, lineEditTLow->text());
      table->setText(i, 1, lineEditTLowSigma->text());
      table->setText(i, 2, lineEditTHigh->text());
      table->setText(i, 3, lineEditTHighSigma->text());

      table->setText(i, 4, lineEditVLow->text());
      table->setText(i, 5, lineEditVLowSigma->text());
      table->setText(i, 6, lineEditVHigh->text());
      table->setText(i, 7, lineEditVHighSigma->text());

      break;
    }
  }
}

void ReceiverInputOffsetI::blockTextChangeSignals(bool block) {
  lineEditTLow->blockSignals(block);
  lineEditTLowSigma->blockSignals(block);
  lineEditTHigh->blockSignals(block);
  lineEditTHighSigma->blockSignals(block);

  lineEditVLow->blockSignals(block);
  lineEditVLowSigma->blockSignals(block);
  lineEditVHigh->blockSignals(block);
  lineEditVHighSigma->blockSignals(block);
}

void ReceiverInputOffsetI::cellValueChanged(int row, int col) {
  QString value = table->text(row, col);

  switch( col ) {
    case 0:
      lineEditTLow->setText(value);
      break;
    case 1:
      lineEditTLowSigma->setText(value);
      break;
    case 2:
      lineEditTHigh->setText(value);
      break;
    case 3:
      lineEditTHighSigma->setText(value);
      break;
    case 4:
      lineEditVLow->setText(value);
      break;
    case 5:
      lineEditVLowSigma->setText(value);
      break;
    case 6:
      lineEditVHigh->setText(value);
      break;
    case 7:
      lineEditVHighSigma->setText(value);
      break;
    default:
      break;
  }
}

void ReceiverInputOffsetI::rowChanged() {
  int i;

  for( i=0; i<table->numRows(); i++) {
    if( table->isRowSelected(i) ) {
      blockTextChangeSignals(TRUE);
      
      lineEditTLow->setText(table->text(i, 0));
      lineEditTLowSigma->setText(table->text(i, 1));
      lineEditTHigh->setText(table->text(i, 2));
      lineEditTHighSigma->setText(table->text(i, 3));

      lineEditVLow->setText(table->text(i, 4));
      lineEditVLowSigma->setText(table->text(i, 5));
      lineEditVHigh->setText(table->text(i, 6));
      lineEditVHighSigma->setText(table->text(i, 7));

      blockTextChangeSignals(FALSE);

      vOffsetCalculate();
      tOffsetCalculate();

      break;
    }
  }
}

void ReceiverInputOffsetI::shut() {
  save();
  close();
}

void ReceiverInputOffsetI::save() {
  KConfig cfg("kstrc", false, false);
  int i;
  
  cfg.setGroup("RANA_ReceiverInputOffset");
  cfg.writeEntry("Number", table->numRows());

  for(i=0; i<table->numRows(); i++) {
    cfg.writeEntry(QString("Tlow%1").arg(i), table->text(i, 0));
    cfg.writeEntry(QString("TlowSigma%1").arg(i), table->text(i, 1));
    cfg.writeEntry(QString("Thigh%1").arg(i), table->text(i, 2));
    cfg.writeEntry(QString("ThighSigma%1").arg(i), table->text(i, 3));
    cfg.writeEntry(QString("Vlow%1").arg(i), table->text(i, 4));
    cfg.writeEntry(QString("VlowSigma%1").arg(i), table->text(i, 5));
    cfg.writeEntry(QString("Vhigh%1").arg(i), table->text(i, 6));
    cfg.writeEntry(QString("VhighSigma%1").arg(i), table->text(i, 7));
  }
  cfg.writeEntry("Frequency", lineEditFrequencyGHz->text());
  cfg.sync();
}

void ReceiverInputOffsetI::load() {  
  KConfig cfg("kstrc", false, false);
  QString empty;
  int i;
  
  cfg.setGroup("RANA_ReceiverInputOffset");
  table->blockSignals(TRUE);
  lineEditFrequencyGHz->blockSignals(TRUE);
  table->setNumRows(cfg.readNumEntry("Number", 0));

  for(i=0; i<table->numRows(); i++) {
    table->setText(i, 0, cfg.readEntry(QString("Tlow%1").arg(i), empty));
    table->setText(i, 1, cfg.readEntry(QString("TlowSigma%1").arg(i), empty));
    table->setText(i, 2, cfg.readEntry(QString("Thigh%1").arg(i), empty));
    table->setText(i, 3, cfg.readEntry(QString("ThighSigma%1").arg(i), empty));
    table->setText(i, 4, cfg.readEntry(QString("Vlow%1").arg(i), empty));
    table->setText(i, 5, cfg.readEntry(QString("VlowSigma%1").arg(i), empty));
    table->setText(i, 6, cfg.readEntry(QString("Vhigh%1").arg(i), empty));
    table->setText(i, 7, cfg.readEntry(QString("VhighSigma%1").arg(i), empty));
  }
  lineEditFrequencyGHz->setText(cfg.readEntry("Frequency", empty));

  if(table->numRows() > 0) {
    table->selectRow(0);
    table->selectCells(0, 0, 0, 7);
    rowChanged();
  }
  
  table->blockSignals(FALSE);
  lineEditFrequencyGHz->blockSignals(FALSE);
}

#include "receiverinputoffset_i.moc"

// vim: ts=2 sw=2 et
