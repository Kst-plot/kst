#ifndef KSTAPPLICATION_H
#define KSTAPPLICATION_H

#include <QPointer>
#include <QApplication>
#include <QMainWindow>

#include "kst_export.h"

#define kstApp \
  (static_cast<QCode*>(QCoreApplication::instance()))

class KST_EXPORT KstApplication : public QApplication
{
  Q_OBJECT
public:
  KstApplication(int &argc, char **argv);
  virtual ~KstApplication();

  QMainWindow *mainWindow() const;

private:
  QPointer<QMainWindow> m_mainWindow;
};

#endif
// vim: ts=2 sw=2 et
