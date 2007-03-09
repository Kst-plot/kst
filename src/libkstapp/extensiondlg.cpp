/***************************************************************************
                   extensiondlg.cpp
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "extensiondlg.h"

#include <kst_export.h>

#include <klibloader.h>
#include <kservicetype.h>
#include <kservicetypetrader.h>
#include "kst.h"
#include "extensionmgr.h"

ExtensionDialog::ExtensionDialog(QWidget *parent)
    : QDialog(parent) {
  setupUi(this);

 connect(_ok, SIGNAL(clicked()), this, SLOT(accept()));

 connect(_cancel, SIGNAL(clicked()), this, SLOT(reject()));
}


ExtensionDialog::~ExtensionDialog() {}


void ExtensionDialog::show() {
  _extensions->clear();
  KService::List sl = KServiceTypeTrader::self()->query("Kst Extension");
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    KService::Ptr service = *it;
    QString name = service->property("Name").toString();
    Q3CheckListItem *i = new Q3CheckListItem(_extensions, name, Q3CheckListItem::CheckBox);
    i->setText(1, service->property("Comment").toString());
    i->setText(2, service->property("X-Kst-Plugin-Author").toString());
    i->setText(3, KLibLoader::findLibrary(service->library().toLatin1()));
    if (!ExtensionMgr::self()->extensions().contains(name)) {
      ExtensionMgr::self()->setEnabled(name, service->property("X-Kst-Enabled").toBool());
    }
    i->setOn(ExtensionMgr::self()->enabled(name));
  }
  QDialog::show();
}


void ExtensionDialog::accept() {
  ExtensionMgr *mgr = ExtensionMgr::self();
  Q3ListViewItemIterator it(_extensions); // don't use Checked since it is too new
  while (it.current()) {
    mgr->setEnabled(it.current()->text(0), static_cast<Q3CheckListItem*>(it.current())->isOn());
    ++it;
  }
  mgr->updateExtensions();
  QDialog::accept();
}

#include "extensiondlg.moc"

// vim: ts=2 sw=2 et
