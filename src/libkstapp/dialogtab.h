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

#ifndef DIALOGTAB_H
#define DIALOGTAB_H

#include <QWidget>

#include "kst_export.h"

namespace Kst {

class Dialog;
class DialogPage;

//FIXME maybe we should really have a model...

class DialogTab : public QWidget
{
  Q_OBJECT
  public:
    DialogTab(QWidget *parent);
    virtual ~DialogTab();

    Dialog *dialog() const;
    DialogPage *dialogPage() const;

    QString tabTitle() const { return _tabTitle; }
    void setTabTitle(const QString &tabTitle) { _tabTitle = tabTitle; }

  Q_SIGNALS:
    void ok();
    void apply();
    void cancel();

    //subclasses must emit...
    void modified();

  private:
    QString _tabTitle;
};

}

#endif

// vim: ts=2 sw=2 et
