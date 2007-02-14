/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

// vim: ts=8 sw=4 noet

void KstPulseGenerator::accept()
{
    KST::vectorList.lock().writeLock();
    KstVectorPtr vp = *KST::vectorList.findTag(_vector->text());
    bool had = vp != 0L;
    KST::vectorList.lock().unlock();
    KstGVectorPtr gv = kst_cast<KstGVector>(vp);
    if (!gv) {
	if (had) {
	    KMessageBox::sorry(this, i18n("Vector [%1] already exists.  Please choose a different name."));
	    return;
	}
	gv = new KstGVector(_vector->text());
	KST::vectorList.append(gv.data());
	KstApp::inst()->updateDataDialogs();
    }
    gv->writeLock();
    gv->setFrequency(_freq->value());
    gv->unlock();
    close();
}
