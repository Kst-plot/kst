/***************************************************************************
                                  enodes.h
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

#ifndef ENODES_H
#define ENODES_H

#include "kststring.h"
#include "kstvector.h"
#include "kst_export.h"

class Plugin;

#define EQ_FALSE 0.0
#define EQ_TRUE  1.0

namespace Equation {

  /* Global lock for the parser */
  KST_EXPORT QMutex& mutex();

  /*    Evaluate the expression @p txt and returns the value as a double.
   *    Returns the value, or 0.0 and sets ok = false on error.
   */
  KST_EXPORT double interpret(const char *txt, bool *ok = 0L, int len = -1);

  class Context {
    public:
      Context() : i(0), x(0.0), xVector(0L), noPoint(0.0), sampleCount(0) {}
      ~Context() {}

      long i;
      double x;
      KstVectorPtr xVector;
      double noPoint;
      long sampleCount;
  };

  class NodeVisitor;

  class Node {
    public:
      Node();
      virtual ~Node();

      virtual bool isConst() = 0; // can't be const
      virtual bool collectObjects(KstVectorMap& v, KstScalarMap& s, KstStringMap& t);
      virtual bool takeVectors(const KstVectorMap& c);
      virtual double value(Context*) = 0;
      virtual void visit(NodeVisitor*);
      virtual KstObject::UpdateType update(int counter, Context *ctx);
      virtual QString text() const = 0;

      void parenthesize() { _parentheses = true; }

    protected:
      bool _parentheses;
  } KST_EXPORT;

  class BinaryNode : public Node {
    public:
      BinaryNode(Node *left, Node *right);
      virtual ~BinaryNode();

      virtual bool collectObjects(KstVectorMap& v, KstScalarMap& s, KstStringMap& t);
      virtual bool takeVectors(const KstVectorMap& c);
      virtual void visit(NodeVisitor*);
      virtual KstObject::UpdateType update(int counter, Context *ctx);

      Node *& left();
      Node *& right(); 

      virtual QString text() const = 0;

    protected:
      Node *_left, *_right;
  };


  class ArgumentList : public Node {
    friend class Function;
    public:
      ArgumentList();
      virtual ~ArgumentList();

      void appendArgument(Node *arg);

      // Makes no sense for this type
      double value(Context*) { return 0.0; }

      bool isConst();
      bool collectObjects(KstVectorMap& v, KstScalarMap& s, KstStringMap& t);
      bool takeVectors(const KstVectorMap& c);
      double at(int, Context*);
      Node *node(int idx);
      KstObject::UpdateType update(int counter, Context *ctx);
      QString text() const;

    protected:
      QList<Node*> _args;
  };


  // FIXME: consider an approach to reduce the weight of this node since most
  //        of the weight is due entirely to plugin calling.
  class Function : public Node {
    public:
      Function(char *name, ArgumentList *args);
      ~Function();

      bool isPlugin() const;
      bool isConst();
      double value(Context*);
      bool collectObjects(KstVectorMap& v, KstScalarMap& s, KstStringMap& t);
      bool takeVectors(const KstVectorMap& c);
      KstObject::UpdateType update(int counter, Context *ctx);
      QString text() const;

    protected:
      char *_name;
      ArgumentList *_args;
      void *_f;
      KstSharedPtr<Plugin> _plugin;

    private:
      double evaluatePlugin(Context *ctx);
      int _inputScalarCnt, _inputVectorCnt, _inputStringCnt;
      int _inPid, _outputScalarCnt, _outputVectorCnt, _outputStringCnt;
      int *_inArrayLens, *_outArrayLens;
      double *_inScalars, *_outScalars;
      double **_inVectors, **_outVectors;
      int _outputIndex; // -1 = scalar 0, -424242 = none/error, +0...n = vector
      void *_localData;
      int _argCount;
  };


  class Number : public Node {
    public:
      Number(double n);
      ~Number();

      bool isConst();
      double value(Context*);
      QString text() const;

    protected:
      double _n;
  } KST_EXPORT;


  class Identifier : public Node {
    public:
      Identifier(char *name);
      ~Identifier();

      bool isConst();
      double value(Context*);
      const char *name() const;
      QString text() const;

    protected:
      char *_name;
      double *_const;
  };


  class Data : public Node {
    friend class Function;
    public:
      Data(char *name);
      ~Data();

      bool isConst();
      double value(Context*);
      bool collectObjects(KstVectorMap& v, KstScalarMap& s, KstStringMap& t);
      bool takeVectors(const KstVectorMap& c);
      KstObject::UpdateType update(int counter, Context *ctx);
      QString text() const;

    protected:
      QString _tagName;
      KstVectorPtr _vector;
      KstScalarPtr _scalar;
      bool _isEquation;
      Node *_equation;
      QString _vectorIndex;
  };

  class NodeVisitor {
    public:
      NodeVisitor();
      virtual ~NodeVisitor();

      virtual void visitNode(Node*) = 0;
      virtual void visitBinaryNode(BinaryNode*) = 0;
  };


  class KST_EXPORT FoldVisitor : public NodeVisitor {
    public:
      FoldVisitor(Context*, Node**);
      ~FoldVisitor();

      void visitNode(Node*);
      void visitBinaryNode(BinaryNode*);

    protected:
      Context *_ctx;
  };


  class Negation : public Node {
    public:
      Negation(Node *node);
      ~Negation();
      bool isConst();
      double value(Context*);
      QString text() const;

    protected:
      Node *_n;
  };

  class LogicalNot : public Node {
    public:
      LogicalNot(Node *node);
      ~LogicalNot();
      bool isConst();
      double value(Context*);
      QString text() const;

    protected:
      Node *_n;
  };

#define CreateNode(x)                     \
  class x : public BinaryNode {           \
    public:                               \
      x(Node *left, Node *right);         \
      ~x();                               \
      bool isConst();                     \
      double value(Context*);             \
      QString text() const;               \
  };

CreateNode(Addition)
CreateNode(Subtraction)
CreateNode(Multiplication)
CreateNode(Division)
CreateNode(Modulo)
CreateNode(Power)
CreateNode(BitwiseAnd)
CreateNode(BitwiseOr)
CreateNode(LogicalAnd)
CreateNode(LogicalOr)
CreateNode(LessThan)
CreateNode(LessThanEqual)
CreateNode(GreaterThan)
CreateNode(GreaterThanEqual)
CreateNode(EqualTo)
CreateNode(NotEqualTo)
#undef CreateNode

}

#endif

// vim: ts=2 sw=2 et
