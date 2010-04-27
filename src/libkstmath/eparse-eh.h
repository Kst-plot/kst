/***************************************************************************
                               eparse-eh.h
                             ----------------
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

/* This file must be C-clean */

#ifndef EPARSEEH_H
#define EPARSEEH_H

#ifdef __cplusplus

#include "kstmath_export.h"
#include <QStringList>

namespace Equations {
   KSTMATH_EXPORT extern QStringList errorStack;
}

namespace Kst {
  class ObjectStore;
}

#endif

#ifdef __cplusplus
//extern "C" {
#endif
  void yyerror(Kst::ObjectStore *store, const char *s);
  void yyerrortoken(char c);

  void yyClearErrors();

  int yyErrorCount();

  extern const char *EParseErrorEmpty;
  extern const char *EParseErrorEmptyArg;
  extern const char *EParseErrorTwoOperands;
  extern const char *EParseErrorEmptyParentheses;
  extern const char *EParseErrorMissingClosingParenthesis;
  extern const char *EParseErrorNoImplicitMultiply;
  extern const char *EParseErrorRequiresOperand;
#ifdef __cplusplus
//}
#endif

#endif

/*
vim: ts=2 sw=2 et
*/
