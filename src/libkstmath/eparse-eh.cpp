/***************************************************************************
                               eparse-eh.cpp
                             -------------------
    begin                : Nov. 24, 2004
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

#include <QString>
#include <QStringList>

#include "kst_i18n.h"
#include "kstmath_export.h"
#include "objectstore.h"

namespace Equations {
  KSTMATH_EXPORT QStringList errorStack;
}

/*extern "C"*/ const char *EParseErrorEmpty = I18N_NOOP("Equations is empty.");
/*extern "C"*/ const char *EParseErrorEmptyArg = I18N_NOOP("Function argument is empty.");
/*extern "C"*/ const char *EParseErrorTwoOperands = I18N_NOOP("Two operands are required.");
/*extern "C"*/ const char *EParseErrorEmptyParentheses = I18N_NOOP("Empty parentheses are forbidden except in function calls.");
/*extern "C"*/ const char *EParseErrorMissingClosingParenthesis = I18N_NOOP("Closing parenthesis is missing.");
/*extern "C"*/ const char *EParseErrorNoImplicitMultiply = I18N_NOOP("Term must be followed by an operator.  Implicit multiplication is not supported.");
/*extern "C"*/ const char *EParseErrorRequiresOperand = I18N_NOOP("This operator requires an operand.");
/*extern "C"*/ const char *EParseErrorToken = I18N_NOOP("Unknown character '%1'.");


/*extern "C"*/ void yyClearErrors() {
  Equations::errorStack.clear();
}


/*extern "C"*/ int yyErrorCount() {
  return Equations::errorStack.count();
}


/*extern "C"*/ void yyerror(Kst::ObjectStore *store, const char *s) {
  Q_UNUSED(store)
  Equations::errorStack << i18n(s);
}

/*extern "C"*/ void yyerrortoken(char c) {
  Equations::errorStack << i18n(EParseErrorToken).arg(c);
}

// vim: ts=2 sw=2 et
