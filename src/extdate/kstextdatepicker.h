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
#ifndef KSTEXTDATEPICKER_H
#define KSTEXTDATEPICKER_H


#include "kstextdatetime.h"

#ifndef KDE_DEPRECATED
#define KDE_DEPRECATED
#endif

class KLineEdit;
class QToolButton;

namespace KST {
class ExtDateValidator;
class ExtDateTable;

/**
 * Provides a widget for calendar date input.
 *
 *     Different from the
 *     previous versions, it now emits two types of signals, either
 * dateSelected() or dateEntered() (see documentation for both
 *     signals).
 *
 *     A line edit has been added in the newer versions to allow the user
 *     to select a date directly by entering numbers like 19990101
 *     or 990101.
 *
 * \image html kdatepicker.png "KDE Date Widget"
 *
 *     @version $Id$
 *     @author Tim Gilman, Mirko Boehm
 *
 * @short A date selection widget.
 **/
class ExtDatePicker: public QFrame
{
  Q_OBJECT
//  Q_PROPERTY( ExtDate date READ date WRITE setDate)
  Q_PROPERTY( bool closeButton READ hasCloseButton WRITE setCloseButton )
  Q_PROPERTY( int fontSize READ fontSize WRITE setFontSize )

public:
  /** The usual constructor.  The given date will be displayed
   * initially.
   **/
  ExtDatePicker(QWidget *parent=0,
	      ExtDate=ExtDate::currentDate(),
	      const char *name=0);

  /** The usual constructor.  The given date will be displayed
   * initially.
   * @since 3.1
   **/
  ExtDatePicker(QWidget *parent,
	      ExtDate,
	      const char *name,
	      WFlags f); // ### KDE 4.0: Merge

  /**
   * Standard qt widget constructor. The initial date will be the
   * current date.
   * @since 3.1
   */
  ExtDatePicker( QWidget *parent, const char *name );

  /**
   * The destructor.
   **/
  virtual ~ExtDatePicker();

  /** The size hint for date pickers. The size hint recommends the
   *   minimum size of the widget so that all elements may be placed
   *  without clipping. This sometimes looks ugly, so when using the
   *  size hint, try adding 28 to each of the reported numbers of
   *  pixels.
   **/
  QSize sizeHint() const;

  /**
   * Sets the date.
   *
   *  @returns @p false and does not change anything
   *      if the date given is invalid.
   **/
  bool setDate(const ExtDate&);

  /**
   * Returns the selected date.
   * @deprecated
   **/
  const ExtDate& getDate() const KDE_DEPRECATED;

  /**
   * @returns the selected date.
   */
  const ExtDate &date() const;

  /**
   * Enables or disables the widget.
   **/
  void setEnabled(bool);

  /**
   * @returns the ExtDateTable widget child of this ExtDatePicker
   * widget.
   * @since 3.2
   */
  ExtDateTable *dateTable() const { return table; };

  /**
   * Sets the font size of the widgets elements.
   **/
  void setFontSize(int);
  /**
   * Returns the font size of the widget elements.
   */
  int fontSize() const
    { return fontsize; }

  /**
   * By calling this method with @p enable = true, ExtDatePicker will show
   * a little close-button in the upper button-row. Clicking the
   * close-button will cause the ExtDatePicker's topLevelWidget()'s close()
   * method being called. This is mostly useful for toplevel datepickers
   * without a window manager decoration.
   * @see hasCloseButton
   * @since 3.1
   */
  void setCloseButton( bool enable );

  /**
   * @returns true if a ExtDatePicker shows a close-button.
   * @see setCloseButton
   * @since 3.1
   */
  bool hasCloseButton() const;

protected:
  /// to catch move keyEvents when QLineEdit has keyFocus
  virtual bool eventFilter(QObject *o, QEvent *e );
  /// the resize event
  virtual void resizeEvent(QResizeEvent*);
  /// the year forward button
  QToolButton *yearForward;
  /// the year backward button
  QToolButton *yearBackward;
  /// the month forward button
  QToolButton *monthForward;
  /// the month backward button
  QToolButton *monthBackward;
  /// the button for selecting the month directly
  QToolButton *selectMonth;
  /// the button for selecting the year directly
  QToolButton *selectYear;
  /// the line edit to enter the date directly
  KLineEdit *line;
  /// the validator for the line edit:
  ExtDateValidator *val;
  /// the date table
  ExtDateTable *table;
  /// the size calculated during resize events
    //  QSize sizehint;
  /// the widest month string in pixels:
  QSize maxMonthRect;
protected slots:
  void dateChangedSlot(const ExtDate&);
  void tableClickedSlot();
  void monthForwardClicked();
  void monthBackwardClicked();
  void yearForwardClicked();
  void yearBackwardClicked();
  /**
   * @since 3.1
   * @deprecated in 3.2
   */
  void selectWeekClicked();
  /**
   * @since 3.1
   */
  void selectMonthClicked();
  /**
   * @since 3.1
   */
  void selectYearClicked();
  /**
   * @since 3.1
   */
  void lineEnterPressed();
  /**
   * @since 3.2
   */
  void todayButtonClicked();
  /**
   * @since 3.2
   */
  void weekSelected(int);

signals:
  /** This signal is emitted each time the selected date is changed.
   *  Usually, this does not mean that the date has been entered,
   *  since the date also changes, for example, when another month is
   *  selected.
   *  @see dateSelected
   */
  void dateChanged(const ExtDate&);
  /** This signal is emitted each time a day has been selected by
   *  clicking on the table (hitting a day in the current month). It
   *  has the same meaning as dateSelected() in older versions of
   *  ExtDatePicker.
   */
  void dateSelected(const ExtDate&);
  /** This signal is emitted when enter is pressed and a VALID date
   *  has been entered before into the line edit. Connect to both
   *  dateEntered() and dateSelected() to receive all events where the
   *  user really enters a date.
   */
  void dateEntered(const ExtDate&);
  /** This signal is emitted when the day has been selected by
   *  clicking on it in the table.
   */
  void tableClicked();

private:
  /// the font size for the widget
  int fontsize;

protected:
  virtual void virtual_hook( int id, void* data );
private:
  void init( const ExtDate &dt );
  void fillWeeksCombo(const ExtDate &date);
  class ExtDatePickerPrivate;
  ExtDatePickerPrivate *d;
};

}

#endif //  EXTDATEPICKER_H
