//Added by qt3to4:
#include <QPixmap>
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void ColorPaletteWidget::init( ) 
{
    refresh();
}

void ColorPaletteWidget::updatePalette( const QString &palette)
{
  KPalette* newPal = 0L;
  QColor color;
  int nrColors = 0;
  int height = _palette->height();
  int width = 7 * height;
  QPixmap pix(width, height);
  QPainter p(&pix);
  int size = 1;
  int step = 1;
  int pos = 0;
  int i;

  if (!palette.isEmpty()) {
     newPal = new KPalette(palette);
     nrColors = newPal->nrColors();
  }
  
  if (nrColors > 0) {
      size = width / nrColors;
      if (size == 0) {
        size = 1;
        step = nrColors / width;
    }
  }

  p.fillRect(p.window(), QColor("white"));
  for (i=0; i<nrColors; i+=step) {
    color = newPal->color(i);
    p.fillRect(pos*size, 0, size, height, QBrush(color));
    ++pos;
  }  
  _paletteDisplay->setPixmap(pix);
  
  delete newPal;
}



QString ColorPaletteWidget::selectedPalette()
{
    return _palette->currentText();
}


void ColorPaletteWidget::refresh()
{
  _palette->clear();
  QStringList palList = KPalette::getPaletteList();
  palList.sort();
  _palette->insertStringList(palList);
  
  if (palList.contains("Kst Spectrum 1021")) {
    _palette->setCurrentText("Kst Spectrum 1021"); 
  } else if (palList.contains("Kst Grayscale 256")) {
    _palette->setCurrentText("Kst Grayscale 256"); 
  }
}


void ColorPaletteWidget::refresh( const QString & palette )
{
  QStringList palList = KPalette::getPaletteList();
  int i;
  
  _palette->clear();
  palList.sort();
  _palette->insertStringList(palList);

  for (i = 0; i < _palette->count(); ++i) {
    if (_palette->text(i) == palette) {
      break;
    }
  }
  if (i == _palette->count()) {
    i = 0;
  }

  _palette->setCurrentItem(i);
}



int ColorPaletteWidget::currentPaletteIndex()
{
  return _palette->currentItem();
}




