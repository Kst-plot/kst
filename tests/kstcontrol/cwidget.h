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
#ifndef CWIDGET_H
#define CWIDGET_H

#include <QMainWindow>
#include <QLocalSocket>

namespace Ui {
    class CWidget;
}

class CWidget : public QMainWindow
{
    Q_OBJECT
    QLocalSocket ls;

public:
    explicit CWidget(QWidget *parent = 0);
    ~CWidget();

public slots:
    void send();

private:
    Ui::CWidget *ui;
};

#endif // CWIDGET_H
