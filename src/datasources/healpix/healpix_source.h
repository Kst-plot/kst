/***************************************************************************
                  healpix_source.h  -  HEALPIX data source plugin
                             -------------------
    begin                : Wed June 01 2005
    copyright            : (C) 2005 Ted Kisner
    email                : tskisner.public@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HEALPIX_SOURCE_H
#define HEALPIX_SOURCE_H

#include <kconfig.h>
#include <kstmath.h>
#include <kst_export.h>

#include "healpix_tools.h"

class HealpixSource : public KstDataSource {
  public:
    HealpixSource(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());

    ~HealpixSource();
    
    void loadConfig(KConfig *cfg);
    
    void saveConfig(KConfig *cfg);

    KstObject::UpdateType update(int = -1);
    
    bool reset();
    
    int readField(double *v, const QString &field, int s, int n);

    bool isValidField(const QString &field) const;

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;
    
    int readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps);
    
    bool isValidMatrix(const QString& field) const;
    
    bool matrixDimensions(const QString& matrix, int* xDim, int* yDim);

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString::null);
    
    void load(const QDomElement& e);

    bool isEmpty() const;
    
  private:
    // config widget
    friend class ConfigWidgetHealpix;
    
    // coordinate conversion
    void theta2Internal(int units, double& theta);
    void theta2External(int units, double& theta);
    void phi2Internal(int units, double& phi);
    void phi2External(int units, double& phi);
    
    // degrade check
    void checkDegrade(int& degrade);
    
    // file parameters
    int _mapType;
    int _mapOrder;
    int _mapCoord;
    size_t _mapNside;
    size_t _nMaps;
    size_t _mapNpix;
    char _healpixfile[HEALPIX_STRNL];
    healpix_keys *_keys;
    char _creator[HEALPIX_STRNL];
    char _extname[HEALPIX_STRNL];
    char **_names;
    char **_units;
    
    // data range
    int _nX;
    int _nY;
    double _thetaMin;
    double _phiMin;
    double _thetaMax;
    double _phiMax;
    bool _autoTheta;
    bool _autoPhi;
    
    //values are
    // 0 : Radians 
    // 1 : Degrees
    // 2 : Degrees RA/DEC
    // 3 : Degrees Lat/Long
    int _thetaUnits;
    int _phiUnits;
    int _vecUnits;
    
    // vector field
    int _vecDegrade;
    int _vecTheta;
    int _vecPhi;
    bool _autoMag;
    double _maxMag;
    bool _vecQU;
    
};

#endif
// vim: ts=2 sw=2 et
