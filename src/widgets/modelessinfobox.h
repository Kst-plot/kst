#ifndef MODELESSINFOBOX_H
#define MODELESSINFOBOX_H

#include <QDialog>

namespace Ui {
class ModelessInfoBox;
}

class ModelessInfoBox : public QDialog
{
    Q_OBJECT
    
  public:
    explicit ModelessInfoBox(QWidget *parent = 0);
    ~ModelessInfoBox();

    void setText(QString text);
    void setWidth(int charwidth);
    
  private:
    Ui::ModelessInfoBox *ui;
};

#endif // MODELESSINFOBOX_H
