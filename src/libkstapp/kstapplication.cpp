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
#include "qgetoptions.h"

#include "builtinprimitives.h"
#include "builtinobjects.h"
#include "builtingraphics.h"

KstApplication::KstApplication(int &argc, char **argv)
    : QApplication(argc, argv) {

  QCoreApplication::setApplicationName("Kst");

  Kst::Builtins::initPrimitives(); //libkst
  Kst::Builtins::initObjects();    //libkstmath
  Kst::Builtins::initGraphics();   //libkstapp

  _mainWindow = new Kst::MainWindow;
  connect(this, SIGNAL(aboutToQuit()), _mainWindow, SLOT(aboutToQuit()));

  QGetOptions options;

  QVariant file;
  options.addOption("file", QOption::Argument, &file);

  options.getValues();

  if (!file.toString().isEmpty())
    _mainWindow->openFile(file.toString());

  _mainWindow->show();
}


KstApplication::~KstApplication() {
  delete _mainWindow;
}


Kst::MainWindow *KstApplication::mainWindow() const {
  return _mainWindow;
}

// vim: ts=2 sw=2 et
