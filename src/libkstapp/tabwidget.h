/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

namespace Kst {

class View;

class TabWidget : public QTabWidget
{
    Q_OBJECT

  public:
    TabWidget(QWidget *parent);
    ~TabWidget();

    View *currentView() const;
    QList<View*> views() const;
    // pass tab name as objectName
    void addView(View*);
    void deleteView(View* view);
    void clear();

  Q_SIGNALS:
        void currentViewModeChanged();

  public Q_SLOTS:
    View *createView();
    void renameCurrentView();
    void closeCurrentView();
    void setCurrentViewName(QString name);

  private Q_SLOTS:
    void contextMenu(const QPoint&);

  private:
    int _cnt;
};

}

#endif

// vim: ts=2 sw=2 et
