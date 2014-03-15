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

#include "application.h"
#include "settings.h"

#include <QLibraryInfo>
#include <QTranslator>
#include <QLocale>

#ifdef Q_CC_MSVC
__declspec(dllexport)
#endif
int main(int argc, char *argv[]) {

#if QT_VERSION < 0x050000 && QT_VERSION >= 0x040500
  // The GraphicsSystem needs to be set before the instantiation of the QApplication.
  // Therefore we need to parse the current setting in this unusual place :-/
  QSettings& settings = Kst::createSettings("application");
  if (settings.value("general/opengl", false).toBool()) {
    QApplication::setGraphicsSystem("opengl");
  }
#endif

  Kst::Application app(argc, argv);

  //--------
  QTranslator qtTranslator;
  qtTranslator.load(QLatin1String("qt_") + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QLatin1String localeSuffix("/locale");
  QString localeName(QLatin1String("kst_common_") + QLocale::system().name());

  // The "installed to system" localization:
  // FIXME: see https://bugs.kde.org/show_bug.cgi?id=323197
#ifdef PKGDATADIR
  QTranslator appSystemTranslator;
  appSystemTranslator.load(localeName, PKGDATADIR + localeSuffix);
  app.installTranslator(&appSystemTranslator);
#endif

  // The "in the directory with the binary" localization
  QTranslator kstDirectoryTranslator;
  kstDirectoryTranslator.load(localeName, app.applicationDirPath() + localeSuffix);
  app.installTranslator(&kstDirectoryTranslator);

  if (app.mainWindow()->initFromCommandLine()) {
    app.mainWindow()->show();
    return app.exec();
  }
  return 0;
}
