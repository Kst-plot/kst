/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void ExtensionDialog::show()
{
    _extensions->clear();
    KService::List sl = KServiceType::offers("Kst Extension");
    for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
	KService::Ptr service = *it;
	QString name = service->property("Name").toString();
	QCheckListItem *i = new QCheckListItem(_extensions, name, QCheckListItem::CheckBox);
	i->setText(1, service->property("Comment").toString());
	i->setText(2, service->property("X-Kst-Plugin-Author").toString());
	i->setText(3, KLibLoader::findLibrary(service->library().latin1(), KstApp::inst()->instance()));
	if (!ExtensionMgr::self()->extensions().contains(name)) {
	    ExtensionMgr::self()->setEnabled(name, service->property("X-Kst-Enabled").toBool());
	}
	i->setOn(ExtensionMgr::self()->enabled(name));
    }
    QDialog::show();
}


void ExtensionDialog::accept()
{
    ExtensionMgr *mgr = ExtensionMgr::self();
    QListViewItemIterator it(_extensions); // don't use Checked since it is too new
    while (it.current()) {
	mgr->setEnabled(it.current()->text(0), static_cast<QCheckListItem*>(it.current())->isOn());
	++it;
    }
    mgr->updateExtensions();
    QDialog::accept();
}
