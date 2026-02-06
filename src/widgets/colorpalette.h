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

#pragma once

#include <QWidget>

#include "kstwidgets_export.h"

namespace Ui {
class ColorPalette;
}

namespace Kst {

class KSTWIDGETS_EXPORT ColorPalette : public QWidget
{
    Q_OBJECT

  public:
    explicit ColorPalette(QWidget *parent = 0);
    ~ColorPalette();

    QString selectedPalette();
    bool selectedPaletteDirty() const;
    void refresh(const QString &palette = QString());
    int currentPaletteIndex();
    void setPalette(const QString palette);

    void clearSelection();

  public slots:
    void updatePalette(const QString &palette = QString());

  Q_SIGNALS:
    void selectionChanged();

private:
    Ui::ColorPalette *ui;
};

}
