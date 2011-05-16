/***************************************************************************
                     relation.h: base curve type for kst
                             -------------------
    begin                : Fri Oct 22 2000
    copyright            : (C) 2000 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
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

#include "dataobject.h"
#include "painter.h"
#include "kstmath_export.h"
#include "labelparser.h"
#include "labelinfo.h"

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

  struct CurveContextDetails {
    double Lx, Hx, Ly, Hy;
    double m_X, m_Y;
    double b_X, b_Y;
    double XMin, XMax;
    bool xLog, yLog;
    double xLogBase, yLogBase;
    int penWidth;
  };

class ObjectStore;
class Relation;

class KSTMATH_EXPORT Relation : public Object {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }

    Relation(ObjectStore *store);
    virtual ~Relation();

    virtual void showNewDialog() { }
    virtual void showEditDialog() { }
    virtual void save(QXmlStreamWriter &s);

    virtual QString propertyString() const = 0;

    virtual int sampleCount() const { return NS; }

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
    void paint(const CurveRenderContext& context);

    virtual void paintObjects(const CurveRenderContext& context) = 0;
    virtual void updatePaintObjects(const CurveRenderContext& context) = 0;

    // render the legend symbol for this curve
    virtual void paintLegendSymbol(QPainter *p, const QRectF& bound) = 0;

    //virtual SharedPtr<Relation> makeDuplicate(QMap< SharedPtr<Relation>, SharedPtr<Relation> > &duplicatedRelations) = 0;
    virtual SharedPtr<Relation> makeDuplicate() const = 0;

    virtual void replaceInput(PrimitivePtr p, PrimitivePtr new_p);

    // Compare the cached the context to the provided one.
    bool redrawRequired(const CurveRenderContext& context); 

    // If you use these, you must lock() and unlock() the object as long as you
    // hold the reference
    const VectorMap& inputVectors()  const { return _inputVectors;  }
    const VectorMap& outputVectors() const { return _outputVectors; }
    VectorMap& inputVectors() { return _inputVectors;  }
    VectorMap& outputVectors() { return _outputVectors; }

    const ScalarMap& inputScalars()  const { return _inputScalars;  }
    const ScalarMap& outputScalars() const { return _outputScalars; }
    ScalarMap& inputScalars() { return _inputScalars;  }
    ScalarMap& outputScalars() { return _outputScalars; }

    const StringMap& inputStrings()  const { return _inputStrings;  }
    const StringMap& outputStrings() const { return _outputStrings; }
    StringMap& inputStrings() { return _inputStrings;  }
    StringMap& outputStrings() { return _outputStrings; }

    const MatrixMap& inputMatrices() const { return _inputMatrices; }
    const MatrixMap& outputMatrices() const { return _outputMatrices; }
    MatrixMap& inputMatrices() { return _inputMatrices; }
    MatrixMap& outputMatrices() { return _outputMatrices; }

    PrimitiveList inputPrimitives() const;

    virtual bool invertXHint() const {return false;}
    virtual bool invertYHint() const {return false;}

    virtual LabelInfo xLabelInfo() const = 0;
    virtual LabelInfo yLabelInfo() const = 0;
    virtual LabelInfo titleInfo() const = 0;

  protected:
    virtual void writeLockInputsAndOutputs() const;
    virtual void unlockInputsAndOutputs() const;

    virtual qint64 minInputSerial() const;
    virtual qint64 maxInputSerialOfLastChange() const;

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

    CurveContextDetails _contextDetails;
    bool _redrawRequired;

  private:
    void commonConstructor();
};


typedef SharedPtr<Relation> RelationPtr;
typedef ObjectList<Relation> RelationList;

}

#endif
// vim: ts=2 sw=2 et
