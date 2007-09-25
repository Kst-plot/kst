/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "databutton.h"

#include <QStylePainter>
#include <QStyleOption>


namespace Kst {

DataButton::DataButton(const QString &text, QWidget *parent)
  : QPushButton(text, parent) {
}


DataButton::~DataButton() {
}


void DataButton::paintEvent(QPaintEvent *) {
  QStylePainter p(this);
  QStyleOptionButton option;
  initStyleOption(&option);

  p.drawControl(QStyle::CE_PushButtonBevel, option);
  p.drawPrimitive(QStyle::PE_FrameFocusRect, option);

  QRect textPosition(rect());
  textPosition.setX(textPosition.x() + 5);
  p.drawText(textPosition, QPushButton::text(), QTextOption(Qt::AlignLeft|Qt::AlignVCenter));
}

}

// vim: ts=2 sw=2 et
