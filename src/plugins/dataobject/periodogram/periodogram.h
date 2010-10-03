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

#ifndef PERIODOGRAMPLUGIN_H
#define PERIODOGRAMPLUGIN_H

#include <QFile>

#include <basicplugin.h>
#include <dataobjectplugin.h>

class PeriodogramSource : public Kst::BasicPlugin {
  Q_OBJECT

  public:
    virtual QString _automaticDescriptiveName() const;

    Kst::VectorPtr vectorTime() const;
    Kst::VectorPtr vectorData() const;
    Kst::ScalarPtr scalarOversampling() const;
    Kst::ScalarPtr scalarANFF() const;

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
    PeriodogramSource(Kst::ObjectStore *store);
    ~PeriodogramSource();

  private:
    int max(int a, int b);
    int min(int a, int b);
    void spread(double y, double yy[], unsigned long n, double x, int m);
    void four1(double data[], unsigned long nn, int isign);
    void realft(double data[], unsigned long n, int isign);
    void avevar(double const  data[], unsigned long n, double* ave, double* var);
    void FastLombPeriodogram(
      double const x[],
      double const y[],
      unsigned long   n,
      double          ofac,
      double          hifac,
      double          wk1[],
      double          wk2[],
      unsigned long   ndim,
      unsigned long*  nout,
      unsigned long*  jmax,
      double*         prob,
      double*         pvar,
      int            iIsWindowFunction);
    void SlowLombPeriodogram(
      double const    x[],
      double const    y[],
      unsigned long   n,
      double          ofac,
      double          hifac,
      double          px[],
      double          py[],
      unsigned long   ndim,
      unsigned long*  nout,
      unsigned long*  jmax,
      double*         prob,
      double*         pvar,
      int            iIsWindowFunction);

  friend class Kst::ObjectStore;


};


class PeriodogramPlugin : public QObject, public Kst::DataObjectPluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataObjectPluginInterface)
  public:
    virtual ~PeriodogramPlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual DataObjectPluginInterface::PluginTypeID pluginType() const { return Generic; }

    virtual bool hasConfigWidget() const { return true; }

    virtual Kst::DataObject *create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs = true) const;

    virtual Kst::DataObjectConfigWidget *configWidget(QSettings *settingsObject) const;
};

#endif
// vim: ts=2 sw=2 et
