/***************************************************************************
                                 enodes.cpp
                                 ----------      
    begin                : Feb 12 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "enodes.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <qnamespace.h>
#ifndef Q_OS_WIN32
#include <unistd.h>
#else
#define strcasecmp _stricmp
#endif



#include <QMutex>
#include <QRegExp>

#include "datacollection.h"
#include "debug.h"
#include "math_kst.h"
#include "objectstore.h"
#include "doublecompare.h"

KSTMATH_EXPORT int yyparse(Kst::ObjectStore *store);
KSTMATH_EXPORT extern void *ParsedEquation;
KSTMATH_EXPORT/*"C"*/ struct yy_buffer_state *yy_scan_string(const char*);
KSTMATH_EXPORT struct yy_buffer_state *yy_scan_bytes(const char*, int);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
KSTMATH_EXPORT void yy_delete_buffer(YY_BUFFER_STATE);

using namespace Equations;
using namespace Kst;

static QMutex _mutex;

QMutex& Equations::mutex() {
  return _mutex;
}


double Equations::interpret(ObjectStore *store, const char *txt, bool *ok, int len) {
  if (!txt || !*txt) {
    if (ok) {
      *ok = false;
    }
    return 0.0;
  }

  mutex().lock();
  YY_BUFFER_STATE b;
  if (len > 0) {
    b = yy_scan_bytes(txt, len);
  } else {
    b = yy_scan_string(txt);
  }
  int rc = yyparse(store);
  yy_delete_buffer(b);
  if (rc == 0) {
    Equations::Node *eq = static_cast<Equations::Node*>(ParsedEquation);
    ParsedEquation = 0L;
    mutex().unlock();
    Equations::Context ctx;
    ctx.sampleCount = 2;
    ctx.noPoint = Kst::NOPOINT;
    ctx.x = 0.0;
    ctx.xVector = 0L;
    Equations::FoldVisitor vis(&ctx, &eq);
    double v = eq->value(&ctx);
    delete eq;
    if (ok) {
      *ok = true;
    }
    return v;
  } else {
    ParsedEquation = 0L;
    mutex().unlock();
    if (ok) {
      *ok = false;
    }
    return 0.0;
  }
}


Node::Node() {
  _parentheses = false;
}


Node::~Node() {
}


bool Node::collectObjects(Kst::VectorMap&, Kst::ScalarMap&, Kst::StringMap&) {
  return true;
}


bool Node::takeVectors(const Kst::VectorMap&) {
  return true;
}


void Node::visit(NodeVisitor* v) {
  v->visitNode(this);
}


Kst::Object::UpdateType Node::update(Context *ctx) {
  Q_UNUSED(ctx)
  return Kst::Object::NoChange;
}


/////////////////////////////////////////////////////////////////
BinaryNode::BinaryNode(Node *left, Node *right)
: Node(), _left(left), _right(right) {
}


BinaryNode::~BinaryNode() {
  delete _left;
  _left = 0L;
  delete _right;
  _right = 0L;
}


bool BinaryNode::collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t) {
  bool ok = true;
  ok = _left->collectObjects(v, s, t) ? ok : false;
  ok = _right->collectObjects(v, s, t) ? ok : false;
  return ok;
}


bool BinaryNode::takeVectors(const Kst::VectorMap& c) {
  bool rc = _left->takeVectors(c);
  rc = _right->takeVectors(c) && rc;
  return rc;
}


void BinaryNode::visit(NodeVisitor* v) {
  v->visitBinaryNode(this);
}


Node *& BinaryNode::left() {
  return _left;
}


Node *& BinaryNode::right() {
  return _right;
}


Kst::Object::UpdateType BinaryNode::update(Context *ctx) {
  Kst::Object::UpdateType l = _left->update(ctx);
  Kst::Object::UpdateType r = _right->update(ctx);

  return (l == Kst::Object::Updated || r == Kst::Object::Updated) ? Kst::Object::Updated : Kst::Object::NoChange;
}


/////////////////////////////////////////////////////////////////
Addition::Addition(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Addition: %p + %p\n", (void*)this, (void*)left, (void*)right);
}


Addition::~Addition() {
}


double Addition::value(Context *ctx) {
  return _left->value(ctx) + _right->value(ctx);
}


bool Addition::isConst() {
  return _left->isConst() && _right->isConst();
}


QString Addition::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + '+' + _right->text() + ')';
  } else {
    return _left->text() + '+' + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
Subtraction::Subtraction(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Subtraction: %p - %p\n", (void*)this, (void*)left, (void*)right);
}


Subtraction::~Subtraction() {
}


double Subtraction::value(Context *ctx) {
  return _left->value(ctx) - _right->value(ctx);
}


bool Subtraction::isConst() {
  return _left->isConst() && _right->isConst();
}


QString Subtraction::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + '-' + _right->text() + ')';
  } else {
    return _left->text() + '-' + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
Multiplication::Multiplication(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Multiplication: %p - %p\n", (void*)this, (void*)left, (void*)right);
}


Multiplication::~Multiplication() {
}


double Multiplication::value(Context *ctx) {
  return _left->value(ctx) * _right->value(ctx);
}


bool Multiplication::isConst() {
  return _left->isConst() && _right->isConst();
}


QString Multiplication::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + '*' + _right->text() + ')';
  } else {
    return _left->text() + '*' + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
Division::Division(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Division: %p - %p\n", (void*)this, (void*)left, (void*)right);
}


Division::~Division() {
}


double Division::value(Context *ctx) {
  return _left->value(ctx) / _right->value(ctx);
}


bool Division::isConst() {
  return _left->isConst() && _right->isConst();
}


QString Division::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + '/' + _right->text() + ')';
  } else {
    return _left->text() + '/' + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
Modulo::Modulo(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Modulo: %p - %p\n", (void*)this, (void*)left, (void*)right);
}


Modulo::~Modulo() {
}


double Modulo::value(Context *ctx) {
  return fmod(_left->value(ctx), _right->value(ctx));
}


bool Modulo::isConst() {
  return _left->isConst() && _right->isConst();
}


QString Modulo::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + '%' + _right->text() + ')';
  } else {
    return _left->text() + '%' + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
Power::Power(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Power: %p - %p\n", (void*)this, (void*)left, (void*)right);
}


Power::~Power() {
}


double Power::value(Context *ctx) {
  return pow(_left->value(ctx), _right->value(ctx));
}


bool Power::isConst() {
  return _left->isConst() && _right->isConst();
}


QString Power::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + '^' + _right->text() + ')';
  } else {
    return _left->text() + '^' + _right->text();
  }
}


/////////////////////////////////////////////////////////////////

static double cot(double x) {
  return 1.0/tan(x);
}


static double csc(double x) {
  return 1.0/sin(x);
}


static double sec(double x) {
  return 1.0/cos(x);
}

static double acosd(double x) {
  return 180.0/M_PI*acos(x);
}

static double asind(double x) {
  return 180.0/M_PI*asin(x);
}

static double atand(double x) {
  return 180.0/M_PI*atan(x);
}

static double cosd(double x) {
  return cos(fmod(x, 360.0)*M_PI/180.0);
}

static double cotd(double x) {
  return cot(fmod(x, 360.0)*M_PI/180.0);
}

static double cscd(double x) {
  return csc(fmod(x, 360.0)*M_PI/180.0);
}

static double secd(double x) {
  return sec(fmod(x, 360.0)*M_PI/180.0);
}

static double sind(double x) {
  return sin(fmod(x, 360.0)*M_PI/180.0);
}

static double tand(double x) {
  return tan(fmod(x, 360.0)*M_PI/180.0);
}

static double atanx(double *x) {
  return atan2(x[0],x[1]);
}

static double step(double x) {
  if (x>0.0) {
    return 1.0;
  } else {
    return 0.0;
  }
}


static struct {
  const char *name;
  double (*func)(double);
} FTable[] = {
  {"abs",   &fabs},
  {"acos",  &acos},
  {"acosd", &acosd},
  {"asin",  &asin},
  {"asind", &asind},
  {"atan",  &atan},
  {"atand", &atand},
#ifndef Q_OS_WIN32
  {"cbrt", &cbrt},
#endif
  {"cos",  &cos},
  {"cosd", &cosd},
  {"cosh", &cosh},
  {"cot",  &cot},
  {"cotd", &cotd},
  {"csc",  &csc},
  {"cscd", &cscd},
  {"exp",  &exp},
  {"log",  &log10},
  {"ln",   &log},
  {"sec",  &sec},
  {"secd", &secd},
  {"sin",  &sin},
  {"sind", &sind},
  {"sinh", &sinh},
  {"sqrt", &sqrt},
  {"step", &step},
  {"tan",  &tan},
  {"tand", &tand},
  {"tanh", &tanh},
  {0, 0}
};

static struct {
  const char *name;
  double (*func)(double*);
} F2Table[] = {

  {"atanx", &atanx},
  //{"atan2d", &atand2d},
  {0, 0}
};

Function::Function(char *name, ArgumentList *args)
: Node(), _name(name), _args(args), _f(0L) {
  _argCount = 1; // Presently no functions take != 1 argument
  _inPid = 0L;
  _inScalars = 0L;
  _inVectors = 0L;
  _outScalars = 0L;
  _outVectors = 0L;
  _inArrayLens = 0L;
  _outArrayLens = 0L;
  _outputIndex = -424242;
  _localData = 0L;
  _outputVectorCnt = 0;
  _inputVectorCnt = 0;
  //printf("%p: New Function: %s - %p\n", (void*)this, name, (void*)args);
  for (int i = 0; FTable[i].name; ++i) {
    if (strcasecmp(FTable[i].name, name) == 0) {
      _f = (void*)FTable[i].func;
      break;
    }
  }
  if (!_f) {
    for (int i = 0; F2Table[i].name; ++i) {
      if (strcasecmp(F2Table[i].name, name) == 0) {
        _f = (void*)F2Table[i].func;
        _argCount = 2;
        break;
      }
    }
  }
}


Function::~Function() {
  free(_name);
  _name = 0L;
  delete _args;
  _args = 0L;
  _f = 0L;
  delete[] _inScalars;
  delete[] _inVectors;
  delete[] _outScalars;
  for (int i = 0; i < _outputVectorCnt; ++i) {
    free(_outVectors[i]);
  }
  delete[] _outVectors;
  delete[] _inArrayLens;
  delete[] _outArrayLens;
}


Kst::Object::UpdateType Function::update(Context *ctx) {
  _args->update(ctx);
  return Kst::Object::NoChange;
}


double Function::value(Context *ctx) {
  if (!_f) {
    return ctx->noPoint;
  }

  if (_argCount == 1) {
    double x = _args->at(0, ctx);
    return ((double (*)(double))_f)(x);
  } else if (_argCount > 1) {
    double *x = new double[_argCount];
    for (int i = 0; i < _argCount; ++i) {
      x[i] = _args->at(i, ctx);
    }
    double r = ((double (*)(double*))_f)(x);
    delete[] x;
    return r;
  } else {
    return ((double (*)())_f)();
  }
}


bool Function::isConst() {
  return _args->isConst();
}


bool Function::collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t) {
  return _args->collectObjects(v, s, t);
}


bool Function::takeVectors(const Kst::VectorMap& c) {
  return _args->takeVectors(c);
}


QString Function::text() const {
  return QString::fromLatin1(_name) + '(' + _args->text() + ')';
}


/////////////////////////////////////////////////////////////////
ArgumentList::ArgumentList()
: Node() {
  //printf("%p: New Argument List\n", (void*)this);
}


ArgumentList::~ArgumentList() {
  qDeleteAll(_args);
}


void ArgumentList::appendArgument(Node *arg) {
  _args.append(arg);
}


double ArgumentList::at(int arg, Context *ctx) {
  Node *n = _args.value(arg); // catches out-of-bounds
  if (n) {
    return n->value(ctx);
  }
  return ctx->noPoint;
}


bool ArgumentList::isConst() {
  foreach (Node *i, _args) {
    if (!i->isConst()) {
      return false;
    }
  }
  return true;
}


bool ArgumentList::collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t) {
  bool ok = true;
  foreach (Node *i, _args) {
    ok = i->collectObjects(v, s, t) ? ok : false;
  }
  return ok;
}


bool ArgumentList::takeVectors(const Kst::VectorMap& c) {
  bool rc = true;
  foreach (Node *i, _args) {
    rc = i->takeVectors(c) && rc;
  }
  return rc;
}


Node *ArgumentList::node(int idx) {
  return _args.at(idx);
}


Kst::Object::UpdateType ArgumentList::update(Context *ctx) {
  bool updated = false;
  foreach (Node *i, _args) {
    updated = updated || Kst::Object::Updated == i->update(ctx);
  }
  return updated ? Kst::Object::Updated : Kst::Object::NoChange;
}


QString ArgumentList::text() const {
  QString rc;
  bool first = true;
  QListIterator<Node*> it(_args);
  while (it.hasNext()) {
    if (!first) {
      rc += ", ";
    } else {
      first = false;
    }
    rc += it.next()->text();
  }
  return rc;
}


/////////////////////////////////////////////////////////////////
static struct {
  const char *name;
  double value;
} ITable[] = {
  {"e", 2.7128182846},
  {"pi", 3.1415926536},
  {0, 0.0}
};

Identifier::Identifier(char *name)
: Node(), _name(name), _const(0L) {
  //printf("%p: New Identifier: %s\n", (void*)this, name);
  for (int i = 0; ITable[i].name; ++i) {
    if (strcasecmp(ITable[i].name, name) == 0) {
      _const = &ITable[i].value;
      break;
    }
  }
}


Identifier::~Identifier() {
  free(_name);
  _name = 0L;
}


const char *Identifier::name() const {
  return _name;
}


double Identifier::value(Context *ctx) {
  if (_const) {
    return *_const;
  } else if (_name[0] == 'x' && _name[1] == 0) {
    return ctx->x;
  } else {
    return ctx->noPoint;
  }
}


bool Identifier::isConst() {
  return _const != 0L || !(_name[0] == 'x' && _name[1] == 0);
}


QString Identifier::text() const {
  return _name;
}


/////////////////////////////////////////////////////////////////
DataNode::DataNode(ObjectStore *store, char *name)
: Node(), _store(store), _isEquation(false), _equation(0L) {
  //printf("%p: New Data Object: [%s]\n", (void*)this, name);
//  ObjectStore *store = 0L; // FIXME: initialize this
  Q_ASSERT(store);
  if (name[0] == '=') {
    _tagName = QString(&name[1]).trimmed();
    _isEquation = true;
  } else if (strchr(name, '[')) {
    _tagName = QString(name).trimmed();
    QRegExp re("(.*)\\[(.*)\\]");
    int hit = re.indexIn(_tagName);
    if (hit > -1 && re.captureCount() == 2) {
      _vector = kst_cast<Vector>(store->retrieveObject(re.cap(1)));
      if (_vector) {
        _vectorIndex = re.cap(2);
      }
    }
  } else {
    _tagName = QString(name).trimmed();
    ObjectPtr o = store->retrieveObject(_tagName);
    if (kst_cast<Vector>(o)) {
      _vector = kst_cast<Vector>(o);
    } else if (kst_cast<Scalar>(o)) {
      _scalar = kst_cast<Scalar>(o);
    }
  }
  free(name);
  name = 0L;
}


DataNode::~DataNode() {
  delete _equation;
  _equation = 0L;
}


double DataNode::value(Context *ctx) {
  if (_isEquation) {
    if (!_equation) {
      mutex().lock();
      YY_BUFFER_STATE b = yy_scan_bytes(_tagName.toLatin1(), _tagName.length());
      int rc = yyparse(_store);
      yy_delete_buffer(b);
      if (rc == 0 && ParsedEquation) {
        _equation = static_cast<Equations::Node*>(ParsedEquation);
        ParsedEquation = 0L;
        mutex().unlock();
        Equations::Context ctx;
        ctx.sampleCount = 2;
        ctx.noPoint = Kst::NOPOINT;
        ctx.x = 0.0;
        ctx.xVector = 0L;
        Equations::FoldVisitor vis(&ctx, &_equation);
      } else {
        ParsedEquation = 0L;
        mutex().unlock();
        _isEquation = false;
        return ctx->noPoint;
      }
    }
    return _equation->value(ctx);
  } else if (_vector) {
    if (!_equation && !_vectorIndex.isEmpty()) {
      mutex().lock();
      YY_BUFFER_STATE b = yy_scan_bytes(_vectorIndex.toLatin1(), _vectorIndex.length());
      int rc = yyparse(_store);
      yy_delete_buffer(b);
      if (rc == 0 && ParsedEquation) {
        _equation = static_cast<Equations::Node*>(ParsedEquation);
        ParsedEquation = 0L;
        mutex().unlock();
        Equations::Context ctx;
        ctx.sampleCount = 2;
        ctx.noPoint = Kst::NOPOINT;
        ctx.x = 0.0;
        ctx.xVector = 0L;
        Equations::FoldVisitor vis(&ctx, &_equation);
      } else {
        ParsedEquation = 0L;
        mutex().unlock();
        _vectorIndex.clear();
        _vector = 0L;
        return ctx->noPoint;
      }
    }
    if (_equation) {
      // Note: should we use a fresh context here?
      return _vector->value(int(_equation->value(ctx)));
    }
    return _vector->interpolate(ctx->i, ctx->sampleCount);
  } else if (_scalar) {
    return _scalar->value();
  } else {
    return ctx->noPoint;
  }
}


bool DataNode::isConst() {
  return (_isEquation && _equation) ? _equation->isConst() : false;
}


bool DataNode::collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t) {
  if (_isEquation) {
    if (_equation) {
      _equation->collectObjects(v, s, t);
    }
  } else if (_vector && !v.contains(_tagName)) {
    v.insert(_tagName, _vector);
  } else if (_scalar && !s.contains(_tagName)) {
    s.insert(_tagName, _scalar);
  } else if (!_scalar && !_vector) {
    Kst::Debug::self()->log(Kst::Debug::tr("Equation has unknown object [%1].").arg(_tagName), Kst::Debug::Error);
    return false;
  }
  return true;
}


bool DataNode::takeVectors(const Kst::VectorMap& c) {
  if (_isEquation) {
    if (_equation) {
      return _equation->takeVectors(c);
    }
    return false;
  }
  if (!_scalar) {
    if (c.contains(_tagName)) {
      _vector = c[_tagName];
    } else {
      return false;
    }
  }
  return true;
}


Kst::Object::UpdateType DataNode::update(Context *ctx) {
  Q_UNUSED(ctx)
  if (_isEquation) {
    if (_equation) {
      //return _equation->update(ctx);
    }
  } else if (_vector) {
    KstWriteLocker l(_vector);
    //return _vector->update();
  } else if (_scalar) {
    KstWriteLocker l(_scalar);
    //return _scalar->registerChange();
  }
  //return Kst::Object::NoChange;
  return Kst::Object::Updated;
}


// Hack alert: [ and ] in names confuse the parser, so strip them out.
QString DataNode::text() const {
  if (_isEquation) {
    return QString("[=") + _tagName + ']';
  } else if (_vector) {
    QString Name = _vector->Name();
    Name.remove("\\[").remove("\\]");
    return QString('[') + Name.remove('[').remove(']') + QString(']');
  } else if (_scalar) {
    QString Name = _scalar->Name();
    Name.remove("\\[").remove("\\]");
    return QString('[') + Name.remove('[').remove(']') + QString(']');
  } else {
    return QString();
  }
}

/////////////////////////////////////////////////////////////////
Number::Number(double n)
: Node(), _n(n) {
}


Number::~Number() {
}


double Number::value(Context*) {
  return _n;
}


bool Number::isConst() {
  return true;
}


QString Number::text() const {
  if (_parentheses) {
    return QString('(') + QString::number(_n, 'g', 15) + ')';
  } else {
    return QString::number(_n, 'g', 15);
  }
}


/////////////////////////////////////////////////////////////////
Negation::Negation(Node *node)
: Node(), _n(node) {
  //printf("%p: New Negation: %p\n", (void*)this, (void*)n);
}


Negation::~Negation() {
  delete _n;
  _n = 0L;
}


double Negation::value(Context *ctx) {
  double v = _n->value(ctx);
  return (v == v) ? -v : v;
}

bool Negation::collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t) {
  bool ok = _n->collectObjects(v, s, t);
  return ok;
}



bool Negation::isConst() {
  return _n->isConst();
}


QString Negation::text() const {
  if (_parentheses) {
    return QString("(-") + _n->text() + ')';
  } else {
    return QString('-') + _n->text();
  }
}


/////////////////////////////////////////////////////////////////
LogicalNot::LogicalNot(Node *node)
: Node(), _n(node) {
  //printf("%p: New LogicalNot: %p\n", (void*)this, (void*)n);
}


LogicalNot::~LogicalNot() {
  delete _n;
  _n = 0L;
}


double LogicalNot::value(Context *ctx) {
  double v = _n->value(ctx);
  return (v == v) ? (v == 0.0) : 1.0;
}


bool LogicalNot::isConst() {
  return _n->isConst();
}


QString LogicalNot::text() const {
  if (_parentheses) {
    return QString("(!") + _n->text() + ')';
  } else {
    return QString('!') + _n->text();
  }
}


/////////////////////////////////////////////////////////////////
BitwiseAnd::BitwiseAnd(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New And: %p & %p\n", (void*)this, (void*)left, (void*)right);
}


BitwiseAnd::~BitwiseAnd() {
}


double BitwiseAnd::value(Context *ctx) {
  return long(_left->value(ctx)) & long(_right->value(ctx));
}


bool BitwiseAnd::isConst() {
  return _left->isConst() && _right->isConst();
}


QString BitwiseAnd::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString('&') + _right->text() + ')';
  } else {
    return _left->text() + QString('&') + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
BitwiseOr::BitwiseOr(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Or: %p | %p\n", (void*)this, (void*)left, (void*)right);
}


BitwiseOr::~BitwiseOr() {
}


double BitwiseOr::value(Context *ctx) {
  return long(_left->value(ctx)) | long(_right->value(ctx));
}


bool BitwiseOr::isConst() {
  return _left->isConst() && _right->isConst();
}


QString BitwiseOr::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString('|') + _right->text() + ')';
  } else {
    return _left->text() + QString('|') + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
LogicalAnd::LogicalAnd(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New And: %p && %p\n", (void*)this, (void*)left, (void*)right);
}


LogicalAnd::~LogicalAnd() {
}


double LogicalAnd::value(Context *ctx) {
  return (_left->value(ctx) && _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool LogicalAnd::isConst() {
  return _left->isConst() && _right->isConst();
}


QString LogicalAnd::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString("&&") + _right->text() + ')';
  } else {
    return _left->text() + QString("&&") + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
LogicalOr::LogicalOr(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New Or: %p || %p\n", (void*)this, (void*)left, (void*)right);
}


LogicalOr::~LogicalOr() {
}


double LogicalOr::value(Context *ctx) {
  return (_left->value(ctx) || _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool LogicalOr::isConst() {
  return _left->isConst() && _right->isConst();
}


QString LogicalOr::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString("||") + _right->text() + ')';
  } else {
    return _left->text() + QString("||") + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
LessThan::LessThan(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New LessThan: %p < %p\n", (void*)this, (void*)left, (void*)right);
}


LessThan::~LessThan() {
}


double LessThan::value(Context *ctx) {
  return doubleLessThan(_left->value(ctx), _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool LessThan::isConst() {
  return _left->isConst() && _right->isConst();
}


QString LessThan::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString('<') + _right->text() + ')';
  } else {
    return _left->text() + QString('<') + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
LessThanEqual::LessThanEqual(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New LessThanEqual: %p <= %p\n", (void*)this, (void*)left, (void*)right);
}


LessThanEqual::~LessThanEqual() {
}


double LessThanEqual::value(Context *ctx) {
  return doubleLessThanEqual(_left->value(ctx), _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool LessThanEqual::isConst() {
  return _left->isConst() && _right->isConst();
}


QString LessThanEqual::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString("<=") + _right->text() + ')';
  } else {
    return _left->text() + QString("<=") + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
GreaterThan::GreaterThan(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New GreaterThan: %p > %p\n", (void*)this, (void*)left, (void*)right);
}


GreaterThan::~GreaterThan() {
}


double GreaterThan::value(Context *ctx) {
  return doubleGreaterThan(_left->value(ctx), _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool GreaterThan::isConst() {
  return _left->isConst() && _right->isConst();
}


QString GreaterThan::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString('>') + _right->text() + ')';
  } else {
    return _left->text() + QString('>') + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
GreaterThanEqual::GreaterThanEqual(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New GreaterThanEqual: %p >= %p\n", (void*)this, (void*)left, (void*)right);
}


GreaterThanEqual::~GreaterThanEqual() {
}


double GreaterThanEqual::value(Context *ctx) {
  return doubleGreaterThanEqual(_left->value(ctx), _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool GreaterThanEqual::isConst() {
  return _left->isConst() && _right->isConst();
}


QString GreaterThanEqual::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString(">=") + _right->text() + ')';
  } else {
    return _left->text() + QString(">=") + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
EqualTo::EqualTo(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New EqualTo: %p == %p\n", (void*)this, (void*)left, (void*)right);
}


EqualTo::~EqualTo() {
}


double EqualTo::value(Context *ctx) {
  return doubleEqual(_left->value(ctx), _right->value(ctx)) ? EQ_TRUE : EQ_FALSE;
}


bool EqualTo::isConst() {
  return _left->isConst() && _right->isConst();
}


QString EqualTo::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString("==") + _right->text() + ')';
  } else {
    return _left->text() + QString("==") + _right->text();
  }
}


/////////////////////////////////////////////////////////////////
NotEqualTo::NotEqualTo(Node *left, Node *right)
: BinaryNode(left, right) {
  //printf("%p: New NotEqualTo: %p != %p\n", (void*)this, (void*)left, (void*)right);
}


NotEqualTo::~NotEqualTo() {
}


double NotEqualTo::value(Context *ctx) {
  return (!doubleEqual(_left->value(ctx), _right->value(ctx))) ? EQ_TRUE : EQ_FALSE;
}


bool NotEqualTo::isConst() {
  return _left->isConst() && _right->isConst();
}


QString NotEqualTo::text() const {
  if (_parentheses) {
    return QString('(') + _left->text() + QString("!=") + _right->text() + ')';
  } else {
    return _left->text() + QString("!=") + _right->text();
  }
}


/////////////////////////////////////////////////////////////////

NodeVisitor::NodeVisitor() {
}


NodeVisitor::~NodeVisitor() {
}


/////////////////////////////////////////////////////////////////

FoldVisitor::FoldVisitor(Context* ctxIn, Node** rootNode) : NodeVisitor(), _ctx(ctxIn) {
  if ((*rootNode)->isConst() && dynamic_cast<Number*>(*rootNode) == 0L) {
    double v = (*rootNode)->value(ctxIn);
    delete *rootNode;
    *rootNode = new Number(v);
  } else {
    (*rootNode)->visit(this);
  }
  _ctx = 0L; // avoids context being marked as 'still reachable'
}


FoldVisitor::~FoldVisitor() {
}


void FoldVisitor::visitNode(Node*) {
  // useful?
}


void FoldVisitor::visitBinaryNode(BinaryNode *n) {
  if (n->left()->isConst() && dynamic_cast<Number*>(n->left()) == 0L) {
    double v = n->left()->value(_ctx);
    delete n->left();
    n->left() = new Number(v);
  } else {
    n->left()->visit(this);
  }

  if (n->right()->isConst() && dynamic_cast<Number*>(n->right()) == 0L) {
    double v = n->right()->value(_ctx);
    delete n->right();
    n->right() = new Number(v);
  } else {
    n->right()->visit(this);
  }
}

// vim: ts=2 sw=2 et
