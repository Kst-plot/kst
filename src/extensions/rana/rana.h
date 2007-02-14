/***************************************************************************
                                    rana.h
                             -------------------
    begin                : Jan 27 2005
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

#ifndef RANA_H
#define RANA_H

#include <kstextension.h>
#include <kxmlguiclient.h>

#include "noisetemperaturecalculation_i.h"
#include "receiverinputoffset_i.h"
#include "linearitygainisolation_i.h"

class KstRANA : public KstExtension, public KXMLGUIClient {
  Q_OBJECT
  public:
    KstRANA(QObject *parent, const char *name, const QStringList&);
    virtual ~KstRANA();
    
    virtual void load(QDomElement& e);
    virtual void save(QTextStream& ts, const QString& indent = QString::null);

    static void convertToAntennaTemperature( double dSky, double dSkySigma, double dFreqGHz, double& dAntenna, double& dAntennaSigma);
            
  public slots:
    void doNoiseTemperatureCalculation();
    void doReceiverInputOffset();
    void doLinearityGainIsolation();
    
  private:
    NoiseTemperatureCalculationI* _noiseTemperatureCalculation;
    ReceiverInputOffsetI* _receiverInputOffset;
    LinearityGainIsolationI* _linearityGainIsolation;
};

#endif

// vim: ts=2 sw=2 et
