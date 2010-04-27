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

#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QToolButton>

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT ColorButton : public QToolButton {
  Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor USER true)
  public:
    ColorButton(QWidget *parent = 0);
    ColorButton(const QColor &color, QWidget *parent = 0);
    virtual ~ColorButton();

    QColor color() const;
    bool colorDirty() const;

    void clearSelection();

  public Q_SLOTS:
    void setColor(const QColor &color);

  Q_SIGNALS:
    void changed(const QColor &color);

  protected:
    virtual void paintEvent(QPaintEvent *event);

  private Q_SLOTS:
    void chooseColor();

  private:
    QColor _color;
};

}

#endif

// vim: ts=2 sw=2 et
