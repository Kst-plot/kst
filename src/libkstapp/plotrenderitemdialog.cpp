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

#include "plotrenderitemdialog.h"

namespace Kst {

PlotRenderItemDialog::PlotRenderItemDialog(PlotRenderItem *item, QWidget *parent)
  : QDialog(parent), _item(item) {
   setupUi(this);

  //TODO Need icons.
  _add->setText("Add");
  _remove->setText("Remove");
  _up->setText("Up");
  _down->setText("Down");
}


PlotRenderItemDialog::~PlotRenderItemDialog() {
}


void PlotRenderItemDialog::exec() {
  QDialog::exec();
}


}

// vim: ts=2 sw=2 et
