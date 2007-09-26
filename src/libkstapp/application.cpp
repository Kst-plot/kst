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

#include "application.h"
#include "qgetoptions.h"

#include "builtinprimitives.h"
#include "builtinobjects.h"
#include "builtingraphics.h"

#include "kstdatasource.h"

namespace Kst {

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv) {

  QCoreApplication::setApplicationName("Kst");

  Builtins::initPrimitives(); //libkst
  Builtins::initObjects();    //libkstmath
  Builtins::initGraphics();   //libkstapp

  //FIXME Set the datasource settings file...
  //I think that KstDataSource can have a manager singleton that does this itself
  //inside of libkst... no?
  QSettings *settingsObject = new QSettings("kstdatarc", QSettings::IniFormat);
  KstDataSource::setupOnStartup(settingsObject);

  _mainWindow = new MainWindow;
  connect(this, SIGNAL(aboutToQuit()), _mainWindow, SLOT(aboutToQuit()));

  QGetOptions options;

  QVariant file;
  options.addOption("file", QOption::Argument, &file);

  options.getValues();

  if (!file.toString().isEmpty())
    _mainWindow->openFile(file.toString());

  _mainWindow->show();
}


Application::~Application() {
  delete _mainWindow;
}


MainWindow *Application::mainWindow() const {
  return _mainWindow;
}

}

// vim: ts=2 sw=2 et
