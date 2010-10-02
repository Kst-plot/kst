/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  setStyleSheet("QPushButton {"
                  "border: 0px solid;"
                  "background-color: white;"
                  "margin: 5;"
                "}"

                "QPushButton:hover {"
                  "border-bottom: 1px solid;"
                  "border-right: 1px solid;"
                "}"

                "QPushButton:pressed {"
                  "border-bottom: 0px solid;"
                  "border-right: 0px solid;"
                  "border-top: 1px solid;"
                  "border-left: 1px solid;"
                "}"
                );
}


DataButton::~DataButton() {
}

}

// vim: ts=2 sw=2 et
