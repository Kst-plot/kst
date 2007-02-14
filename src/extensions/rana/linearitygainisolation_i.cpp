/***************************************************************************
                    linearitygainisolation_i.cpp  -  Part of KST
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
#include <gsl/gsl_fit.h>

#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtable.h>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

#include "rana.h"
#include "linearitygainisolation_i.h"

#define  T_SKY        0
#define  T_SKY_SIGMA  1
#define  T_REF        2
#define  T_REF_SIGMA  3
#define  V_SKY        4
#define  V_SKY_SIGMA  5
#define  V_REF        6
#define  V_REF_SIGMA  7

LinearityGainIsolationI::LinearityGainIsolationI(KstRANA* rana,
                                       QWidget* parent,
                                       const char* name,
                                       bool modal,
                                       WFlags fl) : LinearityGainIsolation(parent, name, modal, fl) {
  _rana = rana;
}

LinearityGainIsolationI::~LinearityGainIsolationI() {
}

void LinearityGainIsolationI::initialize() {
  QHeader* header;

  connect( pushButtonClose, SIGNAL( clicked() ), this, SLOT( shut() ) );
  connect( pushButtonNew, SIGNAL( clicked() ), this, SLOT( newEntry() ) );
  connect( pushButtonDelete, SIGNAL( clicked() ), this, SLOT( deleteEntry() ) );
  connect( pushButtonClear, SIGNAL( clicked() ), this, SLOT( clearEntries() ) );

  connect( lineEditVSky, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  connect( lineEditVRef, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  connect( lineEditVSkySigma, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  connect( lineEditVRefSigma, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );

  connect( lineEditTSky, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  connect( lineEditTRef, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  connect( lineEditTSkySigma, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  connect( lineEditTRefSigma, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );
  
  connect( lineEditFrequencyGHz, SIGNAL( textChanged(const QString&) ), this, SLOT( calculate() ) );

  connect( table, SIGNAL( selectionChanged() ), this, SLOT( rowChanged() ) );
  connect( table, SIGNAL( valueChanged(int, int) ), this, SLOT( cellValueChanged(int, int) ) );
  
  table->setNumCols( 8 );
  table->setNumRows( 0 );
  table->setReadOnly( false );
  table->setSelectionMode( QTable::SingleRow );
  header = table->horizontalHeader();
  if (header) {
    header->setLabel( T_SKY, i18n("T_sky (K)") );
    header->setLabel( T_SKY_SIGMA, i18n("sigma<T_sky> (K)") );
    header->setLabel( T_REF, i18n("T_ref (K)") );
    header->setLabel( T_REF_SIGMA, i18n("sigma<T_ref> (K)") );
    header->setLabel( V_SKY, i18n("V_sky (V)") );
    header->setLabel( V_SKY_SIGMA, i18n("sigma<V_sky> (V)") );
    header->setLabel( V_REF, i18n("V_ref (V)") );
    header->setLabel( V_REF_SIGMA, i18n("sigma<V_ref> (V)") );
  }
  
  load( );

  if( table->numRows() == 0 ) {
    table->setNumRows( 1 );
  }
}

void LinearityGainIsolationI::calculate() {
  QString strError = "---";
  double* pX;
  double* pXErr;
  double* pY;
  double* pYErr;
  double* pWeight;
  double dA;
  double dB;
  double dCovar00;
  double dCovar01;
  double dCovar11;
  double dChiSquared;
  bool bFitted = false;
  bool bOK;
  int iCountValid = 0;
  int iCount;
  int i;

  rowUpdated();
    
  //
  // determine the gain, which requires fitting a straight line
  //  to a dataset which has errors in both co-ordinates...
  //
  iCount = table->numRows();
  pX = new double[iCount];
  pXErr = new double[iCount];
  pY = new double[iCount];
  pYErr = new double[iCount];
  pWeight = new double[iCount];
  
  if (pX && pXErr && pY && pYErr && pWeight) {
    for (i=0; i<iCount; i++) {
      pX[iCountValid] = table->text(i, T_SKY).toDouble( &bOK );
      if (bOK) {
        pY[iCountValid] = table->text(i, V_SKY).toDouble( &bOK );
        if (bOK) {
          pXErr[iCountValid] = table->text(i, T_SKY_SIGMA).toDouble( &bOK );
          pYErr[iCountValid] = table->text(i, V_SKY_SIGMA).toDouble( &bOK );

          iCountValid++;
        }
      }
    }

    if (iCountValid > 0) {
      if (gsl_fit_linear(pX, 1, pY, 1, iCountValid, &dA, &dB, &dCovar00, &dCovar01, &dCovar11, &dChiSquared) == 0) {
        double dVariance;
        double dBOld;
        double dY;
        double dYErr;
        double dLinearityCoefficient = 0.0;
        double dLinearityCoefficientErr = 0.0;
        int iIterations = 0;
        int iResult = -1;

        //
        // iterate towards a solution...
        //
        do {
          for (i=0; i<iCountValid; i++) {
            dVariance = (pYErr[i] * pYErr[i]) + (dB * dB * pXErr[i] * pXErr[i]);
            if (dVariance == 0.0) {
              dVariance = 1.0e-06;
            }
            pWeight[i] = 1.0 / dVariance;
          }
          iResult = gsl_fit_wlinear(pX, 1, pWeight, 1, pY, 1, iCountValid, &dA, &dB, &dCovar00, &dCovar01, &dCovar11, &dChiSquared);
          iIterations++;
          dBOld = dB;
        } while( iIterations < 5 && fabs(dBOld - dB) > 0.25 * sqrt(dCovar11));        

        //
        // determine the linearity coefficient...
        //
        for (i=0; i<iCountValid; i++) {
          gsl_fit_linear_est(pX[i], dA, dB, dCovar00, dCovar01, dCovar11, &dY, &dYErr);
          if (fabs(dY - pY[i]) > dLinearityCoefficient) {
            dLinearityCoefficient = fabs(dY - pY[i]);
            dLinearityCoefficientErr = sqrt((pYErr[i] * pYErr[i]) + (dYErr * dYErr));
          }
        }
        
        if (iResult == 0) {
          lineEditGain->setText(QString::number(dB));
          if (dCovar11 > 0.0) {
            lineEditGainSigma->setText(QString::number(sqrt(dCovar11)));
          } else {
            lineEditGainSigma->setText(strError);
          }
          lineEditLinearityCoefficient->setText(QString::number(dLinearityCoefficient));
          lineEditLinearityCoefficientSigma->setText(QString::number(dLinearityCoefficientErr));
          bFitted = true;
        }
        
        //
        // determine the isolation...
        //
      }
    }

    delete[] pX;
    delete[] pXErr;
    delete[] pY;
    delete[] pYErr;
    delete[] pWeight;
  }  

  if (!bFitted) {
    lineEditLinearityCoefficient->setText(strError);
    lineEditLinearityCoefficientSigma->setText(strError);
    lineEditIsolation->setText(strError);
    lineEditIsolationSigma->setText(strError);
    lineEditGain->setText(strError);
    lineEditGainSigma->setText(strError);
  }
}

void LinearityGainIsolationI::newEntry() {
  table->insertRows(table->numRows(), 1);
}

void LinearityGainIsolationI::deleteEntry() {
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

void LinearityGainIsolationI::clearEntries() {
  QString empty;
  
  table->setNumRows(0);
  table->setNumRows(1);

  lineEditTSky->setText(empty);
  lineEditTSkySigma->setText(empty);
  lineEditTRef->setText(empty);
  lineEditTRefSigma->setText(empty);

  lineEditVSky->setText(empty);
  lineEditVSkySigma->setText(empty);
  lineEditVRef->setText(empty);
  lineEditVRefSigma->setText(empty);

  table->selectRow(0);  
}

void LinearityGainIsolationI::rowUpdated() {
  int i;

  for( i=0; i<table->numRows(); i++) {
    if( table->isRowSelected(i) ) {
      table->setText(i, T_SKY, lineEditTSky->text());
      table->setText(i, T_SKY_SIGMA, lineEditTSkySigma->text());
      table->setText(i, T_REF, lineEditTRef->text());
      table->setText(i, T_REF_SIGMA, lineEditTRefSigma->text());

      table->setText(i, V_SKY, lineEditVSky->text());
      table->setText(i, V_SKY_SIGMA, lineEditVSkySigma->text());
      table->setText(i, V_REF, lineEditVRef->text());
      table->setText(i, V_REF_SIGMA, lineEditVRefSigma->text());

      break;
    }
  }
}

void LinearityGainIsolationI::blockTextChangeSignals(bool block) {
  lineEditTSky->blockSignals(block);
  lineEditTSkySigma->blockSignals(block);
  lineEditTRef->blockSignals(block);
  lineEditTRefSigma->blockSignals(block);

  lineEditVSky->blockSignals(block);
  lineEditVSkySigma->blockSignals(block);
  lineEditVRef->blockSignals(block);
  lineEditVRefSigma->blockSignals(block);
}

void LinearityGainIsolationI::cellValueChanged(int row, int col) {
  QString value = table->text(row, col);

  switch( col ) {
    case T_SKY:
      lineEditTSky->setText(value);
      break;
    case T_SKY_SIGMA:
      lineEditTSkySigma->setText(value);
      break;
    case T_REF:
      lineEditTRef->setText(value);
      break;
    case T_REF_SIGMA:
      lineEditTRefSigma->setText(value);
      break;
    case V_SKY:
      lineEditVSky->setText(value);
      break;
    case V_SKY_SIGMA:
      lineEditVSkySigma->setText(value);
      break;
    case V_REF:
      lineEditVRef->setText(value);
      break;
    case V_REF_SIGMA:
      lineEditVRefSigma->setText(value);
      break;
    default:
      break;
  }
}

void LinearityGainIsolationI::rowChanged() {
  int i;

  for( i=0; i<table->numRows(); i++) {
    if( table->isRowSelected(i) ) {
      blockTextChangeSignals(TRUE);
      
      lineEditTSky->setText(table->text(i, T_SKY));
      lineEditTSkySigma->setText(table->text(i, T_SKY_SIGMA));
      lineEditTRef->setText(table->text(i, T_REF));
      lineEditTRefSigma->setText(table->text(i, T_REF_SIGMA));

      lineEditVSky->setText(table->text(i, V_SKY));
      lineEditVSkySigma->setText(table->text(i, V_SKY_SIGMA));
      lineEditVRef->setText(table->text(i, V_REF));
      lineEditVRefSigma->setText(table->text(i, V_REF_SIGMA));

      blockTextChangeSignals(FALSE);

      calculate();

      break;
    }
  }
}

void LinearityGainIsolationI::shut() {
  save();
  close();
}

void LinearityGainIsolationI::save() {
  KConfig cfg("kstrc", false, false);
  int i;
  
  cfg.setGroup("RANA_LinearityGainIsolation");
  cfg.writeEntry("Number", table->numRows());

  for(i=0; i<table->numRows(); i++) {
    cfg.writeEntry(QString("Tsky%1").arg(i), table->text(i, T_SKY));
    cfg.writeEntry(QString("TskySigma%1").arg(i), table->text(i, T_SKY_SIGMA));
    cfg.writeEntry(QString("Tref%1").arg(i), table->text(i, T_REF));
    cfg.writeEntry(QString("TrefSigma%1").arg(i), table->text(i, T_REF_SIGMA));
    cfg.writeEntry(QString("Vsky%1").arg(i), table->text(i, V_SKY));
    cfg.writeEntry(QString("VskySigma%1").arg(i), table->text(i, V_SKY_SIGMA));
    cfg.writeEntry(QString("Vref%1").arg(i), table->text(i, V_REF));
    cfg.writeEntry(QString("VrefSigma%1").arg(i), table->text(i, V_REF_SIGMA));
  }
  cfg.writeEntry("Frequency", lineEditFrequencyGHz->text());
  cfg.sync();
}

void LinearityGainIsolationI::load() {  
  KConfig cfg("kstrc", false, false);
  QString empty;
  int i;
  
  cfg.setGroup("RANA_LinearityGainIsolation");
  table->blockSignals(TRUE);
  lineEditFrequencyGHz->blockSignals(TRUE);
  table->setNumRows(cfg.readNumEntry("Number", 0));

  for(i=0; i<table->numRows(); i++) {
    table->setText(i, T_SKY, cfg.readEntry(QString("Tsky%1").arg(i), empty));
    table->setText(i, T_SKY_SIGMA, cfg.readEntry(QString("TskySigma%1").arg(i), empty));
    table->setText(i, T_REF, cfg.readEntry(QString("Tref%1").arg(i), empty));
    table->setText(i, T_REF_SIGMA, cfg.readEntry(QString("TrefSigma%1").arg(i), empty));
    table->setText(i, V_SKY, cfg.readEntry(QString("Vsky%1").arg(i), empty));
    table->setText(i, V_SKY_SIGMA, cfg.readEntry(QString("VskySigma%1").arg(i), empty));
    table->setText(i, V_REF, cfg.readEntry(QString("Vref%1").arg(i), empty));
    table->setText(i, V_REF_SIGMA, cfg.readEntry(QString("VrefSigma%1").arg(i), empty));
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

#include "linearitygainisolation_i.moc"

// vim: ts=2 sw=2 et
