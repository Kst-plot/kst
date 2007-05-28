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

#include "kstapplication.h"

KstApplication::KstApplication(int &argc, char **argv)
    : QApplication(argc, argv) {

  QCoreApplication::setApplicationName("Kst");

  _mainWindow = new Kst::MainWindow;
  connect(this, SIGNAL(aboutToQuit()), _mainWindow, SLOT(aboutToQuit()));

  _mainWindow->show();
}


KstApplication::~KstApplication() {
  delete _mainWindow;
}


Kst::MainWindow *KstApplication::mainWindow() const {
  return _mainWindow;
}

#include "kstapplication.moc"

// vim: ts=2 sw=2 et
