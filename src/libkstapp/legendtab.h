/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEGENDTAB_H
#define LEGENDTAB_H

#include "dialogtab.h"
#include "ui_legendtab.h"

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT LegendTab : public DialogTab, Ui::LegendTab {
  Q_OBJECT
  public:
    LegendTab(QWidget *parent = 0);
    virtual ~LegendTab();

    void setDisplayedRelations(QStringList displayedRelations, QStringList displayedRelationTips);
    void setAvailableRelations(QStringList availableRelations, QStringList availableRelationTips);

    QStringList displayedRelations();

    QString title() const;
    void setTitle(const QString& title);

    bool autoContents() const;
    void setAutoContents(const bool auto);

    QFont font() const;
    void setFont(const QFont &font);

    qreal fontScale() const;
    void setFontScale(const qreal scale);

    bool verticalDisplay() const;
    void setVerticalDisplay(const bool vertical);

  private:
    ObjectStore* _store;

  private Q_SLOTS:
    void updateActive();
    void updateButtons();
    void addButtonClicked();
    void removeButtonClicked();
    void upButtonClicked();
    void downButtonClicked();
};

}

#endif

// vim: ts=2 sw=2 et
