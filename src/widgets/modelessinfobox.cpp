/***************************************************************************
                          modelessinfobox.cpp: a dialog with a read-only
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

#include "modelessinfobox.h"
#include "ui_modelessinfobox.h"

ModelessInfoBox::ModelessInfoBox(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ModelessInfoBox)
{
  ui->setupUi(this);
}

ModelessInfoBox::~ModelessInfoBox()
{
  delete ui;
}

void ModelessInfoBox::setText(QString text) {
  ui->_text->setText(text);
}

void ModelessInfoBox::setWidth(int charwidth) {
  float one_char = ui->_text->fontMetrics().averageCharWidth();

  ui->_text->setMinimumWidth(charwidth*one_char);

  //setGeometry(geo);

}
