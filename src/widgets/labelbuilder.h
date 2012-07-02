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

#ifndef LABELBUILDER_H
#define LABELBUILDER_H

#include <QWidget>
#include "ui_labelbuilder.h"

#include <string_kst.h>

#include "kstwidgets_export.h"
#include "modelessinfobox.h"

namespace Kst {

class ObjectStore;

class KSTWIDGETS_EXPORT LabelBuilder : public QWidget, public Ui::LabelBuilder {
  Q_OBJECT
  public:
    explicit LabelBuilder(QWidget *parent = 0, ObjectStore *store = 0);
    virtual ~LabelBuilder();

    void setObjectStore(ObjectStore *store);

    QString labelText() const;
    void setLabelText(const QString &label);

  Q_SIGNALS:
    void labelChanged();

  public Q_SLOTS:
    void showHelp();

  private:
    ObjectStore *_store;
    ModelessInfoBox *_helpBox;
};

}

#endif

// vim: ts=2 sw=2 et
