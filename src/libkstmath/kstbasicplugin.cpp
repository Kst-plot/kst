/***************************************************************************
                                kstbasicplugin.cpp
                             -------------------
    begin                : 09/15/06
    copyright            : (C) 2006 The University of Toronto
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

#include <stdlib.h>
#include <unistd.h>

#include <QTextDocument>

#include "kst_i18n.h"

#include "kstdebug.h"
#include "kstbasicplugin.h"
#include "dialoglauncher.h"
#include "kstdatacollection.h"

KstBasicPlugin::KstBasicPlugin()
: KstDataObject(), _isFit(false) {
  _typeString = i18n("Plugin");
  _type = "Plugin";
}


KstBasicPlugin::KstBasicPlugin(const QDomElement& e)
: KstDataObject(e), _isFit(false) {
  _typeString = i18n("Plugin");
  _type = "Plugin";
}


KstBasicPlugin::~KstBasicPlugin() {
}


KstDataObjectPtr KstBasicPlugin::makeDuplicate(KstDataObjectDataObjectMap &map) {
  KstBasicPluginPtr plugin = kst_cast<KstBasicPlugin>(KstDataObject::createPlugin(propertyString()));

  // use same inputs
  for (KstVectorMap::ConstIterator iter = _inputVectors.begin(); iter != _inputVectors.end(); ++iter) {
    plugin->inputVectors().insert(iter.key(), iter.value());
  }
  for (KstScalarMap::ConstIterator iter = _inputScalars.begin(); iter != _inputScalars.end(); ++iter) {
    plugin->inputScalars().insert(iter.key(), iter.value());
  }
  for (KstStringMap::ConstIterator iter = _inputStrings.begin(); iter != _inputStrings.end(); ++iter) {
    plugin->inputStrings().insert(iter.key(), iter.value());
  }

  // create new outputs
  for (KstVectorMap::ConstIterator iter = outputVectors().begin(); iter != outputVectors().end(); ++iter) {
    KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
    KstVectorPtr v = new KstVector(KstObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), 0, plugin.data()); // FIXME: unique tag generation
    plugin->outputVectors().insert(iter.key(), v);
  }
  for (KstScalarMap::ConstIterator iter = outputScalars().begin(); iter != outputScalars().end(); ++iter) {
    KstScalarPtr s = new KstScalar(KstObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputScalars().insert(iter.key(), s);
  }
  for (KstStringMap::ConstIterator iter = outputStrings().begin(); iter != outputStrings().end(); ++iter) {
    KstStringPtr s = new KstString(KstObjectTag(iter.value()->tag().tag() + "'", iter.value()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputStrings().insert(iter.key(), s);
  }

  // set the same plugin
  plugin->setTagName(KstObjectTag(tag().tag() + "'", tag().context())); // FIXME: unique tag generation method
  map.insert(this, KstDataObjectPtr(plugin));
  return KstDataObjectPtr(plugin);
}

void KstBasicPlugin::showNewDialog() {
  //FIXME shouldn't tagName() == propertyString() ??
  KstDialogs::self()->showBasicPluginDialog(propertyString());
}


void KstBasicPlugin::showEditDialog() {
  KstDialogs::self()->showBasicPluginDialog(tagName(), true);
}


KstVectorPtr KstBasicPlugin::inputVector(const QString& vector) const {
  KstVectorMap::ConstIterator i = _inputVectors.find(vector);
  if (i != _inputVectors.end())
    return *i;
  else
    return 0;
}


KstScalarPtr KstBasicPlugin::inputScalar(const QString& scalar) const {
  KstScalarMap::ConstIterator i = _inputScalars.find(scalar);
  if (i != _inputScalars.end())
    return *i;
  else
    return 0;
}


KstStringPtr KstBasicPlugin::inputString(const QString& string) const {
  KstStringMap::ConstIterator i = _inputStrings.find(string);
  if (i != _inputStrings.end())
    return *i;
  else
    return 0;
}


KstVectorPtr KstBasicPlugin::outputVector(const QString& vector) const {
  KstVectorMap::ConstIterator i = _outputVectors.find(vector);
  if (i != _outputVectors.end())
    return *i;
  else
    return 0;
}


KstScalarPtr KstBasicPlugin::outputScalar(const QString& scalar) const {
  KstScalarMap::ConstIterator i = _outputScalars.find(scalar);
  if (i != _outputScalars.end())
    return *i;
  else
    return 0;
}


KstStringPtr KstBasicPlugin::outputString(const QString& string) const {
  KstStringMap::ConstIterator i = _outputStrings.find(string);
  if (i != _outputStrings.end())
    return *i;
  else
    return 0;
}


void KstBasicPlugin::setInputVector(const QString &type, KstVectorPtr ptr) {
  // TODO: deal with tags
  if (ptr) {
    _inputVectors[type] = ptr;
  } else {
    _inputVectors.remove(type);
  }
  setDirty();
}


void KstBasicPlugin::setInputScalar(const QString &type, KstScalarPtr ptr) {
  // TODO: deal with tags
  if (ptr) {
    _inputScalars[type] = ptr;
  } else {
    _inputScalars.remove(type);
  }
  setDirty();
}


void KstBasicPlugin::setInputString(const QString &type, KstStringPtr ptr) {
  // TODO: deal with tags
  if (ptr) {
    _inputStrings[type] = ptr;
  } else {
    _inputStrings.remove(type);
  }
  setDirty();
}


void KstBasicPlugin::setOutputVector(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  KstVectorPtr v = new KstVector(KstObjectTag(txt, tag()), 0, this, false);
  _outputVectors.insert(type, v);
}


void KstBasicPlugin::setOutputScalar(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  KstScalarPtr s = new KstScalar(KstObjectTag(txt, tag()), this);
  _outputScalars.insert(type, s);
}


void KstBasicPlugin::setOutputString(const QString &type, const QString &name) {
  QString txt = !name.isEmpty() ? name : type;
  KstStringPtr s = new KstString(KstObjectTag(txt, tag()), this);
  _outputStrings.insert(type, s);
}


KstObject::UpdateType KstBasicPlugin::update(int updateCounter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

  //Make sure we have all the necessary inputs
  if (!inputsExist())
    return setLastUpdateResult(NO_CHANGE);

  writeLockInputsAndOutputs();

  //Update the dependent inputs
  bool depUpdated = updateInput(updateCounter, force);

  //Call the plugins algorithm to operate on the inputs
  //and produce the outputs
  if ( !algorithm() ) {
    KstDebug::self()->log(i18n("There is an error in the %1 algorithm.").arg(propertyString()), KstDebug::Error);
    unlockInputsAndOutputs();
    return lastUpdateResult();
  }

  //Perform update on the outputs
  updateOutput(updateCounter);

  createFitScalars();

  unlockInputsAndOutputs();

  return setLastUpdateResult(depUpdated ? UPDATE : NO_CHANGE);
}

void KstBasicPlugin::load(const QDomElement &e) {
  QDomNode n = e.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(KstObjectTag::fromString(e.text()));
      } else if (e.tagName() == "ivector") {
        _inputVectorLoadQueue.append(qMakePair(e.attribute("name"), e.text()));
      } else if (e.tagName() == "iscalar") {
        _inputScalarLoadQueue.append(qMakePair(e.attribute("name"), e.text()));
      } else if (e.tagName() == "istring") {
        _inputStringLoadQueue.append(qMakePair(e.attribute("name"), e.text()));
      } else if (e.tagName() == "ovector") {
        KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
        KstVectorPtr v;
        if (e.attribute("scalarList", "0").toInt()) {
          v = new KstVector(KstObjectTag(e.text(), tag()), 0, this, true);
        } else {
          v = new KstVector(KstObjectTag(e.text(), tag()), 0, this, false);
        }
        _outputVectors.insert(e.attribute("name"), v);
      } else if (e.tagName() == "oscalar") {
        KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
        KstScalarPtr sp = new KstScalar(KstObjectTag(e.text(), tag()), this);
        _outputScalars.insert(e.attribute("name"), sp);
      } else if (e.tagName() == "ostring") {
        KstWriteLocker blockStringUpdates(&KST::stringList.lock());
        KstStringPtr sp = new KstString(KstObjectTag(e.text(), tag()), this);
        _outputStrings.insert(e.attribute("name"), sp);
      }
    }
    n = n.nextSibling();
  }
}


// FIXME: KstBasicPlugin should not know about fit scalars!!
void KstBasicPlugin::createFitScalars() {
  // Assumes that this is called with a write lock in place on this object
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_isFit && _outputVectors.contains("Parameters")) {
    KstVectorPtr vectorParam = _outputVectors["Parameters"];
    if (vectorParam) {
      QString paramName;
      int i = 0;
      int length = vectorParam->length();

      KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
      KST::scalarList.setUpdateDisplayTags(false);
      for (paramName = parameterName(i);
          !paramName.isEmpty() && i < length;
           paramName = parameterName(++i)) {
        double scalarValue = vectorParam->value(i);
        if (!_outputScalars.contains(paramName)) {
          KstScalarPtr s = new KstScalar(KstObjectTag(paramName, tag()), this, scalarValue);
          s->KstObject::writeLock();  // must write lock, since fit scalars are created from update()
          _outputScalars.insert(paramName, s);
        } else {
          _outputScalars[paramName]->setValue(scalarValue);
        }
      }
      KST::scalarList.setUpdateDisplayTags(true);
    }
  }
}


QString KstBasicPlugin::parameterName(int /*index*/) const {
    return QString::null;
}


QString KstBasicPlugin::label(int precision) const {
  QString label;

  label = i18n("%1: %2").arg(name()).arg(tagName());
  if ((outputVectors())["Parameters"]) {
    QString strParamName;
    QString strValue;
    int length = (outputVectors())["Parameters"]->length();
    int i = 0;

    for (strParamName = parameterName(0);
        !strParamName.isEmpty() && i < length;
        strParamName = parameterName(++i)) {
      KstScalarPtr scalar = outputScalars()[strParamName];
      if (scalar) {
        strValue = QString::number(scalar->value(), 'g', precision);
        label += i18n("\n%1: %2").arg(strParamName).arg(strValue);
      }
    }
  }

  return label;
}


void KstBasicPlugin::save(QTextStream& ts, const QString& indent) {
  QString l2 = indent + "  ";
  //The plugin name _must_ be the same as the entry in the .desktop file
  ts << indent << "<plugin name=\"" << propertyString() << "\">" << endl;
  ts << l2 << "<tag>" << Qt::escape(tagName()) << "</tag>" << endl;
  for (KstVectorMap::Iterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    ts << l2 << "<ivector name=\"" << Qt::escape(i.key()) << "\">"
        << Qt::escape(i.value()->tag().tagString())
        << "</ivector>" << endl;
  }
  for (KstScalarMap::Iterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    ts << l2 << "<iscalar name=\"" << Qt::escape(i.key()) << "\">"
        << Qt::escape(i.value()->tag().tagString())
        << "</iscalar>" << endl;
  }
  for (KstStringMap::Iterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    ts << l2 << "<istring name=\"" << Qt::escape(i.key()) << "\">"
        << Qt::escape(i.value()->tag().tagString())
        << "</istring>" << endl;
  }
  for (KstVectorMap::Iterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    ts << l2 << "<ovector name=\"" << Qt::escape(i.key());
    if (i.value()->isScalarList()) {
      ts << "\" scalarList=\"1";
    }
    ts << "\">" << Qt::escape(i.value()->tag().tag())
        << "</ovector>" << endl;
  }
  for (KstScalarMap::Iterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    ts << l2 << "<oscalar name=\"" << Qt::escape(i.key()) << "\">"
        << Qt::escape(i.value()->tag().tag())
        << "</oscalar>" << endl;
  }
  for (KstStringMap::Iterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    ts << l2 << "<ostring name=\"" << Qt::escape(i.key()) << "\">"
        << Qt::escape(i.value()->tag().tag())
        << "</ostring>" << endl;
  }
  ts << indent << "</plugin>" << endl;
}


//TODO Could use some templates perhaps...
bool KstBasicPlugin::inputsExist() const {
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


bool KstBasicPlugin::updateInput(int updateCounter, bool force) const {
  bool depUpdated = force;

  //First, update the inputVectors...
  QStringList iv = inputVectorList();
  QStringList::ConstIterator ivI = iv.begin();
  for (; ivI != iv.end(); ++ivI) {
    Q_ASSERT(inputVector(*ivI)->myLockStatus() == KstRWLock::WRITELOCKED);
    depUpdated =
        UPDATE == inputVector(*ivI)->update(updateCounter) || depUpdated;
  }

  //Now, update the inputScalars...
  QStringList is = inputScalarList();
  QStringList::ConstIterator isI = is.begin();
  for (; isI != is.end(); ++isI) {
    Q_ASSERT(inputScalar(*isI)->myLockStatus() == KstRWLock::WRITELOCKED);
    depUpdated =
        UPDATE == inputScalar(*isI)->update(updateCounter) || depUpdated;
  }

  //Finally, update the inputStrings...
  QStringList istr = inputStringList();
  QStringList::ConstIterator istrI = istr.begin();
  for (; istrI != istr.end(); ++istrI) {
    Q_ASSERT(inputString(*istrI)->myLockStatus() == KstRWLock::WRITELOCKED);
    depUpdated =
        UPDATE == inputString(*istrI)->update(updateCounter) || depUpdated;
  }
  return depUpdated;
}


void KstBasicPlugin::updateOutput(int updateCounter) const {
  //output vectors...
  QStringList ov = outputVectorList();
  QStringList::ConstIterator ovI = ov.begin();
  for (; ovI != ov.end(); ++ovI) {
    if (KstVectorPtr o = outputVector(*ovI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
      vectorRealloced(o, o->value(), o->length());
      o->setDirty();
      o->setNewAndShift(o->length(), o->numShift());
      o->update(updateCounter);
    }
  }

  //output scalars...
  QStringList os = outputScalarList();
  QStringList::ConstIterator osI = os.begin();
  for (; osI != os.end(); ++osI) {
    if (KstScalarPtr o = outputScalar(*osI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
      o->update(updateCounter);
    }
  }

  //ouput strings...
  QStringList ostr = outputStringList();
  QStringList::ConstIterator ostrI = ostr.begin();
  for (; ostrI != ostr.end(); ++ostrI) {
    if (KstStringPtr o = outputString(*ostrI)) {
      Q_ASSERT(o->myLockStatus() == KstRWLock::WRITELOCKED);
      o->update(updateCounter);
    }
  }
}

// vim: ts=2 sw=2 et
