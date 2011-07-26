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

#include "cclineedit.h"
#include "cclineedit_p.h"

#include "dialoglauncher.h"
#include "scalar.h"
#include "vector.h"
#include "objectstore.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QCompleter>
#include <QHash>
#include <QMenu>

namespace Kst {

// CompletionCase
bool operator<(const CompletionCase&a,const CompletionCase&b)
{
    return a.prefix().size()<b.prefix().size();
}

// CategoricalCompleter
CategoricalCompleter::CategoricalCompleter(QLineEdit *lineEdit, QList<CompletionCase> data) : QCompleter(getDefault(data)),
    _data(data), _tableView(new CCTableView(&_data[0])), _currentSubset(&data[0])
{
    setPopup(_tableView);
    lineEdit->setCompleter(this);
    _tableView->setCompleter(this);
    _tableView->updateSuggestions();
    setCompletionMode(PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);
    setWrapAround(false);
    qSort(_data.begin(),_data.end());
    connect(_tableView,SIGNAL(activateHint(QString)),this,SIGNAL(activated(QString)));
}

CategoricalCompleter::CategoricalCompleter(QTextEdit *textEdit, QList<CompletionCase> data) : QCompleter(getDefault(data)),
    _data(data), _tableView(new CCTableView(&_data[0])), _currentSubset(&data[0])
{
    setPopup(_tableView);
    _tableView->setCompleter(this);
    _tableView->updateSuggestions();
    setCompletionMode(PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);
    setWrapAround(false);
    qSort(_data.begin(),_data.end());
    connect(_tableView,SIGNAL(activateHint(QString)),this,SIGNAL(activated(QString)));
}

bool CategoricalCompleter::eventFilter(QObject *o, QEvent *e)
{
    if(e->type()!=QEvent::KeyPress) {
        return QCompleter::eventFilter(o,e);
    } else {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        int key=ke->key();
        int cc=_tableView->currentIndex().column();
        bool cantMoveRight=(cc==_tableView->model()->columnCount()-1);
        bool cantMoveLeft=!cc;
        if(!cantMoveLeft) {
            bool ok=1;
            for(int i=0;i<cc;i++) {
                if(_tableView->model()->data(_tableView->model()->index(_tableView->currentIndex().row(),i)).toString()!="") {
                    ok=0;
                    break;
                }
            }
            cantMoveLeft=ok;
        }
        if(!cantMoveRight) {
            bool ok=1;
            for(int i=cc+1;i<_tableView->model()->columnCount();i++) {
                if(_tableView->model()->data(_tableView->model()->index(_tableView->currentIndex().row(),i)).toString()!="") {
                    ok=0;
                    break;
                }
            }
            cantMoveRight=ok;

        }
        if((key!=Qt::Key_Left||!cantMoveLeft)&&(key!=Qt::Key_Right||!cantMoveRight)) {
            if(_tableView->isVisible()) {
                _tableView->keyPressEvent(ke);
            }
        }

        if(_tableView->isHidden()||((key!=Qt::Key_Enter)&&(key!=Qt::Key_Return)/*&&(key!=Qt::Key_Space)*/&&(key!=Qt::Key_Up)&&(key!=Qt::Key_Down&&key!=Qt::Key_PageDown&&key!=Qt::Key_PageUp))) {
            if(_tableView->isHidden()||(((key==Qt::Key_Left&&cantMoveLeft)||(key==Qt::Key_Right&&cantMoveRight))||(key!=Qt::Key_Left&&key!=Qt::Key_Right))) {
                if(_tableView->isHidden()||(!(cantMoveRight&&key==Qt::Key_Right&&_tableView->widgetCursorPosition()==_tableView->widgetText().size()))) {
                    if(_tableView->isHidden()||(!(cantMoveLeft&&key==Qt::Key_Left&&!_tableView->widgetCursorPosition()))) {
                        if(key!=Qt::Key_Alt&&key!=Qt::Key_Shift) {
                            _tableView->_goingRight=key==Qt::Key_Right;
                            (static_cast<QObject *>(widget()))->event(ke);  //in QWidget, event(...) is protected
                        }
                    }
                }
            }
        }
        switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Tab:
            if(_tableView->isVisible()) {
                if(_tableView->currentIndex().row()!=-1&&_tableView->currentIndex().column()!=-1) {
                    setCompletionPrefix(_tableView->currentIndex().model()->data(_tableView->currentIndex()).toString());
                    emit activated(completionPrefix());
                    _tableView->hide();
                }
            }
            break;

        case Qt::Key_Backtab:
        case Qt::Key_Escape:
            _tableView->hide();
            break;
        default:
            break;
        }

        verifyPrefix();
    }
    return 1;
}

void CategoricalCompleter::verifyPrefix()
{
    QString search=completionPrefix();

    for(int i=_data.size()-1;i>=0;i--) {
        if(!_data[i].prefix().size()||!search.indexOf(_data[i].prefix())) {
            if(_currentSubset!=&_data[i]) {
                setModel(new QStringListModel(join(_data[i],_data[i].prefix())));
                _tableView->setData(&_data[i],_data[i].prefix());
                complete();
                _currentSubset=&_data[i];
            }
            break;
        }
    }
}

CategoricalCompleter::~CategoricalCompleter()
{
    //_tableView is child
}

QStringList CategoricalCompleter::join(CompletionCase& l,QString prefix)
{
    QStringList ret;
    for(int i=0;i<l.size();i++) {
        ret<<l[i];
    }
    if(prefix.size()) {
        for(int i=0;i<ret.size();i++) {
            ret[i].prepend(prefix);
        }
    }
    return ret;
}

QStringList CategoricalCompleter::getDefault(QList<CompletionCase>& ccl)
{
    for(int i=0;i<ccl.size();i++) {
        if(ccl[i].prefix()=="") {
            return join(ccl[i]);
        }
    }
    //if none found:
    ccl.push_back(CompletionCase(""));
    return join(ccl.back());
}

// CCLineEdit and CCTextEdit
CCLineEdit::CCLineEdit(QWidget*p) : QLineEdit(p), _cc() { }
CCTextEdit::CCTextEdit(QWidget*p) : QTextEdit(p), _cc() { }

CCLineEdit::CCLineEdit(const QString &s, QWidget *p) : QLineEdit(s,p), _cc() { }
CCTextEdit::CCTextEdit(const QString &s, QWidget *p) : QTextEdit(s,p), _cc() { }

CCLineEdit::~CCLineEdit() { delete _cc; }
CCTextEdit::~CCTextEdit() { delete _cc; }

void CCLineEdit::insert(const QString &i,bool completion)
{
    QString x=text(),y=text();
    y.remove(0,cursorPosition());
    x.truncate(cursorPosition());

    if(completion) {
        bool caught=0;
        if(i.size()) {
            caught= i[i.size()-1]==']';     // this is a hard-coded exception to allow spaces in vector or scalar names. make less hacky
        }

        QChar search;
        if(caught) {
            search='[';
        } else {
            const QString& possiblePhraseEndings=" +-=$.\n:/*]()%^&|!<>";
            // also change in divide!!
            int maxIndex=-1;
            for(int j=0;j<possiblePhraseEndings.size();j++) {
                if(x.lastIndexOf(possiblePhraseEndings[j])>maxIndex) {
                    maxIndex=x.lastIndexOf(possiblePhraseEndings[j]);
                    search=possiblePhraseEndings[j];
                }
            }
        }

        if(x.lastIndexOf(search)!=-1) {
            x.remove(x.lastIndexOf(search)+(caught?0:1),999999);
        } else {
            x.remove(0,x.size());
        }
    }
    setText(x+i+y);
    setCursorPosition((x+i).size());

    //Special exception:
    int cbPos=text().indexOf("]",(x+i).size());
    int cbPosNot=text().indexOf("\\]",(x+i).size())+1;
    int obPos=text().indexOf("[",(x+i).size());
    if(cbPos!=-1&&cbPos!=cbPosNot&&(cbPos<obPos||obPos==-1))
    {
        QString t=text();
        t.remove(cursorPosition(),cbPos-cursorPosition()+1);
        setText(t);
    }
}

void CCTextEdit::insert(const QString &i, bool completion)
{
    QString x=toPlainText(),y=toPlainText();
    y.remove(0,textCursor().position());
    x.truncate(textCursor().position());

    if(completion) {
        bool caught=0;
        if(i.size()) {
            caught = i[i.size()-1]==']';     // this is a hard-coded exception to allow spaces in vector or scalar names. make less hacky
        }

        QChar search;
        if(caught) {
            search='[';
        } else {
            const QString& possiblePhraseEndings=" +-=$.\n:/*]()%^&|!<>";
            // also change in divide!!
            int maxIndex=-1;
            for(int j=0;j<possiblePhraseEndings.size();j++) {
                if(x.lastIndexOf(possiblePhraseEndings[j])>maxIndex) {
                    maxIndex=x.lastIndexOf(possiblePhraseEndings[j]);
                    search=possiblePhraseEndings[j];
                }
            }
        }

        if(x.lastIndexOf(search)!=-1) {
            x.remove(x.lastIndexOf(search)+(caught?0:1),999999);
        }
    }
    setText(x+i+y);
    QTextCursor tc=textCursor();
    tc.setPosition((x+i).size());
    setTextCursor(tc);

    //Special exception:
    int cbPos=toPlainText().indexOf("]",(x+i).size());
    int cbPosNot=toPlainText().indexOf("\\]",(x+i).size())+1;
    int obPos=toPlainText().indexOf("[",(x+i).size());
    if(cbPos!=-1&&cbPos!=cbPosNot&&(cbPos<obPos||obPos==-1))
    {
        QString t=toPlainText();
        t.remove(textCursor().position(),cbPos-textCursor().position()+1);
        setPlainText(t);
    }
}

void CCLineEdit::divide(QString x)
{
    x=(x=="\0")?text():x;
    x.truncate(cursorPosition());

    // this is a hard-coded exception to allow spaces in vector or scalar names. make less hacky {

    bool caught = (x.indexOf("[")!=-1)&&(x.indexOf("]",x.lastIndexOf("["))==-1);
    if(caught) {
        caught=((x.lastIndexOf("\\[")==-1)||(x.lastIndexOf("\\[")+1!=x.lastIndexOf("[")));
    }

    //}

    QChar search;
    if(caught) {
        search='[';
    } else {
        const QString& possiblePhraseEndings=" +-=$.\n:/*]()%^&|!<>";
        // also change in insert!!

        int maxIndex=-2;
        for(int i=0;i<possiblePhraseEndings.size();i++) {
            if(x.lastIndexOf(possiblePhraseEndings[i])>maxIndex) {
                maxIndex=x.lastIndexOf(possiblePhraseEndings[i]);
                search=possiblePhraseEndings[i];
            }
        }
    }

    if(x.lastIndexOf(search)) {
        x.remove(0,x.lastIndexOf(search)+(caught?0:1));
    }

    emit currentPrefixChanged(x);
}

void CCTextEdit::divide(QString x)
{
    x=(x=="\0")?toPlainText():x;
    x.truncate(textCursor().position());

    // this is a hard-coded exception to allow spaces in vector or scalar names. make less hacky {

    bool caught = (x.indexOf("[")!=-1)&&(x.indexOf("]",x.lastIndexOf("["))==-1);
    if(caught) {
        caught=((x.lastIndexOf("\\[")==-1)||(x.lastIndexOf("\\[")+1!=x.lastIndexOf("[")));
    }

    //}

    QChar search;
    if(caught) {
        search='[';
    } else {
        const QString& possiblePhraseEndings=" +-=$.\n:/*]()%^&|!<>";
        // also change in insert!!

        int maxIndex=-2;
        for(int i=0;i<possiblePhraseEndings.size();i++) {
            if(x.lastIndexOf(possiblePhraseEndings[i])>maxIndex) {
                maxIndex=x.lastIndexOf(possiblePhraseEndings[i]);
                search=possiblePhraseEndings[i];
            }
        }
    }

    if(x.lastIndexOf(search)) {
        x.remove(0,x.lastIndexOf(search)+(caught?0:1));
    }

    emit currentPrefixChanged(x);
}

void CCLineEdit::keyPressEvent(QKeyEvent*ev)
{
    if(!_cc) {
        QLineEdit::keyPressEvent(ev);
        return;
    }
    _cc->_tableView->keyPressEvent(ev);
    QLineEdit::keyPressEvent(ev);
    _cc->_tableView->updateSuggestions();
}

void CCTextEdit::keyPressEvent(QKeyEvent*ev)
{
    if(!_cc) {
        QTextEdit::keyPressEvent(ev);
        return;
    }
    _cc->_tableView->keyPressEvent(ev);
    QTextEdit::keyPressEvent(ev);
    _cc->_tableView->updateSuggestions();
}

void CCLineEdit::init(QList<CompletionCase> data)
{
    delete _cc;

    _cc=new CategoricalCompleter(this,data);
    setCompleter(0);
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(divide(QString)));
    connect(this,SIGNAL(cursorPositionChanged(int,int)),this,SLOT(divide()));

    connect(this,SIGNAL(currentPrefixChanged(QString)),_cc,SLOT(setCompletionPrefix(QString)));
    connect(_cc,SIGNAL(activated(QString)),this,SLOT(insert(QString)));
    _cc->_tableView->setLineEdit(this);
    _cc->setWidget(this);
}

void CCTextEdit::init(QList<CompletionCase> data)
{
    delete _cc;

    _cc=new CategoricalCompleter(this,data);
    connect(this,SIGNAL(textChanged()),this,SLOT(divide()));
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(divide()));

    connect(this,SIGNAL(currentPrefixChanged(QString)),_cc,SLOT(setCompletionPrefix(QString)));
    connect(_cc,SIGNAL(activated(QString)),this,SLOT(insert(QString)));
    _cc->_tableView->setTextEdit(this);
    _cc->setWidget(this);
}

// SVCCLineEdit & SVCCTextEdit

SVCCLineEdit::SVCCLineEdit(QWidget*p) : CCLineEdit(p),_store(0),_extraData(0),_svData(0),_allData(0) { }
SVCCTextEdit::SVCCTextEdit(QWidget*p) : CCTextEdit(p),_store(0),_extraData(0),_svData(0),_allData(0) { }

SVCCLineEdit::SVCCLineEdit(const QString &s, QWidget *p) : CCLineEdit(s,p),_store(0),_extraData(0),_svData(0),_allData(0) { }
SVCCTextEdit::SVCCTextEdit(const QString &s, QWidget *p) : CCTextEdit(s,p),_store(0),_extraData(0),_svData(0),_allData(0) { }

SVCCLineEdit::~SVCCLineEdit() { delete _extraData; delete _svData; delete _allData; }
SVCCTextEdit::~SVCCTextEdit() { delete _extraData; delete _svData; delete _allData; }

void SVCCLineEdit::setObjectStore(ObjectStore *store)
{
    _store=store;
    fillKstObjects();
}

void SVCCTextEdit::setObjectStore(ObjectStore *store)
{
    _store=store;
    fillKstObjects();
}


void SVCCLineEdit::init(QList<CompletionCase> data)
{
    if(data.size())
    {
        _extraData=_extraData?_extraData:new QList<CompletionCase>();
        *_extraData=data;
    }
    if(_cc) {
        delete _allData;
    }
    _allData = new QList<CompletionCase>;
    for(int k=0;k<2;k++) {
        QList<CompletionCase>* add=k?_svData:_extraData;
        for(int i=0;add&&i<add->size();i++) {
            _allData->push_back((*add)[i]);
        }
    }
    CCLineEdit::init(*_allData);
}

void SVCCTextEdit::init(QList<CompletionCase> data)
{
    if(data.size())
    {
        _extraData=_extraData?_extraData:new QList<CompletionCase>();
        *_extraData=data;
    }
    if(_cc) {
        delete _allData;
    }
    _allData = new QList<CompletionCase>;
    for(int k=0;k<2;k++) {
        QList<CompletionCase>* add=k?_svData:_extraData;
        for(int i=0;add&&i<add->size();i++) {
            _allData->push_back((*add)[i]);
        }
    }
    CCTextEdit::init(*_allData);
}


void SVCCLineEdit::fillKstObjects()
{
    if(!_store) {
        return;
    }
    if(_svData) {
        delete _svData;
    }
    _svData = new QList<CompletionCase>;
    _svData->push_back(CompletionCase("["));
    _svData->back().push_back(Category("Scalars"));
    _svData->back().push_back(Category("Vectors"));

    ScalarList scalarList = _store->getObjects<Scalar>();
    VectorList vectorList = _store->getObjects<Vector>();

    {
        ScalarList::ConstIterator it = scalarList.begin();
        for (; it != scalarList.end(); ++it) {
            ScalarPtr scalar = (*it);

            scalar->readLock();
            _svData->back()[0].push_back(scalar->Name()+"]");
            scalar->unlock();
        }
    }

    {
        VectorList::ConstIterator it = vectorList.begin();
        for (; it != vectorList.end(); ++it) {
            VectorPtr vector = (*it);

            vector->readLock();
            _svData->back()[1].push_back(vector->Name()+"]");
            vector->unlock();
        }
    }

    init();
}

void SVCCTextEdit::fillKstObjects()
{
    if(!_store) {
        return;
    }
    if(_svData) {
        delete _svData;
    }
    _svData = new QList<CompletionCase>;
    _svData->push_back(CompletionCase("["));
    _svData->back().push_back(Category("Scalars"));
    //_svData->back().push_back(Category("Vectors"));

    ScalarList scalarList = _store->getObjects<Scalar>();
    //VectorList vectorList = _store->getObjects<Vector>();
    StringList stringList = _store->getObjects<String>();

    {
        ScalarList::ConstIterator it = scalarList.begin();
        for (; it != scalarList.end(); ++it) {
            ScalarPtr scalar = (*it);

            scalar->readLock();
            _svData->back()[0].push_back(scalar->Name()+"]");
            scalar->unlock();
        }
    }

    /*{
        VectorList::ConstIterator it = vectorList.begin();
        for (; it != vectorList.end(); ++it) {
            VectorPtr vector = (*it);

            vector->readLock();
            _svData->back()[1].push_back(vector->Name()+"]");
            vector->unlock();
        }
    }*/

    _svData->back().push_back(Category("Strings"));

    StringList::ConstIterator it = stringList.begin();
    for (; it != stringList.end(); ++it) {
        StringPtr string = (*it);

        string->readLock();
        _svData->back()[1].push_back(string->Name()+"]");
        string->unlock();
    }



    init();
}

void SVCCLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMouseEvent fake(QEvent::MouseButtonPress,event->pos(),Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
    mousePressEvent(&fake); //update cursor
    QMenu *menu = createStandardContextMenu();
    QAction* newVectorAction=new QAction(tr("Insert New &Vector"),this);
    connect(newVectorAction,SIGNAL(triggered()),this,SLOT(newVector()));
    menu->insertAction(menu->actions()[0],newVectorAction);

    QAction* newScalarAction=new QAction(tr("Insert New &Scalar"),this);
    connect(newScalarAction,SIGNAL(triggered()),this,SLOT(newScalar()));
    menu->insertAction(menu->actions()[1],newScalarAction);

    QAction* editAction=0;
    QString x=text();
    int openVector=x.lastIndexOf("[",cursorPosition());
    int closeVector=x.indexOf("]",cursorPosition());
    if(openVector!=-1&&closeVector!=-1) {
        x.remove(0,openVector+1);
        x.remove(closeVector-openVector-1,99999);
        if(x.indexOf("[")==x.indexOf("]")&&x.indexOf("[")==-1) {

            QString x=text();
            int openVector=x.lastIndexOf("[",cursorPosition());
            int closeVector=x.indexOf("]",cursorPosition());
            if(openVector!=-1&&closeVector!=-1) {
                x.remove(0,openVector+1);
                x.remove(closeVector-openVector-1,99999);
                if(x.indexOf("[")==x.indexOf("]")&&x.indexOf("[")==-1) {
                    ObjectPtr ptr = _store->retrieveObject(x);

                    VectorPtr vector = kst_cast<Vector>(ptr);
                    ScalarPtr scalar = kst_cast<Scalar>(ptr);

                    if((scalar&&scalar->editable())||(vector&&vector->editable())) {

                        editAction=new QAction(tr("Edit")+" "+x,this);
                        connect(editAction,SIGNAL(triggered()),this,SLOT(editItem()));
                        menu->insertAction(menu->actions()[2],editAction);
                    }
                }
            }
        }
    }

    menu->insertSeparator(menu->actions()[editAction?3:2]);

    menu->exec(event->globalPos());
    delete menu;
    delete newVectorAction;
    delete newScalarAction;
    delete editAction;
}

void SVCCTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMouseEvent fake(QEvent::MouseButtonPress,event->pos(),Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
    mousePressEvent(&fake); //update cursor
    QMenu *menu = createStandardContextMenu();
    /*QAction* newVectorAction=new QAction(tr("Insert New &Vector"),this);
    connect(newVectorAction,SIGNAL(triggered()),this,SLOT(newVector()));
    menu->insertAction(menu->actions()[0],newVectorAction);*/

    QAction* newScalarAction=new QAction(tr("Insert New &Scalar"),this);
    connect(newScalarAction,SIGNAL(triggered()),this,SLOT(newScalar()));
    menu->insertAction(menu->actions()[0],newScalarAction);

    QAction* newStringAction=new QAction(tr("Insert New &String"),this);
    connect(newStringAction,SIGNAL(triggered()),this,SLOT(newString()));
    menu->insertAction(menu->actions()[1],newStringAction);

    QAction* editAction=0;
    QString x=toPlainText();
    int openVector=x.lastIndexOf("[",textCursor().position());
    int closeVector=x.indexOf("]",textCursor().position());
    if(openVector!=-1&&closeVector!=-1) {
        x.remove(0,openVector+1);
        x.remove(closeVector-openVector-1,99999);
        if(x.indexOf("[")==x.indexOf("]")&&x.indexOf("[")==-1) {
            QString x=toPlainText();
            int openVector=x.lastIndexOf("[",textCursor().position());
            int closeVector=x.indexOf("]",textCursor().position());
            if(openVector!=-1&&closeVector!=-1) {
                x.remove(0,openVector+1);
                x.remove(closeVector-openVector-1,99999);
                if(x.indexOf("[")==x.indexOf("]")&&x.indexOf("[")==-1) {
                    ObjectPtr ptr = _store->retrieveObject(x);

                    //VectorPtr vector = kst_cast<Vector>(ptr);
                    ScalarPtr scalar = kst_cast<Scalar>(ptr);
                    StringPtr string = kst_cast<String>(ptr);

                    if((scalar&&scalar->editable())||(string&&string->editable())) {
                        editAction=new QAction(tr("Edit")+" "+x,this);
                        connect(editAction,SIGNAL(triggered()),this,SLOT(editItem()));
                        menu->insertAction(menu->actions()[2],editAction);
                    }
                }
            }
        }
    }

    menu->insertSeparator(menu->actions()[editAction?3:2]);

    menu->exec(event->globalPos());
    delete menu;
    //delete newVectorAction;
    delete newScalarAction;
    delete newStringAction;
    delete editAction;
}


void SVCCLineEdit::newVector()
{
    QString newName;
    DialogLauncher::self()->showVectorDialog(newName, 0, true);
    fillKstObjects();
    VectorPtr vector = kst_cast<Vector>(_store->retrieveObject(newName));

    if (vector) {
        QString vName="["+vector->Name()+"]";
        insert(vName,0);
    }
}

void SVCCTextEdit::newVector()
{
    QString newName;
    DialogLauncher::self()->showVectorDialog(newName, 0, true);
    fillKstObjects();
    VectorPtr vector = kst_cast<Vector>(_store->retrieveObject(newName));

    if (vector) {
        QString vName="["+vector->Name()+"]";
        insert(vName,0);
    }
}

void SVCCLineEdit::newScalar()
{
    QString scalarName;
    DialogLauncher::self()->showScalarDialog(scalarName, 0, true);
    fillKstObjects();
    ScalarPtr scalar = kst_cast<Scalar>(_store->retrieveObject(scalarName));

    if (scalar) {
        QString sName="["+scalar->Name()+"]";
        insert(sName,0);
    }
}

void SVCCTextEdit::newScalar()
{
    QString scalarName;
    DialogLauncher::self()->showScalarDialog(scalarName, 0, true);
    fillKstObjects();
    ScalarPtr scalar = kst_cast<Scalar>(_store->retrieveObject(scalarName));

    if (scalar) {
        QString sName="["+scalar->Name()+"]";
        insert(sName,0);
    }
}

void SVCCTextEdit::newString()
{
    QString stringName;
    DialogLauncher::self()->showStringDialog(stringName, 0, true);
    fillKstObjects();
    StringPtr string = kst_cast<String>(_store->retrieveObject(stringName));

    if (string) {
        QString sName="["+string->Name()+"]";
        insert(sName,0);
    }
}

void SVCCLineEdit::editItem()
{
    QString x=text();
    int openVector=x.lastIndexOf("[",cursorPosition());
    int closeVector=x.indexOf("]",cursorPosition());
    if(openVector!=-1&&closeVector!=-1) {
        x.remove(0,openVector+1);
        x.remove(closeVector-openVector-1,99999);
        if(x.indexOf("[")==x.indexOf("]")&&x.indexOf("[")==-1) {
            ObjectPtr ptr = _store->retrieveObject(x);

            //could be:
            VectorPtr vector = kst_cast<Vector>(ptr);
            ScalarPtr scalar = kst_cast<Scalar>(ptr);
            if(vector) {
                if (vector->provider()) {
                    DialogLauncher::self()->showObjectDialog(vector->provider());
                } else {
                    if(ObjectPtr(vector))
                    {
                        QString vectorName;
                        DialogLauncher::self()->showVectorDialog(vectorName,ObjectPtr(vector),1);
                    }
                }
                fillKstObjects();
            } else if(scalar) {
                if (scalar->provider()) {
                    DialogLauncher::self()->showObjectDialog(scalar->provider());
                } else {
                    if(ObjectPtr(scalar))
                    {
                        QString scalarName;
                        DialogLauncher::self()->showScalarDialog(scalarName,ObjectPtr(scalar),1);
                    }
                }
                fillKstObjects();
            } else {
            }
        }
    }
}

void SVCCTextEdit::editItem()
{
    QString x=toPlainText();
    int openVector=x.lastIndexOf("[",textCursor().position());
    int closeVector=x.indexOf("]",textCursor().position());
    if(openVector!=-1&&closeVector!=-1) {
        x.remove(0,openVector+1);
        x.remove(closeVector-openVector-1,99999);
        if(x.indexOf("[")==x.indexOf("]")&&x.indexOf("[")==-1) {
            ObjectPtr ptr = _store->retrieveObject(x);

            //could be:
            VectorPtr vector = kst_cast<Vector>(ptr);
            ScalarPtr scalar = kst_cast<Scalar>(ptr);
            StringPtr string = kst_cast<String>(ptr);
            if(vector) {
                if (vector->provider()) {
                    DialogLauncher::self()->showObjectDialog(vector->provider());
                } else {
                    if(ObjectPtr(vector))
                    {
                        QString vectorName;
                        DialogLauncher::self()->showVectorDialog(vectorName,ObjectPtr(vector),1);
                    }
                }
                fillKstObjects();
            } else if(scalar) {
                if (scalar->provider()) {
                    DialogLauncher::self()->showObjectDialog(scalar->provider());
                } else {
                    if(ObjectPtr(scalar))
                    {
                        QString scalarName;
                        DialogLauncher::self()->showScalarDialog(scalarName,ObjectPtr(scalar),1);
                    }
                }
                fillKstObjects();
            } else if(string) {
                if (string->provider()) {
                    DialogLauncher::self()->showObjectDialog(string->provider());
                } else {
                    if(ObjectPtr(string))
                    {
                        QString stringName;
                        DialogLauncher::self()->showScalarDialog(stringName,ObjectPtr(string),1);
                    }
                }
                fillKstObjects();
            }
        }
    }
}

// CCTableModel

int CCTableModel::rowCount(const QModelIndex &parent=QModelIndex()) const
{
    Q_UNUSED(parent);

    int max=0;
    for(int i=0;i<_visibleData.size();i++) {
        max=qMax(max,_visibleData[i].size());
    }
    return max-1; // top row is title
}

int CCTableModel::columnCount(const QModelIndex &parent=QModelIndex()) const
{
    Q_UNUSED(parent);
    return _visibleData.size();
}

QVariant CCTableModel::data(const QModelIndex &index, int role) const
{
    if(role!=Qt::DisplayRole) return QVariant();  // Return unvalid QVariant
    Q_ASSERT(index.column()<columnCount());
    if(index.column()==-1) {
        return QVariant("");
    }
    if (index.row()+1>=_visibleData[index.column()].size()) return QVariant("");  // +1 because top row is title
    return _visibleData[index.column()][index.row()+1]; // +1 because top row is title
}

Qt::ItemFlags CCTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant CCTableModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED(orientation);
    if(role!=Qt::DisplayRole) return QVariant();
    else return _visibleData[section][0];
}

// CCTableView
CCTableView::CCTableView(CompletionCase* data) : _data(data), origModel(0), completer(0), _le(0), _te(0), _goingRight(0)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setMinimumHeight(150);
    //    setMinimumWidth(300);
    verticalHeader()->hide();
}

void CCTableView::updateSuggestions()
{
    //this could be made more efficient.
    if(!model()||(!_le&&!_te)) {
        return;
    }

    if(!origModel) origModel=model();
    else delete model();

    QList<QStringList> items;
    for(int i=0;i<_data->size();i++) {
        items.push_back(QStringList());
        items[i].push_back((*_data)[i].title());
    }
    QString x=completer->completionPrefix();
    if(_le) {
        x.truncate(_le->cursorPosition());
    } else if (_te) {
        x.truncate(_te->textCursor().position());
    }

    for(int i=0;i<completer->completionCount();i++) {
        completer->setCurrentRow(i);
        QString item=completer->currentCompletion();
        item.remove(_prefix);
        int x=-1;
        for(int j=0;j<_data->size();j++) {
            switch((*_data)[j].indexOf(item)) {
            case -1:
                break;
            default:
                x=j;
                break;
            }
            if(x==j) break;
        }
        if(x!=-1) {
            items[x]<<_data->prefix()+item;
        }
    }
    for(int i=0;i<items.size();i++) {
        if(items[i].size()==1) {
            horizontalHeader()->hideSection(i);
        } else {
            horizontalHeader()->showSection(i);
        }
    }
    setModel(new CCTableModel(items));
    int minOK=-1;
    int maxOK=-1;
    for(int i=0;i<_data->size();i++) {
        if(items[i].size()>1) {
            minOK=(minOK==-1)?i:minOK;
            maxOK=i;
        }
    }
    if(minOK!=-1) {
        setCurrentIndex(model()->index(0,_goingRight?maxOK:minOK));
    }

    resizeColumnsToContents();
    completer->complete();
}

void CCTableView::setColumnHeaders(QStringList columnHeaders)
{
    for(int i=0;i<columnHeaders.size();i++) {
        model()->setHeaderData(i,Qt::Horizontal,columnHeaders[i]);
    }
}

void CCTableView::setCompleter(CategoricalCompleter* completer)
{
    this->completer=completer;
}

void CCTableView::setData(CompletionCase* data,QString prefix)
{
    _prefix=prefix;
    _data=data;
    origModel=0;
    updateSuggestions();
}

void CCTableView::keyPressEvent(QKeyEvent *event)
{
    QTableView::keyPressEvent(event);
}

void CCTableView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    QModelIndex idx = indexAt(pos);
    setCurrentIndex(idx);
    if(currentIndex().row()!=-1&&currentIndex().column()!=-1)
    {
        completer->setCompletionPrefix(currentIndex().model()->data(currentIndex()).toString());
        emit activateHint(completer->completionPrefix());
        if(_le) {
            _le->insert(completer->completionPrefix());
        } else {
            _te->insert(completer->completionPrefix());
        }
        hide();
    }
}

void CCTableView::showEvent(QShowEvent *)
{
    //    resizeColumnsToContents();
    int bestWidth=qMax(width(),(int)((horizontalHeader()->length()+verticalScrollBar()->width())));
    horizontalHeader()->setStretchLastSection(1);
    setMinimumWidth(bestWidth);
    setMaximumWidth(bestWidth);
}

}
