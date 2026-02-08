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

void nullMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  Q_UNUSED(type)
  Q_UNUSED(context)
  Q_UNUSED(msg)
}

int main(int argc, char *argv[]) {

#ifdef QT_NO_WARNING_OUTPUT
  qInstallMessageHandler(nullMessageOutput);
#endif
  bool translator_loaded = false;

  srand(time(NULL));
  Kst::Application app(argc, argv);

  Q_INIT_RESOURCE(icons);
  QTranslator qtTranslator;
  translator_loaded = qtTranslator.load(QLatin1String("qt_") + QLocale::system().name(),
                    QLibraryInfo::path(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QString localeName(QLatin1String("kst_common_") + QLocale::system().name());

  // The "installed to system" localization:
  // FIXME: see https://bugs.kde.org/show_bug.cgi?id=323197
#ifdef PKGDATADIR
  QLatin1String localeSuffix("/locale/");
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


  app.initMainWindow();
  if (app.mainWindow()->initFromCommandLine()) {
    app.mainWindow()->show();
    return app.exec();
  }
  return 0;
}
