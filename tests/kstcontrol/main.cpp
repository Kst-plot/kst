#include <QtGui/QApplication>
#include "cwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CWidget w;
    w.show();

    return a.exec();
}
