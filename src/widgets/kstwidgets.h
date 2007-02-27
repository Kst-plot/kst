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

#include "curveappearancewidget.h"

class CurveAppearanceWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
  CurveAppearanceWidgetPlugin(QObject *parent = 0) : QObject(parent), _initialized(false) {}

  bool isContainer() const {
    return false;
  }
  bool isInitialized() const {
    return _initialized;
  };
  QIcon icon() const { return QIcon(); }

  QString domXml() const {
       return "<widget class=\"CurveAppearanceWidget\" name=\"curveAppearanceWidget\">\n"
              "</widget>\n";
  }

  QString group() const { return "Kst Widgets"; }
  QString includeFile() const { return "curveappearancewidget.ui.h"; }
  QString name() const { return "CurveAppearanceWidget"; }
  QString toolTip() const { return ""; }
  QString whatsThis() const { return ""; }

  QWidget *createWidget(QWidget *parent) {
    return new CurveAppearanceWidget(parent);
  }

  void initialize(QDesignerFormEditorInterface *) {
    if (_initialized)
      return;

    _initialized = true;
  }

private:
  bool _initialized;
};
Q_EXPORT_PLUGIN2(curveappearancewidgetplugin, CurveAppearanceWidgetPlugin)

#endif
// vim: ts=2 sw=2 et
