/***************************************************************************
                   kstwidgets.h
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
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

#ifndef KSTWIDGETS_H
#define KSTWIDGETS_H

#include <QObject>
#include <QDesignerCustomWidgetInterface>

//FIXME Remove this eventually...
#include <kcomponentdata.h>

#include <QtPlugin>

class KstWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
public:
  KstWidgetPlugin(QObject *parent = 0) : QObject(parent), _initialized(false) {}

  QString group() const {
    return tr("Kst Widgets");
  }
  QString toolTip() const {
    return QString::null;
  }
  QString whatsThis() const {
    return QString::null;
  }

  QString instanceName() const {
    QChar camel = name().at(0).toLower();
    return name().replace(0,1,camel.toLower());
  }

  QString includeFile() const {
    return name().toLower() + ".h";
  }

  QString domXml() const {
    return QString::fromUtf8("<widget class=\"%1\" name=\"%2\"/>")
           .arg(name()).arg(instanceName().toLower());
  }

  bool isContainer() const {
    return false;
  }
  bool isInitialized() const {
    return _initialized;
  }
  QIcon icon() const {
    return QIcon();
  }

  void initialize(QDesignerFormEditorInterface *) {
    if (_initialized)
      return;

    _initialized = true;
  }

private:
  bool _initialized;
};


#include "curveappearancewidget.h"
class CurveAppearanceWidgetPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  CurveAppearanceWidgetPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("CurveAppearanceWidget");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new CurveAppearanceWidget(parent);
  }
};


#include "curveplacementwidget.h"
class CurvePlacementWidgetPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  CurvePlacementWidgetPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("CurvePlacementWidget");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new CurvePlacementWidget(parent);
  }
};


#include "editmultiplewidget.h"
class EditMultipleWidgetPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  EditMultipleWidgetPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("EditMultipleWidget");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new EditMultipleWidget(parent);
  }
};


#include "kstdatarange.h"
class KstDataRangePlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  KstDataRangePlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("KstDataRange");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new KstDataRange(parent);
  }
};


#include "kstfftoptions.h"
class KstFFTOptionsPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  KstFFTOptionsPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("KstFFTOptions");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new KstFFTOptions(parent);
  }
};


#include "matrixselector.h"
class MatrixSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  MatrixSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("MatrixSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new MatrixSelector(parent);
  }
};


#include "scalarselector.h"
class ScalarSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  ScalarSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("ScalarSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new ScalarSelector(parent);
  }
};


#include "stringselector.h"
class StringSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  StringSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("StringSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new StringSelector(parent);
  }
};


#include "vectorselector.h"
class VectorSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  VectorSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("VectorSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new VectorSelector(parent);
  }
};


#include "plotlistbox.h"
class PlotListBoxPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  PlotListBoxPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("PlotListBox");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new PlotListBox(parent);
  }
};


#include "vectorlistview.h"
class VectorListViewPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  VectorListViewPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("VectorListView");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new VectorListView(parent);
  }
};


#include "kstcombobox.h"
class KstComboBoxPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  KstComboBoxPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("KstComboBox");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new KstComboBox(parent);
  }
};


class KstWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
  KstWidgets(QObject *parent = 0);
  virtual ~KstWidgets() {}
  QList<QDesignerCustomWidgetInterface*> customWidgets() const {
    return _plugins;
  }

private:
  QList<QDesignerCustomWidgetInterface*> _plugins;
};
Q_EXPORT_PLUGIN2(kstwidgets, KstWidgets)


KstWidgets::KstWidgets(QObject *parent)
    : QObject(parent) {
  (void) new KComponentData("kstwidgets");
  _plugins.append(new CurveAppearanceWidgetPlugin(this));
  _plugins.append(new CurvePlacementWidgetPlugin(this));
  _plugins.append(new EditMultipleWidgetPlugin(this));
  _plugins.append(new KstDataRangePlugin(this));
  _plugins.append(new KstFFTOptionsPlugin(this));
  _plugins.append(new MatrixSelectorPlugin(this));
  _plugins.append(new ScalarSelectorPlugin(this));
  _plugins.append(new StringSelectorPlugin(this));
  _plugins.append(new VectorSelectorPlugin(this));
  _plugins.append(new PlotListBoxPlugin(this));
  _plugins.append(new VectorListViewPlugin(this));
  _plugins.append(new KstComboBoxPlugin(this));
}

#endif
// vim: ts=2 sw=2 et
