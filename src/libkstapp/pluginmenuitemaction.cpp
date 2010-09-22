/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "pluginmenuitemaction.h"

namespace Kst {

PluginMenuItemAction::PluginMenuItemAction(const QString &text, QWidget *parent)
  : QAction(parent),
    m_text(text) {
  connect(this, SIGNAL(triggered()), this, SLOT(triggerWithName()));
  this->setText(text);
}

void PluginMenuItemAction::triggerWithName() {
  emit triggered(m_text);
}

}

// vim: ts=2 sw=2 et

