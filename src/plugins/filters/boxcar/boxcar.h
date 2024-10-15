/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2024 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BOXCARPLUGIN_H
#define BOXCARPLUGIN_H

#include <QFile>

#include <basicplugin.h>
#include <dataobjectplugin.h>

class BoxcarSource : public Kst::BasicPlugin {
  Q_OBJECT

  public:
    virtual QString _automaticDescriptiveName() const;

    virtual QString descriptionTip() const;

    Kst::VectorPtr vector() const;
    Kst::ScalarPtr scalarSamples() const;

    virtual void change(Kst::DataObjectConfigWidget *configWidget);

    void setupOutputs();
    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;

    virtual void setProperty(const QString &key, const QString &val);

    virtual void saveProperties(QXmlStreamWriter &s);

    // bool weighted() const { return _weighted; }
    // void setWeighted(bool value) { _weighted = value; }
    int stages() const {return _stages;}
    void setStages(int stages) { if (stages < 1) _stages = 1; else if (stages>100) _stages=100; else _stages=stages;}
    double sampleRate() const {return _sample_rate;}
    void setSampleRate(double sample_rate) { if (sample_rate <= 0.0) _sample_rate = 1.0; else _sample_rate=sample_rate;}

  protected:
    BoxcarSource(Kst::ObjectStore *store);
    ~BoxcarSource();
    // bool _weighted;
    // bool _accuracy;
    int _stages;
    double _sample_rate;

  private:
    void SingleStageBoxcar(double *v_out, const double *v_in, int vec_len, int box_len);

  friend class Kst::ObjectStore;


};


class BoxcarPlugin : public QObject, public Kst::DataObjectPluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataObjectPluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataObjectPluginInterface/2.0")
  public:
    virtual ~BoxcarPlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual DataObjectPluginInterface::PluginTypeID pluginType() const { return Filter; }

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataObject *create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true) const;

    virtual Kst::DataObjectConfigWidget *configWidget(QSettings *settingsObject) const;
};

#endif
// vim: ts=2 sw=2 et
