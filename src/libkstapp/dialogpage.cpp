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

#include "dialogpage.h"

#include "dialog.h"
#include "dialogtab.h"

#include <QTabBar>

namespace Kst {

DialogPage::DialogPage(Dialog *parent)
  : QWidget(parent), _dialog(parent) {
}


DialogPage::~DialogPage() {
}


Dialog *DialogPage::dialog() const {
  return _dialog;
}


void DialogPage::addDialogTab(DialogTab *tab) {
  connect(this, SIGNAL(ok()), tab, SIGNAL(ok()));
  connect(this, SIGNAL(apply()), tab, SIGNAL(apply()));
  connect(this, SIGNAL(cancel()), tab, SIGNAL(cancel()));
  connect(tab, SIGNAL(modified()), this, SIGNAL(modified()));
  _widget = tab;
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(tab);
  setLayout(layout);
}


QWidget* DialogPage::currentWidget() {
  return _widget;
}

DialogPageTab::DialogPageTab(Dialog *parent)
  : DialogPage(parent) {

  _tabWidget = new QTabWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(_tabWidget);
  setLayout(layout);
}


DialogPageTab::~DialogPageTab() {
}


void DialogPageTab::addDialogTab(DialogTab *tab) {
  connect(this, SIGNAL(ok()), tab, SIGNAL(ok()));
  connect(this, SIGNAL(apply()), tab, SIGNAL(apply()));
  connect(this, SIGNAL(cancel()), tab, SIGNAL(cancel()));
  connect(tab, SIGNAL(modified()), this, SIGNAL(modified()));
  _tabWidget->addTab(tab, tab->tabTitle());
}


QWidget* DialogPageTab::currentWidget() {
  return _tabWidget->currentWidget();
}

}

// vim: ts=2 sw=2 et
