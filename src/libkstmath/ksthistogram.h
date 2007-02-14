/***************************************************************************
                          ksthistogram.h: Histogram for KST
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
#ifndef KSTHISTOGRAM_H
#define KSTHISTOGRAM_H

#include "kstbasecurve.h"
#include "kst_export.h"

enum KstHsNormType { KST_HS_NUMBER, KST_HS_PERCENT, KST_HS_FRACTION, KST_HS_MAX_ONE};

class KST_EXPORT KstHistogram: public KstDataObject {
public:
  KstHistogram(const QString &in_tag, KstVectorPtr in_V,
               double xmin_in, double xmax_in,
               int in_n_bins,
               KstHsNormType new_norm_in);
  KstHistogram(const QDomElement &e);
  virtual ~KstHistogram();

  virtual UpdateType update(int update_counter = -1);
  virtual void save(QTextStream &ts, const QString& indent = QString::null);
  virtual QString propertyString() const;

  int nBins() const;
  void setNBins(int in_n_bins);

  void setXRange(double xmin_in, double xmax_in);

  QString vTag() const;

  void setVector(KstVectorPtr);

  virtual QString yLabel() const;
  virtual QString xLabel() const;

  bool isNormNum()        const { return _NormMode == KST_HS_NUMBER; }
  void setIsNormNum()           { _NormMode = KST_HS_NUMBER; }
  bool isNormPercent()    const { return _NormMode == KST_HS_PERCENT; }
  void setIsNormPercent()       { _NormMode = KST_HS_PERCENT; }
  bool isNormFraction()   const { return _NormMode == KST_HS_FRACTION; }
  void setIsNormFraction()      { _NormMode = KST_HS_FRACTION; }
  bool isNormOne()        const { return _NormMode == KST_HS_MAX_ONE; }
  void setIsNormOne()           { _NormMode = KST_HS_MAX_ONE; }

  static void AutoBin(const KstVectorPtr, int *n, double *max, double *min);

  virtual void showNewDialog();
  virtual void showEditDialog();

  virtual bool slaveVectorsUsed() const;

  virtual QString xVTag() const {return (*_bVector)->tagName();}
  virtual QString yVTag() const {return (*_hVector)->tagName();}

  void setRealTimeAutoBin(bool autobin);
  bool realTimeAutoBin() const;

  KstVectorPtr vX() const { return *_bVector; }
  KstVectorPtr vY() const { return *_hVector; }

  double xMin() const { return _MinX; }
  double xMax() const { return _MaxX; }
  double width() const { return _W; }

  double vMax() const;
  double vMin() const;
  int vNumSamples() const;
  
  virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap);

private:
  KstHsNormType _NormMode;
  KstVectorMap::Iterator _bVector;
  KstVectorMap::Iterator _hVector;
  double _MaxX;
  double _MinX;
  int _NS;
  int _NBins;
  unsigned long *_Bins;
  double _Normalization;
  double _W;
  bool _realTimeAutoBin;
  
  void commonConstructor(const QString &in_tag, KstVectorPtr in_V,
                         double xmin_in, double xmax_in,
                         int in_n_bins,
                         KstHsNormType in_norm);
  void internalSetNBins(int in_n_bins);
};

typedef KstSharedPtr<KstHistogram> KstHistogramPtr;
typedef KstObjectList<KstHistogramPtr> KstHistogramList;

#endif
// vim: ts=2 sw=2 et
