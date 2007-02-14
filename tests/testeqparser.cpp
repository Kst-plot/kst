/*
 *  Copyright 2004, The University of Toronto
 *  Licensed under GPL.
 */

//#define DUMP_FAIL_MSGS

#include "ksttestcase.h"
#include <enodes.h>
#include <eparse-eh.h>

extern "C" int yyparse();
extern "C" void *ParsedEquation;
extern "C" struct yy_buffer_state *yy_scan_string(const char*);

bool optimizerFailed = false;

KstVectorMap vectorsUsed;
KstVectorPtr xVector;

int rc = KstTestSuccess;

void testText(const char *equation, const char *expect) {
  bool failure = false;
  QString txt;
  yy_scan_string(equation);
  int rc = yyparse();
  if (rc == 0) {
    vectorsUsed.clear();
    Equation::Node *eq = static_cast<Equation::Node*>(ParsedEquation);
    assert(eq);
    ParsedEquation = 0L;
    //eq->collectVectors(vectorsUsed);
    txt = eq->text();
    failure = txt != expect;
    delete eq;
  } else {
    // Parse error
    delete (Equation::Node*)ParsedEquation;
    ParsedEquation = 0L;
    failure = true;
  }

  if (failure) {
    if (!Equation::errorStack.isEmpty()) {
      printf("Failures on [%s] -------------------------\n", equation);
      for (QStringList::ConstIterator i = Equation::errorStack.constBegin(); i != Equation::errorStack.constEnd(); ++i) {
        printf("%s\n", (*i).latin1());
      }
      printf("------------------------------------------\n");
    } else {
      printf("Got [%s], expected [%s]\n", txt.latin1(), expect);
    }
    --rc;
  }
}


bool doTest(const char *equation, double x, double result, const double tol = 0.00000000001) {
  yy_scan_string(equation);
  int rc = yyparse();
  if (rc == 0) {
    vectorsUsed.clear();
    Equation::Node *eq = static_cast<Equation::Node*>(ParsedEquation);
    assert(eq);
    ParsedEquation = 0L;
    Equation::Context ctx;
    ctx.sampleCount = 2;
    ctx.noPoint = NOPOINT;
    ctx.x = x;
    ctx.xVector = xVector;
    if (xVector) {
      ctx.sampleCount = xVector->length();
    }
    Equation::FoldVisitor vis(&ctx, &eq);
    if (eq->isConst() && !dynamic_cast<Equation::Number*>(eq)) {
      if (!optimizerFailed) {
        optimizerFailed = true;
        ::rc--;
        printf("Optimizer bug: found an unoptimized const equation.  Optimizing for coverage purposes.\n");
      }
      double v = eq->value(&ctx);
      delete eq;
      eq = new Equation::Number(v);
    }
    KstScalarMap scm;
    KstStringMap stm;
    eq->collectObjects(vectorsUsed, scm, stm);
    eq->update(-1, &ctx);
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
    for (QStringList::ConstIterator i = Equation::errorStack.constBegin(); i != Equation::errorStack.constEnd(); ++i) {
      printf("%s\n", (*i).latin1());
    }
    printf("------------------------------------------\n");
    delete (Equation::Node*)ParsedEquation;
    ParsedEquation = 0L;
    return false;
  }
}


void test(const char *equation, double x, double result, const double tol = 0.00000000001) {
  if (!doTest(equation, x, result, tol)) {
    rc--;
    printf("Test of (%s)[%.16f] == %.16f failed.\n", equation, x, result);
  }
}


void testParseFail(const char *equation) {
  yy_scan_string(equation);
  if (0 == yyparse()) {
    printf("Test of (%s) parsing passed, but should have failed.\n", equation);
    rc--;
  } else {
    if (Equation::errorStack.count() == 1 && (Equation::errorStack.first() == "parse error" || Equation::errorStack.first() == "syntax error")) {
      printf("ERROR: [%s] doesn't have error handling yet!\n", equation);
      rc--;
#ifdef DUMP_FAIL_MSGS
    } else {
      printf("Failures on [%s] -------------------------\n", equation);
      for (QStringList::ConstIterator i = Equation::errorStack.constBegin(); i != Equation::errorStack.constEnd(); ++i) {
        printf("%s\n", (*i).latin1());
      }
      printf("------------------------------------------\n");
#endif
    }
  }
}


static void exitHelper() {
  xVector = 0L;
  vectorsUsed.clear();
  KST::vectorList.clear();
  KST::scalarList.clear();
}


extern int yydebug;

int main(int argc, char **argv) {
  atexit(exitHelper);

  KApplication app(argc, argv, "testeqparser", false, false);

  // Base cases
  test("0", 1.0, 0.0);
  test("1.0", 2.0, 1.0);

  // Basics
  test("x", -1.0, -1.0);
  test("-x", -1.0, 1.0);
  test("x^2", -1.0, 1.0);
  test("     x^2", -1.0, 1.0);
  test("     x^2    	   ", -1.0, 1.0);
  test("x^2	", -1.0, 1.0);
  test("x^2	+	x^2", -1.0, 2.0);
  test("y", 0.0, NOPOINT);
  test("foo()", 0.0, NOPOINT);
  test("foo(1.0, 2.0, 3.0)", 0.0, NOPOINT);

  // Order of evaluation and parser issues
  test("-x^2", 2.0, -4.0);
  test("(-x)^2", 2.0, 4.0);
  test("-(x^2)", 2.0, -4.0);
  test("(-x^2)", 2.0, -4.0);
  test("x*x+5", 2.0, 9.0);
  test("5+x*x", 2.0, 9.0);
  test("5*4/2*5", 1.0, 50.0);
  test("asin(sin(x))", 3.14159265358979323, 0.0);
  test("x^(1/2)", 2.0, sqrt(2.0));
  test("x^(1/2)*2", 2.0, 2.0*sqrt(2.0));
  test("(1/2)*x^2+3*x-5", 1.0, -1.5);
  test("2^3^4", 0.0, 2417851639229258349412352.0);
  test("sin(x)^2", 0.5*3.14159265358979323, 1.0);
  test("(2)^(3)", 0.0, 8);
  test("(2*3)^(3*4)", 0.0, 2176782336.0);
  test("sin(x^2)", sqrt(3.14159265358979323), 0.0);
  test("5*4/2", 0.0, 10.0);
  test("5/4*2", 0.0, 2.5);
  test("10%2", 0.0, 0.0);
  test("10.5%2", 0.0, 0.5);
  test("0%2", 0.0, 0.0);
  test("2%0", 0.0, NOPOINT);
  test("--x", 1.0, 1.0);
  test("--x", -1.0, -1.0);
  test("---x", -1.0, 1.0);
  test("---x", 1.0, -1.0);

  // Constants
  test("e", 0.0, 2.7128182846);
  test("pi", 0.0, 3.1415926536);

  // Functions
  test("sin()", 0.0, NOPOINT);
  test("sin(0.0)", 0.0, 0.0);
  test("sin(3.14159265358979323)", 0.0, 0.0);
  test("sin(3.14159265358979323/2.00000000000000000000000000)", 0.0, 1.0);
  test("cos()", 0.0, NOPOINT);
  test("cos(0.0)", 0.0, 1.0);
  test("cos(3.14159265358979323)", 0.0, -1.0);
  test("cos(3.14159265358979323/2.00000000000000000000000000)", 0.0, 0.0);
  test("sec(x) == 1/cos(x)", 0.2332744, 1.0);
  test("csc(x) == 1/sin(x)", 0.2332744, 1.0);
  test("cot(x) == 1/tan(x)", 0.2332744, 1.0);

  test("abs(0.0)", 0.0, 0.0);
  test("abs(x)", 1.0, 1.0);
  test("abs(x)", -1.0, 1.0);
  test("abs(x)", NOPOINT, NOPOINT);
  test("abs(x)", INF, INF);
  test("abs(x)", -INF, INF);
  test("abs(-0.000000000001)", 0.0, 0.000000000001);

  test("cos(acos(x))", 0.3875823288, 0.3875823288, 0.0000000001);
  test("acos(cos(x))", 2.3875823288, 2.3875823288, 0.0000000001);
  test("asin(sin(x))", 0.7540103248, 0.7540103248, 0.0000000001);
  test("sin(asin(x))", 0.3875823288, 0.3875823288, 0.0000000001);
  test("tan(atan(x))", 2.3875823288, 2.3875823288, 0.0000000001);
  test("atan(tan(x))", 0.3875823288, 0.3875823288, 0.0000000001);

  test("sqrt(4) == 2.0", 0.0, 1.0);
  test("sqrt(0) == 0.0", 0.0, 1.0);
  test("sqrt(-1)", 0.0, NOPOINT);
  test("sqrt(2)", 0.0, 1.4142135623730951);

  test("cbrt(0.0) == 0.0", 0.0, 1.0);
  test("cbrt(8.0) == 2.0", 0.0, 1.0);
  test("cbrt(-1)", 0.0, -1.0);
  test("cbrt(2)", 0.0, 1.2599210498948734);

  // TODO: cosh, exp, log, ln, sinh, tanh

  // Expressions / Comparison
  test("0.0>0.0", 0.0, 0.0);
  test("0.0>=0.0", 0.0, 1.0);
  test("0.0<0.0", 0.0, 0.0);
  test("0.0<=0.0", 0.0, 1.0);
  test("0.0=0.0", 0.0, 1.0);
  test("0.0!=0.0", 0.0, 0.0);
  test("1.0!=0.0", 0.0, 1.0);
  test("sin(1.0)!=sin(0.0)", 0.0, 1.0);
  test("sin()!=sin()", 0.0, 1.0);
  test("0.0==0.0", 0.0, 1.0);
  test("1.0>0.0", 0.0, 1.0);
  test("1.0>=0.0", 0.0, 1.0);
  test("0.0>1.0", 0.0, 0.0);
  test("0.0>=1.0", 0.0, 0.0);
  test("1.0<0.0", 0.0, 0.0);
  test("1.0<=0.0", 0.0, 0.0);
  test("0.0<1.0", 0.0, 1.0);
  test("0.0<=1.0", 0.0, 1.0);
  test("(0.0/0.0)==(0.0/0.0)", 0.0, 0.0);
  test("(0.0/0.0)==(1.0/0.0)", 0.0, 0.0);
  test("(1.0/0.0)==(1.0/0.0)", 0.0, 1.0); // inf == inf
  test("(1.0/0.0)==-(1.0/0.0)", 0.0, 0.0);
  test("(0.0/0.0)==-(1.0/0.0)", 0.0, 0.0);
  test("(1.0/0.0)==(0.0/0.0)", 0.0, 0.0);
  test("1&&1", 0.0, 1.0);
  test("1&&0", 0.0, 0.0);
  test("0&&1", 0.0, 0.0);
  test("0&&2", 0.0, 0.0);
  test("3&&2", 0.0, 1.0);
  test("1||1", 0.0, 1.0);
  test("0||1", 0.0, 1.0);
  test("1||0", 0.0, 1.0);
  test("0||0", 0.0, 0.0);
  test("2||4", 0.0, 1.0);
  test("1||(2&&0)", 0.0, 1.0);
  test("(1||2)&&0", 0.0, 0.0);
  test("1||2&&0", 0.0, 1.0);
  test("2&&0||1", 0.0, 1.0);
  test("2&&1||0", 0.0, 1.0);
  test("0||1&&0", 0.0, 0.0);
  test("1.0 == (1.0 == 1.0)", 0.0, 1.0);
  test("1.0 != (1.0 == 0.0)", 0.0, 1.0);
  test("1.0 != (1.0 == 1.0)", 0.0, 0.0);
  test("0.0 == (1.0 == 0.0)", 0.0, 1.0);
  test("-1==1", 0.0, 0.0);
  test("-1==-1", 0.0, 1.0);
  test("1==-1", 0.0, 0.0);
  test("1!=-1", 0.0, 1.0);
  test("-1!=1", 0.0, 1.0);
  test("-1!=-1", 0.0, 0.0);
  test("!0.0", 0.0, 1.0);
  test("!1.0", 0.0, 0.0);
  test("!-1.0", 0.0, 0.0);
  test("!2.0", 0.0, 0.0);
  test("!x", INF, 0.0);
  test("!x", NOPOINT, 1.0);
  test("!(1 > 2)", 0.0, 1.0);
  test("!1.0 > -1.0", 0.0, 1.0);  // (!1.0) > -1.0
  test("!!x", 1.0, 1.0);
  test("!!!x", 1.0, 0.0);
  test("!!!!x", 1.0, 1.0);

  // Bit operations
  test("32&4", 0.0, 0.0);
  test("32&4|2", 0.0, 2.0);
  test("32|4&2", 0.0, 0.0);
  test("32|4", 0.0, 36.0);
  test("0&257", 0.0, 0.0);
  test("257&0", 0.0, 0.0);
  test("257|0", 0.0, 257.0);
  test("0|257", 0.0, 257.0);
  test("-1|257", 0.0, -1);
  test("257|-1", 0.0, -1);

  // Scalars
  new KstScalar("test1", 0L, 1.0, true);
  new KstScalar("test2", 0L, 0.0, true);
  new KstScalar("test3", 0L, -1.0, true);
  new KstScalar("test4", 0L, NOPOINT, true);
  new KstScalar("test5", 0L, INF, true);
  new KstScalar("test6", 0L, -INF, true);

  test("[test1]", 0.0, 1.0);
  test("[test4]", 0.0, NOPOINT);
  test("[test4] - [test4]", 0.0, NOPOINT);
  test("[test4] - [test5]", 0.0, NOPOINT);
  test("[test4]*[test5]", 0.0, NOPOINT);
  test("[sdf]", 0.0, NOPOINT);

  test("[=10+10]", 0.0, 20.0);
  
  // Vectors
  KstVector::generateVector(0, 1.0, 10, "1");
  KstVector::generateVector(0, 1.0, 10, "V1");
  KstVector::generateVector(1.0, 2.0, 10, "V2");
  KstVector::generateVector(0, 1.0, 2, "V3");
  KstVector::generateVector(-1.0, 1.0, 1000, "V4");
  KstVector::generateVector(-1.0, 1.0, 1000, "V5-%+-_!");
  test("[V2] - [V1]", 0.0, 1.0);
  test("[V2[9]]", 0.0, 2.0);
  test("[V2[5+4]]", 0.0, 2.0);
  test("[V2[]]", 0.0, 1.0);
  test("2*sin([V5-%+-_!])", 0.0, -1.6829419696157930);
  // TODO: interpolation, more vector combinations

  // Plugins
  test("2*plugin(bin, [V4], 12)", 1.0, -1.9779779779779778);
  test("4*plugin(bin, [V4], x)", 5.0, -3.9839839839839839);
  test("-3*plugin(bin, x, 12)", 2.0, NOPOINT);
  xVector = KstVector::generateVector(0, 100, 2000, "XVector");
  test("-3*plugin(bin, x, 12)", 2.0, -0.8254127063531767);
  test("-3*plugin(bin, y, 12)", 2.0, NOPOINT);

  // TODO: more plugin tests

  // TODO: float notation

  // Combinations
  test("1|0&&0", 0.0, 0.0);
  test("0&&0|1", 0.0, 0.0);
  test("0&&1|0", 0.0, 0.0);
  test("0||1&1", 0.0, 1.0);
  test("0||2&1", 0.0, 0.0);
  test("2&1||0", 0.0, 0.0);
  test("1-1||0", 0.0, 0.0);
  test("2-2||0", 0.0, 0.0);
  test("0||2-2", 0.0, 0.0);
  test("8|2*2", 0.0, 12.0);
  test("2*2|8", 0.0, 12.0);
  test("[V1] > 0.0", 0.0, 0.0);
  test("[V1] > -1.0", 0.0, 1.0);
  test("[1] > 0.0", 0.0, 0.0);
  test("[1] > -1.0", 0.0, 1.0);

  test("-([V1]*sin([V1]*[V2])+[V3]*cos([V3]*[V3]))", 0.0, 0.0);
  test("[V3] * -1", 0.0, 0.0);

  /*  Wrap a testcase with this and run bison with -t in order to get a trace
   *  of the parse stack */
#if 0
  yydebug = 1;
  yydebug = 0;
#endif
  // Errors:
  testParseFail("");
  testParseFail(" ");
  testParseFail("\t");
  testParseFail(" \t \t");
  testParseFail("[]");
  testParseFail("[[]");
  testParseFail("[]]");
  testParseFail("]");
  testParseFail("[");
  testParseFail("][");
  testParseFail("[][");
  testParseFail("foo(, 3)");
  testParseFail("foo(3,)");
  testParseFail("foo(3,,5)");
  testParseFail("foo([])");
  testParseFail("foo(4, [])");
  testParseFail("/");
  testParseFail("/2");
  testParseFail("2/");
  testParseFail("2//2");
  testParseFail("%");
  testParseFail("%2");
  testParseFail("2%");
  testParseFail("2%%2");
  testParseFail("|");
  testParseFail("||");
  testParseFail("|2");
  testParseFail("2|");
  testParseFail("||2");
  testParseFail("2||");
  testParseFail("2|||2");
  testParseFail("&");
  testParseFail("&&");
  testParseFail("&2");
  testParseFail("2&");
  testParseFail("&&2");
  testParseFail("2&&");
  testParseFail("2&&&2");
  testParseFail("*");
  testParseFail("*2");
  testParseFail("2*");
  testParseFail("2**2");
  testParseFail("^");
  testParseFail("^2");
  testParseFail("2^^2");
  testParseFail("2^");
  testParseFail("+");
  testParseFail("+2");
  testParseFail("2+");
  testParseFail("2++2");
  testParseFail("-");
  testParseFail("2-");
  testParseFail("-2-");
  testParseFail("2!");
  testParseFail("!");
  testParseFail("()");
  testParseFail("2()");
  testParseFail("()2");
  testParseFail("_");
  testParseFail("#");
  testParseFail("$");
  testParseFail(")");
  testParseFail("(");
  testParseFail(")(");
  testParseFail("2&|2");
  testParseFail("2&&||2");
  testParseFail("2&&+2");
  testParseFail("2+&&2");
  testParseFail("2*&&2");
  testParseFail("2&&*2");
  testParseFail("2<>2");
  testParseFail("2=<2");
  testParseFail("2=>2");
  testParseFail("2><2");
  testParseFail("<");
  testParseFail("<2");
  testParseFail("2<");
  testParseFail("2<<2");
  testParseFail(">");
  testParseFail(">2");
  testParseFail("2>");
  testParseFail("2>>2");
  testParseFail(">=");
  testParseFail(">=2");
  testParseFail("2>=");
  testParseFail("2>=>=2");
  testParseFail("<=");
  testParseFail("<=2");
  testParseFail("2<=");
  testParseFail("2<=<=2");
  testParseFail("2<==2");
  testParseFail(".");
  testParseFail(".2");
  testParseFail("2.");
  testParseFail(",");
  testParseFail(",2");
  testParseFail("2,"); // Doesn't give a specific error - how to catch this?
  testParseFail("2*sin(x");
  testParseFail("2*sin(x)()");

  testText("3*x", "3*x");
  testText("(3*x)", "(3*x)");
  testText("3*(x)", "3*x");
  testText("((3*x))", "(3*x)");
  testText(" ((3 * x)) ", "(3*x)");
  testText(" ((3 * x)) ", "(3*x)");
  testText("(-3)", "(-3)");
  testText("(x)", "x");
  testText("(3*(-(x+5)))", "(3*(-(x+5)))");
  testText("(sin((x)))", "sin(x)");

  exitHelper();
  if (rc == KstTestSuccess) {
    printf("All tests passed!\n");
  }
  return -rc;
}

// vim: ts=2 sw=2 et
