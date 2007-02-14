/***************************************************************************
                    noisetemperaturecalculation_i.cpp  -  Part of KST
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

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

#include "noisetemperaturecalculation_i.h"
#include "rana.h"

NoiseTemperatureCalculationI::NoiseTemperatureCalculationI(KstRANA* rana,
                                       QWidget* parent,
                                       const char* name,
                                       bool modal,
                                       WFlags fl) : NoiseTemperatureCalculation(parent, name, modal, fl) {
  _rana = rana;
}

NoiseTemperatureCalculationI::~NoiseTemperatureCalculationI() {
}

void NoiseTemperatureCalculationI::initialize() {
  connect( pushButtonCalculate, SIGNAL( clicked() ), this, SLOT( calculate() ) );
  connect( pushButtonClose, SIGNAL( clicked() ), this, SLOT( shut() ) );

  load( );
}

void NoiseTemperatureCalculationI::calculate() {
  double dVLo, dVLoSigma;
  double dVHi, dVHiSigma;
  double dTLo, dTLoSigma;
  double dTHi, dTHiSigma;
  bool ok;
  
  dTLo = lineEditTLow->text().toDouble(&ok);
  if (ok) {
    dTLoSigma = lineEditTLowSigma->text().toDouble();

    dTHi = lineEditTHigh->text().toDouble(&ok);
    if (ok) {
      dTHiSigma = lineEditTHighSigma->text().toDouble();

      dVLo = lineEditVLow->text().toDouble(&ok);
      if (ok) {
        dVLoSigma = lineEditVLowSigma->text().toDouble();

        dVHi = lineEditVHigh->text().toDouble(&ok);
        if (ok) {
          dVHiSigma = lineEditVHighSigma->text().toDouble();

          if (dVLo != dVHi) {
            double dATLo, dATLoSigma;
            double dATHi, dATHiSigma;
            double dY, dYSigma;
            double dTNoise, dTNoiseSigma;
            double dFrequency;

            dFrequency = lineEditFrequencyGHz->text().toDouble();

            KstRANA::convertToAntennaTemperature(dTLo, dTLoSigma, dFrequency, dATLo, dATLoSigma);
            KstRANA::convertToAntennaTemperature(dTHi, dTHiSigma, dFrequency, dATHi, dATHiSigma);
            
            dY = dVHi / dVLo;
            dYSigma = sqrt( ( dVHiSigma * dVHiSigma ) + ( dVLoSigma * dVLoSigma * dY * dY ) ) / dVLo;

            dTNoise = ( dATHi - ( dATLo * dY ) ) / ( dY - 1.0 );
            dTNoiseSigma  = pow( dATHiSigma * 1.0 / ( dY - 1.0 ), 2.0);
            dTNoiseSigma += pow( dATLoSigma * -dY / ( dY - 1.0 ), 2.0);
            dTNoiseSigma += pow( dYSigma * (dATLo - dATHi) / ((dY-1.0) * (dY-1.0)), 2.0);
            dTNoiseSigma  = sqrt( dTNoiseSigma );

            lineEditTNoise->setText( QString::number(dTNoise) );
            lineEditTNoiseSigma->setText( QString::number(dTNoiseSigma) );
          } else {
            QMessageBox::critical( this, i18n("Invalid value:"), i18n("Please enter different values for V_high and V_low") );
          }
        } else {
          QMessageBox::critical( this, i18n("Invalid value:"), i18n("Please enter a valid value for V_high") );
        }
      } else {
        QMessageBox::critical( this, i18n("Invalid value:"), i18n("Please enter a valid value for V_low") );
      }
    } else {
      QMessageBox::critical( this, i18n("Invalid value:"), i18n("Please enter a valid value for T_high") );
    }
  } else {
    QMessageBox::critical( this, i18n("Invalid value:"), i18n("Please enter a valid value for T_low") );
  }
}

void NoiseTemperatureCalculationI::shut() {
  save();
  close();
}

void NoiseTemperatureCalculationI::save() {
  KConfig cfg("kstrc", false, false);

  cfg.setGroup("RANA_NoiseTemperatureCalculation");
  cfg.writeEntry("Tlow", lineEditTLow->text());
  cfg.writeEntry("TlowSigma", lineEditTLowSigma->text());
  cfg.writeEntry("Thigh", lineEditTHigh->text());
  cfg.writeEntry("ThighSigma", lineEditTHighSigma->text());
  cfg.writeEntry("Vlow", lineEditVLow->text());
  cfg.writeEntry("VlowSigma", lineEditVLowSigma->text());
  cfg.writeEntry("Vhigh", lineEditVHigh->text());
  cfg.writeEntry("VhighSigma", lineEditVHighSigma->text());
  cfg.writeEntry("Frequency", lineEditFrequencyGHz->text());
  cfg.sync();
}

void NoiseTemperatureCalculationI::load() {  
  KConfig cfg("kstrc", false, false);

  cfg.setGroup("RANA_NoiseTemperatureCalculation");
  lineEditTLow->setText(cfg.readEntry("Tlow", ""));
  lineEditTLowSigma->setText(cfg.readEntry("TlowSigma", ""));
  lineEditTHigh->setText(cfg.readEntry("Thigh", ""));
  lineEditTHighSigma->setText(cfg.readEntry("ThighSigma", ""));
  lineEditVLow->setText(cfg.readEntry("Vlow", ""));
  lineEditVLowSigma->setText(cfg.readEntry("VlowSigma", ""));
  lineEditVHigh->setText(cfg.readEntry("Vhigh", ""));
  lineEditVHighSigma->setText(cfg.readEntry("VhighSigma", ""));
  lineEditFrequencyGHz->setText(cfg.readEntry("Frequency", ""));
}

#include "noisetemperaturecalculation_i.moc"

// vim: ts=2 sw=2 et
