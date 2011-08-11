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

#include <QCompleter>
#include <QStringList>
#include <QAbstractTableModel>
#include <QTableView>
#include "cclineedit.h"

#ifndef CCLINEEDIT_P_H
#define CCLINEEDIT_P_H

namespace Kst {

class CCTableModel;
class CCTableView;

class CategoricalCompleter : public QCompleter {
    Q_OBJECT
    QList<CompletionCase> _data;
    CCTableView* _tableView;
    CompletionCase* _currentSubset;
public:
    friend class CCCommonEdit;
    friend class CCLineEdit;
    friend class CCTextEdit;
    friend class SVCCLineEdit;
    friend class SVCCTextEdit;
    /**
   * Creates an autocompleter for child.
   *
   * Each QStringList inside "data" represents a column.
   * The first element of each column is the column title (category)
   * The remaining elements are items under that category.
   *
   * CategoricalCompleter does not take ownership of data.
   */
    CategoricalCompleter(QLineEdit *lineEdit, QList<CompletionCase> data);
    CategoricalCompleter(QTextEdit *textEdit, QList<CompletionCase> data);
    ~CategoricalCompleter();

protected:
    bool eventFilter(QObject *o, QEvent *e);
    //  QStringList splitPath(const QString &path) const;
    //  QString pathFromIndex(const QModelIndex &index) const;
    static QStringList join(CompletionCase&,QString prefix="");
    static QStringList getDefault(QList<CompletionCase>&);
public slots:
    void verifyPrefix();
};

class CCTableModel : public QAbstractTableModel {
    Q_OBJECT
    QList<QStringList> _visibleData;
    QSize s_minSizeCache[32];
public:
    explicit CCTableModel(const QList<QStringList>& visibleData) : _visibleData(visibleData)
    {
        for(int i=0;i<32;i++) {
            s_minSizeCache[i]=QSize(-1,-1);
        }
    }

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
signals:
    void checkSize() const;
};

class CCTableView : public QTableView {
    Q_OBJECT
    CompletionCase* _data;
    QAbstractItemModel* origModel;
    CategoricalCompleter* completer;
    CCLineEdit* _le;
    CCTextEdit* _te;
    QString _prefix;
    bool _goingRight;

public:
    friend class CCLineEdit;
    friend class CCTextEdit;
    friend class CategoricalCompleter;

    CCTableView(CompletionCase* _data);
    const QString& prefix() { return _prefix; }

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *event);
    int widgetCursorPosition() {
        if(_le) {
            return _le->cursorPosition();
        } else if (_te) {
            return _te->textCursor().anchor();
        } else {
            qDebug()<<"CCTableView::widgetCursorPosition(): invalid widget";
            return -1;
        }
    }
    QString widgetText() {
        if(_le) {
            return _le->text();
        } else if (_te) {
            return _te->toPlainText();
        } else {
            qDebug()<<"CCTableView::widgetText(): invalid widget";
            return "CCTableView::widgetText(): invalid widget";
        }
    }

public slots:
    void updateSuggestions();
    void setColumnHeaders(QStringList columnHeaders);
    void setCompleter(CategoricalCompleter* completer);
    void setData(CompletionCase* data,QString prefix="");
    void setLineEdit(CCLineEdit* le) { bool set=_le; _le=le; if(set) le->fillKstObjects(); }
    void setTextEdit(CCTextEdit* te) { _te=te;}

signals:
    void activateHint(const QString&);
};

}

#endif // CCLINEEDIT_P_H
