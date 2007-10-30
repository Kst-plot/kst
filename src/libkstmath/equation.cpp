/***************************************************************************
                          equation.cpp: Equations for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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

/** A class for handling equations for kst
 *@author C. Barth Netterfield
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <QTextDocument>
#include <QXmlStreamWriter>

#include "dialoglauncher.h"
#include "enodes.h"
#include "eparse-eh.h"
#include "datacollection.h"
#include "debug.h"
#include "equation.h"
#include "kst_i18n.h"
#include "generatedvector.h"
#include "objectstore.h"

/*extern "C"*/ int yyparse(Kst::ObjectStore *store);
extern void *ParsedEquation;
/*extern "C"*/ struct yy_buffer_state *yy_scan_string(const char*);

namespace Kst {

const QString Equation::staticTypeString = I18N_NOOP("Equation");

const QString Equation::XINVECTOR = "X";
const QString Equation::XOUTVECTOR = "XO"; // Output (slave) vector
const QString Equation::YOUTVECTOR = "O"; // Output (slave) vector


Equation::Equation(ObjectStore *store, const ObjectTag& in_tag, const QString& equation, double x0, double x1, int nx)
: DataObject(store, in_tag) {
  Q_ASSERT(store);
  ObjectTag vtag = store->suggestObjectTag<GeneratedVector>(QString("(%1..%2)").arg(x0).arg(x1), in_tag);

  GeneratedVectorPtr v = store->createObject<GeneratedVector>(vtag);
  v->changeRange(x0, x1, nx);
  // FIXME: provider?
  _xInVector = v;

  _doInterp = false;

  commonConstructor(store, equation);
  setDirty();
}


Equation::Equation(ObjectStore *store, const ObjectTag& in_tag, const QString& equation, VectorPtr xvector, bool do_interp)
: DataObject(store, in_tag), _xInVector(xvector) {
  _doInterp = do_interp; //false;
  _inputVectors.insert(XINVECTOR, xvector);

  commonConstructor(store, equation);
  setDirty();
}


Equation::~Equation() {
  delete _pe;
  _pe = 0L;
}


void Equation::attach() {
}


void Equation::commonConstructor(ObjectStore *store, const QString& in_equation) {
  _ns = 2;
  _pe = 0L;
  _typeString = i18n("Equation");
  _type = "Equation";

  Q_ASSERT(store);
  _xOutVector = store->createObject<Vector>(ObjectTag("xsv", tag()));
  Q_ASSERT(_xOutVector);
  _xOutVector->setProvider(this);
  _xOutVector->resize(2);
  _outputVectors.insert(XOUTVECTOR, _xOutVector);

  _yOutVector = store->createObject<Vector>(ObjectTag("sv", tag()));
  Q_ASSERT(_yOutVector);
  _yOutVector->setProvider(this);
  _yOutVector->resize(2);
  _outputVectors.insert(YOUTVECTOR, _yOutVector);

  _isValid = false;
  _numNew = _numShifted = 0;

  setEquation(in_equation);
}


const CurveHintList *Equation::curveHints() const {
  _curveHints->clear();
  _curveHints->append(new CurveHint(i18n("Equation Curve"),
                      _xOutVector->tag().displayString(), _yOutVector->tag().displayString()));
  return _curveHints;
}


bool Equation::isValid() const {
  return _isValid;
}


Object::UpdateType Equation::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  bool xUpdated = false;
  bool usedUpdated = false;

  if (Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  if (!_pe) {
    return setLastUpdateResult(NO_CHANGE);
  }

  assert(update_counter >= 0);

  // FIXME: this is broken
#if 0
  if (_xInVector == _inputVectors.end()) {
    _xInVector = _inputVectors.find(XINVECTOR);
    if (!*_xInVector) { // This is technically sort of fatal
      return setLastUpdateResult(NO_CHANGE);
    }
  }
#endif

  writeLockInputsAndOutputs();

  VectorPtr v = _xInVector;

  xUpdated = Object::UPDATE == v->update(update_counter);

  Equations::Context ctx;
  ctx.sampleCount = _ns;
  ctx.xVector = v;
  usedUpdated = _pe && Object::UPDATE == _pe->update(update_counter, &ctx);

  Object::UpdateType rc = NO_CHANGE; // if force, rc = UPDATE anyway.
  if (force || xUpdated || usedUpdated) {
    _isValid = FillY(force);
    rc = UPDATE;
  }
  v = _yOutVector;
  if (rc == UPDATE) {
    v->setDirty();
  }
  v->update(update_counter);

  unlockInputsAndOutputs();

  return setLastUpdateResult(rc);
}


void Equation::save(QXmlStreamWriter &s) {
  s.writeStartElement("equation");
  s.writeAttribute("tag", tag().tagString());
  // Reparse the equation, then write it back out in text so that we can update
  // any vectors or scalars that had name changes, but we don't get affected by
  // the optimizer
  if (!_equation.isEmpty()) {
    QMutexLocker ml(&Equations::mutex());
    yy_scan_string(_equation.toLatin1());
    ParsedEquation = 0L;
    int rc = yyparse(store());
    Equations::Node *en = static_cast<Equations::Node*>(ParsedEquation);
    if (rc == 0 && en) {
      if (!en->takeVectors(VectorsUsed)) {
        Debug::self()->log(i18n("Equation [%1] failed to find its vectors when saving.  Resulting Kst file may have issues.").arg(_equation), Debug::Warning);
      }
      QString etext = en->text();
      s.writeAttribute("expression", etext);
    }
    delete en;
    ParsedEquation = 0L;
  }

  s.writeAttribute("xvector", _xInVector->tag().tagString());
  if (_doInterp) {
    s.writeAttribute("interpolate", "true");
  }

  s.writeEndElement();
}


void Equation::setEquation(const QString& in_fn) {
  // assert(*_xVector); - ugly, we have to allow this here due to
  // document loading with vector lazy-loading
  setDirty();
  _equation = in_fn;

  VectorsUsed.clear();
  ScalarsUsed.clear();

  _ns = 2; // reset the updating
  delete _pe;
  _pe = 0L;
  if (!_equation.isEmpty()) {
    Equations::mutex().lock();
    yy_scan_string(_equation.toLatin1());
    int rc = yyparse(store());
    _pe = static_cast<Equations::Node*>(ParsedEquation);
    if (rc == 0 && _pe) {
      ParsedEquation = 0L;
      Equations::mutex().unlock();
      Equations::Context ctx;
      ctx.sampleCount = _ns;
      ctx.xVector = _xInVector;
      Equations::FoldVisitor vis(&ctx, &_pe);
      StringMap sm;

      if (_pe->collectObjects(VectorsUsed, ScalarsUsed, sm)) {
        _pe->update(-1, &ctx);
      } else {
        //we have bad objects...
        Debug::self()->log(i18n("Equation [%1] references non-existent objects.").arg(_equation), Debug::Error);
        delete (Equations::Node*)ParsedEquation;
        ParsedEquation = 0L;
        Equations::mutex().unlock();
      }
    } else {
      // Parse error
      Debug::self()->log(i18n("Equation [%1] failed to parse.  Errors follow.").arg(_equation), Debug::Warning);
      for (QStringList::ConstIterator i = Equations::errorStack.begin(); i != Equations::errorStack.end(); ++i) {
        Debug::self()->log(i18n("Parse Error: %1").arg(*i), Debug::Warning);
      }
      delete (Equations::Node*)ParsedEquation;
      ParsedEquation = 0L;
      _pe = 0L;
      Equations::mutex().unlock();
    }
  }
  _isValid = _pe != 0L;
}


void Equation::setExistingXVector(VectorPtr in_xv, bool do_interp) {
  VectorPtr v = _inputVectors[XINVECTOR];
  if (v == in_xv) {
    return;
  }

  setDirty();

  _inputVectors.remove(XINVECTOR);
  _xInVector = in_xv;
  _inputVectors.insert(XINVECTOR, in_xv);

  _ns = 2; // reset the updating
  _doInterp = do_interp;
}


/************************************************************************/
/*                                                                      */
/*                      Fill Y: Evaluates the equation                  */
/*                                                                      */
/************************************************************************/
bool Equation::FillY(bool force) {
  int v_shift=0, v_new;
  int i0=0;
  int ns;

  writeLockInputsAndOutputs();

  // determine value of Interp
  if (_doInterp) {
    ns = _xInVector->length();
    for (VectorMap::ConstIterator i = VectorsUsed.begin(); i != VectorsUsed.end(); ++i) {
      if (i.value()->length() > ns) {
        ns = i.value()->length();
      }
    }
  } else {
    ns = _xInVector->length();
  }

  if (_ns != _xInVector->length() || ns != _xInVector->length() ||
      _xInVector->numShift() != _xInVector->numNew()) {
    _ns = ns;

    VectorPtr xv = _xOutVector;
    VectorPtr yv = _yOutVector;
    if (!xv->resize(_ns)) {
      // FIXME: handle error?
      unlockInputsAndOutputs();
      return false;
    }
    if (!yv->resize(_ns)) {
      // FIXME: handle error?
      unlockInputsAndOutputs();
      return false;
    }
    yv->zero();
    i0 = 0; // other vectors may have diffent lengths, so start over
    v_shift = _ns;
  } else {
    // calculate shift and new samples
    // only do shift optimization if all used vectors are same size and shift
    v_shift = _xInVector->numShift();
    v_new = _xInVector->numNew();

    for (VectorMap::ConstIterator i = VectorsUsed.begin(); i != VectorsUsed.end(); ++i) {
      if (v_shift != i.value()->numShift()) {
        v_shift = _ns;
      }
      if (v_new != i.value()->numNew()) {
        v_shift = _ns;
      }
      if (_ns != i.value()->length()) {
        v_shift = _ns;
      }
    }

    if (v_shift > _ns/2 || force) {
      i0 = 0;
      v_shift = _ns;
    } else {
      VectorPtr xv = _xOutVector;
      VectorPtr yv = _yOutVector;
      for (int i = v_shift; i < _ns; i++) {
        yv->value()[i - v_shift] = yv->value()[i];
        xv->value()[i - v_shift] = xv->value()[i];
      }
      i0 = _ns - v_shift;
    }
  }

  _numShifted = _yOutVector->numShift() + v_shift;
  if (_numShifted > _ns) {
    _numShifted = _ns;
  }

  _numNew = _ns - i0 + _yOutVector->numNew();
  if (_numNew > _ns) {
    _numNew = _ns;
  }

  _xOutVector->setNewAndShift(_numNew, _numShifted);
  _yOutVector->setNewAndShift(_numNew, _numShifted);

  double *rawxv = _xOutVector->value();
  double *rawyv = _yOutVector->value();
  VectorPtr iv = _xInVector;

  Equations::Context ctx;
  ctx.sampleCount = _ns;
  ctx.xVector = iv;

  if (!_pe) {
    if (_equation.isEmpty()) {
      unlockInputsAndOutputs();
      return true;
    }

    QMutexLocker ml(&Equations::mutex());
    yy_scan_string(_equation.toLatin1());
    int rc = yyparse(store());
    _pe = static_cast<Equations::Node*>(ParsedEquation);
    if (_pe && rc == 0) {
      Equations::FoldVisitor vis(&ctx, &_pe);
      StringMap sm;
      _pe->collectObjects(VectorsUsed, ScalarsUsed, sm);
      ParsedEquation = 0L;
    } else {
      delete (Equations::Node*)ParsedEquation;
      ParsedEquation = 0L;
      _pe = 0L;
      unlockInputsAndOutputs();
      return false;
    }
  }

  for (ctx.i = i0; ctx.i < _ns; ++ctx.i) {
    rawxv[ctx.i] = iv->value(ctx.i);
    ctx.x = iv->interpolate(ctx.i, _ns);
    rawyv[ctx.i] = _pe->value(&ctx);
  }

  if (!_xOutVector->resize(iv->length())) {
    // FIXME: handle error?
    unlockInputsAndOutputs();
    return false;
  }

  unlockInputsAndOutputs();
  return true;
}


QString Equation::propertyString() const {
  return equation();
}


void Equation::showNewDialog() {
  DialogLauncher::self()->showEquationDialog();
}


void Equation::showEditDialog() {
  DialogLauncher::self()->showEquationDialog(this);
}


DataObjectPtr Equation::makeDuplicate(DataObjectDataObjectMap& duplicatedMap) {
  // FIXME: implement this
  return 0L;
#if 0
  QString name(tagName() + '\'');
  while (Data::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  EquationPtr eq = new Equation(name, _equation, _inputVectors[XINVECTOR], _doInterp);
  duplicatedMap.insert(this, DataObjectPtr(eq));
  return DataObjectPtr(eq);
#endif
}


void Equation::replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject) {

  QString newExp = _equation;

  // replace all occurences of outputVectors, outputScalars from oldObject
  for (VectorMap::Iterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    QString oldTag = j.value()->tag().tagString();
    QString newTag = ((newObject->outputVectors())[j.key()])->tag().tagString();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  for (ScalarMap::Iterator j = oldObject->outputScalars().begin(); j != oldObject->outputScalars().end(); ++j) {
    QString oldTag = j.value()->tag().tagString();
    QString newTag = ((newObject->outputScalars())[j.key()])->tag().tagString();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  // and dependencies on matrix stats (there won't be matrices themselves in the expression)
  for (MatrixMap::Iterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      QString oldTag = scalarDictIter.value()->tag().tagString();
      QString newTag = ((((newObject->outputMatrices())[j.key()])->scalars())[scalarDictIter.key()])->tag().tagString();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
    }
  }

  // only replace _inputVectors
  for (VectorMap::Iterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    for (VectorMap::Iterator k = _inputVectors.begin(); k != _inputVectors.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputVectors[k.key()] = (newObject->outputVectors())[j.key()];
      }
    }
    // and dependencies on vector stats
    QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      QString oldTag = scalarDictIter.value()->tag().tagString();
      QString newTag = ((((newObject->outputVectors())[j.key()])->scalars())[scalarDictIter.key()])->tag().tagString();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
    }
  }

  setEquation(newExp);
}


void Equation::replaceDependency(VectorPtr oldVector, VectorPtr newVector) {
  QString oldTag = oldVector->tag().tagString();
  QString newTag = newVector->tag().tagString();

  // replace all occurences of oldTag with newTag
  QString newExp = _equation.replace("["+oldTag+"]", "["+newTag+"]");

  // also replace all occurences of scalar stats for the oldVector
  QHashIterator<QString, Scalar*> scalarDictIter(oldVector->scalars());
  while (scalarDictIter.hasNext()) {
    scalarDictIter.next();
    QString oldTag = scalarDictIter.value()->tag().tagString();
    QString newTag = ((newVector->scalars())[scalarDictIter.key()])->tag().tagString();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  setEquation(newExp);

  // do the dependency replacements for _inputVectors, but don't call parent function as it
  // replaces _inputScalars
  for (VectorMap::Iterator j = _inputVectors.begin(); j != _inputVectors.end(); ++j) {
    if (j.value() == oldVector) {
      _inputVectors[j.key()] = newVector;
    }
  }
}


void Equation::replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix) {

  QString newExp = _equation;

  // also replace all occurences of scalar stats for the oldMatrix
  QHashIterator<QString, Scalar*> scalarDictIter(oldMatrix->scalars());
  while (scalarDictIter.hasNext()) {
    scalarDictIter.next();
    QString oldTag = scalarDictIter.value()->tag().tagString();
    QString newTag = ((newMatrix->scalars())[scalarDictIter.key()])->tag().tagString();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  setEquation(newExp);
}


bool Equation::uses(ObjectPtr p) const {

  // check VectorsUsed in addition to _input*'s
  if (VectorPtr vect = kst_cast<Vector>(p)) {
    for (VectorMap::ConstIterator j = VectorsUsed.begin(); j != VectorsUsed.end(); ++j) {
      if (j.value() == vect) {
        return true;
      }
    }
  } else if (DataObjectPtr obj = kst_cast<DataObject>(p) ) {
    // check all connections from this expression to p
    for (VectorMap::Iterator j = obj->outputVectors().begin(); j != obj->outputVectors().end(); ++j) {
      for (VectorMap::ConstIterator k = VectorsUsed.begin(); k != VectorsUsed.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }
  }
  return DataObject::uses(p);
}

}
// vim: ts=2 sw=2 et
