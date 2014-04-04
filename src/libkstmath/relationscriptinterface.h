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

namespace Kst {

class CurveSI;
typedef QString (CurveSI::*CurveInterfaceMemberFn)(QString& command);

class KSTMATH_EXPORT CurveSI : public ScriptInterface
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

};

}

#endif // RELATIONSCRIPTINTERFACE_H
