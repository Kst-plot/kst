#ifndef CWIDGET_H
#define CWIDGET_H

#include <QMainWindow>
#include <QLocalSocket>

namespace Ui {
    class CWidget;
}

class CWidget : public QMainWindow
{
    Q_OBJECT
    QLocalSocket ls;

public:
    explicit CWidget(QWidget *parent = 0);
    ~CWidget();

public slots:
    void send();

private:
    Ui::CWidget *ui;
};

#endif // CWIDGET_H
