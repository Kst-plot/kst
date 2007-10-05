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

#ifndef WIDGETS_H
#define WIDGETS_H

#include <QObject>
#include <QDesignerCustomWidgetInterface>

#include <QtPlugin>

#include "colorbutton.h"
#include "gradienteditor.h"
#include "filerequester.h"
#include "combobox.h"
#include "datarange.h"
#include "vectorselector.h"
#include "matrixselector.h"
#include "scalarselector.h"
#include "stringselector.h"
#include "curveappearance.h"
#include "curveplacement.h"

namespace Kst {

class Widgets : public QObject, public QDesignerCustomWidgetCollectionInterface {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
  public:
    Widgets(QObject *parent = 0);
    virtual ~Widgets();
    QList<QDesignerCustomWidgetInterface*> customWidgets() const {
      return _plugins;
    }

  private:
    QList<QDesignerCustomWidgetInterface*> _plugins;
};

class WidgetPlugin : public QObject, public QDesignerCustomWidgetInterface {
  public:
    WidgetPlugin(QObject *parent = 0);
    virtual ~WidgetPlugin();

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

class ColorButtonPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    ColorButtonPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ColorButton");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ColorButton(parent);
    }
};

class GradientEditorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    GradientEditorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("GradientEditor");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new GradientEditor(parent);
    }
};

class FileRequesterPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    FileRequesterPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("FileRequester");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new FileRequester(parent);
    }
};

class ComboBoxPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    ComboBoxPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ComboBox");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ComboBox(parent);
    }
};

class DataRangePlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    DataRangePlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("DataRange");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new DataRange(parent);
    }
};

class VectorSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    VectorSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("VectorSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new VectorSelector(parent);
    }
};

class MatrixSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    MatrixSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("MatrixSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new MatrixSelector(parent);
    }
};

class ScalarSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    ScalarSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("ScalarSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new ScalarSelector(parent);
    }
};

class StringSelectorPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    StringSelectorPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("StringSelector");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new StringSelector(parent);
    }
};

class CurveAppearancePlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    CurveAppearancePlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("CurveAppearance");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new CurveAppearance(parent);
    }
};

class CurvePlacementPlugin : public WidgetPlugin {
  Q_OBJECT
  Q_INTERFACES(QDesignerCustomWidgetInterface)
  public:
    CurvePlacementPlugin(QObject *parent = 0) : WidgetPlugin(parent) {}
    QString name() const {
      return QLatin1String("CurvePlacement");
    } //do not translate
    QWidget *createWidget(QWidget *parent) {
      return new CurvePlacement(parent);
    }
};

}

#endif

// vim: ts=2 sw=2 et
