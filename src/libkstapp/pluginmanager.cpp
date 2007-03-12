/***************************************************************************
                   pluginmanager.cpp
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

#include "pluginmanager.h"

#define COLUMN_READABLE_NAME  0
#define COLUMN_LOADED         1
#define COLUMN_NAME           5

#include "plugincollection.h"
#include <kstandarddirs.h>
#include "pluginxmlparser.h"
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kdeversion.h>
#include <kio/netaccess.h>
#include <qregexp.h>

#include <kst_export.h>

PluginManager::PluginManager(QWidget *parent)
    : QDialog(parent) {
  setupUi(this);

 connect(_close, SIGNAL(clicked()), this, SLOT(close()));

 connect(_pluginList, SIGNAL(selectionChanged(Q3ListViewItem*)), this, SLOT(selectionChanged(Q3ListViewItem*)));

 connect(_install, SIGNAL(clicked()), this, SLOT(install()));

 connect(_remove, SIGNAL(clicked()), this, SLOT(remove()));

 connect(pushButton4, SIGNAL(clicked()), this, SLOT(rescan()));
}


PluginManager::~PluginManager() {}


void PluginManager::init() {
  _pluginList->setAllColumnsShowFocus(true);
  reloadList();
}


void PluginManager::selectionChanged( Q3ListViewItem *item ) {
  _remove->setEnabled(item != 0L);
}


void PluginManager::install() {
  KUrl xmlfile = KFileDialog::getOpenUrl(KUrl(), "*.xml", this, i18n("Select Plugin to Install"));

  if (xmlfile.isEmpty()) {
    return;
  }

  QString tmpFile;
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)

  if (!KIO::NetAccess::download(xmlfile, tmpFile, this)) {
#else
  if (!KIO::NetAccess::download(xmlfile, tmpFile)) {
#endif
    KMessageBox::error(this, i18n("Unable to access file %1.").arg(xmlfile.prettyUrl()), i18n("KST Plugin Loader"));
    return;
  }

  PluginXMLParser parser;

  if (parser.parseFile(tmpFile)) {
    KIO::NetAccess::removeTempFile(tmpFile);
    KMessageBox::error(this, i18n("Invalid plugin file."), i18n("KST Plugin Loader"));
    return;
  }

  QString path = KGlobal::dirs()->saveLocation("kstplugins");
  KUrl pathUrl;
  pathUrl.setPath(path);

  // First try copying the .so file in
  KUrl sofile = xmlfile;
  QString tmpSoFile = sofile.path();
  tmpSoFile.replace(QRegExp(".xml$"), ".so");
  sofile.setPath(tmpSoFile);

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)

  if (!KIO::NetAccess::dircopy(sofile, pathUrl, this)) {
#else
  if (!KIO::NetAccess::dircopy(sofile, pathUrl)) {
#endif
    KIO::NetAccess::removeTempFile(tmpFile);
    KMessageBox::error(this, i18n("Unable to copy plugin file %1 to %2.").arg(sofile.prettyUrl()).arg(pathUrl.prettyUrl()), i18n("KST Plugin Loader"));
    return;
  }

  KUrl tmpFileUrl;
  tmpFileUrl.setPath(tmpFile);
  pathUrl.setFileName(xmlfile.fileName());

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)

  if (!KIO::NetAccess::dircopy(tmpFileUrl, pathUrl, this)) {
#else
  if (!KIO::NetAccess::dircopy(tmpFileUrl, pathUrl)) {
#endif
    KMessageBox::error(this, i18n("Internal temporary file %1 could not be copied to plugin directory %2.").arg(tmpFile).arg(path), i18n("KST Plugin Loader"));
  }

  KIO::NetAccess::removeTempFile(tmpFile);
  rescan();
}


void PluginManager::remove
  () {
  Q3ListViewItem *item = _pluginList->selectedItem();
  if (!item) {
    return;
  }

  int rc = KMessageBox::questionYesNo(this, i18n("Are you sure you wish to remove the plugin \"%1\" from the system?").arg(item->text(COLUMN_READABLE_NAME)), i18n("KST Plugin Loader"));

  if (rc != KMessageBox::Yes) {
    return;
  }

  if (PluginCollection::self()->isLoaded(item->text(COLUMN_NAME))) {
    PluginCollection::self()->unloadPlugin(item->text(COLUMN_NAME));
    item->setPixmap(COLUMN_LOADED, KStandardDirs::locate("data", "kst/pics/no.png"));
  }

  PluginCollection::self()->deletePlugin(PluginCollection::self()->pluginNameList()[item->text(COLUMN_NAME)]);

  delete item;
  selectionChanged(_pluginList->selectedItem());
}


void PluginManager::rescan() {
  PluginCollection::self()->rescan();
  reloadList();
  emit rescanned();
}


void PluginManager::reloadList() {
  _pluginList->clear();
  PluginCollection *pc = PluginCollection::self();
  QStringList loadedPluginList = pc->loadedPluginList();
  const QMap<QString,Plugin::Data>& pluginList = pc->pluginList();
  QMap<QString,Plugin::Data>::ConstIterator it;

  for (it = pluginList.begin(); it != pluginList.end(); ++it) {
    QString path = pc->pluginNameList()[it.value()._name];
    Q3ListViewItem *i = new Q3ListViewItem(_pluginList,
                                         it.value()._readableName,
                                         QString::null,
                                         it.value()._description,
                                         it.value()._version,
                                         it.value()._author,
                                         it.value()._name,
                                         path);
    if (loadedPluginList.contains(it.value()._name)) {
      i->setPixmap(COLUMN_LOADED, KStandardDirs::locate("data", "kst/pics/yes.png"));
      // Don't use no.png - it looks bad
    }
  }
}

#include "pluginmanager.moc"

// vim: ts=2 sw=2 et
