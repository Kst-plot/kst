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
#include <kiconloader.h>

void ViewLegendWidget::init() 
{
    connect(DisplayedCurveList, SIGNAL(clicked(QListBoxItem*)), 
	    this, SLOT(updateButtons()));
    connect(AvailableCurveList, SIGNAL(clicked(QListBoxItem*)),
	    this, SLOT(updateButtons()));
    connect(DisplayedCurveList, SIGNAL(doubleClicked(QListBoxItem*)),
	    this, SLOT(removeDisplayedCurve()));
    connect(AvailableCurveList, SIGNAL(doubleClicked(QListBoxItem*)),
	    this, SLOT(addDisplayedCurve()));
    connect(DisplayedCurveList, SIGNAL(selectionChanged()),
	    this, SLOT(updateButtons()));
    connect(AvailableCurveList, SIGNAL(selectionChanged()),
	    this, SLOT(updateButtons()));
    connect(_remove, SIGNAL(clicked()),
	    this, SLOT(removeDisplayedCurve()));
    connect(_add, SIGNAL(clicked()),
	    this, SLOT(addDisplayedCurve()));
    connect(_up, SIGNAL(clicked()),
            DisplayedCurveList, SLOT(up()));
    connect(_down, SIGNAL(clicked()),
            DisplayedCurveList, SLOT(down()));

    _up->setPixmap(BarIcon("up"));
    _up->setEnabled(false);
    _down->setPixmap(BarIcon("down"));
    _down->setEnabled(false);
    _add->setPixmap(BarIcon("forward"));
    _add->setEnabled(false);
    _remove->setPixmap(BarIcon("back"));
    _remove->setEnabled(false);
    QToolTip::add(_up, i18n("Shortcut: Alt+Up"));
    QToolTip::add(_down, i18n("Shortcut: Alt+Down"));
    QToolTip::add(_add, i18n("Shortcut: Alt+s"));
    QToolTip::add(_remove, i18n("Shortcut: Alt+r"));

    _thisLegend->setChecked(true);

}

void ViewLegendWidget::updateButtons()
{
    bool selected = false;
    uint count = AvailableCurveList->count();

    for (uint i = 0; i < count; i++) {
        if (AvailableCurveList->isSelected(i)) {
            selected = true;
        }
    }
    if (selected && !_add->isEnabled()) {
        _add->setEnabled(true);
    } else if (!selected && _add->isEnabled()) {
        _add->setEnabled(false);
    }

    selected = false;
    count = DisplayedCurveList->count();
    for (uint i = 0; i < count; i++) {
        if (DisplayedCurveList->isSelected(i)) {
            selected = true;
        }
    }
    if (selected && !_remove->isEnabled()) {
        _remove->setEnabled(true);
    } else if (!selected && _remove->isEnabled()) {
        _remove->setEnabled(false);
    }
    if (selected && !_up->isEnabled()) {
      _up->setEnabled(true);
      _down->setEnabled(true);
    } else if (!selected && _up->isEnabled()) {
      _up->setEnabled(false);
      _down->setEnabled(false);
    }

}


void ViewLegendWidget::removeDisplayedCurve()
{
    uint count = DisplayedCurveList->count();

    if (count > 0) {
        for (int i = count-1; i >= 0; i--) {
            if (DisplayedCurveList->isSelected(i)) {
                AvailableCurveList->insertItem(DisplayedCurveList->text(i));
                DisplayedCurveList->removeItem(i);
            }
        }
        updateButtons();
        emit changed();
    }
    TrackContents->setChecked(false);
}


void ViewLegendWidget::addDisplayedCurve()
{
    uint count = AvailableCurveList->count();

    if (count > 0) {
        for (int i = count-1; i >= 0; i--) {
            if (AvailableCurveList->isSelected(i)) {
                DisplayedCurveList->insertItem(AvailableCurveList->text(i));
                AvailableCurveList->removeItem(i);
            }
        }
        updateButtons();
        emit changed();
    }
    TrackContents->setChecked(false);
}





