/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QByteArray>
#include <QString>
#include <QSizeF>
#include <QList>

#include "scriptinterface.h"
#include "basicplugin.h"
#include "equation.h"
#include "psd.h"
#include "objectstore.h"
#include "updatemanager.h"
#include "updateserver.h"

#ifndef DATAOBJECTSCRIPTINTERFACE_H
#define DATAOBJECTSCRIPTINTERFACE_H

namespace Kst {

class KSTMATH_EXPORT DataObjectSI : public ScriptInterface
{
    Q_OBJECT

  public:
    QString setInputVector(QString& command);
    QString setInputScalar(QString& command);

    QString outputVector(QString& command);
    QString outputScalar(QString& command);

  protected:
    DataObjectPtr _dataObject;

};


class PluginSI;
typedef QString (PluginSI::*PluginInterfaceMemberFn)(QString& command);

class KSTMATH_EXPORT PluginSI : public DataObjectSI
{    
    Q_OBJECT
public:
    explicit PluginSI(BasicPluginPtr plugin);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newPlugin(ObjectStore *store, QByteArray pluginName);

  protected:
    QString noSuchFn(QString&) {return ""; }

  private:
    BasicPluginPtr _plugin;

    QMap<QString,PluginInterfaceMemberFn> _fnMap;


};


class EquationSI;
typedef QString (EquationSI::*EquationInterfaceMemberFn)(QString& command);

class KSTMATH_EXPORT EquationSI : public DataObjectSI
{
    Q_OBJECT
public:
    explicit EquationSI(EquationPtr equation);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newEquation(ObjectStore *store);

  protected:
    QString noSuchFn(QString&) {return ""; }

  private:
    EquationPtr _equation;

    QMap<QString,EquationInterfaceMemberFn> _fnMap;

    QString equation(QString &);
    QString setEquation(QString &eq);
};


class SpectrumSI;
typedef QString (SpectrumSI::*SpectrumInterfaceMemberFn)(QString& command);

class KSTMATH_EXPORT SpectrumSI : public DataObjectSI
{
    Q_OBJECT
public:
    explicit SpectrumSI(PSDPtr psd);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newSpectrum(ObjectStore *store);

  protected:
    QString noSuchFn(QString&) {return ""; }

  private:
    PSDPtr _psd;

    QMap<QString,SpectrumInterfaceMemberFn> _fnMap;

    QString change(QString &command);
    QString sampleRate(QString &);
    QString interleavedAverage(QString &);
    QString fftLength(QString &);
    QString apodize(QString &);
    QString removeMean(QString &);
    QString vectorUnints(QString &);
    QString rateUnits(QString &);
    QString apodizeFunctionIndex(QString &);
    QString gaussianSigma(QString &);
    QString outputTypeIndex(QString &);
    QString interpolateOverHoles(QString &);
};


}
#endif // DATAOBJECTSCRIPTINTERFACE_H
