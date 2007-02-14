/***************************************************************************
                                   rana.cpp
                             -------------------
    begin                : Jan 29 2005
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

#include "rana.h"

#include <math.h>

#include <kapplication.h>
#include <kgenericfactory.h>

#include <kst.h>

#define PLANCK_CONSTANT      6.6260755e-34
#define BOLTZMANN_CONSTANT   1.380658e-23
#define GHZ_TO_HZ            1.0e9

K_EXPORT_COMPONENT_FACTORY(kstextension_rana, KGenericFactory<KstRANA>)

KstRANA::KstRANA(QObject *parent, const char *name, const QStringList& list) : KstExtension(parent, name, list), KXMLGUIClient() {
  new KAction(i18n("Linearity, Gain, and Isolation..."), 0L, CTRL+ALT+Key_L, this, SLOT(doLinearityGainIsolation()), actionCollection(), "linearity_gain_isolation");
  new KAction(i18n("Noise Temperature Calculation..."), 0L, CTRL+ALT+Key_N, this, SLOT(doNoiseTemperatureCalculation()), actionCollection(), "noise_temperature_calculation");
  new KAction(i18n("Receiver Input Offset..."), 0L, CTRL+ALT+Key_R, this, SLOT(doReceiverInputOffset()), actionCollection(), "receiver_input_offset");
  setInstance(app()->instance());
  setXMLFile("kstextension_rana.rc", true);
  app()->guiFactory()->addClient(this);

  _noiseTemperatureCalculation = new NoiseTemperatureCalculationI(this, app());
  _receiverInputOffset = new ReceiverInputOffsetI(this, app());
  _linearityGainIsolation = new LinearityGainIsolationI(this, app());
}


KstRANA::~KstRANA() {
  if( app() ) {
    if( app()->guiFactory() ) {
      app()->guiFactory()->removeClient(this);
    }
  }

  delete _noiseTemperatureCalculation;
  delete _receiverInputOffset;
  delete _linearityGainIsolation;
}


void KstRANA::doNoiseTemperatureCalculation() { 
  if (_noiseTemperatureCalculation) {
    _noiseTemperatureCalculation->initialize();
    _noiseTemperatureCalculation->show();
    _noiseTemperatureCalculation->raise();
  }
}


void KstRANA::doReceiverInputOffset() {
  if (_receiverInputOffset) {
    _receiverInputOffset->initialize();
    _receiverInputOffset->show();
    _receiverInputOffset->raise();
  }
}


void KstRANA::doLinearityGainIsolation() {
  if (_linearityGainIsolation) {
    _linearityGainIsolation->initialize();
    _linearityGainIsolation->show();
    _linearityGainIsolation->raise();
  }
}


void KstRANA::convertToAntennaTemperature( double dSky, double dSkySigma, double dFreqGHz, double& dAntenna, double& dAntennaSigma) {
  double dX;

  dX = PLANCK_CONSTANT * dFreqGHz * GHZ_TO_HZ / (BOLTZMANN_CONSTANT * dSky);
  dAntenna = dSky * dX / ( exp(dX) - 1.0 );
  dAntennaSigma = dSkySigma * dX * exp(dX/2.0) / ( exp(dX) - 1.0 );
}


void KstRANA::load(QDomElement& e) {
  Q_UNUSED(e)
}


void KstRANA::save(QTextStream& ts, const QString& indent) {
  Q_UNUSED(ts)
  Q_UNUSED(indent)
}

#include "rana.moc"
// vim: ts=2 sw=2 et
