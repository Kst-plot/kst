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
  setWindowIcon(QPixmap(":kst.png"));

  _dialogDefaults = new QSettings("kst", "dialog");

  Builtins::initPrimitives(); //libkst
  Builtins::initDataSources(); //libkst
  Builtins::initObjects();    //libkstmath
  Builtins::initRelations();  //libkstmath
  Builtins::initGraphics();   //libkstapp

  _mainWindow = new MainWindow;

  //Replace the data singleton with one that actually works
  Data::replaceSelf(new DataGui);

  //Replace the dialoglauncher singleton with one that actually works
  DialogLauncher::replaceSelf(new DialogLauncherGui);

  connect(this, SIGNAL(aboutToQuit()), _mainWindow, SLOT(aboutToQuit()));
  _mainWindow->show();
  //_mainWindow->hide();
}


Application::~Application() {
  // lets not clean up before we leave....
  // if we do, we'll end up crashing on exit
  // unless we fix some stuff related to destruction
  delete _mainWindow;
  delete _dialogDefaults;
}


MainWindow *Application::mainWindow() const {
  return _mainWindow;
}

}

// vim: ts=2 sw=2 et
