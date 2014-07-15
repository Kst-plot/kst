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

#ifndef VECTORSCRIPTINTERFACE_H
#define VECTORSCRIPTINTERFACE_H

#include <QString>

#include "scriptinterface.h"
#include "datavector.h"
#include "generatedvector.h"
#include "editablevector.h"

namespace Kst {

/*******************/
class KSTCORE_EXPORT VectorCommonSI : public ScriptInterface
{
    Q_OBJECT

  public:
    QString value(QString& command);
    QString length(QString&);
    QString min(QString&);
    QString max(QString&);
    QString mean(QString&);
    QString descriptionTip(QString&);
    QString store(QString &command);

  protected:
    VectorPtr _vector;
    QString noSuchFn(QString&) {return ""; }

};


/*******************/
class VectorSI;
typedef QString (VectorSI::*VectorInterfaceMemberFn)(QString& command);

class KSTCORE_EXPORT VectorSI : public VectorCommonSI
{
    Q_OBJECT
public:
    explicit VectorSI(VectorPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newVector(ObjectStore *);

  private:
    QMap<QString,VectorInterfaceMemberFn> _fnMap;
};


/*******************/
class DataVectorSI;
typedef QString (DataVectorSI::*DataVectorInterfaceMemberFn)(QString& command);

class KSTCORE_EXPORT DataVectorSI : public VectorCommonSI
{
    Q_OBJECT
public:
    explicit DataVectorSI(DataVectorPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newVector(ObjectStore *store);

    QString change(QString &command);
    QString field(QString &command);
    QString filename(QString &command);
    QString start(QString &command);
    QString NFrames(QString &command);
    QString skip(QString &command);
    QString boxcarFirst(QString &command);

private:
    DataVectorPtr _datavector;
    QMap<QString,DataVectorInterfaceMemberFn> _fnMap;
};

/*******************/
class GeneratedVectorSI;
typedef QString (GeneratedVectorSI::*GeneratedVectorInterfaceMemberFn)(QString& command);

class KSTCORE_EXPORT GeneratedVectorSI : public VectorCommonSI
{
    Q_OBJECT
public:
    explicit GeneratedVectorSI(GeneratedVectorPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newVector(ObjectStore *store);

    QString change(QString &command);

  private:
    GeneratedVectorPtr _generatedvector;
    QMap<QString,GeneratedVectorInterfaceMemberFn> _fnMap;
};


/*******************/
class EditableVectorSI;
typedef QString (EditableVectorSI::*EditableVectorInterfaceMemberFn)(QString& command);

class KSTCORE_EXPORT EditableVectorSI : public VectorCommonSI
{
    Q_OBJECT
public:
    explicit EditableVectorSI(EditableVectorPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newVector(ObjectStore *);

    QString load(QString &command);
    QString setValue(QString &command);
    QString resize(QString &command);
    QString zero(QString &);

  private:
    EditableVectorPtr _editablevector;
    QMap<QString,EditableVectorInterfaceMemberFn> _fnMap;

};

}
#endif // VECTORSCRIPTINTERFACE_H
