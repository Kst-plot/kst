/***************************************************************************
                          histogram.h: Histogram for KST
                             -------------------
    begin                : Wed July 11 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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

/** A class for handling power spectra for kst

 *@author C. Barth Netterfield
 */
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "dataobject.h"
#include "kst_export.h"

namespace Kst {

class KST_EXPORT Histogram : public DataObject {

public:
  enum NormalizationType { Number, Percent, Fraction, MaximumOne};

  Histogram(const QString &in_tag, VectorPtr in_V,
               double xmin_in, double xmax_in,
               int in_n_bins,
               NormalizationType new_norm_in);
  Histogram(const QDomElement &e);
  virtual ~Histogram();

  virtual UpdateType update(int update_counter = -1);
  virtual void save(QXmlStreamWriter &xml);
  virtual QString propertyString() const;

  int numberOfBins() const;
  void setNumberOfBins(int in_n_bins);

  void setXRange(double xmin_in, double xmax_in);

  QString vTag() const;

  void setVector(VectorPtr);

  virtual QString yLabel() const;
  virtual QString xLabel() const;

  bool isNormalizationNumber()        const { return _NormalizationMode == Number; }
  void setIsNormalizationNumber()           { _NormalizationMode = Number; }
  bool isNormalizationPercent()    const { return _NormalizationMode == Percent; }
  void setIsNormalizationPercent()       { _NormalizationMode = Percent; }
  bool isNormalizationFraction()   const { return _NormalizationMode == Fraction; }
  void setIsNormalizationFraction()      { _NormalizationMode = Fraction; }
  bool isNormalizationMaximumOne()        const { return _NormalizationMode == MaximumOne; }
  void setIsNormNormalizationMaximumOne()           { _NormalizationMode = MaximumOne; }

  static void AutoBin(const VectorPtr, int *n, double *max, double *min);

  virtual void showNewDialog();
  virtual void showEditDialog();

  virtual bool slaveVectorsUsed() const;

  virtual QString xVTag() const {return (*_bVector)->tagName();}
  virtual QString yVTag() const {return (*_hVector)->tagName();}

  void setRealTimeAutoBin(bool autobin);
  bool realTimeAutoBin() const;

  VectorPtr vX() const { return *_bVector; }
  VectorPtr vY() const { return *_hVector; }

  double xMin() const { return _MinX; }
  double xMax() const { return _MaxX; }
  double width() const { return _W; }

  double vMax() const;
  double vMin() const;
  int vNumSamples() const;
  
  virtual DataObjectPtr makeDuplicate(DataObjectDataObjectMap& duplicatedMap);

private:
  NormalizationType _NormalizationMode;
  VectorMap::Iterator _bVector;
  VectorMap::Iterator _hVector;
  double _MaxX;
  double _MinX;
  int _NS;
  int _NumberOfBins;
  unsigned long *_Bins;
  double _Normalization;
  double _W;
  bool _realTimeAutoBin;
  
  void commonConstructor(const QString &in_tag, VectorPtr in_V,
                         double xmin_in, double xmax_in,
                         int in_n_bins,
                         NormalizationType in_norm);
  void internalSetNumberOfBins(int in_n_bins);
};

typedef SharedPtr<Histogram> HistogramPtr;
typedef ObjectList<HistogramPtr> HistogramList;

}

#endif
// vim: ts=2 sw=2 et
