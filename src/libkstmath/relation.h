/***************************************************************************
                     relation.h: base curve type for kst
                             -------------------
    begin                : Fri Oct 22 2000
    copyright            : (C) 2000 by C. Barth Netterfield
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

#ifndef RELATION_H
#define RELATION_H

#include <qcolor.h>
#include <q3valuestack.h>

#include "dataobject.h"
#include "painter.h"
#include "kst_export.h"
#include "labelparser.h"

/**A class for handling curves for kst
 *@author C. Barth Netterfield
 */

class QXmlStreamWriter;

namespace Kst {

// context for rendering a curve in a plot
class CurveRenderContext {
  public:
    // FIXME: use reasonable defaults
    CurveRenderContext() : painter(0L), Lx(0.0), Hx(0.0), Ly(0.0), Hy(0.0),
                              m_X(0.0), m_Y(0.0), b_X(0.0), b_Y(0.0),
                              x_max(0.0), y_max(0.0), x_min(0.0), y_min(0.0),
                              XMin(0.0), YMin(0.0), XMax(0.0), YMax(0.0),
                              xLog(false), yLog(false), xLogBase(0.0),
                              yLogBase(0.0), penWidth(0)
    {}

    QPainter* painter;
    QRect window;
    double Lx, Hx, Ly, Hy;
    double m_X, m_Y, b_X, b_Y;
    double x_max, y_max, x_min, y_min;
    double XMin, YMin, XMax, YMax; // range and domain of plot
    bool xLog, yLog;
    double xLogBase, yLogBase;
    QColor foregroundColor; // plot foreground color
    QColor backgroundColor; // plot background color
    int penWidth;
};

enum CurveType { VCURVE, HISTOGRAM, IMAGE };

class ObjectStore;
class Relation;

class KST_EXPORT Relation : public Object {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }

    Relation(ObjectStore *store, const ObjectTag& tag);
    Relation(ObjectStore *store, const QDomElement& e);
    virtual ~Relation();

    virtual void showNewDialog() { }
    virtual void showEditDialog() { }
    virtual void save(QXmlStreamWriter &s);

    virtual QString propertyString() const = 0;

    virtual CurveType curveType() const = 0;

    virtual int sampleCount() const { return NS; }

    virtual QString xLabel() const   { return QString::null; }
    virtual QString yLabel() const   { return QString::null; }
    virtual QString topLabel() const { return QString::null; }

    virtual void setIgnoreAutoScale(bool ignoreAutoScale);
    virtual bool ignoreAutoScale() const { return _ignoreAutoScale; }

    virtual int samplesPerFrame() const { return 1; }

    virtual void deleteDependents();

    virtual double maxX() const { return MaxX; }
    virtual double minX() const { return MinX; }
    virtual double maxY() const { return MaxY; }
    virtual double minY() const { return MinY; }
    virtual double minPosY() const { return MinPosY; }
    virtual double ns_maxX()    const { return _ns_maxx; }
    virtual double ns_minX()    const { return _ns_minx; }
    virtual double ns_maxY()    const { return _ns_maxy; }
    virtual double ns_minY()    const { return _ns_miny; }
    virtual double minPosX() const { return MinPosX; }
    virtual double midX() const { return (MaxX+MinX)*0.5; }
    virtual double midY() const { return (MaxY+MinY)*0.5; }
    virtual void yRange(double xFrom, double xTo, double* yMin, double* yMax) = 0;

    virtual bool uses(ObjectPtr p) const;

    // this returns the data object providing the data for this basecurve.
    // E.g. for VCurves, it returns the data object providing the y vector
    // E.g. for Images, it returns the data object providing the matrix
    // Null is returned if no provider exists
    virtual DataObjectPtr providerDataObject() const = 0;

    // return closest distance to the given point
    // images always return a rating >= 5
    virtual double distanceToPoint(double xpos, double dx, double ypos) const = 0;

    // render this curve
    virtual void paint(const CurveRenderContext& context) = 0;

    // render the legend symbol for this curve
    virtual void paintLegendSymbol(Painter *p, const QRect& bound) = 0;


    // just store the size of the legend tag here.  The viewLegend actually uses and calculates it.
    virtual QSize legendLabelSize() const {return _legendLabelSize;}
    virtual void setLegendLabelSize(QSize ls) {_legendLabelSize = ls;}

    // Deal with the legend tag.
    // The actual text is always stored in the base class.
    // If _legendText == QString("") then the tagname should be used as the legend text.
    // The label is parsed every time the legend text is changed.
    virtual Label::Parsed *parsedLegendTag();
    virtual void updateParsedLegendTag();

    void setLegendText(const QString& theValue);
    QString legendText() const { return _legendText;}

    virtual SharedPtr<Relation> makeDuplicate(QMap< SharedPtr<Relation>, SharedPtr<Relation> > &duplicatedRelations) = 0;

    virtual void replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject);
    virtual void replaceDependency(VectorPtr oldVector, VectorPtr newVector);
    virtual void replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix);

  protected:
    virtual void writeLockInputsAndOutputs() const;
    virtual void unlockInputsAndOutputs() const;

    bool _isInputLoaded;
    QList<QPair<QString,QString> > _inputVectorLoadQueue;
    QList<QPair<QString,QString> > _inputScalarLoadQueue;
    QList<QPair<QString,QString> > _inputStringLoadQueue;
    QList<QPair<QString,QString> > _inputMatrixLoadQueue;
    CurveHintList *_curveHints;
    QString _typeString, _type;
    VectorMap _inputVectors;
    VectorMap _outputVectors;
    ScalarMap _inputScalars;
    ScalarMap _outputScalars;
    StringMap _inputStrings;
    StringMap _outputStrings;
    MatrixMap _inputMatrices;
    MatrixMap _outputMatrices;

    double _ns_maxx;
    double _ns_minx;
    double _ns_maxy;
    double _ns_miny;
    double MaxX;
    double MinX;
    double MinPosX;
    double MeanX;
    double MaxY;
    double MinY;
    double MinPosY;

    int NS;

    bool _ignoreAutoScale;

    Label::Parsed *_parsedLegendTag;
    QSize _legendLabelSize;

  private:
    void commonConstructor();
    QString _legendText;

  signals:
    void modifiedLegendEntry();
};


typedef SharedPtr<Relation> RelationPtr;
typedef ObjectList<Relation> RelationList;

}

#endif
// vim: ts=2 sw=2 et
