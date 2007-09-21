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
    return name().replace("Kst::", "").toLower() + ".h";
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

#endif
// vim: ts=2 sw=2 et
