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

#ifndef DATASOURCESELECTOR_H
#define DATASOURCESELECTOR_H

#include <QWidget>
#include <QFileDialog>

#include "kstwidgets_export.h"

class QLineEdit;
class QToolButton;

namespace Kst {

class KSTWIDGETS_EXPORT DataSourceSelector : public QWidget {
  Q_OBJECT
  Q_PROPERTY(QString file READ file WRITE setFile USER true)
  public:
    DataSourceSelector(QWidget *parent = 0);
    virtual ~DataSourceSelector();

    QString file() const;
    void setMode(QFileDialog::FileMode mode) { _mode = mode; }

    QLineEdit *_fileEdit;
  public Q_SLOTS:
    void setFile(const QString &file);
    void updateFile(const QString &file);

  Q_SIGNALS:
    void changed(const QString &file);

  private Q_SLOTS:
    void chooseFile();

  private:
    void setup();

    QToolButton *_fileButton;
    QFileDialog::FileMode _mode;
    QString _file;
};

}

#endif

// vim: ts=2 sw=2 et
