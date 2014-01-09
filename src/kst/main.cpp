/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *               netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <application.h>
#include <QLibraryInfo>
#include <QTranslator>
#include <QLocale>

#ifdef Q_CC_MSVC
__declspec(dllexport)
#endif
int main(int argc, char *argv[]) {
  Kst::Application app(argc, argv);

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator kstTranslator;
  kstTranslator.load("kst_" + QLocale::system().name());
  app.installTranslator(&kstTranslator);

  if (app.mainWindow()->initFromCommandLine()) {
    app.mainWindow()->show();
    return app.exec();
  }
  return 0;
}
