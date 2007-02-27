//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

bool CurveAppearanceWidget::showLines()
{
    return _showLines->isChecked();
}


bool CurveAppearanceWidget::showPoints()
{
    return _showPoints->isChecked();
}


bool CurveAppearanceWidget::showBars()
{
    return _showBars->isChecked();
}


void CurveAppearanceWidget::init()
{
    reset();
    QTimer::singleShot(0, this, SLOT(drawLine()));
}


void CurveAppearanceWidget::fillCombo()
{
    bool keepBlank = _combo->count() > 0 && _combo->text(0) == " ";
    
    QRect rect = _combo->style().querySubControlMetrics(
            QStyle::CC_ComboBox,
            _combo,
            QStyle::SC_ComboBoxEditField );
    rect.setLeft( rect.left() + 2 );
    rect.setRight( rect.right() - 2 );
    rect.setTop( rect.top() + 2 );
    rect.setBottom( rect.bottom() - 2 );

    // fill the point type dialog with point types
    QPixmap ppix( rect.width(), rect.height() );
    QPainter pp( &ppix );

    int currentItem = _combo->currentItem();
    _combo->clear();
    pp.setPen(color());

    if (keepBlank) {
      _combo->insertItem(" ", 0);
    }
    
    for (int ptype = 0; ptype < KSTPOINT_MAXTYPE; ptype++) {
        pp.fillRect(pp.window(), QColor("white"));
        KstCurvePointSymbol::draw(ptype, &pp, ppix.width()/2, ppix.height()/2, 0, 600);
        _combo->insertItem(ppix);
    }

    if (currentItem > 0) {
        _combo->setCurrentItem( currentItem );
    }
}


void CurveAppearanceWidget::setColor( QColor c )
{
    _color->setColor(c);
    drawLine();
}


QColor CurveAppearanceWidget::color()
{
    return _color->color();
}


void CurveAppearanceWidget::drawLine()
{
    QPixmap pix(_label->contentsRect().height()*7, _label->contentsRect().height());
    QPainter p(&pix);
    QPen pen(color(),lineWidth(),KstLineStyle[lineStyle()]);

    p.fillRect(p.window(), QColor("white"));

    if (showBars()) {
        QRect rectBar((pix.width()-pix.height())/2,
                pix.height()/2, 
                pix.height(), 
                (pix.height()/2)+1);

        if (barStyle() == 1) {
            p.fillRect(rectBar,QBrush(QColor(color())));
            p.setPen(QPen(QColor("black"),lineWidth(),KstLineStyle[lineStyle()]));
        } else {
            p.setPen(pen);
        }
        p.drawRect(rectBar);
    }

    p.setPen(pen);
    if (_showLines->isChecked()) {
        p.drawLine(1, pix.height()/2, pix.width()-1, pix.height()/2);
    }

    if (_showPoints->isChecked()) {
        pen.setStyle(Qt::SolidLine);
        p.setPen(pen);
        KstCurvePointSymbol::draw(pointType(), &p, pix.width()/2, pix.height()/2, lineWidth(), 600);
    }

    _label->setPixmap(pix);
}


int CurveAppearanceWidget::pointType()
{
    if (_combo->count() > 0 && _combo->text(0) == " ") {
        return _combo->currentItem() - 1;
    } else {
        return _combo->currentItem();
    }
}


void CurveAppearanceWidget::reset(QColor newColor)
{
    _showLines->setChecked(true);
    _showPoints->setChecked(false);
    _showBars->setChecked(false);
    _barStyle->setCurrentItem(1);
    _color->setColor(newColor);
    _spinBoxLineWidth->setValue(KstSettings::globalSettings()->defaultLineWeight);
    _comboPointDensity->setCurrentItem(0);
    fillLineStyleCombo();
    fillCombo();
    drawLine();
}


void CurveAppearanceWidget::comboChanged()
{
    // but combo can't be changed unless _showPoints is checked anyways
    // or we are in editMultipleMode
    //_showPoints->setChecked(true); 
    drawLine();
}


void CurveAppearanceWidget::reset()
{
    reset(KstColorSequence::next(KstSettings::globalSettings()->backgroundColor));
}


void CurveAppearanceWidget::setUsePoints( bool usePoints )
{
    _showPoints->setEnabled(usePoints);
    _combo->setEnabled(usePoints);
    _textLabelPointStyle->setEnabled(usePoints);
    if (!usePoints && _showPoints->isChecked()) {
        _showPoints->setChecked(false);
        drawLine();
    }
}


void CurveAppearanceWidget::setMustUseLines( bool bMustUseLines )
{
    _showLines->setEnabled(!bMustUseLines);
    if (bMustUseLines) {
        _showLines->setChecked(true);
        _showLines->hide();
        _textLabelLineStyle->setText(i18n("Line type:"));
        drawLine();
    } else {
        _showLines->show();
        _textLabelLineStyle->setText(i18n("Type:"));
    }
}


void CurveAppearanceWidget::redrawCombo()
{
    fillCombo();
    fillLineStyleCombo();  
}


void CurveAppearanceWidget::fillLineStyleCombo()
{
    bool keepBlank = _comboLineStyle->count() > 0 && _comboLineStyle->text(0) == " ";

    QRect rect = _comboLineStyle->style().querySubControlMetrics(
            QStyle::CC_ComboBox,
            _comboLineStyle,
            QStyle::SC_ComboBoxEditField);
    rect.setLeft(rect.left() + 2);
    rect.setRight(rect.right() - 2);
    rect.setTop(rect.top() + 2);
    rect.setBottom(rect.bottom() - 2);

    // fill the point type dialog with point types
    QPixmap ppix(rect.width(), rect.height());
    QPainter pp(&ppix);
    QPen pen(color(), 0);

    int currentItem = _comboLineStyle->currentItem();
    _comboLineStyle->clear();

    if (keepBlank) {
      _comboLineStyle->insertItem(" ", 0);
    }
    
    for (int style = 0; style < (int)KSTLINESTYLE_MAXTYPE; style++) {
        pen.setStyle(KstLineStyle[style]);
        pp.setPen(pen);
        pp.fillRect( pp.window(), QColor("white"));
        pp.drawLine(1,ppix.height()/2,ppix.width()-1, ppix.height()/2);
        _comboLineStyle->insertItem(ppix);
    }

    _comboLineStyle->setCurrentItem(currentItem);
}


int CurveAppearanceWidget::lineStyle()
{
    if (_comboLineStyle->count() > 0 && _comboLineStyle->text(0) == " ") {
        return _comboLineStyle->currentItem() -1;
    } else {
        return _comboLineStyle->currentItem();
    }
}


int CurveAppearanceWidget::lineWidth()
{
    if (_spinBoxLineWidth->text() == " ") {
        return 0;
    } else {
        return _spinBoxLineWidth->value();
    }
}


void CurveAppearanceWidget::setValue( bool hasLines, bool hasPoints, bool hasBars, const QColor & c, int pointType, int lineWidth, int lineStyle, int barStyle, int pointDensity )
{
    fillCombo();
    fillLineStyleCombo();

    _showLines->setChecked(hasLines);
    _showPoints->setChecked(hasPoints);
    _showBars->setChecked(hasBars);
    _color->setColor(c);
    _spinBoxLineWidth->setValue(lineWidth);
    _combo->setCurrentItem(pointType);
    _barStyle->setCurrentItem(barStyle);
    if (lineStyle < 0 || lineStyle >= (int)KSTLINESTYLE_MAXTYPE) {
        lineStyle = 0;
    }
    _comboLineStyle->setCurrentItem(lineStyle);
    if (pointDensity < 0 || pointDensity >= KSTPOINTDENSITY_MAXTYPE) {
        pointDensity = 0;
    }
    _comboPointDensity->setCurrentItem(pointDensity);
    enableSettings();
    drawLine();
}


void CurveAppearanceWidget::resizeEvent( QResizeEvent * pEvent )
{
    QWidget::resizeEvent(pEvent);
    redrawCombo();
}


int CurveAppearanceWidget::barStyle()
{
    if (_barStyle->count() > 0 && _barStyle->text(0) == " ") {
        return _barStyle->currentItem() - 1;
    } else {
        return _barStyle->currentItem();
    }
}


int CurveAppearanceWidget::pointDensity()
{
    if (_comboPointDensity->count() > 0 && _comboPointDensity->text(0) == " ") {
        return _comboPointDensity->currentItem() - 1;
    } else {
        return _comboPointDensity->currentItem();
    }
}


void CurveAppearanceWidget::enableSettings()
{
    bool enable;

    enable = showLines() || showBars();
    _comboLineStyle->setEnabled(enable);
    _textLabelLineStyle->setEnabled(enable);

    enable = enable || showPoints();
    _textLabelWeight->setEnabled(enable);
    _spinBoxLineWidth->setEnabled(enable);

    enable = showBars();
    _textLabelBarStyle->setEnabled(enable);
    _barStyle->setEnabled(enable);

    enable = showPoints();  
    _textLabelPointStyle->setEnabled(enable);
    _combo->setEnabled(enable);

    enable = enable && showLines();
    _textLabelPointDensity->setEnabled(enable);
    _comboPointDensity->setEnabled(enable);
}

// vim: ts=8 sw=4 noet
