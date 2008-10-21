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

#ifndef PHASEPLUGIN_H
#define PHASEPLUGIN_H

#include <QFile>

#include <basicplugin.h>
#include <dataobjectplugin.h>

class PhaseSource : public Kst::BasicPlugin {
  Q_OBJECT

  public:
    virtual QString _automaticDescriptiveName() const;

    Kst::VectorPtr vectorTime() const;
    Kst::VectorPtr vectorData() const;
    Kst::ScalarPtr scalarPeriod() const;
    Kst::ScalarPtr scalarZeroPhase() const;

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
    PhaseSource(Kst::ObjectStore *store);
    ~PhaseSource();

  private:
    void swap(double* pData[], int iOne, int iTwo);
    void quicksort(double* pData[], int iLeft, int iRight);

  friend class Kst::ObjectStore;


};


class PhasePlugin : public QObject, public Kst::DataObjectPluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataObjectPluginInterface)
  public:
    virtual ~PhasePlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual DataObjectPluginInterface::PluginTypeID pluginType() const { return Generic; }

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataObject *create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true) const;

    virtual Kst::DataObjectConfigWidget *configWidget(QSettings *settingsObject) const;
};

#endif
// vim: ts=2 sw=2 et
