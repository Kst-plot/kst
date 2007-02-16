/***************************************************************************
                     kstimage.h: image type for kst
                             -------------------
    begin                : Mon July 19 2004
    copyright            : (C) 2005 by University of British Columbia
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

#ifndef KSTIMAGE_H
#define KSTIMAGE_H

#include <kpalette.h>

#include "kstmatrix.h"
#include "kstbasecurve.h"
#include "kst_export.h"
//Added by qt3to4:
#include <Q3ValueList>

/**A class for handling images for Kst
 *@author University of British Columbia
 */
class KST_EXPORT KstImage: public KstBaseCurve {
  public:
    //constructor for colormap only
    KstImage(const QString &in_tag, KstMatrixPtr in_matrix, double lowerZ, double upperZ, bool autoThreshold, KPalette* pal);
    //constructor for contour map only
    KstImage(const QString &in_tag, KstMatrixPtr in_matrix, int numContours, const QColor& contourColor, int contourWeight);
    //constructor for both colormap and contour map
    KstImage(const QString &in_tag,
        KstMatrixPtr in_matrix,
        double lowerZ,
        double upperZ,
        bool autoThreshold,
        KPalette* pal,
        int numContours,
        const QColor& contourColor,
        int contourWeight);

    KstImage(const QDomElement& e);
    virtual ~KstImage();

    virtual void showNewDialog();
    virtual void showEditDialog();
    virtual void save(Q3TextStream &ts, const QString& indent = QString::null);
    virtual UpdateType update(int update_counter = -1);
    virtual QString propertyString() const;

    virtual KstCurveType curveType() const;

    virtual bool getNearestZ(double x, double y, double& z);
    virtual QColor getMappedColor(double x, double y);
    virtual void setPalette(KPalette* pal);
    virtual void setUpperThreshold(double z);
    virtual void setLowerThreshold(double z);
    virtual void setAutoThreshold(bool yes);
    virtual void setThresholdToSpikeInsensitive(double per = 0.005);


    virtual double upperThreshold() const { return _zUpper; }
    virtual double lowerThreshold() const { return _zLower; }
    virtual bool autoThreshold() const { return _autoThreshold; }

    virtual QString matrixTag() const;
    virtual KstMatrixPtr matrix() const;
    virtual QString paletteName() const;
    virtual KPalette* palette() const { return _pal; }

    virtual void matrixDimensions(double &x, double &y, double &width, double &height);

    virtual void changeToColorOnly(const QString &in_tag, KstMatrixPtr in_matrix,
        double lowerZ, double upperZ, bool autoThreshold, KPalette* pal);
    virtual void changeToContourOnly(const QString &in_tag, KstMatrixPtr in_matrix,
        int numContours, const QColor& contourColor, int contourWeight);
    virtual void changeToColorAndContour(const QString &in_tag, KstMatrixPtr in_matrix,
        double lowerZ, double upperZ, bool autoThreshold, KPalette* pal,
        int numContours, const QColor& contourColor, int contourWeight);

    //contour lines
    virtual int numContourLines() const { return _numContourLines; }
    virtual Q3ValueList<double> contourLines() const { return _contourLines; }
    virtual bool addContourLine(double line);
    virtual bool removeContourLine(double line);
    virtual void clearContourLines();
    virtual const QColor& contourColor() const { return _contourColor; }
    virtual int contourWeight() const { return _contourWeight; } // a contour weight of -1 means variable weight

    //other properties
    virtual bool hasContourMap() const { return _hasContourMap; }
    virtual bool hasColorMap() const { return _hasColorMap; }
    
    // labels for plots
    virtual QString xLabel() const;
    virtual QString yLabel() const;
    virtual QString topLabel() const;
    
    virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap);

    // see KstBaseCurve::providerDataObject
    virtual KstDataObjectPtr providerDataObject() const;
    
    // see KstBaseCurve::distanceToPoint
    virtual double distanceToPoint(double xpos, double dx, double ypos) const;
    
    // see KstBaseCurve::paint
    virtual void paint(const KstCurveRenderContext& context);
    
    // see KstBaseCurve::yRange
    virtual void yRange(double xFrom, double xTo, double* yMin, double* yMax);
    
    // see KstBaseCurve::paintLegendSymbol
    virtual void paintLegendSymbol(KstPainter *p, const QRect& bound);

  private:
    //use these to set defaults when either is not used.
    void setColorDefaults();
    void setContourDefaults();
    KPalette* _pal;
    //upper and lower thresholds
    double _zUpper;
    double _zLower;
    bool _autoThreshold;

    bool _hasColorMap;
    bool _hasContourMap;

    int _numContourLines;
    Q3ValueList<double> _contourLines;
    QColor _contourColor;
    int _contourWeight; //_contourWeight = -1 means variable weight
    QString _lastPaletteName;
};


typedef KstSharedPtr<KstImage> KstImagePtr;
typedef KstObjectList<KstImagePtr> KstImageList;

#endif
// vim: ts=2 sw=2 et
