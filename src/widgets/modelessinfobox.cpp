#include "modelessinfobox.h"
#include "ui_modelessinfobox.h"

ModelessInfoBox::ModelessInfoBox(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ModelessInfoBox)
{
  ui->setupUi(this);
}

ModelessInfoBox::~ModelessInfoBox()
{
  delete ui;
}

void ModelessInfoBox::setText(QString text) {
  ui->_text->setText(text);
}

void ModelessInfoBox::setWidth(int charwidth) {
  float one_char = ui->_text->fontMetrics().averageCharWidth();

  ui->_text->setMinimumWidth(charwidth*one_char);

  //setGeometry(geo);

}
