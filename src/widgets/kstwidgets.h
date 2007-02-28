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

//widgets supplied...

//CurveAppearanceWidget
//CurvePlacementWidget
//EditMultipleWidget
//KstDataRange
//KstFFTOptions
//MatrixSelector
//PlotListBox
//VectorListView
//ScalarSelector
//StringSelector
//VectorSelector
//KstComboBox

#include <QObject>
#include <QDesignerCustomWidgetInterface>

#include <QtPlugin>

class KstWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
public:
  KstWidgetPlugin(QObject *parent = 0) : QObject(parent), _initialized(false) {}

  QString group() const { return tr("Kst Widgets"); }
  QString toolTip() const { return ""; }
  QString whatsThis() const { return ""; }

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

  bool isContainer() const { return false; }
  bool isInitialized() const { return _initialized; }
  QIcon icon() const { return QIcon(); }

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
  QString name() const { return "CurveAppearanceWidget"; } //do not translate
  QWidget *createWidget(QWidget *parent) { return new CurveAppearanceWidget(parent); }
};
Q_EXPORT_PLUGIN2(curveappearancewidgetplugin, CurveAppearanceWidgetPlugin)

// #include "curveplacementwidget.h"
// class CurvePlacementWidgetPlugin : public KstWidgetPlugin {
//   Q_OBJECT
//   Q_INTERFACES(QDesignerCustomWidgetInterface)
// public:
//   CurvePlacementWidgetPlugin(QObject *parent = 0) : KstWidgetPlugin(parent) {}
//   QString name() const { return "CurvePlacementWidget"; } //do not translate
//   QWidget *createWidget(QWidget *parent) { return new CurvePlacementWidget(parent); }
// };
// Q_EXPORT_PLUGIN2(curveplacementwidgetplugin, CurvePlacementWidgetPlugin)

#endif
// vim: ts=2 sw=2 et
