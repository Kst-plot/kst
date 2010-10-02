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

#include "basicpluginfactory.h"

#include "debug.h"
#include "basicplugin.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

BasicPluginFactory::BasicPluginFactory()
: ObjectFactory() {
  registerFactory(BasicPlugin::staticTypeTag, this);
}


BasicPluginFactory::~BasicPluginFactory() {
}


DataObjectPtr BasicPluginFactory::generateObject(ObjectStore *store, QXmlStreamReader& xml) {
  Q_ASSERT(store);

  DataObjectConfigWidget* configWidget;
  QString pluginName;
  BasicPluginPtr dataObject;
  bool validTag;

  while (!xml.atEnd()) {
    const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == BasicPlugin::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        pluginName = attrs.value("type").toString();
        Object::processShortNameIndexAttributes(attrs);

        configWidget = DataObject::pluginWidget(pluginName);
        if (configWidget) {
          if (!configWidget->configurePropertiesFromXml(store, attrs) ) {
            Debug::self()->log(QObject::tr("Error unable to create data object from plugin"), Debug::Warning);
            return 0;
          } else {
            if (xml.isEndElement() && n == BasicPlugin::staticTypeTag) {
              break;
            }
          }
        } else {
          Debug::self()->log(QObject::tr("Error unable to find plugin for data object"), Debug::Warning);
          return 0;
        }

        dataObject = kst_cast<BasicPlugin>(DataObject::createPlugin(pluginName, store, configWidget, false));

        QString expectedEnd;
        while (!(xml.isEndElement() && (xml.name().toString() == BasicPlugin::staticTypeTag))) {
          if (xml.isStartElement() && xml.name().toString() == "inputvector") {
            expectedEnd = xml.name().toString();
            attrs = xml.attributes();
            QString type = attrs.value("type").toString();
            QString tagName = attrs.value("tag").toString();
            VectorPtr vector = kst_cast<Vector>(store->retrieveObject(tagName));
            if (vector) {
              dataObject->setInputVector(type, vector);
            }
          } else if (xml.isStartElement() && xml.name().toString() == "inputscalar") {
            expectedEnd = xml.name().toString();
            attrs = xml.attributes();
            QString type = attrs.value("type").toString();
            QString tagName = attrs.value("tag").toString();
            ScalarPtr scalar = kst_cast<Scalar>(store->retrieveObject(tagName));
            if (scalar) {
              dataObject->setInputScalar(type, scalar);
            }
          } else if (xml.isStartElement() && xml.name().toString() == "inputstring") {
            expectedEnd = xml.name().toString();
            attrs = xml.attributes();
            QString type = attrs.value("type").toString();
            QString tagName = attrs.value("tag").toString();
            StringPtr string = kst_cast<String>(store->retrieveObject(tagName));
            if (string) {
              dataObject->setInputString(type, string);
            }
          } else if (xml.isStartElement() && xml.name().toString() == "outputvector") {
            expectedEnd = xml.name().toString();
            attrs = xml.attributes();
            QString type = attrs.value("type").toString();
            QString tagName = attrs.value("tag").toString();
            dataObject->setOutputVector(type, tagName);
          } else if (xml.isStartElement() && xml.name().toString() == "outputscalar") {
            expectedEnd = xml.name().toString();
            attrs = xml.attributes();
            QString type = attrs.value("type").toString();
            QString tagName = attrs.value("tag").toString();
            dataObject->setOutputScalar(type, tagName);
          } else if (xml.isStartElement() && xml.name().toString() == "outputstring") {
            expectedEnd = xml.name().toString();
            attrs = xml.attributes();
            QString type = attrs.value("type").toString();
            QString tagName = attrs.value("tag").toString();
            dataObject->setOutputString(type, tagName);
          } else if (xml.isEndElement()) {
            if (xml.name().toString() != expectedEnd) {
              validTag = false;
              break;
            }
          }
          xml.readNext();
        }
      } else {
        return 0;
      }
    }
    if (xml.isEndElement()) {
      if (n == BasicPlugin::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Plugin Object from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  dataObject->writeLock();
  dataObject->registerChange();
  dataObject->unlock();

  return dataObject;
}

}

// vim: ts=2 sw=2 et
