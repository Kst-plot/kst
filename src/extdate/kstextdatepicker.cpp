/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2004 Jason Harris (jharris@30doradus.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <qlayout.h>
#include <qstyle.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qpopupmenu.h>

#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ktoolbar.h>
#include <kdebug.h>
#include <knotifyclient.h>

#include "kstextdatepicker.h"
#include "kstextdatetbl.h"
#include "kstextdatepicker.moc"

// Week numbers are defined by ISO 8601
// See http://www.merlyn.demon.co.uk/weekinfo.htm for details

namespace KST {

class ExtDatePicker::ExtDatePickerPrivate
{
public:
    ExtDatePickerPrivate() : closeButton(0L), selectWeek(0L), todayButton(0),
      navigationLayout(0), calendar(0) {}

    void fillWeeksCombo(const ExtDate &date);

    QToolButton *closeButton;
    QComboBox *selectWeek;
    QToolButton *todayButton;
    QBoxLayout *navigationLayout;
    ExtCalendarSystem *calendar;
};

void ExtDatePicker::fillWeeksCombo(const ExtDate &date)
{
  // every year can have a different number of weeks

//must remain commented unless ExtDate stuff gets added to kdelibs
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

  // it could be that we had 53,1..52 and now 1..53 which is the same number but different
  // so always fill with new values

  d->selectWeek->clear();

  // We show all week numbers for all weeks between first day of year to last day of year
  // This of course can be a list like 53,1,2..52

  ExtDate day(date.year(), 1, 1);
  int lastMonth = d->calendar->monthsInYear(day);
  ExtDate lastDay(date.year(), lastMonth, d->calendar->daysInMonth(ExtDate(date.year(), lastMonth, 1)));

  for (; day <= lastDay; day = d->calendar->addDays(day, 7 /*calendar->daysOfWeek()*/) )
  {
    int year = 0;
    QString week = i18n("Week %1").arg(d->calendar->weekNumber(day, &year));
    if ( year != date.year() ) week += "*";  // show that this is a week from a different year
    d->selectWeek->insertItem(week);
  }
}

ExtDatePicker::ExtDatePicker(QWidget *parent, ExtDate dt, const char *name)
  : QFrame(parent,name)
{
  init( dt );
}

ExtDatePicker::ExtDatePicker(QWidget *parent, ExtDate dt, const char *name, WFlags f)
  : QFrame(parent,name, f)
{
  init( dt );
}

ExtDatePicker::ExtDatePicker( QWidget *parent, const char *name )
  : QFrame(parent,name)
{
  init( ExtDate::currentDate() );
}

void ExtDatePicker::init( const ExtDate &dt )
{
  d = new ExtDatePickerPrivate();

  d->calendar = new ExtCalendarSystemGregorian();

  QBoxLayout * topLayout = new QVBoxLayout(this);

  d->navigationLayout = new QHBoxLayout(topLayout);
  d->navigationLayout->addStretch();
  yearBackward = new QToolButton(this);
  yearBackward->setAutoRaise(true);
  d->navigationLayout->addWidget(yearBackward);
  monthBackward = new QToolButton(this);
  monthBackward ->setAutoRaise(true);
  d->navigationLayout->addWidget(monthBackward);
  d->navigationLayout->addSpacing(KDialog::spacingHint());

  selectMonth = new QToolButton(this);
  selectMonth ->setAutoRaise(true);
  d->navigationLayout->addWidget(selectMonth);
  selectYear = new QToolButton(this);
  selectYear->setToggleButton(true);
  selectYear->setAutoRaise(true);
  d->navigationLayout->addWidget(selectYear);
  d->navigationLayout->addSpacing(KDialog::spacingHint());

  monthForward = new QToolButton(this);
  monthForward ->setAutoRaise(true);
  d->navigationLayout->addWidget(monthForward);
  yearForward = new QToolButton(this);
  yearForward ->setAutoRaise(true);
  d->navigationLayout->addWidget(yearForward);
  d->navigationLayout->addStretch();

  line = new KLineEdit(this);
  val = new ExtDateValidator(this);
  table = new ExtDateTable(this);
  fontsize = KGlobalSettings::generalFont().pointSize();
  if (fontsize == -1)
     fontsize = QFontInfo(KGlobalSettings::generalFont()).pointSize();

  fontsize++; // Make a little bigger

  d->selectWeek = new QComboBox(false, this);  // read only week selection
  d->todayButton = new QToolButton(this);
  d->todayButton->setIconSet(SmallIconSet("today"));

  QToolTip::add(yearForward, i18n("Next year"));
  QToolTip::add(yearBackward, i18n("Previous year"));
  QToolTip::add(monthForward, i18n("Next month"));
  QToolTip::add(monthBackward, i18n("Previous month"));
  QToolTip::add(d->selectWeek, i18n("Select a week"));
  QToolTip::add(selectMonth, i18n("Select a month"));
  QToolTip::add(selectYear, i18n("Select a year"));
  QToolTip::add(d->todayButton, i18n("Select the current day"));

  // -----
  setFontSize(fontsize);
  line->setValidator(val);
  line->installEventFilter( this );
  line->setReadOnly( true );
  if (  QApplication::reverseLayout() )
  {
      yearForward->setIconSet(BarIconSet(QString::fromLatin1("2leftarrow")));
      yearBackward->setIconSet(BarIconSet(QString::fromLatin1("2rightarrow")));
      monthForward->setIconSet(BarIconSet(QString::fromLatin1("1leftarrow")));
      monthBackward->setIconSet(BarIconSet(QString::fromLatin1("1rightarrow")));
  }
  else
  {
      yearForward->setIconSet(BarIconSet(QString::fromLatin1("2rightarrow")));
      yearBackward->setIconSet(BarIconSet(QString::fromLatin1("2leftarrow")));
      monthForward->setIconSet(BarIconSet(QString::fromLatin1("1rightarrow")));
      monthBackward->setIconSet(BarIconSet(QString::fromLatin1("1leftarrow")));
  }
  connect(table, SIGNAL(dateChanged(const ExtDate&)), SLOT(dateChangedSlot(const ExtDate&)));
  connect(table, SIGNAL(tableClicked()), SLOT(tableClickedSlot()));
  connect(monthForward, SIGNAL(clicked()), SLOT(monthForwardClicked()));
  connect(monthBackward, SIGNAL(clicked()), SLOT(monthBackwardClicked()));
  connect(yearForward, SIGNAL(clicked()), SLOT(yearForwardClicked()));
  connect(yearBackward, SIGNAL(clicked()), SLOT(yearBackwardClicked()));
  connect(d->selectWeek, SIGNAL(activated(int)), SLOT(weekSelected(int)));
  connect(d->todayButton, SIGNAL(clicked()), SLOT(todayButtonClicked()));
  connect(selectMonth, SIGNAL(clicked()), SLOT(selectMonthClicked()));
  connect(selectYear, SIGNAL(toggled(bool)), SLOT(selectYearClicked()));
  connect(line, SIGNAL(returnPressed()), SLOT(lineEnterPressed()));
  table->setFocus();


  topLayout->addWidget(table);

  QBoxLayout * bottomLayout = new QHBoxLayout(topLayout);
  bottomLayout->addWidget(d->todayButton);
  bottomLayout->addWidget(line);
  bottomLayout->addWidget(d->selectWeek);

  table->setDate(dt);
  dateChangedSlot(dt);  // needed because table emits changed only when newDate != oldDate
}

ExtDatePicker::~ExtDatePicker()
{
  delete d;
}

bool
ExtDatePicker::eventFilter(QObject *o, QEvent *e )
{
   if ( e->type() == QEvent::KeyPress ) {
      QKeyEvent *k = (QKeyEvent *)e;

      if ( (k->key() == Qt::Key_Prior) ||
           (k->key() == Qt::Key_Next)  ||
           (k->key() == Qt::Key_Up)    ||
           (k->key() == Qt::Key_Down) )
       {
          QApplication::sendEvent( table, e );
          table->setFocus();
          return true; // eat event
       }
   }
   return QFrame::eventFilter( o, e );
}

void
ExtDatePicker::resizeEvent(QResizeEvent* e)
{
  QWidget::resizeEvent(e);
}

void
ExtDatePicker::dateChangedSlot(const ExtDate &date)
{
    kdDebug(298) << "ExtDatePicker::dateChangedSlot: date changed (" << date.year() << "/" << date.month() << "/" << date.day() << ")." << endl;


//must remain commented unless ExtDate gets added to kdelibs
//    const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

//    line->setText(KGlobal::locale()->formatDate(date, true));
		line->setText( date.toString( KGlobal::locale()->dateFormatShort() ) );
    selectMonth->setText(d->calendar->monthName(date, false));
    fillWeeksCombo(date);

    // calculate the item num in the week combo box; normalize selected day so as if 1.1. is the first day of the week
    ExtDate firstDay(date.year(), 1, 1);
    d->selectWeek->setCurrentItem((d->calendar->dayOfYear(date) + d->calendar->dayOfWeek(firstDay) - 2) / 7/*calendar->daysInWeek()*/);

    selectYear->setText(d->calendar->yearString(date, false));

    emit(dateChanged(date));
}

void
ExtDatePicker::tableClickedSlot()
{
  kdDebug(298) << "ExtDatePicker::tableClickedSlot: table clicked." << endl;
  emit(dateSelected(table->getDate()));
  emit(tableClicked());
}

const ExtDate&
ExtDatePicker::getDate() const
{
  return table->getDate();
}

const ExtDate &
ExtDatePicker::date() const
{
    return table->getDate();
}

bool
ExtDatePicker::setDate(const ExtDate& date)
{
    if(date.isValid())
    {
        table->setDate(date);  // this also emits dateChanged() which then calls our dateChangedSlot()
        return true;
    }
    else
    {
        kdDebug(298) << "ExtDatePicker::setDate: refusing to set invalid date." << endl;
        return false;
    }
}

void
ExtDatePicker::monthForwardClicked()
{
    ExtDate temp;
//    temp = KGlobal::locale()->calendar()->addMonths( table->getDate(), 1 );
    temp = d->calendar->addMonths( table->getDate(), 1 );
    setDate( temp );
}

void
ExtDatePicker::monthBackwardClicked()
{
    ExtDate temp;
//    temp = KGlobal::locale()->calendar()->addMonths( table->getDate(), -1 );
    temp = d->calendar->addMonths( table->getDate(), -1 );
    setDate( temp );
}

void
ExtDatePicker::yearForwardClicked()
{
    ExtDate temp;
//    temp = KGlobal::locale()->calendar()->addYears( table->getDate(), 1 );
    temp = d->calendar->addYears( table->getDate(), 1 );
    setDate( temp );
}

void
ExtDatePicker::yearBackwardClicked()
{
    ExtDate temp;
//    temp = KGlobal::locale()->calendar()->addYears( table->getDate(), -1 );
    temp = d->calendar->addYears( table->getDate(), -1 );
    setDate( temp );
}

void ExtDatePicker::selectWeekClicked() {}  // ### in 3.2 obsolete; kept for binary compatibility

void
ExtDatePicker::weekSelected(int week)
{
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

  ExtDate date = table->getDate();
  int year = d->calendar->year(date);

  d->calendar->setYMD(date, year, 1, 1);  // first day of selected year

  // calculate the first day in the selected week (day 1 is first day of week)
  date = d->calendar->addDays(date, week * 7/*calendar->daysOfWeek()*/ -d->calendar->dayOfWeek(date) + 1);

  setDate(date);
}

void
ExtDatePicker::selectMonthClicked()
{
  // every year can have different month names (in some calendar systems)
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();
  ExtDate date = table->getDate();
  int i, month, months = d->calendar->monthsInYear(date);

  QPopupMenu popup(selectMonth);

  for (i = 1; i <= months; i++)
    popup.insertItem(d->calendar->monthName(i, d->calendar->year(date)), i);

  popup.setActiveItem(d->calendar->month(date) - 1);

  if ( (month = popup.exec(selectMonth->mapToGlobal(QPoint(0, 0)), d->calendar->month(date) - 1)) == -1 ) return;  // canceled

  int day = d->calendar->day(date);
  // ----- construct a valid date in this month:
  //date.setYMD(date.year(), month, 1);
  //date.setYMD(date.year(), month, QMIN(day, date.daysInMonth()));
  d->calendar->setYMD(date, d->calendar->year(date), month,
                   QMIN(day, d->calendar->daysInMonth(date)));
  // ----- set this month
  setDate(date);
}

void
ExtDatePicker::selectYearClicked()
{
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

  if (selectYear->state() == QButton::Off)
  {
    return;
  }

  int year;
  KPopupFrame* popup = new KPopupFrame(this);
  ExtDateInternalYearSelector* picker = new ExtDateInternalYearSelector(popup);
  // -----
  picker->resize(picker->sizeHint());
  popup->setMainWidget(picker);
  connect(picker, SIGNAL(closeMe(int)), popup, SLOT(close(int)));
  picker->setFocus();
  if(popup->exec(selectYear->mapToGlobal(QPoint(0, selectMonth->height()))))
    {
      ExtDate date;
      int day;
      // -----
      year=picker->getYear();
      date=table->getDate();
      day=d->calendar->day(date);
      // ----- construct a valid date in this month:
      //date.setYMD(year, date.month(), 1);
      //date.setYMD(year, date.month(), QMIN(day, date.daysInMonth()));
      d->calendar->setYMD(date, year, d->calendar->month(date),
                       QMIN(day, d->calendar->daysInMonth(date)));
      // ----- set this month
      setDate(date);
    } else {
      KNotifyClient::beep();
    }
  delete popup;
}

void
ExtDatePicker::setEnabled(bool enable)
{
  QWidget *widgets[]= {
    yearForward, yearBackward, monthForward, monthBackward,
    selectMonth, selectYear,
    line, table, d->selectWeek, d->todayButton };
  const int Size=sizeof(widgets)/sizeof(widgets[0]);
  int count;
  // -----
  for(count=0; count<Size; ++count)
    {
      widgets[count]->setEnabled(enable);
    }
}

void
ExtDatePicker::lineEnterPressed()
{
  ExtDate temp;
  // -----
  if(val->date(line->text(), temp)==QValidator::Acceptable)
    {
        kdDebug(298) << "ExtDatePicker::lineEnterPressed: valid date entered." << endl;
        emit(dateEntered(temp));
        setDate(temp);
    } else {
      KNotifyClient::beep();
      kdDebug(298) << "ExtDatePicker::lineEnterPressed: invalid date entered." << endl;
    }
}

void
ExtDatePicker::todayButtonClicked()
{
  setDate(ExtDate::currentDate());
}

QSize
ExtDatePicker::sizeHint() const
{
  return QWidget::sizeHint();
}

void
ExtDatePicker::setFontSize(int s)
{
  QWidget *buttons[]= {
    // yearBackward,
    // monthBackward,
    selectMonth,
    selectYear,
    // monthForward,
    // yearForward
  };
  const int NoOfButtons=sizeof(buttons)/sizeof(buttons[0]);
  int count;
  QFont font;
  QRect r;
  // -----
  fontsize=s;
  for(count=0; count<NoOfButtons; ++count)
    {
      font=buttons[count]->font();
      font.setPointSize(s);
      buttons[count]->setFont(font);
    }
  QFontMetrics metrics(selectMonth->fontMetrics());

  for (int i = 1; ; ++i)
    {
      QString str = d->calendar->monthName(i,
         d->calendar->year(table->getDate()), false);
      if (str.isNull()) break;
      r=metrics.boundingRect(str);
      maxMonthRect.setWidth(QMAX(r.width(), maxMonthRect.width()));
      maxMonthRect.setHeight(QMAX(r.height(),  maxMonthRect.height()));
    }

  QSize metricBound = style().sizeFromContents(QStyle::CT_ToolButton,
                                               selectMonth,
                                               maxMonthRect);
  selectMonth->setMinimumSize(metricBound);

  table->setFontSize(s);
}

void
ExtDatePicker::setCloseButton( bool enable )
{
    if ( enable == (d->closeButton != 0L) )
        return;

    if ( enable ) {
        d->closeButton = new QToolButton( this );
        d->closeButton->setAutoRaise(true);
        d->navigationLayout->addSpacing(KDialog::spacingHint());
        d->navigationLayout->addWidget(d->closeButton);
        QToolTip::add(d->closeButton, i18n("Close"));
        d->closeButton->setPixmap( SmallIcon("remove") );
        connect( d->closeButton, SIGNAL( clicked() ),
                 topLevelWidget(), SLOT( close() ) );
    }
    else {
        delete d->closeButton;
        d->closeButton = 0L;
    }

    updateGeometry();
}

bool ExtDatePicker::hasCloseButton() const
{
    return (d->closeButton != 0L);
}

void ExtDatePicker::virtual_hook( int /*id*/, void* /*data*/ )
{ /*BASE::virtual_hook( id, data );*/ }

}
