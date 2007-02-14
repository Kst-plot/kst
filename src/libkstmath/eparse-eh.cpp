/***************************************************************************
                               eparse-eh.cpp
                             -------------------
    begin                : Nov. 24, 2004
    copyright            : (C) 2004 The University of Toronto
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

#include <qstring.h>
#include <qstringlist.h>

#include <klocale.h>
#include "kst_export.h"

namespace Equation {
  KST_EXPORT QStringList errorStack;
}


static const char *EParseErrorUnknown = I18N_NOOP("parse error"); // from bison
extern "C" const char *EParseErrorEmpty = I18N_NOOP("Equation is empty.");
extern "C" const char *EParseErrorEmptyArg = I18N_NOOP("Function argument is empty.");
extern "C" const char *EParseErrorTwoOperands = I18N_NOOP("Two operands are required.");
extern "C" const char *EParseErrorEmptyParentheses = I18N_NOOP("Empty parentheses are forbidden except in function calls.");
extern "C" const char *EParseErrorMissingClosingParenthesis = I18N_NOOP("Closing parenthesis is missing.");
extern "C" const char *EParseErrorNoImplicitMultiply = I18N_NOOP("Term must be followed by an operator.  Implicit multiplication is not supported.");
extern "C" const char *EParseErrorRequiresOperand = I18N_NOOP("This operator requires an operand.");
extern "C" const char *EParseErrorToken = I18N_NOOP("Unknown character '%1'.");


extern "C" void yyClearErrors() {
  Equation::errorStack.clear();
}


extern "C" int yyErrorCount() {
  return Equation::errorStack.count();
}


extern "C" void yyerror(const char *s) {
  Equation::errorStack << i18n(s);
}

extern "C" void yyerrortoken(char c) {
  Equation::errorStack << i18n(EParseErrorToken).arg(c);
}

// vim: ts=2 sw=2 et
