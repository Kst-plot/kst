/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLUGINMENUITEMACTION_H
#define PLUGINMENUITEMACTION_H

#include <QAction>

namespace Kst {

class PluginMenuItemAction : public QAction
{
  Q_OBJECT
  public:
    explicit PluginMenuItemAction(const QString &text, QWidget *parent = 0);

  Q_SIGNALS:
    void triggered(QString &name);

  public Q_SLOTS:
    void triggerWithName();

  private:
    QString m_text;
};

}

#endif // PLUGINMENUITEMACTION_H

// vim: ts=2 sw=2 et
