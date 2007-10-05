/***************************************************************************
                              enodefactory.cpp
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

#include "enodefactory.h"
#include "enodes.h"

using namespace Equations;


extern "C" {

#define CreateFactory(x)                         \
        void *New##x(void *left, void *right) {  \
          Node *l = static_cast<Node*>(left);    \
          Node *r = static_cast<Node*>(right);   \
          return new x(l, r);                    \
        }

CreateFactory(Addition)
CreateFactory(Subtraction)
CreateFactory(Multiplication)
CreateFactory(Division)
CreateFactory(Modulo)
CreateFactory(Power)
CreateFactory(BitwiseAnd)
CreateFactory(BitwiseOr)
CreateFactory(LogicalAnd)
CreateFactory(LogicalOr)
CreateFactory(LessThan)
CreateFactory(LessThanEqual)
CreateFactory(GreaterThan)
CreateFactory(GreaterThanEqual)
CreateFactory(EqualTo)
CreateFactory(NotEqualTo)


#undef CreateFactory

void *NewData(char *name) {
  return new Data(name);
}


void *NewIdentifier(char *name) {
  return new Identifier(name);
}


void *NewFunction(char *name, void *args) {
  return new Function(name, static_cast<ArgumentList*>(args));
}


void *NewNumber(double n) {
  return new Number(n);
}


void *NewArgumentList() {
  return new ArgumentList();
}


void AppendArgument(void *list, void *arg) {
  if (list && arg) {
    static_cast<ArgumentList*>(list)->appendArgument(static_cast<Node*>(arg));
  }
}


void *NewNot(void *n) {
  return new LogicalNot(static_cast<Node*>(n));
}


void *NewNegation(void *n) {
  return new Negation(static_cast<Node*>(n));
}


void DeleteNode(void *n) {
  delete static_cast<Node*>(n);
}


void ParenthesizeNode(void *n) {
  static_cast<Node*>(n)->parenthesize();
}


}


// vim: ts=2 sw=2 et
