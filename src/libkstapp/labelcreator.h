/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LABELCREATOR_H
#define LABELCREATOR_H

#include <QDialog>

#include "ui_labelcreator.h"

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class LabelCreator : public QDialog, Ui::LabelCreator
{
  Q_OBJECT
  public:
    LabelCreator(QWidget *parent = 0);
    virtual ~LabelCreator();

    QString labelText();

    qreal labelScale() const;
    QColor labelColor() const;
    QFont labelFont() const;
};

}

#endif

// vim: ts=2 sw=2 et
