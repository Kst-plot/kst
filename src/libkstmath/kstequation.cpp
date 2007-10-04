/***************************************************************************
                          kstequation.cpp: Equations for KST
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

#include <qtextdocument.h>
#include <QXmlStreamWriter>

#include "defaultprimitivenames.h"
#include "dialoglauncher.h"
#include "enodes.h"
#include "eparse-eh.h"
#include "datacollection.h"
#include "debug.h"
#include "kstequation.h"
#include "kst_i18n.h"
#include "generatedvector.h"

extern "C" int yyparse();
extern "C" void *ParsedEquation;
extern "C" struct yy_buffer_state *yy_scan_string(const char*);

const QString KstEquation::XINVECTOR = "X";
const QString KstEquation::XOUTVECTOR = "XO"; // Output (slave) vector
const QString KstEquation::YOUTVECTOR = "O"; // Output (slave) vector


KstEquation::KstEquation(const QString& in_tag, const QString& equation, double x0, double x1, int nx)
: Kst::DataObject() {

  Kst::VectorPtr xvector;
  QString vtag = Kst::suggestVectorName(QString( "(%1..%2)" ).arg( x0 ).arg( x1 ) );

  xvector = new Kst::GeneratedVector(x0, x1, nx, Kst::ObjectTag(vtag, QStringList(in_tag)));

  _doInterp = false;
  _xInVector = _inputVectors.insert(XINVECTOR, xvector);

  commonConstructor(in_tag, equation);
  setDirty();
}


KstEquation::KstEquation(const QString& in_tag, const QString& equation, Kst::VectorPtr xvector, bool do_interp)
: Kst::DataObject() {
  _doInterp = do_interp; //false;
  _xInVector = _inputVectors.insert(XINVECTOR, xvector);

  commonConstructor(in_tag, equation);
  setDirty();
}


KstEquation::~KstEquation() {
  delete _pe;
  _pe = 0L;
}


void KstEquation::attach() {
}


void KstEquation::commonConstructor(const QString& in_tag, const QString& in_equation) {
  _ns = 2;
  _pe = 0L;
  _typeString = i18n("Equation");
  _type = "Equation";
  Kst::Object::setTagName(Kst::ObjectTag::fromString(in_tag));

  Kst::VectorPtr xv = new Kst::Vector(Kst::ObjectTag("xsv", tag()), 2, this);
  _xOutVector = _outputVectors.insert(XOUTVECTOR, xv);
    
  Kst::VectorPtr yv = new Kst::Vector(Kst::ObjectTag("sv", tag()), 2, this);
  _yOutVector = _outputVectors.insert(YOUTVECTOR, yv);

  _isValid = false;
  _numNew = _numShifted = 0;

  setEquation(in_equation);
}


const KstCurveHintList *KstEquation::curveHints() const {
  _curveHints->clear();
  _curveHints->append(new KstCurveHint(i18n("Equation Curve"), 
                      (*_xOutVector)->tagName(), (*_yOutVector)->tagName()));
  return _curveHints;
}


bool KstEquation::isValid() const {
  return _isValid;
}


Kst::Object::UpdateType KstEquation::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  bool xUpdated = false;
  bool usedUpdated = false;

  if (Kst::Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  if (!_pe) {
    return setLastUpdateResult(NO_CHANGE);
  }

  assert(update_counter >= 0);

  if (_xInVector == _inputVectors.end()) {
    _xInVector = _inputVectors.find(XINVECTOR);
    if (!*_xInVector) { // This is technically sort of fatal
      return setLastUpdateResult(NO_CHANGE);
    }
  }

  writeLockInputsAndOutputs();

  Kst::VectorPtr v = *_xInVector;

  xUpdated = Kst::Object::UPDATE == v->update(update_counter);

  Equation::Context ctx;
  ctx.sampleCount = _ns;
  ctx.xVector = v;
  usedUpdated = _pe && Kst::Object::UPDATE == _pe->update(update_counter, &ctx);

  Kst::Object::UpdateType rc = NO_CHANGE; // if force, rc = UPDATE anyway.
  if (force || xUpdated || usedUpdated) {
    _isValid = FillY(force);
    rc = UPDATE;
  }
  v = *_yOutVector;
  if (rc == UPDATE) {
    v->setDirty();
  }
  v->update(update_counter);

  unlockInputsAndOutputs();

  return setLastUpdateResult(rc);
}


void KstEquation::save(QXmlStreamWriter &s) {
  s.writeStartElement("equation");
  s.writeAttribute("tag", tag().tagString());
  // Reparse the equation, then write it back out in text so that we can update
  // any vectors or scalars that had name changes, but we don't get affected by
  // the optimizer
  if (!_equation.isEmpty()) {
    QMutexLocker ml(&Equation::mutex());
    yy_scan_string(_equation.toLatin1());
    ParsedEquation = 0L;
    int rc = yyparse();
    Equation::Node *en = static_cast<Equation::Node*>(ParsedEquation);
    if (rc == 0 && en) {
      if (!en->takeVectors(VectorsUsed)) {
        Kst::Debug::self()->log(i18n("Equation [%1] failed to find its vectors when saving.  Resulting Kst file may have issues.").arg(_equation), Kst::Debug::Warning);
      }
      QString etext = en->text();
      s.writeAttribute("expression", etext);
    }
    delete en;
    ParsedEquation = 0L;
  }

  s.writeAttribute("xvector", (*_xInVector)->tag().tagString());
  if (_doInterp) {
    s.writeAttribute("interpolate", "true");
  }

  s.writeEndElement();
}


void KstEquation::setEquation(const QString& in_fn) {
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
    Equation::mutex().lock();
    yy_scan_string(_equation.toLatin1());
    int rc = yyparse();
    _pe = static_cast<Equation::Node*>(ParsedEquation);
    if (rc == 0 && _pe) {
      ParsedEquation = 0L;
      Equation::mutex().unlock();
      Equation::Context ctx;
      ctx.sampleCount = _ns;
      ctx.xVector = *_xInVector;
      Equation::FoldVisitor vis(&ctx, &_pe);
      KstStringMap sm;

      if (_pe->collectObjects(VectorsUsed, ScalarsUsed, sm)) {
        _pe->update(-1, &ctx);
      } else {
        //we have bad objects...
        Kst::Debug::self()->log(i18n("Equation [%1] references non-existent objects.").arg(_equation), Kst::Debug::Error);
        delete (Equation::Node*)ParsedEquation;
        ParsedEquation = 0L;
        Equation::mutex().unlock();
      }
    } else {
      // Parse error
      Kst::Debug::self()->log(i18n("Equation [%1] failed to parse.  Errors follow.").arg(_equation), Kst::Debug::Warning);
      for (QStringList::ConstIterator i = Equation::errorStack.begin(); i != Equation::errorStack.end(); ++i) {
        Kst::Debug::self()->log(i18n("Parse Error: %1").arg(*i), Kst::Debug::Warning);
      }
      delete (Equation::Node*)ParsedEquation;
      ParsedEquation = 0L;
      _pe = 0L;
      Equation::mutex().unlock();
    }
  }
  _isValid = _pe != 0L;
}


void KstEquation::setExistingXVector(Kst::VectorPtr in_xv, bool do_interp) {
  Kst::VectorPtr v = _inputVectors[XINVECTOR];
  if (v == in_xv) {
    return;
  }

  setDirty();

  _inputVectors.remove(XINVECTOR);
  _xInVector = _inputVectors.insert(XINVECTOR, in_xv);

  _ns = 2; // reset the updating
  _doInterp = do_interp;
}


void KstEquation::setTagName(const QString &in_tag) {
  Kst::ObjectTag newTag(in_tag, tag().context());  // FIXME: always the same context?

  if (newTag == tag()) {
    return;
  }

  Kst::Object::setTagName(newTag);
  (*_xOutVector)->setTagName(Kst::ObjectTag("xsv", tag()));
  (*_yOutVector)->setTagName(Kst::ObjectTag("sv", tag()));
}


/************************************************************************/
/*                                                                      */
/*                      Fill Y: Evaluates the equation                  */
/*                                                                      */
/************************************************************************/
bool KstEquation::FillY(bool force) {
  int v_shift=0, v_new;
  int i0=0;
  int ns;

  writeLockInputsAndOutputs();

  // determine value of Interp
  if (_doInterp) {
    ns = (*_xInVector)->length();
    for (Kst::VectorMap::ConstIterator i = VectorsUsed.begin(); i != VectorsUsed.end(); ++i) {
      if (i.value()->length() > ns) {
        ns = i.value()->length();
      }
    }
  } else {
    ns = (*_xInVector)->length();
  }

  if (_ns != (*_xInVector)->length() || ns != (*_xInVector)->length() ||
      (*_xInVector)->numShift() != (*_xInVector)->numNew()) {
    _ns = ns;

    Kst::VectorPtr xv = *_xOutVector;
    Kst::VectorPtr yv = *_yOutVector;
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
    v_shift = (*_xInVector)->numShift();
    v_new = (*_xInVector)->numNew();

    for (Kst::VectorMap::ConstIterator i = VectorsUsed.begin(); i != VectorsUsed.end(); ++i) {
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
      Kst::VectorPtr xv = *_xOutVector;
      Kst::VectorPtr yv = *_yOutVector;
      for (int i = v_shift; i < _ns; i++) {
        yv->value()[i - v_shift] = yv->value()[i];
        xv->value()[i - v_shift] = xv->value()[i];
      }
      i0 = _ns - v_shift;
    }
  }

  _numShifted = (*_yOutVector)->numShift() + v_shift;
  if (_numShifted > _ns) {
    _numShifted = _ns;
  }

  _numNew = _ns - i0 + (*_yOutVector)->numNew();
  if (_numNew > _ns) {
    _numNew = _ns;
  }

  (*_xOutVector)->setNewAndShift(_numNew, _numShifted);
  (*_yOutVector)->setNewAndShift(_numNew, _numShifted);

  double *rawxv = (*_xOutVector)->value();
  double *rawyv = (*_yOutVector)->value();
  Kst::VectorPtr iv = (*_xInVector);

  Equation::Context ctx;
  ctx.sampleCount = _ns;
  ctx.xVector = iv;

  if (!_pe) {
    if (_equation.isEmpty()) {
      unlockInputsAndOutputs();
      return true;
    }

    QMutexLocker ml(&Equation::mutex());
    yy_scan_string(_equation.toLatin1());
    int rc = yyparse();
    _pe = static_cast<Equation::Node*>(ParsedEquation);
    if (_pe && rc == 0) {
      Equation::FoldVisitor vis(&ctx, &_pe);
      KstStringMap sm;
      _pe->collectObjects(VectorsUsed, ScalarsUsed, sm);
      ParsedEquation = 0L;
    } else {
      delete (Equation::Node*)ParsedEquation;
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

  if (!(*_xOutVector)->resize(iv->length())) {
    // FIXME: handle error?
    unlockInputsAndOutputs();
    return false;    
  }

  unlockInputsAndOutputs();
  return true;
}


QString KstEquation::propertyString() const {
  return equation();
}


void KstEquation::showNewDialog() {
  Kst::DialogLauncher::self()->showEquationDialog();
}


void KstEquation::showEditDialog() {
  Kst::DialogLauncher::self()->showEquationDialog(this);
}


Kst::DataObjectPtr KstEquation::makeDuplicate(Kst::DataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (Kst::Data::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  KstEquationPtr eq = new KstEquation(name, _equation, _inputVectors[XINVECTOR], _doInterp);
  duplicatedMap.insert(this, Kst::DataObjectPtr(eq));
  return Kst::DataObjectPtr(eq);
}


void KstEquation::replaceDependency(Kst::DataObjectPtr oldObject, Kst::DataObjectPtr newObject) {
  
  QString newExp = _equation;
  
  // replace all occurences of outputVectors, outputScalars from oldObject
  for (Kst::VectorMap::Iterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    QString oldTag = j.value()->tagName();
    QString newTag = ((newObject->outputVectors())[j.key()])->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }
  
  for (Kst::ScalarMap::Iterator j = oldObject->outputScalars().begin(); j != oldObject->outputScalars().end(); ++j) {
    QString oldTag = j.value()->tagName();
    QString newTag = ((newObject->outputScalars())[j.key()])->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }
  
  // and dependencies on matrix stats (there won't be matrices themselves in the expression)
  for (Kst::MatrixMap::Iterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    QHashIterator<QString, Kst::Scalar*> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      QString oldTag = scalarDictIter.value()->tagName();
      QString newTag = ((((newObject->outputMatrices())[j.key()])->scalars())[scalarDictIter.key()])->tagName();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]"); 
    }
  }
  
  // only replace _inputVectors
  for (Kst::VectorMap::Iterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    for (Kst::VectorMap::Iterator k = _inputVectors.begin(); k != _inputVectors.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputVectors[k.key()] = (newObject->outputVectors())[j.key()]; 
      }
    }
    // and dependencies on vector stats
    QHashIterator<QString, Kst::Scalar*> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      QString oldTag = scalarDictIter.value()->tagName();
      QString newTag = ((((newObject->outputVectors())[j.key()])->scalars())[scalarDictIter.key()])->tagName();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]"); 
    }
  }
  
  setEquation(newExp);
}


void KstEquation::replaceDependency(Kst::VectorPtr oldVector, Kst::VectorPtr newVector) {
  QString oldTag = oldVector->tagName();
  QString newTag = newVector->tagName();
  
  // replace all occurences of oldTag with newTag
  QString newExp = _equation.replace("["+oldTag+"]", "["+newTag+"]");
  
  // also replace all occurences of scalar stats for the oldVector
  QHashIterator<QString, Kst::Scalar*> scalarDictIter(oldVector->scalars());
  while (scalarDictIter.hasNext()) {
    scalarDictIter.next();
    QString oldTag = scalarDictIter.value()->tagName();
    QString newTag = ((newVector->scalars())[scalarDictIter.key()])->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]"); 
  }
  
  setEquation(newExp);

  // do the dependency replacements for _inputVectors, but don't call parent function as it
  // replaces _inputScalars 
  for (Kst::VectorMap::Iterator j = _inputVectors.begin(); j != _inputVectors.end(); ++j) {
    if (j.value() == oldVector) {
      _inputVectors[j.key()] = newVector;  
    }      
  }
}


void KstEquation::replaceDependency(Kst::MatrixPtr oldMatrix, Kst::MatrixPtr newMatrix) {

  QString newExp = _equation;
  
  // also replace all occurences of scalar stats for the oldMatrix
  QHashIterator<QString, Kst::Scalar*> scalarDictIter(oldMatrix->scalars());
  while (scalarDictIter.hasNext()) {
    scalarDictIter.next();
    QString oldTag = scalarDictIter.value()->tagName();
    QString newTag = ((newMatrix->scalars())[scalarDictIter.key()])->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]"); 
  }
  
  setEquation(newExp);
}


bool KstEquation::uses(Kst::ObjectPtr p) const {
  
  // check VectorsUsed in addition to _input*'s
  if (Kst::VectorPtr vect = Kst::kst_cast<Kst::Vector>(p)) {
    for (Kst::VectorMap::ConstIterator j = VectorsUsed.begin(); j != VectorsUsed.end(); ++j) {
      if (j.value() == vect) {
        return true;
      }
    }
  } else if (Kst::DataObjectPtr obj = Kst::kst_cast<Kst::DataObject>(p) ) {
    // check all connections from this expression to p
    for (Kst::VectorMap::Iterator j = obj->outputVectors().begin(); j != obj->outputVectors().end(); ++j) {
      for (Kst::VectorMap::ConstIterator k = VectorsUsed.begin(); k != VectorsUsed.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }
  }
  return Kst::DataObject::uses(p);
}

// vim: ts=2 sw=2 et
