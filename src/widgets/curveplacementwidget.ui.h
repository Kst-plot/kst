/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

bool CurvePlacementWidget::existingPlot()
{
    return _inPlot->isChecked();
}

bool CurvePlacementWidget::newPlot()
{
    return _newPlot->isChecked();
}

void CurvePlacementWidget::setExistingPlot( bool existingPlot )
{
    _inPlot->setChecked(existingPlot);
}

void CurvePlacementWidget::setNewPlot( bool newPlot )
{
    _newPlot->setChecked(newPlot);
}

QString CurvePlacementWidget::plotName()
{
    return _plotList->currentText();
}

int CurvePlacementWidget::columns()
{
    return _plotColumns->value();
}

void CurvePlacementWidget::setCols( int c )
{
    _plotColumns->setValue(c);
}

void CurvePlacementWidget::setCurrentPlot( const QString & p )
{
    _plotList->setCurrentText(p);
}

void CurvePlacementWidget::newWindow()
{
    KstData::self()->newWindow(this);
    update();
}

void CurvePlacementWidget::update()
{
    _plotWindow->clear();
    QStringList windows = KstData::self()->windowList();
    for (QStringList::ConstIterator i = windows.begin(); i != windows.end(); ++i) {
	_plotWindow->insertItem(*i);
    }
    QString cur = KstData::self()->currentWindow();
    if (!cur.isEmpty()) {
	_plotWindow->setCurrentItem(cur);
    }

    updatePlotList();

    updateEnabled();

    updateGrid();
}

void CurvePlacementWidget::updatePlotList()
{
    QString old;
    if (_plotList->count()) {
	old = _plotList->currentText();
    }

    QStringList plots = KstData::self()->plotList(_plotWindow->currentText());
    _plotList->clear();
    for (QStringList::ConstIterator i = plots.begin(); i != plots.end(); ++i) {
	_plotList->insertItem(*i);
    }

    if (!old.isNull() && plots.contains(old)) {
	_plotList->setCurrentText(old);
    }
}


void CurvePlacementWidget::updateEnabled()
{
    _plotWindow->setEnabled(_plotWindow->count() > 0);

    _inPlot->setEnabled(_plotList->count() > 0 );

    _plotList->setEnabled(_inPlot->isChecked());
    _reGrid->setEnabled(_newPlot->isChecked());
    _plotColumns->setEnabled(_newPlot->isChecked() && _reGrid->isChecked());
}


void CurvePlacementWidget::updateGrid()
{
    int cols = KstData::self()->columns(_plotWindow->currentText());
    _reGrid->setChecked(cols > -1);
    if (cols > -1) {
	_plotColumns->setValue(cols);
    }
}


bool CurvePlacementWidget::reGrid()
{
    return _reGrid->isChecked();
}

// vim: ts=8 sw=4 noet
