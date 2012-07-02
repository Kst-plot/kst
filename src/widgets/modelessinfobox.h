/***************************************************************************
                          modelessinfobox.h: a dialog with a read-only
                          texedit in it for displaing help modelessly.
                             -------------------
    begin                : June, 2012
    copyright            : (C) 2012 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODELESSINFOBOX_H
#define MODELESSINFOBOX_H

#include <QDialog>

namespace Ui {
class ModelessInfoBox;
}

class ModelessInfoBox : public QDialog
{
    Q_OBJECT
    
  public:
    explicit ModelessInfoBox(QWidget *parent = 0);
    ~ModelessInfoBox();

    void setText(QString text);
    void setWidth(int charwidth);
    
  private:
    Ui::ModelessInfoBox *ui;
};

#endif // MODELESSINFOBOX_H
