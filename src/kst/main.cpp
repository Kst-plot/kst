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
#include <QDebug>
#include <time.h>

#ifdef Q_CC_MSVC
__declspec(dllexport)
#endif

void nullMessageOutput(QtMsgType type, const char *msg)
{
  Q_UNUSED(type)
  Q_UNUSED(msg)
}

int main(int argc, char *argv[]) {

#ifdef QT_NO_WARNING_OUTPUT
  qInstallMsgHandler(nullMessageOutput);
#endif


#if !defined(Q_OS_WIN) && QT_VERSION < 0x050000 && QT_VERSION >= 0x040500
  // The GraphicsSystem needs to be set before the instantiation of the QApplication.
  // Therefore we need to parse the current setting in this unusual place :-/
  QSettings& settings = Kst::createSettings("application");
  if (settings.value("general/raster", false).toBool()) {
    QApplication::setGraphicsSystem("raster");
  } else {
    // this must be actually set, since raster is now the
    // default under linux.  Native is strongly preferred
    // for remote X, and raster mildly preferred otherwise.
    // Note: for QT5, "native" is gone.  Which is bad for
    // remote X users.  Sigh...
    QApplication::setGraphicsSystem("native");
  }
#endif

  srand(time(NULL));
  Kst::Application app(argc, argv);

  //--------
  QTranslator qtTranslator;
  qtTranslator.load(QLatin1String("qt_") + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QLatin1String localeSuffix("/locale/");
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
  //bool ok = kstDirectoryTranslator.load(localeName, app.applicationDirPath() + "/../share/kst" + localeSuffix);
  // qDebug() << "Translation file " + localeName + " loaded:" << ok;
  // qDebug() << "Dir = " + app.applicationDirPath() + "/../share/kst" + localeSuffix;
  app.installTranslator(&kstDirectoryTranslator);

#ifdef QT5
  // use high res pixmaps if pixel-doubling is in effect.
  // But there is are still odd bugs, so don;t get too excited.
  app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif


  app.initMainWindow();
  if (app.mainWindow()->initFromCommandLine()) {
    app.mainWindow()->show();
    return app.exec();
  }
  return 0;
}
