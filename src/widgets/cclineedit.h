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
    const QString& title() const { return _title; }
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
class ObjectStore;

class CCCommonEdit {
protected:
    ObjectStore *_store;
    static QList<CCCommonEdit*> _u;
    virtual QString Text()=0;
    virtual void SetText(QString text)=0;
    virtual int CursorPosition()=0;
    virtual void SetCursorPosition(int)=0;
    virtual void NewPrefix(QString)=0;
    virtual void SetCompleter(QCompleter*) {}
    virtual void ChangeCurrentPrefix(QString)=0;
    void Divide(QString x="\0");
    virtual void fillKstObjects() {}
    void NewVector();
    void NewScalar();
    void NewString();
    void EditItem();
    void Insert(const QString&i,bool stringIsCompletion=true);
    CCCommonEdit() : _store(0) {_u.push_back(this); }
    ~CCCommonEdit() { _u.removeOne(this); }
};

/**
 * This is a LineEdit with autocompletion. The completion items are divided into cases, categories and items.
 * @sa CompletionCase, Category
 */
class KSTWIDGETS_EXPORT CCLineEdit : public QLineEdit, public CCCommonEdit {
    Q_OBJECT
protected:
    CategoricalCompleter* _cc;
    virtual QString Text(){return text();}
    virtual void SetText(QString text){setText(text);}
    virtual int CursorPosition(){return cursorPosition();}
    virtual void SetCursorPosition(int x){setCursorPosition(x);}
    virtual void NewPrefix(QString x){emit currentPrefixChanged(x);}
    virtual void SetCompleter(QCompleter*c) {setCompleter(c);}
    virtual void ChangeCurrentPrefix(QString x){emit currentPrefixChanged(x);}

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
    void mousePressEvent(QMouseEvent *);
signals:
    void currentPrefixChanged(QString);
};

/**
 * This is a special version of CCLineEdit which is used to display scalars and vectors.
 * You can still send your own QList<CompletionCase>* in init() which will be added to the list of scalars and vectors.
 * @sa CCLineEdit
 */
class KSTWIDGETS_EXPORT SVCCLineEdit : public CCLineEdit
{
    Q_OBJECT
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
    inline void newVector(){NewVector();}
    inline void newScalar(){NewScalar();}
    inline void editItem(){EditItem();}
};

/**
 * This is a TextEdit with audocompletion.
 * @sa CCLineEdit
 */
class KSTWIDGETS_EXPORT CCTextEdit : public QTextEdit, public CCCommonEdit {
    Q_OBJECT
protected:
    CategoricalCompleter* _cc;
    virtual QString Text(){return toPlainText();}
    virtual void SetText(QString text){bool dirty=document()->isModified();setPlainText(text);document()->setModified(dirty);}
    virtual int CursorPosition(){return textCursor().position();}
    virtual void SetCursorPosition(int x){QTextCursor tc=textCursor();tc.setPosition(x);setTextCursor(tc);}
    virtual void NewPrefix(QString x){emit currentPrefixChanged(x);}
    virtual void SetCompleter(QCompleter*) {}
    virtual void ChangeCurrentPrefix(QString x){emit currentPrefixChanged(x);}
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
    inline void newVector(){NewVector();}
    inline void newScalar(){NewScalar();}
    inline void newString(){NewString();}
    inline void editItem(){EditItem();}
};

}

#endif // ACLINEEDIT_H
