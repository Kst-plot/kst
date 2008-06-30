/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2006 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASICPLUGIN_H
#define BASICPLUGIN_H

#include "dataobject.h"
#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT BasicPlugin : public DataObject {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }
    static const QString staticTypeTag;

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

    //Pure virtual methods inherited from DataObject
    //This _must_ equal the 'Name' entry in the .desktop file of
    //the plugin
    QString propertyString() const { return name(); } //no longer virtual

    //Provide an impl...
    virtual DataObjectPtr makeDuplicate();

    virtual QString descriptionTip() const;

  public slots:
    //Pure virtual slots from DataObject
    //Each plugin can provide an implementation or use the default
    virtual void showNewDialog();
    virtual void showEditDialog();

  public:
    virtual void change(DataObjectConfigWidget *configWidget) = 0;

    //Returns the respective input object for name
    VectorPtr inputVector(const QString& name) const;
    ScalarPtr inputScalar(const QString& name) const;
    StringPtr inputString(const QString& name) const;

    //Returns the respective output object for name
    VectorPtr outputVector(const QString& name) const;
    ScalarPtr outputScalar(const QString& name) const;
    StringPtr outputString(const QString& name) const;

    void setInputVector(const QString &type, VectorPtr ptr);
    void setInputScalar(const QString &type, ScalarPtr ptr);
    void setInputString(const QString &type, StringPtr ptr);
    void setOutputVector(const QString &type, const QString &name);
    void setOutputScalar(const QString &type, const QString &name);
    void setOutputString(const QString &type, const QString &name);

    void setPluginName(const QString &pluginName);
    QString pluginName() { return _pluginName; }

    //Pure virtual methods inherited from DataObject
    //We do this one ourselves for benefit of all plugins...
    Object::UpdateType update();

    //Regular virtual methods from DataObject
    virtual void save(QXmlStreamWriter &s);
    virtual void saveProperties(QXmlStreamWriter &s);

    bool isFit() const { return _isFit; }
    // FIXME: remove this
    void createFitScalars();
    QString label(int precision) const;

  protected:
    BasicPlugin(ObjectStore *store);
    virtual ~BasicPlugin();

    virtual QString parameterName(int index) const;
    bool _isFit;

  private:
    bool inputsExist() const;
    bool updateInput(bool force) const;
    void updateOutput() const;

    QString _pluginName;
};

typedef SharedPtr<BasicPlugin> BasicPluginPtr;
typedef ObjectList<BasicPlugin> BasicPluginList;

}

#endif

// vim: ts=2 sw=2 et
