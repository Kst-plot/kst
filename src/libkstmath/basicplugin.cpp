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

namespace Kst {

const QString BasicPlugin::staticTypeString = I18N_NOOP("Plugin");
const QString BasicPlugin::staticTypeTag = I18N_NOOP("plugin");

BasicPlugin::BasicPlugin(ObjectStore *store)
: DataObject(store), _isFit(false) {
  _typeString = i18n("Plugin");
  _type = "Plugin";

  _shortName = "P"+QString::number(_pnum);
  if (_pnum>max_pnum) 
    max_pnum = _pnum;
  _pnum++;

}


BasicPlugin::~BasicPlugin() {
}


void BasicPlugin::setPluginName(const QString &pluginName) {
  _pluginName = pluginName;
}


DataObjectPtr BasicPlugin::makeDuplicate() {
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
  setDirty();
}


void BasicPlugin::setInputScalar(const QString &type, ScalarPtr ptr) {
  if (ptr) {
    _inputScalars[type] = ptr;
  } else {
    _inputScalars.remove(type);
  }
  setDirty();
}


void BasicPlugin::setInputString(const QString &type, StringPtr ptr) {
  if (ptr) {
    _inputStrings[type] = ptr;
  } else {
    _inputStrings.remove(type);
  }
  setDirty();
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


Object::UpdateType BasicPlugin::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  bool force = dirty();
  setDirty(false);

  //Make sure we have all the necessary inputs
  if (!inputsExist())
    return NO_CHANGE;

  writeLockInputsAndOutputs();

  //Update the dependent inputs
  bool depUpdated = updateInput(force);

  //Call the plugins algorithm to operate on the inputs
  //and produce the outputs
  if ( !algorithm() ) {
    Debug::self()->log(i18n("There is an error in the %1 algorithm.").arg(propertyString()), Debug::Error);
    unlockInputsAndOutputs();
    return NO_CHANGE;
  }

  //Perform update on the outputs
  updateOutput();

  createFitScalars();

  unlockInputsAndOutputs();

  return (depUpdated ? UPDATE : NO_CHANGE);
}


// FIXME: BasicPlugin should not know about fit scalars!!
void BasicPlugin::createFitScalars() {
  // Assumes that this is called with a write lock in place on this object
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_isFit && _outputVectors.contains("Parameters")) {
    VectorPtr vectorParam = _outputVectors["Parameters"];
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
          //FIXME: set slaveName
          s->setValue(scalarValue);
          _outputScalars.insert(paramName, s);
        } else {
          _outputScalars[paramName]->setValue(scalarValue);
        }
      }
    }
  }
}


QString BasicPlugin::parameterName(int /*index*/) const {
    return QString::null;
}


QString BasicPlugin::label(int precision) const {
  QString label;

  label = i18n("%1: %2").arg(name()).arg(Name());
  if ((outputVectors())["Parameters"]) {
    QString strParamName;
    QString strValue;
    int length = (outputVectors())["Parameters"]->length();
    int i = 0;

    for (strParamName = parameterName(0);
        !strParamName.isEmpty() && i < length;
        strParamName = parameterName(++i)) {
      ScalarPtr scalar = outputScalars()[strParamName];
      if (scalar) {
        strValue = QString::number(scalar->value(), 'g', precision);
        label += i18n("\n%1: %2").arg(strParamName).arg(strValue);
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


bool BasicPlugin::updateInput(bool force) const {
  bool depUpdated = force;

  //First, update the inputVectors...
  QStringList iv = inputVectorList();
  QStringList::ConstIterator ivI = iv.begin();
  for (; ivI != iv.end(); ++ivI) {
    Q_ASSERT(inputVector(*ivI)->myLockStatus() == KstRWLock::WRITELOCKED);
    depUpdated =
        UPDATE == inputVector(*ivI)->update() || depUpdated;
  }

  //Now, update the inputScalars...
  QStringList is = inputScalarList();
  QStringList::ConstIterator isI = is.begin();
  for (; isI != is.end(); ++isI) {
    Q_ASSERT(inputScalar(*isI)->myLockStatus() == KstRWLock::WRITELOCKED);
    depUpdated =
        UPDATE == inputScalar(*isI)->update() || depUpdated;
  }

  //Finally, update the inputStrings...
  QStringList istr = inputStringList();
  QStringList::ConstIterator istrI = istr.begin();
  for (; istrI != istr.end(); ++istrI) {
    Q_ASSERT(inputString(*istrI)->myLockStatus() == KstRWLock::WRITELOCKED);
    depUpdated =
        UPDATE == inputString(*istrI)->update() || depUpdated;
  }
  return depUpdated;
}


void BasicPlugin::updateOutput() const {
  //output vectors...
  QStringList ov = outputVectorList();
  QStringList::ConstIterator ovI = ov.begin();
  for (; ovI != ov.end(); ++ovI) {
    if (VectorPtr o = outputVector(*ovI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
      vectorRealloced(o, o->value(), o->length());
      o->setDirty();
      o->setNewAndShift(o->length(), o->numShift());
//       o->update();
    }
  }

  //output scalars...
  QStringList os = outputScalarList();
  QStringList::ConstIterator osI = os.begin();
  for (; osI != os.end(); ++osI) {
    if (ScalarPtr o = outputScalar(*osI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
//       o->update();
    }
  }

  //ouput strings...
  QStringList ostr = outputStringList();
  QStringList::ConstIterator ostrI = ostr.begin();
  for (; ostrI != ostr.end(); ++ostrI) {
    if (StringPtr o = outputString(*ostrI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
//       o->update();
    }
  }
}

QString BasicPlugin::descriptionTip() const {
  return i18n("Plugin: %1").arg(Name());
}

}
// vim: ts=2 sw=2 et
