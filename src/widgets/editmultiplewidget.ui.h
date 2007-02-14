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

void EditMultipleWidget::selectAllObjects()
{
    _objectList->clearSelection();
    _objectList->invertSelection();
}

void EditMultipleWidget::applyFilter(const QString& filter)
{
    _objectList->clearSelection();

    // case insensitive and wildcards
    QRegExp re(filter, true, true);

    uint c = _objectList->count();
    for (uint i = 0; i < c; ++i) {
	if (re.exactMatch(_objectList->text(i))) {
	    _objectList->setSelected(i, true);
	}
    }
}

// vim: ts=8 sw=4 noet
