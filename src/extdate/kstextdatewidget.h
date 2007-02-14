/* This file is part of the KDE libraries
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

#ifndef __KSTEXTDATEWIDGET_H__
#define __KSTEXTDATEWIDGET_H__

#include "kstextdatetime.h"

namespace KST {
/**
* This widget can be used to display or allow user selection of a date.
*
* @see ExtDatePicker
*
* @short A pushbutton to display or allow user selection of a date.
* @version $Id$
*/
class ExtDateWidget : public QWidget
{
  Q_OBJECT
//  Q_PROPERTY( ExtDate date READ date WRITE setDate )

public:
  /**
   * Constructs a date selection widget, initialized to the current CPU date.
   */
  ExtDateWidget( QWidget *parent=0, const char *name=0 );

  /**
   * Constructs a date selection widget with the initial date set to @p date.
   */
  ExtDateWidget( const ExtDate &date, QWidget *parent=0, const char *name=0 );

  /**
   * Destructs the date selection widget.
   */
  virtual ~ExtDateWidget();

  /**
   * Returns the currently selected date.
   */
  ExtDate date() const;

  /**
   * Changes the selected date to @p date.
   */
  void setDate(const ExtDate &date);


signals:
  /**
   * Emitted whenever the date of the widget
   * is changed, either with setDate() or via user selection.
   */
   void changed(ExtDate);

protected:
   void init();
   void init(const ExtDate&);

protected slots:
  void slotDateChanged();

protected:
  virtual void virtual_hook( int id, void* data );
private:
   class ExtDateWidgetPrivate;
   ExtDateWidgetPrivate *d;
};

}
#endif

