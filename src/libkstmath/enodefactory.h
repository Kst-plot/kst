/***************************************************************************
                               enodefactory.h
                                 ----------      
    begin                : Feb 12 2004
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

#ifndef ENODEFACTORY_H
#define ENODEFACTORY_H


#define CreateNodeBinary(x) void *New##x(void *left, void *right);

#ifdef __cplusplus
extern "C" {
#endif

  CreateNodeBinary(Addition)
  CreateNodeBinary(Subtraction)
  CreateNodeBinary(Multiplication)
  CreateNodeBinary(Division)
  CreateNodeBinary(Modulo)
  CreateNodeBinary(Power)
  CreateNodeBinary(BitwiseAnd)
  CreateNodeBinary(BitwiseOr)
  CreateNodeBinary(LogicalAnd)
  CreateNodeBinary(LogicalOr)
  CreateNodeBinary(LessThan)
  CreateNodeBinary(LessThanEqual)
  CreateNodeBinary(GreaterThan)
  CreateNodeBinary(GreaterThanEqual)
  CreateNodeBinary(EqualTo)
  CreateNodeBinary(NotEqualTo)

  void *NewData(char *name);
  void *NewIdentifier(char *name);
  void *NewFunction(char *name, void *args);
  void *NewArgumentList();
  void *NewNumber(double n);
  void *NewNot(void *n);
  void *NewNegation(void *n);
  void AppendArgument(void *list, void *arg);

  void DeleteNode(void *n); /* WARNING: this does evil things.  will cause a
                               crash if we ever do MI in the tree */
  void ParenthesizeNode(void *n);

#ifdef __cplusplus
}
#endif

#undef CreateNode

#endif

/* vim: ts=2 sw=2 et
 */ 
