#ifndef KSTAPPLICATION_H
#define KSTAPPLICATION_H

#include <QPointer>
#include <QApplication>

#include "kst_export.h"
#include "kstmainwindow.h"

#define kstApp \
  (static_cast<KstApplication*>(QCoreApplication::instance()))

class KST_EXPORT KstApplication : public QApplication
{
  Q_OBJECT
public:
  KstApplication(int &argc, char **argv);
  virtual ~KstApplication();

  KstMainWindow *mainWindow() const;

private:
  QPointer<KstMainWindow> _mainWindow;
};

#endif

// vim: ts=2 sw=2 et
