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

#ifndef LABELLINEEDIT_H
#define LABELLINEEDIT_H

#include <QWidget>
#include <QLineEdit>

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT LabelLineEdit : public QLineEdit {
  Q_OBJECT
  public:
    LabelLineEdit(QWidget *parent = 0);
    virtual ~LabelLineEdit();

    virtual void focusInEvent ( QFocusEvent * event );

  Q_SIGNALS:
    void inFocus();
};

}

#endif

// vim: ts=2 sw=2 et
