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


#include "kstmath_export.h"
#include "objectstore.h"

namespace Equations {
  KSTMATH_EXPORT QStringList errorStack;
}

/*extern "C"*/ const char *EParseErrorEmpty = "Equations is empty.";
/*extern "C"*/ const char *EParseErrorEmptyArg = "Function argument is empty.";
/*extern "C"*/ const char *EParseErrorTwoOperands = "Two operands are required.";
/*extern "C"*/ const char *EParseErrorEmptyParentheses = "Empty parentheses are forbidden except in function calls.";
/*extern "C"*/ const char *EParseErrorMissingClosingParenthesis = "Closing parenthesis is missing.";
/*extern "C"*/ const char *EParseErrorNoImplicitMultiply = "Term must be followed by an operator.  Implicit multiplication is not supported.";
/*extern "C"*/ const char *EParseErrorRequiresOperand = "This operator requires an operand.";
/*extern "C"*/ const char *EParseErrorToken = "Unknown character '%1'.";


/*extern "C"*/ void yyClearErrors() {
  Equations::errorStack.clear();
}


/*extern "C"*/ int yyErrorCount() {
  return Equations::errorStack.count();
}


/*extern "C"*/ void yyerror(Kst::ObjectStore *store, const char *s) {
  Q_UNUSED(store)
  Equations::errorStack << s;
}

/*extern "C"*/ void yyerrortoken(char c) {
  Equations::errorStack << QString(EParseErrorToken).arg(c);
}

// vim: ts=2 sw=2 et
