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

#ifndef DATASOURCESELECTORDIALOG_H
#define DATASOURCESELECTORDIALOG_H

#include <QFileDialog>

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT DataSourceSelectorDialog : public QFileDialog {
  Q_OBJECT
  public:
    explicit DataSourceSelectorDialog(QString &file, QWidget *parent = 0);
    virtual ~DataSourceSelectorDialog();

    QString selectedDataSource();

  public Q_SLOTS:
    void currentChanged(const QString &current);

  protected:
    void accept();
};

}

#endif

// vim: ts=2 sw=2 et
