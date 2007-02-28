/***************************************************************************
                               kstcombobox.h
                             -------------------
    begin                : 12/14/06
    copyright            : (C) 2006 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTCOMBOBOX_H
#define KSTCOMBOBOX_H

#include <kcombobox.h>
//Added by qt3to4:
#include <QFocusEvent>

/*This class is a wrapper around KComboBox that does something different in read only mode...
  basically, we act like a readwrite combo with one important difference... when the widget
  loses focus or return is pressed we validate and make sure that what is left in the
  edit field either matches one of the combo's real entries, or we set the active item to
  the first in the list...

  The upshot is that we can have a readonly combo, but with a completion popup for easy
  searching through large combo lists.
*/
class KstComboBox : public KComboBox {
  Q_OBJECT
  public:
    KstComboBox(QWidget *parent=0);
    KstComboBox(bool rw, QWidget *parent=0);
    void setEditable(bool rw);
    virtual ~KstComboBox();

  protected:
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);

  private slots:
    void validate(bool rp = true);

  private:
    void commonConstructor();
    bool _trueRW; //captures whether this combo is *truly* read write as opposed to psuedo
};


#endif
// vim: ts=2 sw=2 et
