/***************************************************************************
                                  enodes.h
                                 ----------
    begin                : Feb 12 2004
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

#ifndef ENODES_H
#define ENODES_H

#include "string_kst.h"
#include "vector.h"
#include "kstmath_export.h"
#include "objectstore.h"

#define EQ_FALSE 0.0
#define EQ_TRUE  1.0

namespace Equations {

  /* Global lock for the parser */
  KSTMATH_EXPORT QMutex& mutex();

  /*    Evaluate the expression @p txt and returns the value as a double.
   *    Returns the value, or 0.0 and sets ok = false on error.
   */
  KSTMATH_EXPORT double interpret(Kst::ObjectStore *store, const char *txt, bool *ok = 0L, int len = -1);

  class KSTMATH_EXPORT Context 
  {
    public:
      Context() : i(0), x(0.0), xVector(0L), noPoint(0.0), sampleCount(0) {}
      ~Context() {}

      long i;
      double x;
      Kst::VectorPtr xVector;
      double noPoint;
      long sampleCount;
  };

  class NodeVisitor;

  class KSTMATH_EXPORT Node {
    public:
      Node();
      virtual ~Node();

      virtual bool isConst() = 0; // can't be const
      virtual bool collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t);
      virtual bool takeVectors(const Kst::VectorMap& c);
      virtual double value(Context*) = 0;
      virtual void visit(NodeVisitor*);
      virtual Kst::Object::UpdateType update(Context *ctx);
      virtual QString text() const = 0;

      void parenthesize() { _parentheses = true; }

    protected:
      bool _parentheses;
  };

  class KSTMATH_EXPORT BinaryNode : public Node {
    public:
      BinaryNode(Node *left, Node *right);
      virtual ~BinaryNode();

      virtual bool collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t);
      virtual bool takeVectors(const Kst::VectorMap& c);
      virtual void visit(NodeVisitor*);
      virtual Kst::Object::UpdateType update(Context *ctx);

      Node *& left();
      Node *& right();

      virtual QString text() const = 0;

    protected:
      Node *_left, *_right;
  };


  class KSTMATH_EXPORT ArgumentList : public Node {
    friend class Function;
    public:
      ArgumentList();
      virtual ~ArgumentList();

      void appendArgument(Node *arg);

      // Makes no sense for this type
      double value(Context*) { return 0.0; }

      bool isConst();
      bool collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t);
      bool takeVectors(const Kst::VectorMap& c);
      double at(int, Context*);
      Node *node(int idx);
      Kst::Object::UpdateType update(Context *ctx);
      QString text() const;

    protected:
      QList<Node*> _args;
  };


  class KSTMATH_EXPORT Function : public Node {
    public:
      Function(char *name, ArgumentList *args);
      ~Function();

      bool isConst();
      double value(Context*);
      bool collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t);
      bool takeVectors(const Kst::VectorMap& c);
      Kst::Object::UpdateType update(Context *ctx);
      QString text() const;

    protected:
      char *_name;
      ArgumentList *_args;
      void *_f;

    private:
      int _inputScalarCnt, _inputVectorCnt, _inputStringCnt;
      int _inPid, _outputScalarCnt, _outputVectorCnt, _outputStringCnt;
      int *_inArrayLens, *_outArrayLens;
      double *_inScalars, *_outScalars;
      double **_inVectors, **_outVectors;
      int _outputIndex; // -1 = scalar 0, -424242 = none/error, +0...n = vector
      void *_localData;
      int _argCount;
  };


  class KSTMATH_EXPORT Number : public Node 
  {
    public:
      Number(double n);
      ~Number();

      bool isConst();
      double value(Context*);
      QString text() const;

    protected:
      double _n;
  };


  class KSTMATH_EXPORT Identifier : public Node 
  {
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


  class KSTMATH_EXPORT DataNode : public Node {
    friend class Function;
    public:
      DataNode(Kst::ObjectStore *store, char *name);
      ~DataNode();

      bool isConst();
      double value(Context*);
      bool collectObjects(Kst::VectorMap& v, Kst::ScalarMap& s, Kst::StringMap& t);
      bool takeVectors(const Kst::VectorMap& c);
      Kst::Object::UpdateType update(Context *ctx);
      QString text() const;

    protected:
      Kst::ObjectStore *_store;
      QString _tagName;
      Kst::VectorPtr _vector;
      Kst::ScalarPtr _scalar;
      bool _isEquation;
      Node *_equation;
      QString _vectorIndex;
  };

  class KSTMATH_EXPORT NodeVisitor {
    public:
      NodeVisitor();
      virtual ~NodeVisitor();

      virtual void visitNode(Node*) = 0;
      virtual void visitBinaryNode(BinaryNode*) = 0;
  };


  class KSTMATH_EXPORT FoldVisitor : public NodeVisitor {
    public:
      FoldVisitor(Context*, Node**);
      ~FoldVisitor();

      void visitNode(Node*);
      void visitBinaryNode(BinaryNode*);

    protected:
      Context *_ctx;
  };


  class KSTMATH_EXPORT Negation : public Node {
    public:
      Negation(Node *node);
      ~Negation();
      bool isConst();
      double value(Context*);
      QString text() const;

    protected:
      Node *_n;
  };

  class KSTMATH_EXPORT LogicalNot : public Node {
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
  class KSTMATH_EXPORT x : public BinaryNode {           \
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
