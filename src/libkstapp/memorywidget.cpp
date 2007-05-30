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

#include "config.h"

#include "memorywidget.h"
#include <psversion.h>
#include <sysinfo.h>

namespace Kst {

MemoryWidget::MemoryWidget(QWidget *parent, int updateMilliSeconds)
: QLabel(parent) {
  connect(&_timer, SIGNAL(timeout()), this, SLOT(updateFreeMemory()));
  _timer.start(updateMilliSeconds);
  updateFreeMemory();
}


MemoryWidget::~MemoryWidget() {
}


void MemoryWidget::updateFreeMemory() {
#ifdef __linux__
  meminfo();
  unsigned long mi = S(kb_main_free + kb_main_buffers + kb_main_cached);
  setText(tr("%1 MB available").arg(mi / (1024 * 1024)));
#endif
}

}

#include "memorywidget.moc"
// vim: ts=2 sw=2 et
