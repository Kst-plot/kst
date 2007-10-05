/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005 by University of British Columbia
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#include "relation.h"
#include "kst_export.h"

#include <QHash>

namespace Kst {

typedef QHash<int, QColor> Palette;

/**A class for handling images for Kst
 *@author University of British Columbia
 */
class KST_EXPORT Image : public Relation {
  public:
    //constructor for colormap only
    Image(const QString &in_tag, MatrixPtr in_matrix, double lowerZ, double upperZ, bool autoThreshold, const Palette &pal);
    //constructor for contour map only
    Image(const QString &in_tag, MatrixPtr in_matrix, int numContours, const QColor& contourColor, int contourWeight);
    //constructor for both colormap and contour map
    Image(const QString &in_tag,
        MatrixPtr in_matrix,
        double lowerZ,
        double upperZ,
        bool autoThreshold,
        const Palette &pal,
        int numContours,
        const QColor& contourColor,
        int contourWeight);

    Image(const QDomElement& e);
    virtual ~Image();

    virtual void showNewDialog();
    virtual void showEditDialog();
    virtual void save(QTextStream &ts, const QString& indent = QString::null);
    virtual UpdateType update(int update_counter = -1);
    virtual QString propertyString() const;

    virtual CurveType curveType() const;

    virtual bool getNearestZ(double x, double y, double& z);
    virtual QColor getMappedColor(double x, double y);
    virtual void setPalette(const Palette &pal);
    virtual void setUpperThreshold(double z);
    virtual void setLowerThreshold(double z);
    virtual void setAutoThreshold(bool yes);
    virtual void setThresholdToSpikeInsensitive(double per = 0.005);


    virtual double upperThreshold() const { return _zUpper; }
    virtual double lowerThreshold() const { return _zLower; }
    virtual bool autoThreshold() const { return _autoThreshold; }

    virtual QString matrixTag() const;
    virtual MatrixPtr matrix() const;
    virtual QString paletteName() const;
    virtual const Palette &palette() const { return _pal; }

    virtual void matrixDimensions(double &x, double &y, double &width, double &height);

    virtual void changeToColorOnly(const QString &in_tag, MatrixPtr in_matrix,
        double lowerZ, double upperZ, bool autoThreshold, const Palette &pal);
    virtual void changeToContourOnly(const QString &in_tag, MatrixPtr in_matrix,
        int numContours, const QColor& contourColor, int contourWeight);
    virtual void changeToColorAndContour(const QString &in_tag, MatrixPtr in_matrix,
        double lowerZ, double upperZ, bool autoThreshold, const Palette &pal,
        int numContours, const QColor& contourColor, int contourWeight);

    //contour lines
    virtual int numContourLines() const { return _numContourLines; }
    virtual QList<double> contourLines() const { return _contourLines; }
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
    
#if 0
    virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap);
#endif

    // see KstRelation::providerDataObject
    virtual DataObjectPtr providerDataObject() const;
    
    // see KstRelation::distanceToPoint
    virtual double distanceToPoint(double xpos, double dx, double ypos) const;
    
    // see KstRelation::paint
    virtual void paint(const CurveRenderContext& context);
    
    // see KstRelation::yRange
    virtual void yRange(double xFrom, double xTo, double* yMin, double* yMax);
    
    // see KstRelation::paintLegendSymbol
    virtual void paintLegendSymbol(Painter *p, const QRect& bound);

  private:
    //use these to set defaults when either is not used.
    void setColorDefaults();
    void setContourDefaults();
    Palette _pal;
    //upper and lower thresholds
    double _zUpper;
    double _zLower;
    bool _autoThreshold;

    bool _hasColorMap;
    bool _hasContourMap;

    int _numContourLines;
    QList<double> _contourLines;
    QColor _contourColor;
    int _contourWeight; //_contourWeight = -1 means variable weight
    QString _lastPaletteName;
};


typedef SharedPtr<Image> ImagePtr;
typedef ObjectList<ImagePtr> ImageList;

}

#endif
// vim: ts=2 sw=2 et
