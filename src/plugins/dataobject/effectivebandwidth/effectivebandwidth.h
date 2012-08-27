/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EFFECTIVEBANDWIDTHPLUGIN_H
#define EFFECTIVEBANDWIDTHPLUGIN_H

#include <QFile>

#include <basicplugin.h>
#include <dataobjectplugin.h>

class EffectiveBandwidthSource : public Kst::BasicPlugin {
  Q_OBJECT

  public:
    virtual QString _automaticDescriptiveName() const;

    Kst::VectorPtr vectorX() const;
    Kst::VectorPtr vectorY() const;
    Kst::ScalarPtr scalarMin() const;
    Kst::ScalarPtr scalarFreq() const;
    Kst::ScalarPtr scalarK() const;

    virtual void change(Kst::DataObjectConfigWidget *configWidget);

    void setupOutputs();
    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;

    virtual void saveProperties(QXmlStreamWriter &s);

  protected:
    EffectiveBandwidthSource(Kst::ObjectStore *store);
    ~EffectiveBandwidthSource();

  friend class Kst::ObjectStore;


};


class EffectiveBandwidthPlugin : public QObject, public Kst::DataObjectPluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataObjectPluginInterface)
#ifdef QT5
    Q_PLUGIN_METADATA(IID "com.kst.DataObjectPluginInterface/2.0")
#endif
  public:
    virtual ~EffectiveBandwidthPlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual DataObjectPluginInterface::PluginTypeID pluginType() const { return Generic; }

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataObject *create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true) const;

    virtual Kst::DataObjectConfigWidget *configWidget(QSettings *settingsObject) const;
};

#endif
// vim: ts=2 sw=2 et
