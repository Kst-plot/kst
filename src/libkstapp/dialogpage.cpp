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

#include "dialogtab.h"

#include <QTabBar>

namespace Kst {

DialogPage::DialogPage(QWidget *parent)
  : QTabWidget(parent) {

  tabBar()->setVisible(false);
}


DialogPage::~DialogPage() {
}


void DialogPage::addDialogTab(DialogTab *tab) {
  connect(this, SIGNAL(ok()), tab, SIGNAL(ok()));
  connect(this, SIGNAL(apply()), tab, SIGNAL(apply()));
  connect(this, SIGNAL(cancel()), tab, SIGNAL(cancel()));
  connect(tab, SIGNAL(modified()), this, SIGNAL(modified()));
  addTab(tab, tab->tabTitle());
}


void DialogPage::setVisible(bool visible) {

  tabBar()->setVisible(count() > 1);

  QTabWidget::setVisible(visible);
}

}

// vim: ts=2 sw=2 et
