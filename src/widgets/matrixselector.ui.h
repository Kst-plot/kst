/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void MatrixSelector::allowNewMatrices( bool allowed )
{
    _newMatrix->setEnabled(allowed);
}


QString MatrixSelector::selectedMatrix()
{
    if (_provideNoneMatrix && _matrix->currentItem() == 0) {
	return QString::null;
    }
    return _matrix->currentText();
}


void MatrixSelector::update()
{
    if (_matrix->listBox()->isVisible()) {
	QTimer::singleShot(250, this, SLOT(update()));
	return;
    }

    blockSignals(true);
    QString prev = _matrix->currentText();
    bool found = false;
    _matrix->clear();
    if (_provideNoneMatrix) {
	_matrix->insertItem("<None>");
    }

    KstMatrixList matrices = KST::matrixList.list();
    KST::matrixList.lock().readLock();
    for (KstMatrixList::ConstIterator i = matrices.begin(); i != matrices.end(); ++i) {
	(*i)->readLock();
	QString tag = (*i)->tag().displayString();
	(*i)->unlock();
	_matrix->insertItem(tag);
	if (!found && tag == prev) {
	    found = true;
	}
    }
    KST::matrixList.lock().unlock();
    if (found) {
	_matrix->setCurrentText(prev);
    }
    blockSignals(false);
    setEdit(_matrix->currentText());
}


void MatrixSelector::init()
{
    _newMatrix->setPixmap(BarIcon("kst_matrixnew"));
    _editMatrix->setPixmap(BarIcon("kst_matrixedit"));
    _provideNoneMatrix = false;
    update();
    connect(_matrix, SIGNAL(activated(const QString&)), this, SIGNAL(selectionChanged(const QString&)));
}


void MatrixSelector::createNewMatrix()
{
    KstDialogs::self()->newMatrixDialog(this, SLOT(newMatrixCreated(KstMatrixPtr)), SLOT(setSelection(KstMatrixPtr)), SLOT(update()));
}


void MatrixSelector::selectionWatcher( const QString & tag )
{
    QString label = "[" + tag + "]";
    emit selectionChangedLabel(label);
    setEdit(tag);
}


void MatrixSelector::setSelection( const QString & tag )
{
    if (tag.isEmpty()) {
	if (_provideNoneMatrix) {
	    blockSignals(true);
	    _matrix->setCurrentItem(0);
	    blockSignals(false);
	    _editMatrix->setEnabled(false);
	}
	return;
    }
    blockSignals(true);
    _matrix->setCurrentText(tag);  // What if it isn't in the combo?
    blockSignals(false);

    setEdit(tag);
}


void MatrixSelector::newMatrixCreated( KstMatrixPtr v )
{
    v->readLock();
    QString name = v->tagName();
    v->unlock();
    v = 0L; // deref
    emit newMatrixCreated(name);
}


void MatrixSelector::setSelection( KstMatrixPtr v )
{
    v->readLock();
    setSelection(v->tagName());
    v->unlock();
}


void MatrixSelector::provideNoneMatrix( bool provide )
{
    if (provide != _provideNoneMatrix) {
	_provideNoneMatrix = provide;
	update();
    }
}


void MatrixSelector::editMatrix()
{
    KST::matrixList.lock().readLock();
    KstMatrixPtr mat = *KST::matrixList.findTag(_matrix->currentText());
    KST::matrixList.lock().unlock();
    KstDataObjectPtr pro = 0L;
    if (mat) {
      pro = kst_cast<KstDataObject>(mat->provider());
    }
    if (pro) {
      pro->readLock();
      pro->showDialog(false);
      pro->unlock();
    } else {
      KstDialogs::self()->showMatrixDialog(_matrix->currentText(), true);
    }

}


void MatrixSelector::setEdit(const QString& tag)
{
    KST::matrixList.lock().readLock();
    _editMatrix->setEnabled(KST::matrixList.findTag(tag) != KST::matrixList.end());
    KST::matrixList.lock().unlock();
}

// vim: ts=8 sw=4 noet
