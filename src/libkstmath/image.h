/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005 by University of British Columbia
 *                   dscott@phas.ubc.ca                                    *
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
#include "kstmath_export.h"
#include "palette.h"
#include "labelinfo.h"

#include <QHash>

namespace Kst {

class ObjectStore;

class CoutourLineDetails {
  public:
    CoutourLineDetails() { }
    CoutourLineDetails(QLine line, int width) { _line = line; _lineWidth = width; }

  QLine _line;
  int _lineWidth;
};

/**A class for handling images for Kst
 *@author University of British Columbia
 */
class KSTMATH_EXPORT Image : public Relation {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }
    static const QString staticTypeTag;

    virtual void showNewDialog();
    virtual void showEditDialog();
    virtual void save(QXmlStreamWriter &s);
    virtual void internalUpdate();
    virtual QString propertyString() const;

    virtual bool getNearestZ(double x, double y, double& z);
    virtual QColor getMappedColor(double z);
    virtual void setPalette(const Palette &pal);
    virtual void setUpperThreshold(double z);
    virtual void setLowerThreshold(double z);
    virtual void setAutoThreshold(bool yes);
    virtual void setThresholdToSpikeInsensitive(double per = 0.005);


    virtual double upperThreshold() const { return _zUpper; }
    virtual double lowerThreshold() const { return _zLower; }
    virtual bool autoThreshold() const { return _autoThreshold; }

    virtual MatrixPtr matrix() const;
    virtual QString paletteName() const;
    virtual const Palette &palette() const { return _pal; }

    virtual void matrixDimensions(double &x, double &y, double &width, double &height);

    virtual void changeToColorOnly(MatrixPtr in_matrix,
        double lowerZ, double upperZ, bool autoThreshold, const QString &paletteName);
    virtual void changeToContourOnly(MatrixPtr in_matrix,
        int numContours, const QColor& contourColor, int contourWeight);
    virtual void changeToColorAndContour(MatrixPtr in_matrix,
        double lowerZ, double upperZ, bool autoThreshold, const QString &paletteName,
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
    virtual LabelInfo xLabelInfo() const;
    virtual LabelInfo yLabelInfo() const;
    virtual LabelInfo titleInfo() const;

    virtual RelationPtr makeDuplicate() const;

    // see KstRelation::providerDataObject
    virtual DataObjectPtr providerDataObject() const;

    // see KstRelation::distanceToPoint
    virtual double distanceToPoint(double xpos, double dx, double ypos) const;

    // see KstRelation::paint
    void paintObjects(const CurveRenderContext& context);

    // Update the curve details.
    void updatePaintObjects(const CurveRenderContext& context);

    // see KstRelation::yRange
    virtual void yRange(double xFrom, double xTo, double* yMin, double* yMax);

    // see KstRelation::paintLegendSymbol
    virtual QSize legendSymbolSize(QPainter *p);
    virtual void paintLegendSymbol(QPainter *p, const QSize &size);
    virtual bool symbolLabelOnTop() {return true;}

    virtual QString descriptionTip() const;

    virtual bool invertXHint() const;
    virtual bool invertYHint() const;

  protected:
    Image(ObjectStore *store);
    virtual ~Image();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;
    virtual void _initializeShortName();

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

    QVector<CoutourLineDetails> _lines;
    QImage _image;
    QPoint _imageLocation;
};


typedef SharedPtr<Image> ImagePtr;
typedef ObjectList<Image> ImageList;

}

#endif
// vim: ts=2 sw=2 et
