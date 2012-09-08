/***************************************************************************
                          equation.cpp: Equations for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
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

#include "equation.h"

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
#include "kst_i18n.h"
#include "generatedvector.h"
#include "objectstore.h"

/*extern "C"*/ int yyparse(Kst::ObjectStore *store);
extern void *ParsedEquation;
/*extern "C"*/ struct yy_buffer_state *yy_scan_string(const char*);
int yylex_destroy (void );

namespace Kst {

const QString Equation::staticTypeString = I18N_NOOP("Equation");
const QString Equation::staticTypeTag = I18N_NOOP("equation");

static const QLatin1String& XINVECTOR = QLatin1String("X");
static const QLatin1String& XOUTVECTOR = QLatin1String("XO"); // Output (slave) vector
static const QLatin1String& YOUTVECTOR = QLatin1String("O"); // Output (slave) vector

Equation::Equation(ObjectStore *store)
: DataObject(store), _doInterp(false), _xInVector(0) {

  _ns = 2;
  _pe = 0L;
  _typeString = i18n("Equation");
  _type = "Equation";
  _initializeShortName();

  Q_ASSERT(store);
  _xOutVector = store->createObject<Vector>();
  Q_ASSERT(_xOutVector);
  _xOutVector->setProvider(this);
  _xOutVector->setSlaveName("x");
  _xOutVector->resize(2);
  _outputVectors.insert(XOUTVECTOR, _xOutVector);

  _yOutVector = store->createObject<Vector>();
  Q_ASSERT(_yOutVector);
  _yOutVector->setProvider(this);
  _yOutVector->setSlaveName("y");
  _yOutVector->resize(2);
  _outputVectors.insert(YOUTVECTOR, _yOutVector);

  _isValid = false;
  _numNew = _numShifted = 0;
}


Equation::~Equation() {
  delete _pe;
  _pe = 0L;
}

void Equation::_initializeShortName() {
  _shortName = 'E'+QString::number(_enum);
  if (_enum>max_enum)
    max_enum = _enum;
  _enum++;
}


void Equation::attach() {
}


const CurveHintList *Equation::curveHints() const {
  _curveHints->clear();
  _curveHints->append(new CurveHint(i18n("Equation Curve"),
                      _xOutVector->shortName(), _yOutVector->shortName()));
  return _curveHints;
}


bool Equation::isValid() {
  return _isValid;
}


void Equation::internalUpdate() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  if (!_pe) {
    return;
  }

  writeLockInputsAndOutputs();

  Equations::Context ctx;
  ctx.sampleCount = _ns;
  ctx.xVector = _xInVector;

  _pe->update(&ctx);

  _isValid = FillY(true);

  unlockInputsAndOutputs();

  updateVectorLabels();

  return;
}

const QString Equation::reparsedEquation() const {
  QString etext;

  if (!_equation.isEmpty()) {
    if (!Equations::mutex().tryLock()) {
      qDebug() << "Don't reparse equation while it is being reparsed...";
      return (_equation);
    }

    yylex_destroy();
    yy_scan_string(_equation.toLatin1());
    ParsedEquation = 0L;
    int rc = yyparse(store());
    Equations::Node *en = static_cast<Equations::Node*>(ParsedEquation);
    if (rc == 0 && en) {
      if (!en->takeVectors(VectorsUsed)) {
        Debug::self()->log(i18n("Equation [%1] failed to find its vectors when reparsing.").arg(_equation), Debug::Warning);
      }
      etext = en->text();
    }
    delete en;
    ParsedEquation = 0L;
    Equations::mutex().unlock();
  }
  return (etext);
}

void Equation::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  // Reparse the equation, then write it back out in text so that we can update
  // any vectors or scalars that had name changes, but we don't get affected by
  // the optimizer
  if (!_equation.isEmpty()) {
    QMutexLocker ml(&Equations::mutex());
    yylex_destroy();
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

  if (_xInVector) {
    s.writeAttribute("xvector", _xInVector->Name());
  }
  if (_doInterp) {
    s.writeAttribute("interpolate", "true");
  }
  saveNameInfo(s, VNUM|ENUM|XNUM);
  s.writeEndElement();
}


void Equation::setEquation(const QString& in_fn) {
  // assert(*_xVector); - ugly, we have to allow this here due to
  // document loading with vector lazy-loading

  _equation = in_fn;

  VectorsUsed.clear();
  ScalarsUsed.clear();

  _ns = 2; // reset the updating
  delete _pe;
  _pe = 0L;
  if (!_equation.isEmpty()) {
    Equations::mutex().lock();
    yylex_destroy();
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
        _pe->update(&ctx);
      } else {
        //we have bad objects...
        Debug::self()->log(i18n("Equation [%1] references non-existent objects.").arg(_equation), Debug::Error);
        delete (Equations::Node*)ParsedEquation;
        ParsedEquation = 0L;
        //Equations::mutex().unlock();
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

  if (_isValid) {
    _equation = reparsedEquation(); // update the string
  }
}

void Equation::updateVectorLabels() {
  if (!_xInVector) {
    return;
  }

  _xOutVector->setLabelInfo(_xInVector->labelInfo());

  QString yl;
  QString xl;
  QRegExp sn("(\\(V(\\d{1,2})\\))|\\[|\\]"); // short name
  yl = reparsedEquation();
  yl.remove(sn);

  LabelInfo label_info;
  label_info.units.clear();
  label_info.quantity.clear();
  label_info.name = yl;

  _yOutVector->setTitleInfo(label_info);
}

void Equation::setExistingXVector(VectorPtr in_xv, bool do_interp) {
  if (!in_xv) {
    return;
  }

  VectorPtr v = _inputVectors[XINVECTOR];
  if (v == in_xv) {
    return;
  }

  _inputVectors.remove(XINVECTOR);
  _xInVector = in_xv;
  _inputVectors.insert(XINVECTOR, in_xv);

  _ns = 2; // reset the updating
  _doInterp = do_interp;
}

//FIXME: equations should not use ScalarsUsed and VectorsUsed:
//instead, they should use _inputScalars and _inputVectors only.
//This code, and used() is only here because they don't.
qint64 Equation::minInputSerial() const {
  qint64 minSerial = DataObject::minInputSerial();

  foreach (const VectorPtr &P, VectorsUsed) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (const ScalarPtr &P, ScalarsUsed) {
    minSerial = qMin(minSerial, P->serial());
  }
  return minSerial;
}

qint64 Equation::maxInputSerialOfLastChange() const {
  qint64 maxSerial = DataObject::maxInputSerialOfLastChange();

  foreach (VectorPtr P, VectorsUsed) {
    maxSerial = qMax(maxSerial, P->serialOfLastChange());
  }
  foreach (ScalarPtr P, ScalarsUsed) {
    maxSerial = qMax(maxSerial, P->serialOfLastChange());
  }
  return maxSerial;
}

PrimitiveList Equation::inputPrimitives() const {
  PrimitiveList primitive_list = DataObject::inputPrimitives();

  int n = VectorsUsed.count();
  for (int i = 0; i< n; i++) {
    primitive_list.append(kst_cast<Primitive>(VectorsUsed.values().at(i)));
  }

  n = ScalarsUsed.count();
  for (int i = 0; i< n; i++) {
    primitive_list.append(kst_cast<Primitive>(ScalarsUsed.values().at(i)));
  }

  return primitive_list;
}

void Equation::replaceInput(PrimitivePtr p, PrimitivePtr new_p) {
  DataObject::replaceInput(p, new_p);

  QString newExp = _equation;

  QString oldName = p->Name();
  QString newName = new_p->Name();
  newExp = newExp.replace('[' + oldName + ']', '[' + newName + ']');

  setEquation(newExp);

}

/************************************************************************/
/*                                                                      */
/*                      Fill Y: Evaluates the equation                  */
/*                                                                      */
/************************************************************************/
bool Equation::FillY(bool force) {
  if (!_xInVector) {
    return false;
  }

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
    yylex_destroy();
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


DataObjectPtr Equation::makeDuplicate() const {

  EquationPtr equation = store()->createObject<Equation>();
  Q_ASSERT(equation);

  equation->setEquation(_equation);
  equation->setExistingXVector(_inputVectors[XINVECTOR], _doInterp);

  if (descriptiveNameIsManual()) {
    equation->setDescriptiveName(descriptiveName());
  }
  equation->writeLock();
  equation->registerChange();
  equation->unlock();

  return DataObjectPtr(equation);
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

QString Equation::_automaticDescriptiveName() const {
  return _yOutVector->titleInfo().name;
}

QString Equation::descriptionTip() const {
  if(!_xInVector) {
    return QString();
  }
  return i18n("Equation: %1\n  %2\nX: %3").arg(Name()).arg(equation()).arg(_xInVector->descriptionTip());
}


}
// vim: ts=2 sw=2 et
