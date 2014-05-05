/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2014 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RELATIONSCRIPTINTERFACE_H
#define RELATIONSCRIPTINTERFACE_H

#include <QString>

#include "scriptinterface.h"
#include "curve.h"
#include "image.h"

namespace Kst {

class CurveSI;
typedef QString (CurveSI::*CurveInterfaceMemberFn)(QString& command);

class ImageSI;
typedef QString (ImageSI::*ImageInterfaceMemberFn)(QString& command);

class KSTMATH_EXPORT RelationSI : public ScriptInterface
{
    Q_OBJECT

  public:
    QString maxX(QString&);
    QString minX(QString&);
    QString maxY(QString&);
    QString minY(QString&);
    QString showEditDialog(QString&);

  protected:
    RelationPtr relation;

};


class KSTMATH_EXPORT CurveSI : public RelationSI
{
    Q_OBJECT
  public:
    explicit CurveSI(CurvePtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newCurve(ObjectStore *store);

  protected:
    QString noSuchFn(QString&) {return ""; }

  private:
    CurvePtr curve;

    QMap<QString,CurveInterfaceMemberFn> _fnMap;

    QString setXVector(QString& command);
    QString setYVector(QString& command);
    QString setXError(QString& command);
    QString setYError(QString& command);
    QString setXMinusError(QString& command);
    QString setYMinusError(QString& command);

    QString setColor(QString& command);
    QString setHeadColor(QString& command);
    QString setBarFillColor(QString& command);
    QString setHasPoints(QString& command);
    QString setHasLines(QString& command);
    QString setHasBars(QString& command);
    QString setHasHead(QString& command);


    QString setLineWidth(QString& command);
    QString setPointSize(QString& command);

    QString setPointType(QString& command);
    QString setHeadType(QString& command);
    QString setLineStyle(QString& command);
    QString setPointDensity(QString& command);

    QString color(QString&);
    QString headColor(QString&);
    QString barFillColor(QString&);
    QString hasPoints(QString&);
    QString hasLines(QString&);
    QString hasBars(QString&);
    QString hasHead(QString&);

    QString lineWidth(QString&);
    QString pointSize(QString&);

    QString pointType(QString&);
    QString headType(QString&);
    QString lineStyle(QString&);
    QString pointDensity(QString& command);

    QString xVector(QString &);
    QString yVector(QString &);
    QString xErrorVector(QString &);
    QString yErrorVector(QString &);
    QString xMinusErrorVector(QString &);
    QString yMinusErrorVector(QString &);

};

class KSTMATH_EXPORT ImageSI : public RelationSI
{
    Q_OBJECT
  public:
    explicit ImageSI(ImagePtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newImage(ObjectStore *store);

  protected:
    QString noSuchFn(QString&) {return ""; }

  private:
    ImagePtr image;

    QMap<QString,ImageInterfaceMemberFn> _fnMap;

    QString setMatrix(QString& command);
    QString setPalette(QString& command);
    QString setFixedColorRange(QString& command);
    QString setAutoColorRange(QString& command);

    QString lowerThreshold(QString&);
    QString upperThreshold(QString&);

};

}

#endif // RELATIONSCRIPTINTERFACE_H
