/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CCLINEEDIT_H
#define CCLINEEDIT_H

#include <QLineEdit>
#include <QTextEdit>
#include <QStringList>
#include "kstwidgets_export.h"

namespace Kst {

/**
 * A category is a list of possible completions under a certain title.
 * For example, a Category could be a list of Vector names.
 */
class Category : public QStringList {
    QString _title;
public:
    Category(QString title) : QStringList(), _title(title) { }
    Category(QString title, QString i) : QStringList(i), _title(title) { }
    Category(QString title, QStringList l) : QStringList(l), _title(title) { }
    const QString& title() { return _title; }
};

/**
 * A completion case is a prefix for Categories.
 */
class CompletionCase : public QList<Category> {
    QString _prefix;
public:
    CompletionCase(QString prefix) : QList<Category>(), _prefix(prefix) { }
    CompletionCase(QString prefix,QList<Category> l) : QList<Category>(l), _prefix(prefix) { }
    const QString& prefix() const { return _prefix; }
};

bool operator<(const CompletionCase&a,const CompletionCase&b);

class CategoricalCompleter;

/**
 * This is a LineEdit with autocompletion. The completion items are divided into cases, categories and items.
 * @sa CompletionCase, Category
 */
class KSTWIDGETS_EXPORT CCLineEdit : public QLineEdit {
    Q_OBJECT
protected:
    CategoricalCompleter* _cc;
public:
    friend class CCTableView;
    CCLineEdit(QWidget*p=0);
    CCLineEdit(const QString&s,QWidget*p=0);
    ~CCLineEdit();
    virtual void init(QList<CompletionCase> data);

public slots:
    void insert(const QString &i,bool stringIsCompletion=true);
protected slots:
    void divide(QString x="\0");

protected:
    void keyPressEvent(QKeyEvent *);
signals:
    void currentPrefixChanged(QString);
};

class ObjectStore;

/**
 * This is a special version of CCLineEdit which is used to display scalars and vectors.
 * You can still send your own QList<CompletionCase>* in init() which will be added to the list of scalars and vectors.
 * @sa CCLineEdit
 */
class KSTWIDGETS_EXPORT SVCCLineEdit : public CCLineEdit
{
    Q_OBJECT
    ObjectStore *_store;
    QList<CompletionCase> *_extraData, *_svData, *_allData;
public:
    SVCCLineEdit(QWidget*p=0);
    SVCCLineEdit(const QString&s,QWidget*p=0);
    ~SVCCLineEdit();

    void setObjectStore(ObjectStore *store);
    virtual void init(QList<CompletionCase> data=QList<CompletionCase>());
    void fillKstObjects();
protected:
    void contextMenuEvent(QContextMenuEvent *);
public slots:
    void newVector();
    void newScalar();
    void editItem();
};

/**
 * This is a TextEdit with audocompletion.
 * @sa CCLineEdit
 */
class KSTWIDGETS_EXPORT CCTextEdit : public QTextEdit {
    Q_OBJECT
protected:
    CategoricalCompleter* _cc;
public:
    friend class CCTableView;
    CCTextEdit(QWidget*p=0);
    CCTextEdit(const QString&s,QWidget*p=0);
    ~CCTextEdit();
    virtual void init(QList<CompletionCase> data);

public slots:
    void insert(const QString &i,bool stringIsCompletion=true);
protected slots:
    void divide(QString x="\0");

protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void currentPrefixChanged(QString);
};

/**
 * This is a special version of CCTextEdit which is used to display scalars and vectors.
 * @sa SVCCLineEdit
 */
class KSTWIDGETS_EXPORT SVCCTextEdit : public CCTextEdit
{
    Q_OBJECT
    ObjectStore *_store;
    QList<CompletionCase> *_extraData, *_svData, *_allData;
public:
    SVCCTextEdit(QWidget*p=0);
    SVCCTextEdit(const QString&s,QWidget*p=0);
    ~SVCCTextEdit();

    void setObjectStore(ObjectStore *store);
    virtual void init(QList<CompletionCase> data=QList<CompletionCase>());
    void fillKstObjects();
protected:
    void contextMenuEvent(QContextMenuEvent *);
public slots:
    void newVector();
    void newScalar();
    void newString();
    void editItem();
};

}

#endif // ACLINEEDIT_H
