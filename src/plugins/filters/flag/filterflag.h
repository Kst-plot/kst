/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2016 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILTERFLAG_H
#define FILTERFLAG_H

#include <QFile>

#include <basicplugin.h>
#include <dataobjectplugin.h>

class FilterFlagSource : public Kst::BasicPlugin {
  Q_OBJECT

  public:
    virtual QString _automaticDescriptiveName() const;

    virtual QString descriptionTip() const;

    Kst::VectorPtr vector() const;
    Kst::VectorPtr flagVector() const;

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

    unsigned long long mask() {return _mask;}
    void setMask(unsigned long long mask) {_mask = mask;}

    bool validIsZero() {return _validIsZero;}
    void setValidIsZero(bool valid_is_zero) {_validIsZero = valid_is_zero;}

  protected:
    FilterFlagSource(Kst::ObjectStore *store);
    ~FilterFlagSource();
    unsigned long long _mask;
    bool _validIsZero;

  friend class Kst::ObjectStore;


};


class FilterFlagPlugin : public QObject, public Kst::DataObjectPluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataObjectPluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataObjectPluginInterface/2.0")
  public:
    virtual ~FilterFlagPlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual DataObjectPluginInterface::PluginTypeID pluginType() const { return Filter; }

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataObject *create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true) const;

    virtual Kst::DataObjectConfigWidget *configWidget(QSettings *settingsObject) const;
};

#endif // FILTERFLAG_H
// vim: ts=2 sw=2 et


