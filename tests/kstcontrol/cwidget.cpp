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

#include "cwidget.h"
#include "ui_cwidget.h"
#include <QStringList>
#include <QCompleter>

CWidget::CWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CWidget)
{
    ls.connectToServer("kstScript");
    ls.waitForConnected(3000);
    ui->setupUi(this);
    connect(ui->send,SIGNAL(clicked()),this,SLOT(send()));
    connect(ui->commandline,SIGNAL(returnPressed()),ui->send,SLOT(animateClick()));

    QByteArray ba="commands()";
    ls.write(ba); ls.flush(); ls.waitForReadyRead(3000);
    ui->doc->setText(ls.read(300000));
    QStringList comp=ui->doc->toPlainText().split("\n");
    delete ui->commandline->completer();
    ui->commandline->setCompleter(new QCompleter(comp));

    setWindowTitle("Kst Control");
    ui->commandline->setFocus();
}

void CWidget::send() {
    QByteArray ba(ui->commandline->text().toStdString().c_str());
    ls.write(ba); ls.flush(); ls.waitForReadyRead(3000);
    QString x=ui->log->toHtml();
    x+="<B>"+ui->commandline->text()+"</B><br>"+ls.read(30000)+"<br>";
    ui->commandline->clear();
    ui->log->setText(x);
    QTextCursor tc=ui->log->textCursor();
    tc.movePosition(QTextCursor::End);
    ui->log->setTextCursor(tc);

    ba="commands()";
    ls.write(ba); ls.flush(); ls.waitForReadyRead(3000);
    ui->doc->setText(ls.read(300000));
    QStringList comp=ui->doc->toPlainText().split("\n");
    delete ui->commandline->completer();
    ui->commandline->setCompleter(new QCompleter(comp));

    ui->commandline->setFocus();
}

CWidget::~CWidget()
{
    QByteArray ba("done()");
    ls.write(ba); ls.flush(); ls.waitForReadyRead(3000); qDebug()<<ls.read(30000);
    delete ui;
}
