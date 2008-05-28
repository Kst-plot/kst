/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SAMPLEPLUGIN_H
#define SAMPLEPLUGIN_H

#include <QFile>

#include <basicplugin.h>
#include <dataobjectplugin.h>

class SamplePluginSource : public Kst::BasicPlugin {
  Q_OBJECT

  public:
    SamplePluginSource(Kst::ObjectStore *store, const Kst::ObjectTag& tag);

    ~SamplePluginSource();

    virtual QString _automaticDescriptiveName() const;

    Kst::VectorPtr vector() const;

    virtual void change(Kst::DataObjectConfigWidget *configWidget);

    void setupOutputs();
    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;
};


class SamplePlugin : public QObject, public Kst::DataObjectPluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataObjectPluginInterface)
  public:
    virtual ~SamplePlugin() {}

    virtual QString pluginName() const;

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataObject *create(Kst::ObjectStore *store,
                               Kst::ObjectTag &tag, Kst::DataObjectConfigWidget *configWidget) const;

    virtual Kst::DataObjectConfigWidget *configWidget() const;
};

#endif
// vim: ts=2 sw=2 et
