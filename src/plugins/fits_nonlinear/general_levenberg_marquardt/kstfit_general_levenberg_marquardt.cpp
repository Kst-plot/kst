/*
 *  General non-linear fit plugin for KST.
 *  Copyright 2005, The kst development team
 *  Released under the terms of the GPL.
 */

#include "gsl/gsl_multifit_nlin.h"
#include "muParser.h"
#include <gsl/gsl_vector.h>
#include <math.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

int function_f(const gsl_vector * x, void *params, gsl_vector * f);
int function_df(const gsl_vector * x, void *params,gsl_matrix * J);
int function_fdf(const gsl_vector * x, void *params,gsl_vector * f, gsl_matrix * J);

struct fit {
  size_t n;
  const double * X;
  const double * YtoFit;
  double * YFitted;
  const char* function;
  const QStringList paramNames;
};

extern "C" int kstfit_general_levenberg_marquardt(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[],
		const char* inStrings[], char *outStrings[]);

int kstfit_general_levenberg_marquardt(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[],
		const char* inStrings[], char *outStrings[])
{

// Check that X and Y have identical sizes: TODO

int i = 0;

// Algorithm parameters
double tolerance = inScalars[0];
int maxIterations = (int) inScalars[1];
size_t n = inArrayLens[0];

// Extract parameter names
QStringList params = QStringList::split(QRegExp("\\s*(,|;|:)\\s*"), inStrings[1]);
size_t paramsNumber = params.count();
// TODO: replace X with x to make sure :-)

// Set parameter initial guesses
double pInit[paramsNumber];
QStringList paramsInit = QStringList::split(QRegExp("\\s*(,|;|:)\\s*"), inStrings[2]);
bool ok = true;
for (i=0; i<(int)paramsNumber; i++) {
  pInit[i]=paramsInit[i].toDouble(&ok);
  if (!ok) pInit[i]=0.0;
}
gsl_vector_view x = gsl_vector_view_array (pInit, paramsNumber);

// Allocate storage for output vectors
outArrays[0] = (double *) realloc(outArrays[0], (int)n * sizeof(double));
outArrayLens[0] = (int)n;
outArrays[1] = (double *) realloc(outArrays[1], (int)paramsNumber * sizeof(double));
outArrayLens[1] = (int)paramsNumber;
outArrays[2] = (double *) realloc(outArrays[2], (int)paramsNumber * sizeof(double));
outArrayLens[2] = (int)paramsNumber;

// Store information in our struct
struct fit d = { n, inArrays[0], inArrays[1], outArrays[0], inStrings[0], params };

// Initialize gsl solver
const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmsder;
gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc (T, n, paramsNumber);
gsl_multifit_function_fdf f;
gsl_matrix *covar = gsl_matrix_alloc (paramsNumber, paramsNumber);

f.f = &function_f;
f.df = &function_df;
f.fdf = &function_fdf;
f.n = n;
f.p = paramsNumber;
f.params = &d;

gsl_multifit_fdfsolver_set (s, &f, &x.vector);

int status;
int iteration = 0;
do {
  iteration++;
  status = gsl_multifit_fdfsolver_iterate (s);
  if (status) break;
  status = gsl_multifit_test_delta (s->dx, s->x, tolerance, tolerance);
}
while (status == GSL_CONTINUE && iteration < maxIterations);

gsl_multifit_covar (s->J, 0.0, covar);

// Store results
for (i=0; i<(int)paramsNumber; i++) {
  outArrays[1][i]=gsl_vector_get(s->x,i); // Store fitted parameter values
  outArrays[2][i]=sqrt(gsl_matrix_get(covar,i,i)); // Store parameter precision
}

gsl_multifit_fdfsolver_free (s);
gsl_matrix_free (covar);

return 0;
}

int function_f(const gsl_vector * x, void *params, gsl_vector * f)
{
  size_t n = ((struct fit *)params)->n;
  const double *X = ((struct fit *)params)->X;
  const double *YtoFit = ((struct fit *)params)->YtoFit;
  double *YFitted = ((struct fit *)params)->YFitted;
  const char *function = ((struct fit *) params)->function;
  QStringList parameterNames = ((struct fit *) params)->paramNames;
  size_t p = (size_t) parameterNames.count();

  mu::Parser parser;
  double parameters[p];

try {
  double xvar;
  parser.DefineVar("x", &xvar);
  
  size_t i;
  for (i=0;i<p;i++) {
    parser.DefineVar(parameterNames[i].ascii(), &parameters[i]);
    parameters[i]=gsl_vector_get(x,i);
  }
  	  
   if (((QString)function).contains("pi")>0)
  	  parser.DefineConst("pi", M_PI);
   if (((QString)function).contains("Pi")>0)
  	  parser.DefineConst("Pi", M_PI);
   if (((QString)function).contains("PI")>0)
  	  parser.DefineConst("PI", M_PI);

  parser.SetExpr(function);
   
  for (i = 0; i < n; i++) {
    xvar=X[i];
    YFitted[i] = parser.Eval();
    gsl_vector_set (f, i, (YFitted[i] - YtoFit[i])); // Add and manage a vector of weights (sigma) ?
  }
}
catch (mu::Parser::exception_type &e) {
  return GSL_FAILURE;
}

  free(parameters);
  return GSL_SUCCESS;
}

int function_df(const gsl_vector * x, void *params, gsl_matrix * J)
{
  size_t n = ((struct fit *)params)->n;
  const double *X = ((struct fit *)params)->X;
  const char *function = ((struct fit *) params)->function;
  QStringList parameterNames = ((struct fit *) params)->paramNames;
  size_t p = (size_t) parameterNames.count();

  mu::Parser parser;

  double parameters[p];

  size_t i;
try {
  for (i=0;i<p;i++) {
    parser.DefineVar(parameterNames[i].ascii(), &parameters[i]);
    parameters[i]=gsl_vector_get(x,i);
  }

  double xvar;
  parser.DefineVar("x", &xvar);

   if (((QString)function).contains("pi")>0)
  	  parser.DefineConst("pi", M_PI);
   if (((QString)function).contains("Pi")>0)
  	  parser.DefineConst("Pi", M_PI);
   if (((QString)function).contains("PI")>0)
  	  parser.DefineConst("PI", M_PI);
   
  parser.SetExpr(function);

  size_t j;
  for (i = 0; i < n; i++) {
    // Jacobian matrix J(i,j) = dfi / dxj
    xvar = X[i];      
    for (j=0;j<p;j++) {
      double diff=parser.Diff(&parameters[j],xvar);
      gsl_matrix_set (J, i, j, diff);
    }
  }
}
catch (mu::Parser::exception_type &e) {
  return GSL_FAILURE;
}

  free(parameters);
  return GSL_SUCCESS;
}


int function_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J)
{
  function_f (x, params, f);
  function_df (x, params, J);
  return GSL_SUCCESS;
}

