/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "widgets.h"

namespace Kst {

Widgets::Widgets(QObject *parent)
    : QObject(parent) {
  _plugins.append(new ColorButtonPlugin(this));
  _plugins.append(new DataSourceSelectorPlugin(this));
  _plugins.append(new GradientEditorPlugin(this));
  _plugins.append(new FileRequesterPlugin(this));
  _plugins.append(new ComboBoxPlugin(this));
  _plugins.append(new DataRangePlugin(this));
  _plugins.append(new CurveSelectorPlugin(this));
  _plugins.append(new VectorSelectorPlugin(this));
  _plugins.append(new MatrixSelectorPlugin(this));
  _plugins.append(new ScalarSelectorPlugin(this));
  _plugins.append(new StringSelectorPlugin(this));
  _plugins.append(new CurvePlacementPlugin(this));
  _plugins.append(new CurveAppearancePlugin(this));
  _plugins.append(new FFTOptionsPlugin(this));
  _plugins.append(new ColorPalettePlugin(this));
  _plugins.append(new LabelBuilderPlugin(this));
  _plugins.append(new LabelLineEditPlugin(this));
}


Widgets::~Widgets() {
}


WidgetPlugin::WidgetPlugin(QObject *parent)
  : QObject(parent), _initialized(false) {
}


WidgetPlugin::~WidgetPlugin() {
}


QString WidgetPlugin::group() const {
  return tr("Kst Widgets");
}


QString WidgetPlugin::toolTip() const {
  return QString::null;
}


QString WidgetPlugin::whatsThis() const {
  return QString::null;
}


QString WidgetPlugin::instanceName() const {
  QString name = static_cast<const QDesignerCustomWidgetInterface*>(this)->name().replace("", "");
  QChar camel = name.at(0).toLower();
  return name.replace(0,1,camel.toLower());
}


QString WidgetPlugin::includeFile() const {
  return instanceName().toLower() + ".h";
}


QString WidgetPlugin::domXml() const {
  QString name = static_cast<const QDesignerCustomWidgetInterface*>(this)->name();
  return QString::fromUtf8("<widget class=\"%1\" name=\"%2\"/>")
          .arg(name).arg(instanceName());
}


bool WidgetPlugin::isContainer() const {
  return false;
}


bool WidgetPlugin::isInitialized() const {
  return _initialized;
}


QIcon WidgetPlugin::icon() const {
  return QIcon();
}


void WidgetPlugin::initialize(QDesignerFormEditorInterface *) {
  if (_initialized)
    return;

  _initialized = true;
}

Q_EXPORT_PLUGIN2(widgets, Widgets)

}

// vim: ts=2 sw=2 et
