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

#include "kstwidgets.h"

KstWidgets::KstWidgets(QObject *parent)
    : QObject(parent) {
  _plugins.append(new ColorButtonPlugin(this));
  _plugins.append(new GradientEditorPlugin(this));
  _plugins.append(new FileRequesterPlugin(this));
  _plugins.append(new ComboBoxPlugin(this));
  _plugins.append(new DataRangePlugin(this));
  _plugins.append(new VectorSelectorPlugin(this));
  _plugins.append(new MatrixSelectorPlugin(this));
  _plugins.append(new ScalarSelectorPlugin(this));
  _plugins.append(new StringSelectorPlugin(this));
  _plugins.append(new CurvePlacementPlugin(this));
  _plugins.append(new CurveAppearancePlugin(this));
}


KstWidgets::~KstWidgets() {
}


KstWidgetPlugin::KstWidgetPlugin(QObject *parent)
  : QObject(parent), _initialized(false) {
}


KstWidgetPlugin::~KstWidgetPlugin() {
}


QString KstWidgetPlugin::group() const {
  return tr("Kst Widgets");
}


QString KstWidgetPlugin::toolTip() const {
  return QString::null;
}


QString KstWidgetPlugin::whatsThis() const {
  return QString::null;
}


QString KstWidgetPlugin::instanceName() const {
  QString name = static_cast<const QDesignerCustomWidgetInterface*>(this)->name().replace("Kst::", "");
  QChar camel = name.at(0).toLower();
  return name.replace(0,1,camel.toLower());
}


QString KstWidgetPlugin::includeFile() const {
  return instanceName().toLower() + ".h";
}


QString KstWidgetPlugin::domXml() const {
  QString name = static_cast<const QDesignerCustomWidgetInterface*>(this)->name();
  return QString::fromUtf8("<widget class=\"%1\" name=\"%2\"/>")
          .arg(name).arg(instanceName());
}


bool KstWidgetPlugin::isContainer() const {
  return false;
}


bool KstWidgetPlugin::isInitialized() const {
  return _initialized;
}


QIcon KstWidgetPlugin::icon() const {
  return QIcon();
}


void KstWidgetPlugin::initialize(QDesignerFormEditorInterface *) {
  if (_initialized)
    return;

  _initialized = true;
}

Q_EXPORT_PLUGIN2(kstwidgets, KstWidgets)

// vim: ts=2 sw=2 et
