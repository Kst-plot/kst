#include "kstapplication.h"

KstApplication::KstApplication(int &argc, char **argv)
    : QApplication(argc, argv) {

  QCoreApplication::setApplicationName("Kst");

  _mainWindow = new KstMainWindow;
  connect(this, SIGNAL(aboutToQuit()), _mainWindow, SLOT(aboutToQuit()));

  _mainWindow->show();
}


KstApplication::~KstApplication() {
  if (_mainWindow)
      delete _mainWindow;
}


KstMainWindow *KstApplication::mainWindow() const {
  return _mainWindow;
}

#include "kstapplication.moc"

// vim: ts=2 sw=2 et
