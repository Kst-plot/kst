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

#ifndef STRINGSELECTOR_H
#define STRINGSELECTOR_H

#include <QWidget>
#include "ui_stringselector.h"

#include <kststring.h>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT StringSelector : public QWidget, public Ui::StringSelector {
  Q_OBJECT
  public:
    StringSelector(QWidget *parent = 0);
    virtual ~StringSelector();

    KstStringPtr selectedString() const;
    void setSelectedString(KstStringPtr selectedString);

  Q_SIGNALS:
    void selectionChanged();
};

}

#endif

// vim: ts=2 sw=2 et
