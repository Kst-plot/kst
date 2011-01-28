/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debugdialog.h"
#include "logwidget.h"
#include "debug.h"
#include "events.h"
#include "logevents.h"
#include "datasource.h"
#include "datasourcepluginmanager.h"
#include "kst_i18n.h"

#include <QDebug>

namespace Kst {

DebugDialog::DebugDialog(QWidget *parent)
  : QDialog(parent), _store(0) {
  setupUi(this);

  _log = new LogWidget(_logTab);

  _logTabLayout->addWidget(_log,0,0);

  connect(_clear, SIGNAL(clicked()), this, SLOT(clear()));
  connect(_showDebug, SIGNAL(toggled(bool)), _log, SLOT(setShowDebug(bool)));
  connect(_showWarning, SIGNAL(toggled(bool)), _log, SLOT(setShowWarning(bool)));
  connect(_showNotice, SIGNAL(toggled(bool)), _log, SLOT(setShowNotice(bool)));
  connect(_showError, SIGNAL(toggled(bool)), _log, SLOT(setShowError(bool)));

  if (!Debug::self()->kstRevision().isEmpty())
    _buildInfo->setText(i18n("<h1>Kst</h1> Version %1 (%2)").arg(KSTVERSION).arg(Debug::self()->kstRevision()));
  else
    _buildInfo->setText(i18n("<h1>Kst</h1> Version %1").arg(KSTVERSION));
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
  _dataSources->setColumnCount(2);
  const QStringList pl = DataSourcePluginManager::pluginList();
  foreach (const QString& pluginName, pl) {
    QTreeWidgetItem* name = new QTreeWidgetItem(QStringList() << pluginName);
    name->setData(1, Qt::DisplayRole, DataSourcePluginManager::pluginFileName(pluginName));
    _dataSources->insertTopLevelItem (0, name);
  }

  _dataSources->header()->resizeSections(QHeaderView::ResizeToContents);
  QDialog::show();
}

}

// vim: ts=2 sw=2 et
