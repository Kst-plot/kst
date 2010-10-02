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

#ifndef DATABUTTON_H
#define DATABUTTON_H

#include <QPushButton>

namespace Kst {

class Document;

class DataButton : public QPushButton
{
  Q_OBJECT
  public:
    DataButton(const QString & text, QWidget * parent = 0);
    virtual ~DataButton();
};

}

#endif

// vim: ts=2 sw=2 et
