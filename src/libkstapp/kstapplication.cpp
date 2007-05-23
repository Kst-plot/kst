#include "kstapplication.h"

KstApplication::KstApplication(int &argc, char **argv)
    : QApplication(argc, argv) {
    QCoreApplication::setApplicationName("Kst");

    m_mainWindow = new QMainWindow;
    m_mainWindow->show();
}


KstApplication::~KstApplication() {
    if (m_mainWindow)
        delete m_mainWindow;
}


QMainWindow *KstApplication::mainWindow() const {
    return m_mainWindow;
}

#include "kstapplication.moc"
// vim: ts=2 sw=2 et
