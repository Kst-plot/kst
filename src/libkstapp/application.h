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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QPointer>
#include <QApplication>


#include "mainwindow.h"

#define kstApp \
  (static_cast<Kst::Application*>(QCoreApplication::instance()))

namespace Kst {

class Application : public QApplication
{
  Q_OBJECT
  public:
    Application(int &argc, char **argv);
    virtual ~Application();

    MainWindow *mainWindow() const;

  private:
    QPointer<MainWindow> _mainWindow;
};

}

#endif

// vim: ts=2 sw=2 et
