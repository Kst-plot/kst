/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void VectorSaveDialog::save()
{
    KstVectorList toSave;

    KST::vectorList.lock().readLock();
    for (QListBoxItem *i = _vectorList->firstItem(); i; i = i->next()) {
	if (i->isSelected()) {
	    KstVectorPtr v = *KST::vectorList.findTag(i->text());
	    if (v) {
		toSave += v;
	    }
	}
    }
    KST::vectorList.lock().unlock();

    KURL url = KFileDialog::getSaveURL(QString::null, QString::null, this, i18n("Save Vector As"));
    if (!url.isEmpty()) {
	bool interpolate = true;
	switch (_multiOptions->currentItem()) {
	    case 0:
		interpolate = false;
	    case 1:
		{
		    KTempFile tf(locateLocal("tmp", "kstvectors"), "txt");
		    tf.setAutoDelete(true);
		    if (0 != KstData::self()->vectorsToFile(toSave, tf.file(), interpolate)) {
			KMessageBox::sorry(this, i18n("Error saving vector to %1.").arg(url.prettyURL()), i18n("Kst"));
			return;
		    }
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
		    tf.sync();
#else
		    tf.close();
#endif
		    
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
                    if (KIO::NetAccess::exists(url, false, this)) {
#else
                    if (KIO::NetAccess::exists(url)) {
#endif
                        int rc = KMessageBox::warningYesNo(this, i18n("File %1 exists.  Overwrite?").arg(url.prettyURL()), i18n("Kst"));
			if (rc == KMessageBox::No) {
			    return;
			}
		    }
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
		    KIO::NetAccess::file_copy(KURL(tf.name()), url, -1, true, false, this);
#else
		    KIO::NetAccess::upload(tf.name(), url);
#endif
		}
		break;
	    case 2:
		{
		    unsigned n = 0;
		    for (KstVectorList::Iterator i = toSave.begin(); i != toSave.end(); ++i) {
			KURL url2 = url;
			if (toSave.count() > 1) {
			    url2.setFileName(url.fileName() + QString(".%1").arg(++n));
			} else {
			    url2.setFileName(url.fileName());
			}
			KTempFile tf(locateLocal("tmp", "kstvectors"), "txt");
			tf.setAutoDelete(true);
			if (0 != KstData::self()->vectorToFile(*i, tf.file())) {
			    KMessageBox::sorry(this, i18n("Error saving vector to %1.").arg(url2.prettyURL()), i18n("Kst"));
			    return;
			}
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
			tf.sync();
#else
			tf.close();
#endif

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
			if (KIO::NetAccess::exists(url2, false, this)) {
#else
			if (KIO::NetAccess::exists(url2)) {
#endif
                            int rc = KMessageBox::warningYesNo(this, i18n("File %1 exists.  Overwrite?").arg(url2.prettyURL()), i18n("Kst"));
			    if (rc == KMessageBox::No) {
				continue;
			    }
			}
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
			KIO::NetAccess::file_copy(KURL(tf.name()), url2, -1, true, false, this);
#else
			KIO::NetAccess::upload(tf.name(), url2);
#endif
		    }
		}
		break;
	    default:
		KMessageBox::sorry(this, i18n("Internal error.  Please report."), i18n("Kst"));
		break;
	}
    }
}

void VectorSaveDialog::show()
{
    init();
    QDialog::show();
}

void VectorSaveDialog::init()
{
    _vectorList->clear();
    KST::vectorList.lock().readLock();
    for (KstVectorList::ConstIterator i = KST::vectorList.begin(); i != KST::vectorList.end(); ++i) {
	(*i)->readLock();
	_vectorList->insertItem((*i)->tag().displayString());
	(*i)->unlock();
    }
    KST::vectorList.lock().unlock();
    _saveButton->setEnabled(false);
}


void VectorSaveDialog::selectionChanged()
{
    int cnt = 0;
    // expensive but qlistbox provides nothing better at the moment
    for (QListBoxItem *i = _vectorList->firstItem(); i; i = i->next()) {
	if (i->isSelected()) {
	    if (++cnt > 1) {
		break;
	    }
	}
    }
    _multiOptions->setEnabled(cnt > 1);
    _saveButton->setEnabled(cnt > 0);
}	

// vim: ts=8 sw=4 noet
