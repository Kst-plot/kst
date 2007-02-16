/***************************************************************************
                                kstplugin.cpp
                             -------------------
    begin                : May 15 2003
    copyright            : (C) 2003 The University of Toronto
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

// include files for Qt
#include <q3stylesheet.h>
//Added by qt3to4:
#include <Q3ValueList>

// include files for KDE
#include "ksdebug.h"
#include <klocale.h>

// application specific includes
#include "dialoglauncher.h"
#include "kstdatacollection.h"
#include "kstdebug.h"
#include "kstcplugin.h"
#include "plugincollection.h"


KstCPlugin::KstCPlugin()
: KstDataObject() {
  _inStringCnt = 0;
  _outStringCnt = 0;
  commonConstructor();
}


KstCPlugin::KstCPlugin(const QDomElement& pluginElement)
: KstDataObject(pluginElement) {
  QString pluginName;

  _inStringCnt = 0;
  _outStringCnt = 0;
  commonConstructor();

  QDomNode n = pluginElement.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(KstObjectTag::fromString(e.text()));
      } else if (e.tagName() == "name") {
        pluginName = e.text();
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

  _plugin = PluginCollection::self()->plugin(pluginName);

  if (!_plugin.data()) {
    KstDebug::self()->log(i18n("Unable to load plugin %1 for \"%2\".").arg(pluginName).arg(tagName()), KstDebug::Warning);
  } else {
    Plugin::countScalarsVectorsAndStrings(_plugin->data()._inputs, _inScalarCnt, _inArrayCnt, _inStringCnt, _inPid);

    const Q3ValueList<Plugin::Data::IOValue>& otable = _plugin->data()._outputs;
    for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = otable.begin();
                                                           it != otable.end();
                                                                         ++it) {
      // FIXME: i18n?
      if ((*it)._type == Plugin::Data::IOValue::TableType) {
        _outArrayCnt++;
        if (!_outputVectors.contains((*it)._name)) {
          KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
          KstVectorPtr v;

          if ((*it)._subType == Plugin::Data::IOValue::FloatNonVectorSubType) {
            v = new KstVector(KstObjectTag(tagName() + " vector - " + (*it)._name, tag()), 0, this, true);  // FIXME: tag name?
          } else {
            v = new KstVector(KstObjectTag(tagName() + " vector - " + (*it)._name, tag()), 0, this, false);  // FIXME: tag name?
          }
          _outputVectors.insert((*it)._name, v);
        }
      } else if ((*it)._type == Plugin::Data::IOValue::MatrixType) {
          abort(); // FIXME:
#if 0
        _outArrayCnt += 2;
        if (!_outputMatrices.contains((*it)._name)) {
          KstWriteLocker blockMatrixUpdates(&KST::matrixList.lock());
          KstMatrixPtr m;

          if ((*it)._subType == Plugin::Data::IOValue::FloatNonVectorSubType) {
            m = new KstMatrix(KstObjectTag(tagName() + " matrix - " + (*it)._name, tag()), 0, this, true);  // FIXME: tag name?
          } else {
            m = new KstMatrix(KstObjectTag(tagName() + " matrix - " + (*it)._name, tag()), 0, this, false);  // FIXME: tag name?
          }
          m->setProvider(this);
          _outputMatrices.insert((*it)._name, m);
        }
#endif
      } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
        _outScalarCnt++;
        if (!_outputScalars.contains((*it)._name)) {
          KstScalarPtr s = new KstScalar(KstObjectTag(tagName() + " scalar - " + (*it)._name, tag()), this);  // FIXME: tag name?
          _outputScalars.insert((*it)._name, s);
        }
      } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
        _outStringCnt++;
        if (!_outputStrings.contains((*it)._name)) {
          KstStringPtr s = new KstString(KstObjectTag(tagName() + " string - " + (*it)._name, tag()), this);  // FIXME: tag name?
          _outputStrings.insert((*it)._name, s);
        }
      }
    }
    allocateParameters();
  }
}


void KstCPlugin::commonConstructor() {
  _inArrayLens = 0L;
  _outArrayLens = 0L;
  _inScalars = 0L;
  _outScalars = 0L;
  _inVectors = 0L;
  _outVectors = 0L;
  _inStrings = 0L;
  _outStrings = 0L;

  _inScalarCnt = 0;
  _outScalarCnt = 0;
  _inArrayCnt = 0;
  _outArrayCnt = 0;
  _typeString = i18n("Plugin");
  _type = "Plugin";
  _plugin = 0L;
  _localData = 0L;
  //kstdDebug() << "Creating KSTPlugin: " << long(this) << endl;
}


KstCPlugin::~KstCPlugin() {
  freeParameters();
  if (_localData) {
    if (!_plugin || !_plugin->freeLocalData(&_localData)) {
      free(_localData);
    }
    _localData = 0L;
  }
  //kstdDebug() << "Destroying KSTPlugin: " << long(this) << endl;
}



void KstCPlugin::allocateParameters() {
  if (_inArrayCnt > 0) {
    _inArrayLens = new int[_inArrayCnt];
    _inVectors = new double*[_inArrayCnt];
  }

  if (_outArrayCnt > 0) {
    _outArrayLens = new int[_outArrayCnt];
    _outVectors = new double*[_outArrayCnt];
  }

  if (_inScalarCnt > 0) {
    _inScalars = new double[_inScalarCnt];
  }

  if (_outScalarCnt > 0) {
    _outScalars = new double[_outScalarCnt];
  }

  if (_inStringCnt > 0) {
    _inStrings = new char*[_inStringCnt];
    memset(_inStrings, 0, sizeof(char*)*_inStringCnt);
  }

  if (_outStringCnt > 0) {
    _outStrings = new char*[_outStringCnt];
    memset(_outStrings, 0, sizeof(char*)*_outStringCnt);
  }
}


void KstCPlugin::freeParameters() {
  delete[] _inVectors;
  _inVectors = 0L;
  delete[] _outVectors;
  _outVectors = 0L;
  delete[] _outArrayLens;
  _outArrayLens = 0L;
  delete[] _inArrayLens;
  _inArrayLens = 0L;
  delete[] _outScalars;
  _outScalars = 0L;
  delete[] _inScalars;
  _inScalars = 0L;
  if (_outStrings) {
    for (unsigned i = 0; i < _outStringCnt; ++i) {
      if (_outStrings[i]) {
        free(_outStrings[i]);
        _outStrings[i] = 0L;
      }
    }
  }
  delete[] _outStrings;
  _outStrings = 0L;
  if (_inStrings) {
    for (unsigned i = 0; i < _inStringCnt; ++i) {
      if (_inStrings[i]) {
        free(_inStrings[i]);
        _inStrings[i] = 0L;
      }
    }
  }
  delete[] _inStrings;
  _inStrings = 0L;
}


KstObject::UpdateType KstCPlugin::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!isValid()) {
    return setLastUpdateResult(NO_CHANGE);
  }

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

#define CLEANUP() do {\
  for (unsigned i = 0; i < _outStringCnt; ++i) { \
    if (_outStrings[i]) { \
      free(_outStrings[i]); \
      _outStrings[i] = 0L; \
    } \
  } \
  for (unsigned i = 0; i < _inStringCnt; ++i) { \
    if (_inStrings[i]) { \
      free(_inStrings[i]); \
      _inStrings[i] = 0L; \
    } \
  } \
  } while(0)


  writeLockInputsAndOutputs();

  const Q3ValueList<Plugin::Data::IOValue>& itable = _plugin->data()._inputs;
  const Q3ValueList<Plugin::Data::IOValue>& otable = _plugin->data()._outputs;
  int itcnt = 0, vitcnt = 0, sitcnt = 0;
  bool doUpdate = force;

  // Populate the input scalars and vectors
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = itable.begin();
                                                         it != itable.end();
                                                                        ++it) {
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      if (!_inputVectors.contains((*it)._name)) {
        KstDebug::self()->log(i18n("Input vector [%1] for plugin %2 not found.  Unable to continue.").arg((*it)._name).arg(tagName()), KstDebug::Error);
        CLEANUP();
        return setLastUpdateResult(NO_CHANGE);
      }
      KstVectorPtr iv = _inputVectors[(*it)._name];
      if (!iv) {
        kstdFatal() << "Input vector \"" << (*it)._name << "\" for plugin " << tag().displayString() << " is invalid." << endl;
      }
      doUpdate = (UPDATE == iv->update(update_counter)) || doUpdate;
      _inVectors[vitcnt] = iv->value();
      _inArrayLens[vitcnt++] = iv->length();
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      KstScalarPtr is = _inputScalars[(*it)._name];
      if (!is) {
        kstdFatal() << "Input scalar \"" << (*it)._name << "\" for plugin " << tag().displayString() << " is invalid." << endl;
      }
      doUpdate = (UPDATE == is->update(update_counter)) || doUpdate;
      _inScalars[itcnt++] = is->value();
    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      KstStringPtr is = _inputStrings[(*it)._name];
      if (!is) {
        kstdFatal() << "Input string \"" << (*it)._name << "\" for plugin " << tag().displayString() << " is invalid." << endl;
      }
      doUpdate = (UPDATE == is->update(update_counter)) || doUpdate;
      // Maybe we should use UTF-8 instead?
      _inStrings[sitcnt++] = strdup(is->value().latin1());
    } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
      _inScalars[itcnt++] = getpid();
    }
  }

  if (!doUpdate) {
    CLEANUP();
    unlockInputsAndOutputs();
    return setLastUpdateResult(NO_CHANGE);
  }

  vitcnt = 0;
  // Populate the output vectors
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = otable.begin();
                                                         it != otable.end();
                                                                        ++it) {
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      if (!_outputVectors.contains((*it)._name)) {
        KstDebug::self()->log(i18n("Output vector [%1] for plugin %2 not found.  Unable to continue.").arg((*it)._name).arg(tagName()), KstDebug::Error);
        CLEANUP();
        unlockInputsAndOutputs();
        return setLastUpdateResult(NO_CHANGE);
      }
      _outVectors[vitcnt] = _outputVectors[(*it)._name]->value();
      _outArrayLens[vitcnt++] = _outputVectors[(*it)._name]->length();
    }
  }

  if (_outStringCnt > 0) {
    memset(_outStrings, 0, _outStringCnt*sizeof(char *));
  }

  int rc;
  if (_inStringCnt > 0 || _outStringCnt > 0) {
    if (_plugin->data()._localdata) {
      rc = _plugin->call(_inVectors, _inArrayLens, _inScalars,
          _outVectors, _outArrayLens, _outScalars,
          const_cast<const char**>(_inStrings), _outStrings, &_localData);
    } else {
      rc = _plugin->call(_inVectors, _inArrayLens, _inScalars,
          _outVectors, _outArrayLens, _outScalars,
          const_cast<const char**>(_inStrings), _outStrings);
    }
  } else {
    if (_plugin->data()._localdata) {
      rc = _plugin->call(_inVectors, _inArrayLens, _inScalars,
          _outVectors, _outArrayLens, _outScalars, &_localData);
    } else {
      rc = _plugin->call(_inVectors, _inArrayLens, _inScalars,
          _outVectors, _outArrayLens, _outScalars);
    }
  }

  if (rc == 0) {
    itcnt = 0;
    vitcnt = 0;
    sitcnt = 0;
    setLastUpdateResult(UPDATE); // make sure that provider callbacks work
    // Read back the output vectors and scalars
    for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = otable.begin();
        it != otable.end();
        ++it) {
      if ((*it)._type == Plugin::Data::IOValue::TableType) {
        KstVectorPtr vp = _outputVectors[(*it)._name];
        vectorRealloced(vp, _outVectors[vitcnt], _outArrayLens[vitcnt]);
        vp->setDirty();
        // Inefficient, but do we have any other choice?  We don't really know
        // from the plugin how much of this vector is "new" or "shifted"
        vp->setNewAndShift(vp->length(), vp->numShift());
        vp->update(update_counter);
        vitcnt++;
      } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
        KstScalarPtr sp = _outputScalars[(*it)._name];
        sp->setValue(_outScalars[itcnt++]);
        sp->update(update_counter);
      } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
        KstStringPtr sp = _outputStrings[(*it)._name];
        sp->setValue(_outStrings[sitcnt++]);
        sp->update(update_counter);
      }
    }

    // if we have a fit plugin then create the necessary scalars from the parameter vector
    createFitScalars();
    _lastError = QString::null;
  } else if (rc > 0) {
    if (_lastError.isEmpty()) {
      const char *err = _plugin->errorCode(rc);
      if (err && *err) {
        _lastError = err;
        KstDebug::self()->log(i18n("Plugin %1 produced error: %2.").arg(tagName()).arg(_lastError), KstDebug::Error);
      } else {
        _lastError = QString::null;
      }
    }
  } else {
    bool doSend = _lastError.isEmpty() ? true : false;

    switch (rc) {
      case -1:
        _lastError = i18n("Generic Error");
        break;
      case -2:
        _lastError = i18n("Input Error");
        break;
      case -3:
        _lastError = i18n("Memory Error");
        break;
      default:
        _lastError = i18n("Unknown Error");
        break;
    }

    if (doSend) {
      KstDebug::self()->log(i18n("Plugin %2 produced error: %1.").arg(_lastError).arg(tagName()), KstDebug::Error);
    }
  }

  unlockInputsAndOutputs();

  CLEANUP();
#undef CLEANUP
  return setLastUpdateResult(UPDATE);
}

QString KstCPlugin::label(int precision) const {
  QString label;
  
  label = i18n("%1: %2").arg(plugin()->data()._readableName).arg(tagName());
  if ((outputVectors())["Parameters"]) {
    QString strParamName;
    QString strValue;
    int length = (outputVectors())["Parameters"]->length();
    int i = 0;
    
    for (strParamName = plugin()->parameterName(0); 
        !strParamName.isEmpty() && i < length; 
        strParamName = plugin()->parameterName(++i)) {
      KstScalarPtr scalar = outputScalars()[strParamName];
      if (scalar) {
        strValue = QString::number(scalar->value(), 'g', precision);
        label += i18n("\n%1: %2").arg(strParamName).arg(strValue);
      }
    }
  }

  return label;
}


void KstCPlugin::save(Q3TextStream &ts, const QString& indent) {
  if (!_plugin) {
    return;
  }

  QString l2 = indent + "  ";
  ts << indent << "<plugin>" << endl;
  ts << l2 << "<tag>" << Q3StyleSheet::escape(tagName()) << "</tag>" << endl;
  ts << l2 << "<name>" << Q3StyleSheet::escape(_plugin->data()._name) << "</name>" << endl;
  for (KstVectorMap::Iterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    ts << l2 << "<ivector name=\"" << Q3StyleSheet::escape(i.key()) << "\">"
      << Q3StyleSheet::escape(i.data()->tag().tagString())
      << "</ivector>" << endl;
  }
  for (KstScalarMap::Iterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    ts << l2 << "<iscalar name=\"" << Q3StyleSheet::escape(i.key()) << "\">"
      << Q3StyleSheet::escape(i.data()->tag().tagString())
      << "</iscalar>" << endl;
  }
  for (KstStringMap::Iterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    ts << l2 << "<istring name=\"" << Q3StyleSheet::escape(i.key()) << "\">"
      << Q3StyleSheet::escape(i.data()->tag().tagString())
      << "</istring>" << endl;
  }
  for (KstVectorMap::Iterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    ts << l2 << "<ovector name=\"" << Q3StyleSheet::escape(i.key());
    if (i.data()->isScalarList()) {
      ts << "\" scalarList=\"1";
    }
    ts << "\">" << Q3StyleSheet::escape(i.data()->tag().tag())
      << "</ovector>" << endl;
  }
  for (KstScalarMap::Iterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    ts << l2 << "<oscalar name=\"" << Q3StyleSheet::escape(i.key()) << "\">"
      << Q3StyleSheet::escape(i.data()->tag().tag())
      << "</oscalar>" << endl;
  }
  for (KstStringMap::Iterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    ts << l2 << "<ostring name=\"" << Q3StyleSheet::escape(i.key()) << "\">"
      << Q3StyleSheet::escape(i.data()->tag().tag())
      << "</ostring>" << endl;
  }
  ts << indent << "</plugin>" << endl;
}


bool KstCPlugin::slaveVectorsUsed() const {
  return true;
}


bool KstCPlugin::isValid() const {
  return _inputVectors.count() == _inArrayCnt &&
         _inputScalars.count() == _inScalarCnt - _inPid &&
         _inputStrings.count() == _inStringCnt &&
         _plugin.data() != 0L;
}


QString KstCPlugin::propertyString() const {
  if (!isValid()) {
    return i18n("Invalid plugin.");
  }
  return plugin()->data()._name;
}


bool KstCPlugin::setPlugin(KstSharedPtr<Plugin> plugin) {
  // Assumes that this is called with a write lock in place on this object
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (plugin == _plugin) {
    return true;
  }

  freeParameters();

  if (_localData) {
    if (!_plugin || !_plugin->freeLocalData(&_localData)) {
      free(_localData);
    }
    _localData = 0L;
  }

  if (!plugin) {
    _inputVectors.clear();
    _inputScalars.clear();
    _inputStrings.clear();
    _outputVectors.clear();
    _outputScalars.clear();
    _outputStrings.clear();
    _plugin = 0L;
    return true;
  }

  Plugin::countScalarsVectorsAndStrings(plugin->data()._inputs, _inScalarCnt, _inArrayCnt, _inStringCnt, _inPid);

  if (_inputVectors.count() != _inArrayCnt ||
      _inputScalars.count() != _inScalarCnt - _inPid ||
      _inputStrings.count() != _inStringCnt) {
    _plugin = 0L;
    return false;
  }

  _outScalarCnt = 0;
  _outArrayCnt = 0;
  _outStringCnt = 0;
  _outputVectors.clear();
  _outputScalars.clear();
  _outputStrings.clear();

  const Q3ValueList<Plugin::Data::IOValue>& otable = plugin->data()._outputs;
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = otable.begin();
                                                         it != otable.end();
                                                                        ++it) {
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
      KstVectorPtr v;

      if ((*it)._subType == Plugin::Data::IOValue::FloatNonVectorSubType) {
        v = new KstVector(KstObjectTag((*it)._name, tag()), 0, this, true);
      } else {
        v = new KstVector(KstObjectTag((*it)._name, tag()), 0, this, false);
      }
      _outputVectors.insert((*it)._name, v);
      ++_outArrayCnt;
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
      KstScalarPtr s = new KstScalar(KstObjectTag((*it)._name, tag()), this);
      _outputScalars.insert((*it)._name, s);
      ++_outScalarCnt;
    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      KstWriteLocker blockStringUpdates(&KST::stringList.lock());
      KstStringPtr s = new KstString(KstObjectTag((*it)._name, tag()), this);
      _outputStrings.insert((*it)._name, s);
      ++_outStringCnt;
    }
  }

  allocateParameters();
  _plugin = plugin;

  return true;
}


KstSharedPtr<Plugin> KstCPlugin::plugin() const {
  return _plugin;
}


void KstCPlugin::showNewDialog() {
  KstDialogs::self()->showCPluginDialog();
}


void KstCPlugin::showEditDialog() {
  KstDialogs::self()->showCPluginDialog(tagName(), true);
}


const KstCurveHintList* KstCPlugin::curveHints() const {
  _curveHints->clear();
  if (_plugin) {
    for (Q3ValueList<Plugin::Data::CurveHint>::ConstIterator i = _plugin->data()._hints.begin(); i != _plugin->data()._hints.end(); ++i) {
      KstVectorPtr xv = _outputVectors[(*i).x];
      KstVectorPtr yv = _outputVectors[(*i).y];
      if (xv && yv) {
        _curveHints->append(new KstCurveHint((*i).name, xv->tagName(), yv->tagName()));
      }
    }
  }

  return KstDataObject::curveHints();
}


QString KstCPlugin::lastError() const {
  return _lastError;
}


KstDataObject::Kind KstCPlugin::kind() const {

  if (_plugin->data()._isFit)
    return Fit;
  else if (_plugin->data()._isFilter)
    return Filter;
  else
    return Generic;
}


// FIXME: KstCPlugin should not know about fit scalars!!
void KstCPlugin::createFitScalars() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  // Assumes that this is called with a write lock in place on this object

  if (_plugin->data()._isFit && _outputVectors.contains("Parameters")) {
    KstVectorPtr vectorParam = _outputVectors["Parameters"];
    if (vectorParam) {
      QString paramName;
      int i = 0;
      int length = vectorParam->length();

      KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
      KST::scalarList.setUpdateDisplayTags(false);
      for (paramName = _plugin->parameterName(i); 
          !paramName.isEmpty() && i < length; 
           paramName = _plugin->parameterName(++i)) {
        double scalarValue = vectorParam->value(i);
        if (!_outputScalars.contains(paramName)) {
          KstScalarPtr s = new KstScalar(KstObjectTag(paramName, tag()), this, scalarValue);
          s->KstObject::writeLock();  // must write lock, since fit scalars are created from update()
          _outputScalars.insert(paramName, s);
          ++_outScalarCnt;
        } else {
          _outputScalars[paramName]->setValue(scalarValue);
        }
      }
      KST::scalarList.setUpdateDisplayTags(true);
    }
  }
}


KstDataObjectPtr KstCPlugin::makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) {
  KstCPluginPtr plugin = new KstCPlugin;

  // use same inputs
  for (KstVectorMap::ConstIterator iter = _inputVectors.begin(); iter != _inputVectors.end(); ++iter) {
    plugin->inputVectors().insert(iter.key(), iter.data());
  }
  for (KstScalarMap::ConstIterator iter = _inputScalars.begin(); iter != _inputScalars.end(); ++iter) {
    plugin->inputScalars().insert(iter.key(), iter.data());  
  }
  for (KstStringMap::ConstIterator iter = _inputStrings.begin(); iter != _inputStrings.end(); ++iter) {
    plugin->inputStrings().insert(iter.key(), iter.data());  
  }
  
  // create new outputs
  for (KstVectorMap::ConstIterator iter = outputVectors().begin(); iter != outputVectors().end(); ++iter) {
    KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
    KstVectorPtr v = new KstVector(KstObjectTag(iter.data()->tag().tag() + "'", iter.data()->tag().context()), 0, plugin.data()); // FIXME: unique tag generation
    plugin->outputVectors().insert(iter.key(), v);
  }
  for (KstScalarMap::ConstIterator iter = outputScalars().begin(); iter != outputScalars().end(); ++iter) {
    KstScalarPtr s = new KstScalar(KstObjectTag(iter.data()->tag().tag() + "'", iter.data()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputScalars().insert(iter.key(), s);
  }
  for (KstStringMap::ConstIterator iter = outputStrings().begin(); iter != outputStrings().end(); ++iter) {
    KstStringPtr s = new KstString(KstObjectTag(iter.data()->tag().tag() + "'", iter.data()->tag().context()), plugin.data()); // FIXME: unique tag generation
    plugin->outputStrings().insert(iter.key(), s);
  }
  
  // set the same plugin
  plugin->setPlugin(_plugin);
  plugin->setTagName(KstObjectTag(tag().tag() + "'", tag().context())); // FIXME: unique tag generation method
  duplicatedMap.insert(this, KstDataObjectPtr(plugin));  
  return KstDataObjectPtr(plugin);
}

// vim: ts=2 sw=2 et
