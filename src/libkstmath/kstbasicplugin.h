/***************************************************************************
                                 kstbasicplugin.h
                             -------------------
    begin                : 09/15/06
    copyright            : (C) 2006 The University of Toronto
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

#ifndef KSTBASICPLUGIN_H
#define KSTBASICPLUGIN_H

#include "kstdataobject.h"
#include "kst_export.h"

class KST_EXPORT KstBasicPlugin : public KstDataObject {
  public:
    KstBasicPlugin();
    KstBasicPlugin(const QDomElement &e);
    virtual ~KstBasicPlugin();

    //The implementation of the algorithm the plugin provides.
    //Operates on the inputVectors, inputScalars, and inputStrings
    //to produce the outputVectors, outputScalars, and outputStrings.
    virtual bool algorithm() = 0;

    //String lists of the names of the expected inputs.
    virtual QStringList inputVectorList() const = 0;
    virtual QStringList inputScalarList() const = 0;
    virtual QStringList inputStringList() const = 0;
    //String lists of the names of the expected outputs.
    virtual QStringList outputVectorList() const = 0;
    virtual QStringList outputScalarList() const = 0;
    virtual QStringList outputStringList() const = 0;

    //Pure virtual methods inherited from KstDataObject
    //This _must_ equal the 'Name' entry in the .desktop file of
    //the plugin
    QString propertyString() const { return name(); } //no longer virtual

    //Provide an impl...
    virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap&);

  public slots:
    //Pure virtual slots from KstDataObject
    //Each plugin can provide an implementation or use the default
    virtual void showNewDialog();
    virtual void showEditDialog();

  public:
    //Returns the respective input object for name
    KstVectorPtr inputVector(const QString& name) const;
    KstScalarPtr inputScalar(const QString& name) const;
    KstStringPtr inputString(const QString& name) const;

    //Returns the respective output object for name
    KstVectorPtr outputVector(const QString& name) const;
    KstScalarPtr outputScalar(const QString& name) const;
    KstStringPtr outputString(const QString& name) const;

    void setInputVector(const QString &type, KstVectorPtr ptr);
    void setInputScalar(const QString &type, KstScalarPtr ptr);
    void setInputString(const QString &type, KstStringPtr ptr);
    void setOutputVector(const QString &type, const QString &name);
    void setOutputScalar(const QString &type, const QString &name);
    void setOutputString(const QString &type, const QString &name);

    //Pure virtual methods inherited from KstDataObject
    //We do this one ourselves for benefit of all plugins...
    KstObject::UpdateType update(int updateCounter = -1);

    //Regular virtual methods from KstDataObject
    void load(const QDomElement &e);
    void save(QTextStream& ts, const QString& indent = QString::null);

    bool isFit() const { return _isFit; }
    // FIXME: remove this
    void createFitScalars();
    QString label(int precision) const;

  protected:
    virtual QString parameterName(int index) const;
    bool _isFit;

  private:
    bool inputsExist() const;
    bool updateInput(int updateCounter, bool force) const;
    void updateOutput(int updateCounter) const;
};

typedef KstSharedPtr<KstBasicPlugin> KstBasicPluginPtr;
typedef KstObjectList<KstBasicPluginPtr> KstBasicPluginList;

#endif

// vim: ts=2 sw=2 et
