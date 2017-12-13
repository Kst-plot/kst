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

#include "application.h"

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
#include "datasourcepluginmanager.h"
//#include "dialogscriptinterface.h"
#include "settings.h"
#include "geticon.h"


#include <QIcon>


namespace Kst {

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv) {

  QCoreApplication::setApplicationName("Kst");
  setWindowIcon(KstGetIcon("kst"));


  // get the Username from the OS.  On windows this is USERNAME and on linux, USER.
#ifdef Q_OS_WIN
  _userName = qgetenv("USERNAME");
  if (_userName.isEmpty())  {// hmmm... something odd.
    _userName = qgetenv("USER");
  }
#else
  _userName = qgetenv("USER");
  if (_userName.isEmpty()) { // hmmm... something odd.
    _userName = qgetenv("USERNAME");
  }
#endif
  if (_userName.isEmpty()) {
    _userName = "kst";
  }


  Builtins::initPrimitives(); //libkst
  Builtins::initDataSources(); //libkstapp
  Builtins::initObjects();    //libkstmath
  Builtins::initRelations();  //libkstmath
  Builtins::initGraphics();   //libkstapp

  //Replace the data singleton with one that actually works
  Data::replaceSelf(new DataGui);

  //Replace the dialoglauncher singleton with one that actually works
  DialogLauncher::replaceSelf(new DialogLauncherGui);

  //Also give us dialog-script scripting functionality
  //DialogLauncherSI::self = new DialogLauncherSI;

  //_mainWindow->show();
  //_mainWindow->hide();
}

void Application::initMainWindow() {
  _mainWindow = new MainWindow;
  connect(this, SIGNAL(aboutToQuit()), _mainWindow, SLOT(aboutToQuit()));
}

Application::~Application() {
  // lets not clean up before we leave....
  // if we do, we'll end up crashing on exit
  // unless we fix some stuff related to destruction
  delete _mainWindow;
  deleteAllSettings();
}


MainWindow *Application::mainWindow() const {
  return _mainWindow;
}

}

// vim: ts=2 sw=2 et
