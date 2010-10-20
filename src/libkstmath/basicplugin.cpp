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

#include <stdlib.h>

#include <QXmlStreamWriter>

#ifndef Q_WS_WIN32
#include <unistd.h>
#endif

#include "kst_i18n.h"

#include "debug.h"
#include "basicplugin.h"
#include "dialoglauncher.h"
#include "datacollection.h"
#include "objectstore.h"
#include "dataobjectplugin.h"

namespace Kst {

const QString BasicPlugin::staticTypeString = I18N_NOOP("Plugin");
const QString BasicPlugin::staticTypeTag = I18N_NOOP("plugin");

BasicPlugin::BasicPlugin(ObjectStore *store)
: DataObject(store) {
  _typeString = i18n("Plugin");
  _type = "Plugin";

  _initializeShortName();
}


BasicPlugin::~BasicPlugin() {
}

void BasicPlugin::_initializeShortName() {
  _shortName = 'P'+QString::number(_pnum);
  if (_pnum>max_pnum)
    max_pnum = _pnum;
  _pnum++;

}


void BasicPlugin::setPluginName(const QString &pluginName) {
  _pluginName = pluginName;
}


DataObjectPtr BasicPlugin::makeDuplicate() const {
#if 0
  BasicPluginPtr plugin = kst_cast<BasicPlugin>(DataObject::createPlugin(propertyString()));

  // use same inputs
  for (VectorMap::ConstIterator iter = _inputVectors.begin(); iter != _inputVectors.end(); ++iter) {
    plugin->inputVectors().insert(iter.key(), iter.value());
  }
  for (ScalarMap::ConstIterator iter = _inputScalars.begin(); iter != _inputScalars.end(); ++iter) {
    plugin->inputScalars().insert(iter.key(), iter.value());
  }
  for (StringMap::ConstIterator iter = _inputStrings.begin(); iter != _inputStrings.end(); ++iter) {
    plugin->inputStrings().insert(iter.key(), iter.value());
  }

  // create new outputs
  for (VectorMap::ConstIterator iter = outputVectors().begin(); iter != outputVectors().end(); ++iter) {
    KstWriteLocker blockVectorUpdates(&vectorList.lock());
    VectorPtr v = new Vector(ObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), 0, plugin.data()); // FIXME: unique tag generation
    plugin->outputVectors().insert(iter.key(), v);
  }
  for (ScalarMap::ConstIterator iter = outputScalars().begin(); iter != outputScalars().end(); ++iter) {
    ScalarPtr s = new Scalar(ObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputScalars().insert(iter.key(), s);
  }
  for (StringMap::ConstIterator iter = outputStrings().begin(); iter != outputStrings().end(); ++iter) {
    StringPtr s = new String(ObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputStrings().insert(iter.key(), s);
  }

  // set the same plugin
  plugin->setTagName(ObjectTag(tag().tag() + "'", tag().context())); // FIXME: unique tag generation method
  map.insert(this, DataObjectPtr(plugin));
  return DataObjectPtr(plugin);
#endif
  // FIXME: implement this
  return 0L;
}

void BasicPlugin::showNewDialog() {
  DialogLauncher::self()->showBasicPluginDialog(_pluginName);
}


void BasicPlugin::showEditDialog() {
  DialogLauncher::self()->showBasicPluginDialog(_pluginName, this);
}


VectorPtr BasicPlugin::inputVector(const QString& vector) const {
  VectorMap::ConstIterator i = _inputVectors.find(vector);
  if (i != _inputVectors.end())
    return *i;
  else
    return 0;
}


ScalarPtr BasicPlugin::inputScalar(const QString& scalar) const {
  ScalarMap::ConstIterator i = _inputScalars.find(scalar);
  if (i != _inputScalars.end())
    return *i;
  else
    return 0;
}


StringPtr BasicPlugin::inputString(const QString& string) const {
  StringMap::ConstIterator i = _inputStrings.find(string);
  if (i != _inputStrings.end())
    return *i;
  else
    return 0;
}


VectorPtr BasicPlugin::outputVector(const QString& vector) const {
  VectorMap::ConstIterator i = _outputVectors.find(vector);
  if (i != _outputVectors.end())
    return *i;
  else
    return 0;
}


ScalarPtr BasicPlugin::outputScalar(const QString& scalar) const {
  ScalarMap::ConstIterator i = _outputScalars.find(scalar);
  if (i != _outputScalars.end())
    return *i;
  else
    return 0;
}


StringPtr BasicPlugin::outputString(const QString& string) const {
  StringMap::ConstIterator i = _outputStrings.find(string);
  if (i != _outputStrings.end())
    return *i;
  else
    return 0;
}


void BasicPlugin::setInputVector(const QString &type, VectorPtr ptr) {
  if (ptr) {
    _inputVectors[type] = ptr;
  } else {
    _inputVectors.remove(type);
  }
}


void BasicPlugin::setInputScalar(const QString &type, ScalarPtr ptr) {
  if (ptr) {
    _inputScalars[type] = ptr;
  } else {
    _inputScalars.remove(type);
  }
}


void BasicPlugin::setInputString(const QString &type, StringPtr ptr) {
  if (ptr) {
    _inputStrings[type] = ptr;
  } else {
    _inputStrings.remove(type);
  }
}


void BasicPlugin::setOutputVector(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  Q_ASSERT(store());
  VectorPtr v = store()->createObject<Vector>();
  v->setProvider(this);
  v->setSlaveName(txt);
  _outputVectors.insert(type, v);
}


void BasicPlugin::setOutputScalar(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  Q_ASSERT(store());
  ScalarPtr s = store()->createObject<Scalar>();
  s->setProvider(this);
  s->setSlaveName(txt);
  _outputScalars.insert(type, s);
}


void BasicPlugin::setOutputString(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  Q_ASSERT(store());
  StringPtr s = store()->createObject<String>();
  s->setProvider(this);
  s->setSlaveName(txt);
  _outputStrings.insert(type, s);
}


void BasicPlugin::internalUpdate() {
  //Make sure we have all the necessary inputs
  if (!inputsExist())
    return;

  writeLockInputsAndOutputs();

  //Call the plugins algorithm to operate on the inputs
  //and produce the outputs
  if ( !algorithm() ) {
    Debug::self()->log(i18n("There is an error in the %1 algorithm.").arg(propertyString()), Debug::Error);
    unlockInputsAndOutputs();
    return;
  }

  //Perform update on the outputs
  updateOutput();

  createScalars();

  unlockInputsAndOutputs();

  return;
}


// If a plugin provides a Parameters Vector, then scalars will be created, as well as a label.
void BasicPlugin::createScalars() {
  // Assumes that this is called with a write lock in place on this object
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (hasParameterVector()) {
    VectorPtr vectorParam = _outputVectors["Parameters Vector"];
    if (vectorParam) {
      QString paramName;
      int i = 0;
      int length = vectorParam->length();

      Q_ASSERT(store());
      for (paramName = parameterName(i);
          !paramName.isEmpty() && i < length;
           paramName = parameterName(++i)) {
        double scalarValue = vectorParam->value(i);
        if (!_outputScalars.contains(paramName)) {
          ScalarPtr s = store()->createObject<Scalar>();
          s->setProvider(this);
          s->setSlaveName(paramName);
          s->setValue(scalarValue);
          s->writeLock();
          _outputScalars.insert(paramName, s);
        } else {
          _outputScalars[paramName]->setValue(scalarValue);
        }
      }
    }
  }
}


QString BasicPlugin::parameterName(int /*index*/) const {
    return QString();
}


QString BasicPlugin::label(int precision) const {
  Q_UNUSED(precision)
  QString label;
  QString paramName;

  label = Name();

  if (hasParameterVector()) {
    VectorPtr vectorParam = _outputVectors["Parameters Vector"];
    int length = vectorParam->length();
    int i=0;
    for (paramName = parameterName(i);
         !paramName.isEmpty() && i < length;
         paramName = parameterName(++i)) {
        if (_outputScalars.contains(paramName)) {
          label += QString("\n%1: [%2]").arg(paramName).arg(_outputScalars[paramName]->Name());
        }
    }
  }

  return label;
}


void BasicPlugin::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  s.writeAttribute("type", _pluginName);
  saveNameInfo(s, VNUM|PNUM|XNUM);
  for (VectorMap::Iterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    s.writeStartElement("inputvector");
    s.writeAttribute("type", i.key());
    s.writeAttribute("tag", i.value()->Name());
    s.writeEndElement();
  }
  for (ScalarMap::Iterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    s.writeStartElement("inputscalar");
    s.writeAttribute("type", i.key());
    s.writeAttribute("tag", i.value()->Name());
    s.writeEndElement();
  }
  for (StringMap::Iterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    s.writeStartElement("inputstring");
    s.writeAttribute("type", i.key());
    s.writeAttribute("tag", i.value()->Name());
    s.writeEndElement();
  }
  for (VectorMap::Iterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    s.writeStartElement("outputvector");
    s.writeAttribute("type", i.key());
    s.writeAttribute("tag", i.value()->slaveName());
    s.writeEndElement();
  }
  for (ScalarMap::Iterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    s.writeStartElement("outputscalar");
    s.writeAttribute("type", i.key());
    s.writeAttribute("tag", i.value()->slaveName());
    s.writeEndElement();
  }
  for (StringMap::Iterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    s.writeStartElement("outputstring");
    s.writeAttribute("type", i.key());
    s.writeAttribute("tag", i.value()->slaveName());
    s.writeEndElement();
  }
  s.writeEndElement();
}


void BasicPlugin::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
}


//TODO Could use some templates perhaps...
bool BasicPlugin::inputsExist() const {
  //First, check the inputVectors...
  QStringList iv = inputVectorList();
  QStringList::ConstIterator ivI = iv.begin();
  for (; ivI != iv.end(); ++ivI) {
    if (!inputVector(*ivI))
      return false;
  }

  //Now, check the inputScalars...
  QStringList is = inputScalarList();
  QStringList::ConstIterator isI = is.begin();
  for (; isI != is.end(); ++isI) {
    if (!inputScalar(*isI))
      return false;
  }

  //Finally, check the inputStrings...
  QStringList istr = inputStringList();
  QStringList::ConstIterator istrI = istr.begin();
  for (; istrI != istr.end(); ++istrI) {
    if (!inputString(*istrI))
      return false;
  }
  return true;
}


void BasicPlugin::updateOutput() const {
  //output vectors...
  //FIXME: _outputVectors should be used, not this string list!
  QStringList ov = outputVectorList();
  QStringList::ConstIterator ovI = ov.begin();
  for (; ovI != ov.end(); ++ovI) {
    if (VectorPtr o = outputVector(*ovI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
      vectorRealloced(o, o->value(), o->length()); // why here?
      o->setNewAndShift(o->length(), o->numShift()); // why here?
    }
  }
}

QString BasicPlugin::descriptionTip() const {
  return i18n("Plugin: %1").arg(Name());
}

}
// vim: ts=2 sw=2 et
