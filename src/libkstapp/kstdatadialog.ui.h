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

#include "kst.h"
#include "kstdoc.h"

void KstDataDialog::ok()
{
    _ok->setEnabled(false);
    _cancel->setEnabled(false);
    if (_newDialog || _dp == 0L) {
	if (newObject()) {
	    close();
	} else {
	    _ok->setEnabled(true);
	    _cancel->setEnabled(true);
	}
    } else {
	if (editObject()) {
	    close();
	} else {
	    _ok->setEnabled(true);
	    _cancel->setEnabled(true);
	}
    }
}


void KstDataDialog::close()
{
    _dp = 0L;
    QDialog::close();
}


void KstDataDialog::reject()
{
    _dp = 0L;
    QDialog::reject();
}


void KstDataDialog::init()
{
    _dp = 0L;
    _newDialog = false;
    _multiple = false;
    _editMultipleMode = false;
    connect(this, SIGNAL(modified()), KstApp::inst()->document(), SLOT(wasModified()));
    connect(_editMultiple, SIGNAL(clicked()), this, SLOT(toggleEditMultiple()));
    _editMultiple->hide();
    _editMultipleWidget->hide();
}


void KstDataDialog::update()
{
}


void KstDataDialog::show()
{
    showNew(QString::null);
}


void KstDataDialog::showNew(const QString& field)
{
    Q_UNUSED(field) //used by plugin dialogs which inherit this class
    _newDialog = true;
    _dp = 0L;

    update();
    fillFieldsForNew();

    _editMultiple->hide();
    _editMultipleWidget->hide();
    _editMultipleMode = false;
    _tagName->setEnabled(true);
    _legendText->setEnabled(true);

    setCaption(i18n("New %1").arg(objectName()));
    QDialog::show();
    raise();
    _ok->setEnabled(true);
    _cancel->setEnabled(true);
}


void KstDataDialog::showEdit(const QString& field)
{
    _newDialog = false;
    _dp = findObject(field);

    if (!_dp) {
	show();
	return;
    }

    if (_multiple) {
	_editMultiple->show();
	_editMultiple->setText(i18n("Edit Multiple >>"));
	_editMultipleWidget->hide();
	_editMultipleMode = false;
    }

    _tagName->setEnabled(true);
    _legendText->setEnabled(true);

    update();
    fillFieldsForEdit();

    setCaption(i18n("Edit %1").arg(objectName()));
    QDialog::show();
    raise();
    _ok->setEnabled(true);
    _cancel->setEnabled(true);
}


QString KstDataDialog::objectName()
{
    return QString::null;
}


void KstDataDialog::fillFieldsForEdit()
{
}


void KstDataDialog::fillFieldsForNew()
{
}


KstObjectPtr KstDataDialog::findObject( const QString & name )
{
    KST::dataObjectList.lock().readLock();
    KstObjectPtr o = (*KST::dataObjectList.findTag(name)).data();
    KST::dataObjectList.lock().unlock();
    return o;
}


bool KstDataDialog::newObject()
{
    return false;
}


bool KstDataDialog::editObject()
{
    return false;
}


void KstDataDialog::populateEditMultiple()
{
}


bool KstDataDialog::multiple()
{
    return _multiple;
}


void KstDataDialog::setMultiple(bool multiple)
{
    _multiple = multiple;
}


void KstDataDialog::toggleEditMultiple()
{
    if (_multiple) {
	if (_editMultipleMode) {
	    cleanup();
	    showEdit(_dp->tagName()); // redisplay the edit dialog
	} else {
	    _editMultipleMode = true;
	    _editMultipleWidget->_objectList->clear();
	    // fill in list of objects and prepare the fields for multiple edit
	    populateEditMultiple();
	    _editMultipleWidget->show();
	    _editMultiple->setText(i18n("Edit Multiple <<"));
	    adjustSize();
	    resize(minimumSizeHint());
	    setFixedHeight(height());
	}
    }
}


void KstDataDialog::closeEvent(QCloseEvent *e)
{
    cleanup();
    QWidget::closeEvent(e);
}


void KstDataDialog::cleanup()
{
}

// vim: ts=8 sw=4 noet
