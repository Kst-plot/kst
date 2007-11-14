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
#include "builtindatasources.h"
#include "builtinobjects.h"
#include "builtingraphics.h"
#include "builtinrelations.h"

#include "datagui.h"
#include "datacollection.h"
#include "dialoglaunchergui.h"
#include "datasource.h"
#include "dialogdefaults.h"

namespace Kst {

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv) {

  QCoreApplication::setApplicationName("Kst");

  Builtins::initPrimitives(); //libkst
  Builtins::initDataSources(); //libkst
  Builtins::initObjects();    //libkstmath
  Builtins::initRelations();  //libkstmath
  Builtins::initGraphics();   //libkstapp

  //FIXME Set the datasource settings file...
  //I think that KstDataSource can have a manager singleton that does this itself
  //inside of libkst... no?
  QSettings *settingsObject = new QSettings("kstdatarc", QSettings::IniFormat);
  DataSource::setupOnStartup(settingsObject);

  // Initialize the dialogDefaults settings
  Kst::dialogDefaults = new QSettings("kstdialogrc", QSettings::NativeFormat);

  _mainWindow = new MainWindow;

  //Replace the data singleton with one that actually works
  Data::replaceSelf(new DataGui);

  //Replace the dialoglauncher singleton with one that actually works
  DialogLauncher::replaceSelf(new DialogLauncherGui);

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
