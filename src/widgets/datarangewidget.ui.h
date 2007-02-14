/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/


void KstDataRange::init()
{
    _time = false;
    update();
}

void KstDataRange::clickedCountFromEnd()
{
    if (CountFromEnd->isChecked()) {
	N->setEnabled(true);
	_rangeUnits->setEnabled(true);
	F0->setEnabled(false);
	_startUnits->setEnabled(false);
	ReadToEnd->setChecked(false);
    } else {
	F0->setEnabled(true);
	_startUnits->setEnabled(true);
    }
}

void KstDataRange::ClickedReadToEnd()
{
    if (ReadToEnd->isChecked()) {
	F0->setEnabled(true);
	_startUnits->setEnabled(true);
	N->setEnabled(false);
	_rangeUnits->setEnabled(false);
	CountFromEnd->setChecked(false);
    } else {
	N->setEnabled(true);
	if (N->text() == "") {
	   N->setText("100");
	}
	_rangeUnits->setEnabled(true);
    }
}

void KstDataRange::clickedDoSkip()
{
    if (DoSkip->isChecked()) {
	Skip->setEnabled(true);
	DoFilter->setEnabled(true);
    } else {
	Skip->setEnabled(false);
	DoFilter->setEnabled(false);
    }
}

void KstDataRange::updateEnables() {
    if (DoSkip->isChecked()) {
	Skip->setEnabled(true);
	DoFilter->setEnabled(true);
    } else {
	Skip->setEnabled(false);
	DoFilter->setEnabled(false);
    }

    if (CountFromEnd->isChecked()) {
	N->setEnabled(true);
	_rangeUnits->setEnabled(true);
	F0->setEnabled(false);
	_startUnits->setEnabled(false);
	ReadToEnd->setChecked(false);
    } else if (ReadToEnd->isChecked()) {
	F0->setEnabled(true);
	_startUnits->setEnabled(true);
	N->setEnabled(false);
	_rangeUnits->setEnabled(false);
    } else {
	N->setEnabled(true);
	_rangeUnits->setEnabled(true);
	F0->setEnabled(true);
	_startUnits->setEnabled(true);
    }

}

void KstDataRange::update()
{
    CountFromEnd->setChecked(KST::vectorDefaults.countFromEOF());
    ReadToEnd->setChecked(KST::vectorDefaults.readToEOF());
    F0->setText(QString::number(KST::vectorDefaults.f0(), 'g', 15));
    if (KST::vectorDefaults.n()>=0) {
      N->setText(QString::number(KST::vectorDefaults.n(), 'g', 15));
    } else {
      N->setText("");
    }
    Skip->setValue(KST::vectorDefaults.skip());
    DoSkip->setChecked(KST::vectorDefaults.doSkip());
    DoFilter->setChecked(KST::vectorDefaults.doAve());

    clickedCountFromEnd();
    ClickedReadToEnd();
    clickedDoSkip();
}

void KstDataRange::setAllowTime(bool allow)
{
    if (allow != _time) {
	_time = allow;
	_startUnits->clear();
	_startUnits->insertItem(i18n(KST::timeDefinitions[0].name, KST::timeDefinitions[0].description));
	_rangeUnits->clear();
	_rangeUnits->insertItem(i18n(KST::timeDefinitions[0].name, KST::timeDefinitions[0].description));
	if (_time) {
	    for (int i = 1; KST::timeDefinitions[i].name; ++i) {
		_startUnits->insertItem(i18n(KST::timeDefinitions[i].name, KST::timeDefinitions[i].description));
		if (i != KST::dateTimeEntry) {
		    _rangeUnits->insertItem(i18n(KST::timeDefinitions[i].name, KST::timeDefinitions[i].description));
		}
	    }
	}
    }
}

void KstDataRange::setF0Value(double v)
{
    F0->setText(QString::number(v, 'g', 15));
}

void KstDataRange::setNValue(double v)
{
    N->setText(QString::number(v, 'g', 15));
}

double KstDataRange::interpret(const char *txt)
{
    return Equation::interpret(txt);
}

double KstDataRange::f0Value()
{
    const int cur = _startUnits->currentItem();
    if (cur == KST::dateTimeEntry) {
	KST::ExtDateTime edt = KST::parsePlanckDate(F0->text());
	if (edt.isNull()) {
	    return KST::NOPOINT;
	}
	return KST::extDateTimeToMilliseconds(edt);
    }
    return ::d2i(interpret(F0->text().latin1()) * KST::timeDefinitions[cur].factor);
}

double KstDataRange::nValue()
{
    int cur = _rangeUnits->currentItem();
    if (cur >= KST::dateTimeEntry) {
	++cur; // we don't allow "date"
    }
    return ::d2i(interpret(N->text().latin1()) * KST::timeDefinitions[cur].factor);
}

KST::ExtDateTime KstDataRange::f0DateTimeValue()
{
    const int cur = _startUnits->currentItem();
    KST::ExtDateTime edt;
    if (cur == KST::dateTimeEntry) {
	edt = KST::parsePlanckDate(F0->text());
    }
    return edt;
}

bool KstDataRange::isStartRelativeTime()
{
    return _startUnits->currentItem() > KST::dateTimeEntry;
}

bool KstDataRange::isStartAbsoluteTime()
{
    return _startUnits->currentItem() == KST::dateTimeEntry;
}

bool KstDataRange::isRangeRelativeTime()
{
    return _rangeUnits->currentItem() > KST::dateTimeEntry;
}

// vim: ts=8 sw=4 noet
