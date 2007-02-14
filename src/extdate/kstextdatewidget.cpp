/*  This file is part of the KDE libraries
    Copyright (C) 2001 Waldo Bastian (bastian@kde.org)

    Modified to use ExtDate instead of QDate.  Modifications
    Copyright (C) 2004 Jason Harris (jharris@30doradus.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#include <qcombobox.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <knuminput.h>
#include <kdialog.h>

#include "kstextdatewidget.h"
#include "kstextcalendarsystemgregorian.h"

namespace KST {
class ExtDateWidgetSpinBox : public QSpinBox
{
public:
  ExtDateWidgetSpinBox(int min, int max, QWidget *parent)
    : QSpinBox(min, max, 1, parent)
  {
     editor()->setAlignment(AlignRight);
  }
};

class ExtDateWidget::ExtDateWidgetPrivate
{
public:
   ExtDateWidgetPrivate() { calendar = new ExtCalendarSystemGregorian(); }
   ~ExtDateWidgetPrivate() { delete calendar; }
   ExtDateWidgetSpinBox *m_day;
   QComboBox *m_month;
   ExtDateWidgetSpinBox *m_year;
   ExtDate m_dat;
   ExtCalendarSystemGregorian *calendar;
};


ExtDateWidget::ExtDateWidget( QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  init(ExtDate::currentDate());
  setDate(ExtDate());
}

ExtDateWidget::ExtDateWidget( const ExtDate &date, QWidget *parent,
			    const char *name )
  : QWidget( parent, name )
{
  init(date);
  setDate(date);
}

// // ### CFM Repaced by init(const ExtDate&). Can be safely removed
// //     when no risk of BIC
// void ExtDateWidget::init()
// {
//   d = new ExtDateWidgetPrivate;
//   KLocale *locale = KGlobal::locale();
//   QHBoxLayout *layout = new QHBoxLayout(this, 0, KDialog::spacingHint());
//   layout->setAutoAdd(true);
//   d->m_day = new ExtDateWidgetSpinBox(1, 1, this);
//   d->m_month = new QComboBox(false, this);
//   for (int i = 1; ; ++i)
//   {
//     QString str = d->calendar->monthName(i,
//        d->calendar->year(ExtDate()));
//     if (str.isNull()) break;
//     d->m_month->insertItem(str);
//   }
//
//   d->m_year = new ExtDateWidgetSpinBox(d->calendar->minValidYear(),
// 				     d->calendar->maxValidYear(), this);
//
//   connect(d->m_day, SIGNAL(valueChanged(int)), this, SLOT(slotDateChanged()));
//   connect(d->m_month, SIGNAL(activated(int)), this, SLOT(slotDateChanged()));
//   connect(d->m_year, SIGNAL(valueChanged(int)), this, SLOT(slotDateChanged()));
// }

void ExtDateWidget::init(const ExtDate& date)
{
  d = new ExtDateWidgetPrivate;
  //KLocale *locale = KGlobal::locale();
  QHBoxLayout *layout = new QHBoxLayout(this, 0, KDialog::spacingHint());
  layout->setAutoAdd(true);
  d->m_day = new ExtDateWidgetSpinBox(1, 1, this);
  d->m_month = new QComboBox(false, this);
  for (int i = 1; ; ++i)
  {
    QString str = d->calendar->monthName(i,
       d->calendar->year(date));
    if (str.isNull()) break;
    d->m_month->insertItem(str);
  }

  d->m_year = new ExtDateWidgetSpinBox(d->calendar->minValidYear(),
				     d->calendar->maxValidYear(), this);

  connect(d->m_day, SIGNAL(valueChanged(int)), this, SLOT(slotDateChanged()));
  connect(d->m_month, SIGNAL(activated(int)), this, SLOT(slotDateChanged()));
  connect(d->m_year, SIGNAL(valueChanged(int)), this, SLOT(slotDateChanged()));
}

ExtDateWidget::~ExtDateWidget()
{
		delete d;
}

void ExtDateWidget::setDate( const ExtDate &date )
{
//  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  d->m_day->blockSignals(true);
  d->m_month->blockSignals(true);
  d->m_year->blockSignals(true);

  d->m_day->setMaxValue(d->calendar->daysInMonth(date));
  d->m_day->setValue(d->calendar->day(date));
  d->m_month->setCurrentItem(d->calendar->month(date)-1);
  d->m_year->setValue(d->calendar->year(date));

  d->m_day->blockSignals(false);
  d->m_month->blockSignals(false);
  d->m_year->blockSignals(false);

  d->m_dat = date;
  emit changed(d->m_dat);
}

ExtDate ExtDateWidget::date() const
{
  return d->m_dat;
}

void ExtDateWidget::slotDateChanged( )
{
//  const KCalendarSystem * calendar = KGlobal::locale()->calendar();

  ExtDate date;
  int y,m,day;

  y = d->m_year->value();
  y = QMIN(QMAX(y, d->calendar->minValidYear()), d->calendar->maxValidYear());

  d->calendar->setYMD(date, y, 1, 1);
  m = d->m_month->currentItem()+1;
  m = QMIN(QMAX(m,1), d->calendar->monthsInYear(date));

  d->calendar->setYMD(date, y, m, 1);
  day = d->m_day->value();
  day = QMIN(QMAX(day,1), d->calendar->daysInMonth(date));

  d->calendar->setYMD(date, y, m, day);
  setDate(date);
}

void ExtDateWidget::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

}
#include "kstextdatewidget.moc"
