/***************************************************************************
                       noisetemperaturecalculation_i.h  -  Part of KST
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

#ifndef NOISETEMPERATURECALCULATIONI_H
#define NOISETEMPERATURECALCULATIONI_H

#include "noisetemperaturecalculation.h"

class KstRANA;

class NoiseTemperatureCalculationI : public NoiseTemperatureCalculation {
  Q_OBJECT
  public:
    NoiseTemperatureCalculationI( KstRANA* elog,
                        QWidget* parent = 0,
                        const char* name = 0,
                        bool modal = FALSE, WFlags fl = 0 );
    virtual ~NoiseTemperatureCalculationI();

    void initialize();
        
  private slots:
    virtual void calculate();
    virtual void shut();
    
  signals:
    
  protected:
    
  private:
    void save();
    void load();
        
    KstRANA* _rana;
};


#endif // NOISETEMPERATURECALCULATIONI_H
