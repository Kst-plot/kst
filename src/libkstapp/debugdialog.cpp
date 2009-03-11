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

#include "debugdialog.h"
#include "logwidget.h"
#include <debug.h>
#include <events.h>
#include <logevents.h>
#include <datasource.h>

#include "kst_i18n.h"

#include <QDebug>

namespace Kst {

DebugDialog::DebugDialog(QWidget *parent)
  : QDialog(parent), _store(0) {
  setupUi(this);

  _log = new LogWidget(TabPage);

  gridLayout2->addWidget(_log, 0, 0, 0, 2);

  connect(_clear, SIGNAL(clicked()), this, SLOT(clear()));
  connect(_showDebug, SIGNAL(toggled(bool)), _log, SLOT(setShowDebug(bool)));
  connect(_showWarning, SIGNAL(toggled(bool)), _log, SLOT(setShowWarning(bool)));
  connect(_showNotice, SIGNAL(toggled(bool)), _log, SLOT(setShowNotice(bool)));
  connect(_showError, SIGNAL(toggled(bool)), _log, SLOT(setShowError(bool)));

  _buildInfo->setText(i18n("<h1>Kst</h1> Version %1 (%2)").arg(KSTVERSION).arg(Debug::self()->kstRevision()));
}


DebugDialog::~DebugDialog() {
}


bool DebugDialog::event(QEvent* e) {
  if (e->type() == EventTypeLog) {
    LogEvent *le = dynamic_cast<LogEvent*>(e);
    if (le) {
      switch (le->_eventType) {
        case LogEvent::LogAdded:
          _log->logAdded(le->_msg);
          if (le->_msg.level == Debug::Error) {
            emit notifyOfError();
          }
          break;
        case LogEvent::LogCleared:
            _log->clear();
            emit notifyAllClear();
          break;
        default:
          break;
      }
    }
  }
  return QDialog::event(e);
}


void DebugDialog::clear() {
  Debug::self()->clear();
}


void DebugDialog::show() {
  Q_ASSERT(_store);
  _dataSources->clear();

  const QStringList& pl = DataSource::pluginList();
  foreach (QString pluginName, pl) {
    new QTreeWidgetItem(_dataSources, QStringList(pluginName));
  }

  QTreeWidgetItemIterator it(_dataSources);
  while (*it) {
    foreach (DataSourcePtr dataSource, _store->dataSourceList()) {
      if (dataSource->sourceName() == (*it)->text(0)) {
        QStringList list(QString::null);
        list += dataSource->fileName();
        new QTreeWidgetItem(*it, list);
      }
    }
    ++it;
  }
  QDialog::show();
}

}

// vim: ts=2 sw=2 et
