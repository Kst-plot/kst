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

#ifndef KSTWIDGETS_H
#define KSTWIDGETS_H

#include <QObject>
#include <QDesignerCustomWidgetInterface>

#include <QtPlugin>

class KstWidgets : public QObject, public QDesignerCustomWidgetCollectionInterface {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
  KstWidgets(QObject *parent = 0);
  virtual ~KstWidgets();
  QList<QDesignerCustomWidgetInterface*> customWidgets() const {
    return _plugins;
  }

private:
  QList<QDesignerCustomWidgetInterface*> _plugins;
};

class KstWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
public:
  KstWidgetPlugin(QObject *parent = 0);
  virtual ~KstWidgetPlugin();

  QString group() const;
  QString toolTip() const;
  QString whatsThis() const;
  QString instanceName() const;
  QString includeFile() const;
  QString domXml() const;
  bool isContainer() const;
  bool isInitialized() const;
  QIcon icon() const;
  void initialize(QDesignerFormEditorInterface *);

private:
  bool _initialized;
};

#include "colorbutton.h"
class ColorButtonPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  ColorButtonPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::ColorButton");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::ColorButton(parent);
  }
};

#include "gradienteditor.h"
class GradientEditorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  GradientEditorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::GradientEditor");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::GradientEditor(parent);
  }
};

#include "filerequester.h"
class FileRequesterPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  FileRequesterPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::FileRequester");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::FileRequester(parent);
  }
};

#include "combobox.h"
class ComboBoxPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  ComboBoxPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::ComboBox");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::ComboBox(parent);
  }
};

#include "datarange.h"
class DataRangePlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  DataRangePlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::DataRange");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::DataRange(parent);
  }
};

#include "vectorselector.h"
class VectorSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  VectorSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::VectorSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::VectorSelector(parent);
  }
};

#include "matrixselector.h"
class MatrixSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  MatrixSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::MatrixSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::MatrixSelector(parent);
  }
};

#include "scalarselector.h"
class ScalarSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  ScalarSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::ScalarSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::ScalarSelector(parent);
  }
};

#include "stringselector.h"
class StringSelectorPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  StringSelectorPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::StringSelector");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::StringSelector(parent);
  }
};

#include "curveappearancewidget.h"
class CurveAppearancePlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  CurveAppearancePlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::CurveAppearance");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::CurveAppearanceWidget(parent);
  }
};

#include "curveplacementwidget.h"
class CurvePlacementPlugin : public KstWidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
  CurvePlacementPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
  QString name() const {
    return QLatin1String("Kst::CurvePlacement");
  } //do not translate
  QWidget *createWidget(QWidget *parent) {
    return new Kst::CurvePlacementWidget(parent);
  }
};

#endif

// vim: ts=2 sw=2 et
