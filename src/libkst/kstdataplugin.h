/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "kstsharedptr.h"
#include "datasource.h"

#include <QtPlugin>
#include <QSettings>

class KstPluginInterface : public KstShared {
  public:
    KstPluginInterface() {}

    virtual ~KstPluginInterface() {}

    virtual QString pluginName() const { return QString::null; }

    virtual bool hasConfigWidget() const { return false; }
};


class KstDataSourcePluginInterface : public KstPluginInterface {
  public:
    virtual ~KstDataSourcePluginInterface() {}

    virtual Kst::DataSource *create(QSettings *cfg,
                                  const QString &filename,
                                  const QString &type,
                                  const QDomElement &element) const = 0;

    virtual QStringList matrixList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList fieldList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual int understands(QSettings *cfg, const QString& filename) const = 0;

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const = 0;

    virtual QStringList provides() const = 0;

    bool provides(const QString& type) const { return provides().contains(type); }

    virtual Kst::DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const = 0;
};


class KstDataObjectPluginInterface : public KstPluginInterface {
  public:
    virtual ~KstDataObjectPluginInterface() {}

    virtual QWidget *configWidget(const QString& name) const = 0;
};


class KstBasicPluginInterface : public KstDataObjectPluginInterface {
  public:
    virtual ~KstBasicPluginInterface() {}

    virtual QStringList inputVectorList() const = 0;

    virtual QStringList inputScalarList() const = 0;

    virtual QStringList inputStringList() const = 0;

    virtual QStringList outputVectorList() const = 0;

    virtual QStringList outputScalarList() const = 0;

    virtual QStringList outputStringList() const = 0;
};

Q_DECLARE_INTERFACE(KstPluginInterface, "com.kst.KstPluginInterface/1.0")
Q_DECLARE_INTERFACE(KstDataSourcePluginInterface, "com.kst.KstDataSourcePluginInterface/1.0")
Q_DECLARE_INTERFACE(KstDataObjectPluginInterface, "com.kst.KstDataObjectPluginInterface/1.0")
Q_DECLARE_INTERFACE(KstBasicPluginInterface, "com.kst.KstBasicPluginInterface/1.0")

typedef QList<KstSharedPtr<KstPluginInterface> > KstPluginList;
