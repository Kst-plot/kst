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

#include "testeqparser.h"

#include <QtTest>

#include <math_kst.h>
#include <kst_inf.h>
#include <enodes.h>
#include <eparse-eh.h>
#include <objectstore.h>
#include <generatedvector.h>

KSTMATH_EXPORT extern /*"C"*/ int yyparse(Kst::ObjectStore *store);
KSTMATH_EXPORT extern /*"C"*/ void *ParsedEquation;
KSTMATH_EXPORT extern /*"C"*/ struct yy_buffer_state *yy_scan_string(const char*);

bool optimizerFailed = false;

Kst::VectorMap vectorsUsed;
Kst::VectorPtr xVector;


#include <datacollection.h>
#include <objectstore.h>

static Kst::ObjectStore _store;
double _NOPOINT = NAN;

void TestEqParser::cleanupTestCase() {
  xVector = 0L;
  vectorsUsed.clear();
  _store.clear();
}


bool TestEqParser::validateText(const char *equation, const char *expect) {
  bool failure = false;
  QString txt;
  yy_scan_string(equation);
  int rc = yyparse(&_store);
  if (rc == 0) {
    vectorsUsed.clear();
    Equations::Node *eq = static_cast<Equations::Node*>(ParsedEquation);
    if (!eq)
      return false;
    ParsedEquation = 0L;
    //eq->collectVectors(vectorsUsed);
    txt = eq->text();
    failure = txt != expect;
    delete eq;
  } else {
    // Parse error
    delete (Equations::Node*)ParsedEquation;
    ParsedEquation = 0L;
    failure = true;
  }

  if (failure) {
    if (!Equations::errorStack.isEmpty()) {
      printf("Failures on [%s] -------------------------\n", equation);
      for (QStringList::ConstIterator i = Equations::errorStack.constBegin(); i != Equations::errorStack.constEnd(); ++i) {
        printf("%s\n", (*i).toLatin1().data());
      }
      printf("------------------------------------------\n");
    } else {
      printf("Got [%s], expected [%s]\n", txt.toLatin1().data(), expect);
    }
  }
  return !failure;
}


bool TestEqParser::validateEquation(const char *equation, double x, double result, const double tol) {
  yy_scan_string(equation);
  int rc = yyparse(&_store);
  if (rc == 0) {
    vectorsUsed.clear();
    Equations::Node *eq = static_cast<Equations::Node*>(ParsedEquation);
    if (!eq)
      return false;
    ParsedEquation = 0L;
    Equations::Context ctx;
    ctx.sampleCount = 2;
    ctx.noPoint = _NOPOINT;
    ctx.x = x;
    ctx.xVector = xVector;
    if (xVector) {
      ctx.sampleCount = xVector->length();
    }
    Equations::FoldVisitor vis(&ctx, &eq);
    if (eq->isConst() && !dynamic_cast<Equations::Number*>(eq)) {
      if (!optimizerFailed) {
        optimizerFailed = true;
        printf("Optimizer bug: found an unoptimized const equation.  Optimizing for coverage purposes.\n");
      }
      double v = eq->value(&ctx);
      delete eq;
      eq = new Equations::Number(v);
    }
    Kst::ScalarMap scm;
    Kst::StringMap stm;
    eq->collectObjects(vectorsUsed, scm, stm);
    eq->update(&ctx);
    double v = eq->value(&ctx);
    delete eq;
    if (fabs(v - result) < tol || (result != result && v != v) || (result == INF && v == INF) || (result == -INF && v == -INF)) {
      return true;
    } else {
      printf("Result: %.16f\n", v);
      return false;
    }
  } else {
    // Parse error
    printf("Failures on [%s] -------------------------\n", equation);
    for (QStringList::ConstIterator i = Equations::errorStack.constBegin(); i != Equations::errorStack.constEnd(); ++i) {
      printf("%s\n", (*i).toLatin1().data());
    }
    printf("------------------------------------------\n");
    delete (Equations::Node*)ParsedEquation;
    ParsedEquation = 0L;
    return false;
  }
}


bool TestEqParser::validateParserFailures(const char *equation) {
  bool success = true;
  yy_scan_string(equation);
  if (0 == yyparse(&_store)) {
    printf("Test of (%s) parsing passed, but should have failed.\n", equation);
    success = false;
  } else {
    if (Equations::errorStack.count() == 1 && (Equations::errorStack.first() == "parse error" || Equations::errorStack.first() == "syntax error")) {
      printf("ERROR: [%s] doesn't have error handling yet!\n", equation);
      success = false;
#ifdef DUMP_FAIL_MSGS
    } else {
      printf("Failures on [%s] -------------------------\n", equation);
      for (QStringList::ConstIterator i = Equations::errorStack.constBegin(); i != Equations::errorStack.constEnd(); ++i) {
        printf("%s\n", (*i).toLatin1().data());
      }
      printf("------------------------------------------\n");
#endif
    }
  }
  return success;
}



void TestEqParser::testEqParser() {

  // Base cases
  QVERIFY(validateEquation("0", 1.0, 0.0));
  QVERIFY(validateEquation("1.0", 2.0, 1.0));

  // Basics
  QVERIFY(validateEquation("x", -1.0, -1.0));
  QVERIFY(validateEquation("-x", -1.0, 1.0));
  QVERIFY(validateEquation("x^2", -1.0, 1.0));
  QVERIFY(validateEquation("     x^2", -1.0, 1.0));
  QVERIFY(validateEquation("     x^2    	   ", -1.0, 1.0));
  QVERIFY(validateEquation("x^2	", -1.0, 1.0));
  QVERIFY(validateEquation("x^2	+	x^2", -1.0, 2.0));
  QVERIFY(validateEquation("y", 0.0, _NOPOINT));
  QVERIFY(validateEquation("foo()", 0.0, _NOPOINT));
  QVERIFY(validateEquation("foo(1.0, 2.0, 3.0)", 0.0, _NOPOINT));

  // Order of evaluation and parser issues
  QVERIFY(validateEquation("-x^2", 2.0, -4.0));
  QVERIFY(validateEquation("(-x)^2", 2.0, 4.0));
  QVERIFY(validateEquation("-(x^2)", 2.0, -4.0));
  QVERIFY(validateEquation("(-x^2)", 2.0, -4.0));
  QVERIFY(validateEquation("x*x+5", 2.0, 9.0));
  QVERIFY(validateEquation("5+x*x", 2.0, 9.0));
  QVERIFY(validateEquation("5*4/2*5", 1.0, 50.0));
  QVERIFY(validateEquation("asin(sin(x))", 3.14159265358979323, 0.0));
  QVERIFY(validateEquation("x^(1/2)", 2.0, sqrt(2.0)));
  QVERIFY(validateEquation("x^(1/2)*2", 2.0, 2.0*sqrt(2.0)));
  QVERIFY(validateEquation("(1/2)*x^2+3*x-5", 1.0, -1.5));
  QVERIFY(validateEquation("2^3^4", 0.0, 2417851639229258349412352.0));
  QVERIFY(validateEquation("sin(x)^2", 0.5*3.14159265358979323, 1.0));
  QVERIFY(validateEquation("(2)^(3)", 0.0, 8));
  QVERIFY(validateEquation("(2*3)^(3*4)", 0.0, 2176782336.0));
  QVERIFY(validateEquation("sin(x^2)", sqrt(3.14159265358979323), 0.0));
  QVERIFY(validateEquation("5*4/2", 0.0, 10.0));
  QVERIFY(validateEquation("5/4*2", 0.0, 2.5));
  QVERIFY(validateEquation("10%2", 0.0, 0.0));
  QVERIFY(validateEquation("10.5%2", 0.0, 0.5));
  QVERIFY(validateEquation("0%2", 0.0, 0.0));
  QVERIFY(validateEquation("2%0", 0.0, _NOPOINT));
  QVERIFY(validateEquation("--x", 1.0, 1.0));
  QVERIFY(validateEquation("--x", -1.0, -1.0));
  QVERIFY(validateEquation("---x", -1.0, 1.0));
  QVERIFY(validateEquation("---x", 1.0, -1.0));

  // Constants
  QVERIFY(validateEquation("e", 0.0, 2.7128182846));
  QVERIFY(validateEquation("pi", 0.0, 3.1415926536));

  // Functions
  QVERIFY(validateEquation("sin()", 0.0, _NOPOINT));
  QVERIFY(validateEquation("sin(0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("sin(3.14159265358979323)", 0.0, 0.0));
  QVERIFY(validateEquation("sin(3.14159265358979323/2.00000000000000000000000000)", 0.0, 1.0));
  QVERIFY(validateEquation("cos()", 0.0, _NOPOINT));
  QVERIFY(validateEquation("cos(0.0)", 0.0, 1.0));
  QVERIFY(validateEquation("cos(3.14159265358979323)", 0.0, -1.0));
  QVERIFY(validateEquation("cos(3.14159265358979323/2.00000000000000000000000000)", 0.0, 0.0));

  QVERIFY(validateEquation("sec(0.2332744)", 0.0, 1.0278393504553487));
  QVERIFY(validateEquation("csc(0.2332744)", 0.0, 4.3259243283703714));
  QVERIFY(validateEquation("cot(0.2332744)", 0.0, 4.2087553141976128));
  QVERIFY(validateEquation("1/cos(0.2332744)", 0.0, 1.0278393504553487));
  QVERIFY(validateEquation("1/sin(0.2332744)", 0.0, 4.3259243283703714));
  QVERIFY(validateEquation("1/tan(0.2332744)", 0.0, 4.2087553141976128));

  QVERIFY(validateEquation("sec(0.2332744) == 1/cos(0.2332744)", 0.2332744, 1.0));
  QVERIFY(validateEquation("csc(0.2332744) == 1/sin(0.2332744)", 0.2332744, 1.0));
  QVERIFY(validateEquation("cot(0.2332744) == 1/tan(0.2332744)", 0.2332744, 1.0));

  QVERIFY(validateEquation("sec(x) == 1/cos(x)", 0.2332744, 1.0));
  QVERIFY(validateEquation("csc(x) == 1/sin(x)", 0.2332744, 1.0));
  QVERIFY(validateEquation("cot(x) == 1/tan(x)", 0.2332744, 1.0));

  QVERIFY(validateEquation("abs(0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("abs(x)", 1.0, 1.0));
  QVERIFY(validateEquation("abs(x)", -1.0, 1.0));
  QVERIFY(validateEquation("abs(x)", _NOPOINT, _NOPOINT));
  QVERIFY(validateEquation("abs(x)", INF, INF));
  QVERIFY(validateEquation("abs(x)", -INF, INF));
  QVERIFY(validateEquation("abs(-0.000000000001)", 0.0, 0.000000000001));

  QVERIFY(validateEquation("cos(acos(x))", 0.3875823288, 0.3875823288, 0.0000000001));
  QVERIFY(validateEquation("acos(cos(x))", 2.3875823288, 2.3875823288, 0.0000000001));
  QVERIFY(validateEquation("asin(sin(x))", 0.7540103248, 0.7540103248, 0.0000000001));
  QVERIFY(validateEquation("sin(asin(x))", 0.3875823288, 0.3875823288, 0.0000000001));
  QVERIFY(validateEquation("tan(atan(x))", 2.3875823288, 2.3875823288, 0.0000000001));
  QVERIFY(validateEquation("atan(tan(x))", 0.3875823288, 0.3875823288, 0.0000000001));

  QVERIFY(validateEquation("sqrt(4) == 2.0", 0.0, 1.0));
  QVERIFY(validateEquation("sqrt(0) == 0.0", 0.0, 1.0));
  QVERIFY(validateEquation("sqrt(-1)", 0.0, _NOPOINT));
  QVERIFY(validateEquation("sqrt(2)", 0.0, 1.4142135623730951));

#ifndef Q_WS_WIN32
  QVERIFY(validateEquation("cbrt(0.0) == 0.0", 0.0, 1.0));
  QVERIFY(validateEquation("cbrt(8.0) == 2.0", 0.0, 1.0));
  QVERIFY(validateEquation("cbrt(-1)", 0.0, -1.0));
  QVERIFY(validateEquation("cbrt(2)", 0.0, 1.2599210498948734));
#endif

  // TODO: cosh, exp, log, ln, sinh, tanh

  // Expressions / Comparison
  QVERIFY(validateEquation("0.0>0.0", 0.0, 0.0));
  QVERIFY(validateEquation("0.0>=0.0", 0.0, 1.0));
  QVERIFY(validateEquation("0.0<0.0", 0.0, 0.0));
  QVERIFY(validateEquation("0.0<=0.0", 0.0, 1.0));
  QVERIFY(validateEquation("0.0=0.0", 0.0, 1.0));
  QVERIFY(validateEquation("0.0!=0.0", 0.0, 0.0));
  QVERIFY(validateEquation("1.0!=0.0", 0.0, 1.0));
  QVERIFY(validateEquation("sin(1.0)!=sin(0.0)", 0.0, 1.0));
  QVERIFY(validateEquation("sin()!=sin()", 0.0, 1.0));
  QVERIFY(validateEquation("0.0==0.0", 0.0, 1.0));
  QVERIFY(validateEquation("1.0>0.0", 0.0, 1.0));
  QVERIFY(validateEquation("1.0>=0.0", 0.0, 1.0));
  QVERIFY(validateEquation("0.0>1.0", 0.0, 0.0));
  QVERIFY(validateEquation("0.0>=1.0", 0.0, 0.0));
  QVERIFY(validateEquation("1.0<0.0", 0.0, 0.0));
  QVERIFY(validateEquation("1.0<=0.0", 0.0, 0.0));
  QVERIFY(validateEquation("0.0<1.0", 0.0, 1.0));
  QVERIFY(validateEquation("0.0<=1.0", 0.0, 1.0));
  QVERIFY(validateEquation("(0.0/0.0)==(0.0/0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("(0.0/0.0)==(1.0/0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("(1.0/0.0)==(1.0/0.0)", 0.0, 1.0)); // INF == INF
  QVERIFY(validateEquation("(1.0/0.0)==-(1.0/0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("(0.0/0.0)==-(1.0/0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("(1.0/0.0)==(0.0/0.0)", 0.0, 0.0));
  QVERIFY(validateEquation("1&&1", 0.0, 1.0));
  QVERIFY(validateEquation("1&&0", 0.0, 0.0));
  QVERIFY(validateEquation("0&&1", 0.0, 0.0));
  QVERIFY(validateEquation("0&&2", 0.0, 0.0));
  QVERIFY(validateEquation("3&&2", 0.0, 1.0));
  QVERIFY(validateEquation("1||1", 0.0, 1.0));
  QVERIFY(validateEquation("0||1", 0.0, 1.0));
  QVERIFY(validateEquation("1||0", 0.0, 1.0));
  QVERIFY(validateEquation("0||0", 0.0, 0.0));
  QVERIFY(validateEquation("2||4", 0.0, 1.0));
  QVERIFY(validateEquation("1||(2&&0)", 0.0, 1.0));
  QVERIFY(validateEquation("(1||2)&&0", 0.0, 0.0));
  QVERIFY(validateEquation("1||2&&0", 0.0, 1.0));
  QVERIFY(validateEquation("2&&0||1", 0.0, 1.0));
  QVERIFY(validateEquation("2&&1||0", 0.0, 1.0));
  QVERIFY(validateEquation("0||1&&0", 0.0, 0.0));
  QVERIFY(validateEquation("1.0 == (1.0 == 1.0)", 0.0, 1.0));
  QVERIFY(validateEquation("1.0 != (1.0 == 0.0)", 0.0, 1.0));
  QVERIFY(validateEquation("1.0 != (1.0 == 1.0)", 0.0, 0.0));
  QVERIFY(validateEquation("0.0 == (1.0 == 0.0)", 0.0, 1.0));
  QVERIFY(validateEquation("-1==1", 0.0, 0.0));
  QVERIFY(validateEquation("-1==-1", 0.0, 1.0));
  QVERIFY(validateEquation("1==-1", 0.0, 0.0));
  QVERIFY(validateEquation("1!=-1", 0.0, 1.0));
  QVERIFY(validateEquation("-1!=1", 0.0, 1.0));
  QVERIFY(validateEquation("-1!=-1", 0.0, 0.0));
  QVERIFY(validateEquation("!0.0", 0.0, 1.0));
  QVERIFY(validateEquation("!1.0", 0.0, 0.0));
  QVERIFY(validateEquation("!-1.0", 0.0, 0.0));
  QVERIFY(validateEquation("!2.0", 0.0, 0.0));
  QVERIFY(validateEquation("!x", INF, 0.0));
  QVERIFY(validateEquation("!x", _NOPOINT, 1.0));
  QVERIFY(validateEquation("!(1 > 2)", 0.0, 1.0));
  QVERIFY(validateEquation("!1.0 > -1.0", 0.0, 1.0));  // (!1.0) > -1.0
  QVERIFY(validateEquation("!!x", 1.0, 1.0));
  QVERIFY(validateEquation("!!!x", 1.0, 0.0));
  QVERIFY(validateEquation("!!!!x", 1.0, 1.0));

  // Bit operations
  QVERIFY(validateEquation("32&4", 0.0, 0.0));
  QVERIFY(validateEquation("32&4|2", 0.0, 2.0));
  QVERIFY(validateEquation("32|4&2", 0.0, 0.0));
  QVERIFY(validateEquation("32|4", 0.0, 36.0));
  QVERIFY(validateEquation("0&257", 0.0, 0.0));
  QVERIFY(validateEquation("257&0", 0.0, 0.0));
  QVERIFY(validateEquation("257|0", 0.0, 257.0));
  QVERIFY(validateEquation("0|257", 0.0, 257.0));
  QVERIFY(validateEquation("-1|257", 0.0, -1));
  QVERIFY(validateEquation("257|-1", 0.0, -1));

  // Scalars
  Kst::ScalarPtr s = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
  s->setValue(1.0);
  s->setOrphan(true);
  s->setDescriptiveName("test1");
  s = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
  s->setValue(0.0);
  s->setOrphan(true);
  s->setDescriptiveName("test2");
  s = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
  s->setValue(-1.0);
  s->setOrphan(true);
  s->setDescriptiveName("test3");
  s = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
  s->setValue(_NOPOINT);
  s->setOrphan(true);
  s->setDescriptiveName("test4");
  s = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
  s->setValue(INF);
  s->setOrphan(true);
  s->setDescriptiveName("test5");
  s = Kst::kst_cast<Kst::Scalar>(_store.createObject<Kst::Scalar>());
  s->setValue(-INF);
  s->setOrphan(true);
  s->setDescriptiveName("sdf");

  QVERIFY(validateEquation("[test1]", 0.0, 1.0));
  QVERIFY(validateEquation("[test4]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[test4] - [test4]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[test4] - [test5]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[test4]*[test5]", 0.0, _NOPOINT));

  QVERIFY(validateEquation("[test1 (X1)]", 0.0, 1.0));
  QVERIFY(validateEquation("[test4 (X4)]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[test4 (X4)] - [test4 (X4)]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[test4 (X4)] - [test5 (X5)]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[test4 (X4)]*[test5 (X5)]", 0.0, _NOPOINT));

  QVERIFY(validateEquation("[X1]", 0.0, 1.0));
  QVERIFY(validateEquation("[X4]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[X4] - [X4]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[X4] - [X5]", 0.0, _NOPOINT));
  QVERIFY(validateEquation("[X4]*[X5]", 0.0, _NOPOINT));

  QVERIFY(validateEquation("[sdf]", 0.0, -INF));

  QVERIFY(validateEquation("[=10+10]", 0.0, 20.0));

  // Vectors
  Kst::GeneratedVectorPtr gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(0, 1.0, 10);
  gv->setDescriptiveName("gvector1");
  gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(0, 1.0, 10);
  gv->setDescriptiveName("gvector2");
  gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(1.0, 2.0, 10);
  gv->setDescriptiveName("gvector3");
  gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(0, 1.0, 2);
  gv->setDescriptiveName("gvector4");
  gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(-1.0, 1.0, 1000);
  gv->setDescriptiveName("gvector5");
  gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(-1.0, 1.0, 1000);
  gv->setDescriptiveName("gvector6");

  QVERIFY(validateEquation("[gvector3] - [gvector2]", 0.0, 1.0));
  QVERIFY(validateEquation("[gvector3[9]]", 0.0, 2.0));
  QVERIFY(validateEquation("[gvector3[5+4]]", 0.0, 2.0));
  QVERIFY(validateEquation("[gvector3[]]", 0.0, 1.0));
  QVERIFY(validateEquation("2*sin([gvector6])", 0.0, -1.6829419696157930));

  QVERIFY(validateEquation("[gvector3 (V3)] - [gvector2 (V2)]", 0.0, 1.0));
  QVERIFY(validateEquation("[gvector3 (V3)[9]]", 0.0, 2.0));
  QVERIFY(validateEquation("[gvector3 (V3)[5+4]]", 0.0, 2.0));
  QVERIFY(validateEquation("[gvector3 (V3)[]]", 0.0, 1.0));
  QVERIFY(validateEquation("2*sin([gvector6 (V6)])", 0.0, -1.6829419696157930));


  QVERIFY(validateEquation("[V3] - [V2]", 0.0, 1.0));
  QVERIFY(validateEquation("[V3[9]]", 0.0, 2.0));
  QVERIFY(validateEquation("[V3[5+4]]", 0.0, 2.0));
  QVERIFY(validateEquation("[V3[]]", 0.0, 1.0));
  QVERIFY(validateEquation("2*sin([V6])", 0.0, -1.6829419696157930));

  // TODO: interpolation, more vector combinations

  // Plugins
  QEXPECT_FAIL("", "Plugins in equations are not implemented yet", Continue);
  QVERIFY(validateEquation("2*plugin(bin, [V4], 12)", 1.0, -1.9779779779779778));
  QEXPECT_FAIL("", "Plugins in equations are not implemented yet", Continue);
  QVERIFY(validateEquation("4*plugin(bin, [V4], x)", 5.0, -3.9839839839839839));
  QVERIFY(validateEquation("-3*plugin(bin, x, 12)", 2.0, _NOPOINT));
  gv = Kst::kst_cast<Kst::GeneratedVector>(_store.createObject<Kst::GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(0, 100, 2000);
  xVector = gv;
  QEXPECT_FAIL("", "Plugins in equations are not implemented yet", Continue);
  QVERIFY(validateEquation("-3*plugin(bin, x, 12)", 2.0, -0.8254127063531767));
  QVERIFY(validateEquation("-3*plugin(bin, y, 12)", 2.0, _NOPOINT));

  // TODO: more plugin tests

  // TODO: float notation

  // Combinations
  QVERIFY(validateEquation("1|0&&0", 0.0, 0.0));
  QVERIFY(validateEquation("0&&0|1", 0.0, 0.0));
  QVERIFY(validateEquation("0&&1|0", 0.0, 0.0));
  QVERIFY(validateEquation("0||1&1", 0.0, 1.0));
  QVERIFY(validateEquation("0||2&1", 0.0, 0.0));
  QVERIFY(validateEquation("2&1||0", 0.0, 0.0));
  QVERIFY(validateEquation("1-1||0", 0.0, 0.0));
  QVERIFY(validateEquation("2-2||0", 0.0, 0.0));
  QVERIFY(validateEquation("0||2-2", 0.0, 0.0));
  QVERIFY(validateEquation("8|2*2", 0.0, 12.0));
  QVERIFY(validateEquation("2*2|8", 0.0, 12.0));

  QVERIFY(validateEquation("[gvector2] > 0.0", 0.0, 0.0));
  QVERIFY(validateEquation("[gvector2] > -1.0", 0.0, 1.0));

  QVERIFY(validateEquation("[gvector2 (V2)] > 0.0", 0.0, 0.0));
  QVERIFY(validateEquation("[gvector2 (V2)] > -1.0", 0.0, 1.0));

  QVERIFY(validateEquation("[V2] > 0.0", 0.0, 0.0));
  QVERIFY(validateEquation("[V2] > -1.0", 0.0, 1.0));

  QVERIFY(validateEquation("-([gvector2]*sin([gvector2]*[gvector3])+[gvector4]*cos([gvector4]*[gvector4]))", 0.0, 0.0));
  QVERIFY(validateEquation("[gvector4] * -1", 0.0, 0.0));

  QVERIFY(validateEquation("-([gvector2 (V2)]*sin([gvector2 (V2)]*[gvector3 (V3)])+[gvector4 (V4)]*cos([gvector4 (V4)]*[gvector4 (V4)]))", 0.0, 0.0));
  QVERIFY(validateEquation("[gvector4 (V4)] * -1", 0.0, 0.0));

  QVERIFY(validateEquation("-([V2]*sin([V2]*[V3])+[V4]*cos([V4]*[V4]))", 0.0, 0.0));
  QVERIFY(validateEquation("[V4] * -1", 0.0, 0.0));

  QVERIFY(validateText("3*x", "3*x"));
  QVERIFY(validateText("(3*x)", "(3*x)"));
  QVERIFY(validateText("3*(x)", "3*x"));
  QVERIFY(validateText("((3*x))", "(3*x)"));
  QVERIFY(validateText(" ((3 * x)) ", "(3*x)"));
  QVERIFY(validateText(" ((3 * x)) ", "(3*x)"));
  QVERIFY(validateText("(-3)", "(-3)"));
  QVERIFY(validateText("(x)", "x"));
  QVERIFY(validateText("(3*(-(x+5)))", "(3*(-(x+5)))"));
  QVERIFY(validateText("(sin((x)))", "sin(x)"));

  /*  Wrap a testcase with this and run bison with -t in order to get a trace
   *  of the parse stack */
#if 0
  yydebug = 1;
  yydebug = 0;
#endif
  // Errors:
  QVERIFY(validateParserFailures(""));
  QVERIFY(validateParserFailures(" "));
  QVERIFY(validateParserFailures("\t"));
  QVERIFY(validateParserFailures(" \t \t"));
  QVERIFY(validateParserFailures("[]"));
  QVERIFY(validateParserFailures("[[]"));
  QVERIFY(validateParserFailures("[]]"));
  QVERIFY(validateParserFailures("]"));
  QVERIFY(validateParserFailures("["));
  QVERIFY(validateParserFailures("]["));
  QVERIFY(validateParserFailures("[]["));
  QVERIFY(validateParserFailures("foo(, 3)"));
  QVERIFY(validateParserFailures("foo(3,)"));
  QVERIFY(validateParserFailures("foo(3,,5)"));
  QVERIFY(validateParserFailures("foo([])"));
  QVERIFY(validateParserFailures("foo(4, [])"));
  QVERIFY(validateParserFailures("/"));
  QVERIFY(validateParserFailures("/2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2/"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2//2"));
  QVERIFY(validateParserFailures("%"));
  QVERIFY(validateParserFailures("%2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2%"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2%%2"));
  QVERIFY(validateParserFailures("|"));
  QVERIFY(validateParserFailures("||"));
  QVERIFY(validateParserFailures("|2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2|"));
  QVERIFY(validateParserFailures("||2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2||"));
  QVERIFY(validateParserFailures("2|||2"));
  QVERIFY(validateParserFailures("&"));
  QVERIFY(validateParserFailures("&&"));
  QVERIFY(validateParserFailures("&2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2&"));
  QVERIFY(validateParserFailures("&&2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2&&"));
  QVERIFY(validateParserFailures("2&&&2"));
  QVERIFY(validateParserFailures("*"));
  QVERIFY(validateParserFailures("*2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2*"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2**2"));
  QVERIFY(validateParserFailures("^"));
  QVERIFY(validateParserFailures("^2"));
  QVERIFY(validateParserFailures("2^^2"));
  QVERIFY(validateParserFailures("2^"));
  QVERIFY(validateParserFailures("+"));
  QVERIFY(validateParserFailures("+2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2+"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2++2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("-"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2-"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("-2-"));
  QVERIFY(validateParserFailures("2!"));
  QVERIFY(validateParserFailures("!"));
  QVERIFY(validateParserFailures("()"));
  QVERIFY(validateParserFailures("2()"));
  QVERIFY(validateParserFailures("()2"));
  QVERIFY(validateParserFailures("_"));
  QVERIFY(validateParserFailures("#"));
  QVERIFY(validateParserFailures("$"));
  QVERIFY(validateParserFailures(")"));
  QVERIFY(validateParserFailures("("));
  QVERIFY(validateParserFailures(")("));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2&|2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2&&||2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2&&+2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2+&&2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2*&&2"));
  QVERIFY(validateParserFailures("2&&*2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2<>2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2=<2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2=>2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2><2"));
  QVERIFY(validateParserFailures("<"));
  QVERIFY(validateParserFailures("<2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2<"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2<<2"));
  QVERIFY(validateParserFailures(">"));
  QVERIFY(validateParserFailures(">2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2>"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2>>2"));
  QVERIFY(validateParserFailures(">="));
  QVERIFY(validateParserFailures(">=2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2>="));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2>=>=2"));
  QVERIFY(validateParserFailures("<="));
  QVERIFY(validateParserFailures("<=2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2<="));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2<=<=2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2<==2"));
  QVERIFY(validateParserFailures("."));
  QVERIFY(validateParserFailures(".2"));
  QVERIFY(validateParserFailures("2."));
  QVERIFY(validateParserFailures(","));
  QVERIFY(validateParserFailures(",2"));
  QEXPECT_FAIL("", "This has always failed", Continue);
  QVERIFY(validateParserFailures("2,")); // Doesn't give a specific error - how to catch this?
  QVERIFY(validateParserFailures("2*sin(x"));
  QVERIFY(validateParserFailures("2*sin(x)()"));
}

// vim: ts=2 sw=2 et
