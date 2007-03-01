/***************************************************************************
                              kstviewlegend.cpp
                             ------------------
    begin                : September 2005
    copyright            : (C) 2005 by cbn
                           (C) 2005 by The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kstviewlegend.h"

#include "enodes.h"
#include <qdebug.h>
#include "kst2dplot.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kst.h"
#include "kstsettings.h"
#include "ksttimers.h"
#include "kstviewobjectfactory.h"
#include "labelrenderer.h"
#include "viewlegendwidget.h"
#include "plotlistbox.h"
#include "kstviewwindow.h"

#include <kdatastream.h>
#include <kglobal.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <QFontComboBox>

#include <qradiobutton.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <q3deepcopy.h>
#include <qmetaobject.h>
#include <q3ptrstack.h>
#include <q3stylesheet.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <stdlib.h>

#define MIN_FONT_SIZE 5

KstViewLegend::KstViewLegend()
: KstBorderedViewObject("Legend") {
  _fallThroughTransparency = false;
  _container = false;
  _rotation = 0;
  _vertical = true;
  _fontName = KstApp::inst()->defaultFont();
  _isResizable = false;
  setForegroundColor(KstSettings::globalSettings()->foregroundColor);
  setBorderColor(KstSettings::globalSettings()->foregroundColor);
  setBackgroundColor(KstSettings::globalSettings()->backgroundColor);
  setBorderWidth(2);
  _legendMargin = 5;
  _fontSize = -1;  
  setFontSize(0);
  _layoutActions &= ~(MoveTo | Copy | CopyTo);
  _standardActions |= Delete | Edit;
  _parsedTitle = 0L;
  _trackContents = true;
  reparseTitle();
  computeTextSize();
  setDirty(false);
}


KstViewLegend::KstViewLegend(const QDomElement& e)
: KstBorderedViewObject(e) {

  // some defaults and invariants
  _fallThroughTransparency = false;
  _container = false;
  _type = "Legend";
  _rotation = 0.0;
  _fontName = KstApp::inst()->defaultFont();
  _vertical = true;
  _isResizable = false;
  _fontSize = -1;
  setFontSize(0);
  _layoutActions &= ~(MoveTo | Copy | CopyTo);
  _standardActions |= Delete | Edit;
  _legendMargin = 5;
  _parsedTitle = 0L;
  _trackContents = true;
  
  QStringList ctaglist;

  // read the properties
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement();
    if (!el.isNull()) {
      if (metaObject()->findProperty(el.tagName().toLatin1(), true) > -1) {
        setProperty(el.tagName().toLatin1(), QVariant(el.text()));
      } else if (el.tagName() == "curvetag") {
        ctaglist.append(el.text()); 
      }
    }
    n = n.nextSibling();
  }
  if (!_title.isEmpty()) {
    reparseTitle();
  }

  KstBaseCurveList l = kstObjectSubList<KstDataObject,KstBaseCurve>(KST::dataObjectList);
  KstBaseCurveList::ConstIterator end = l.end();
  for (QStringList::ConstIterator str = ctaglist.begin(); str != ctaglist.end(); ++str) {
    KstBaseCurveList::ConstIterator it = l.findTag(*str);
    if (it != end) {
      addCurve(*it);
    }
  }
  
  // Padding was incorrectly saved as 5 in Kst <= 1.2.0 so we need to strip it
  // out here since this object is buggy.  Remove this once padding is properly
  // supported?  Maybe, but geometries are then wrong.
  if (e.ownerDocument().documentElement().attribute("version") == "1.2") {
    setPadding(0);
  }
}


KstViewLegend::KstViewLegend(const KstViewLegend& legend)
: KstBorderedViewObject(legend) {
  _type = "Legend";
  _layoutActions &= ~(MoveTo | Copy | CopyTo);
  _standardActions |= Delete | Edit;
  
  _fallThroughTransparency = legend._fallThroughTransparency;
  _container = legend._container;
  _rotation = legend._rotation;
  _fontName = legend._fontName;
  _fontSize = legend._fontSize;
  _vertical = legend._vertical;
  _isResizable = legend._isResizable;
  _absFontSize = legend._absFontSize;
  _legendMargin = legend._legendMargin;
  _title = legend._title;
  _parsedTitle = 0L;
  _trackContents = legend._trackContents;
  _curves = Q3DeepCopy<KstBaseCurveList>(legend._curves);
  
  reparseTitle();
  computeTextSize();
}


KstViewLegend::~KstViewLegend() {
  delete _parsedTitle;
  _parsedTitle = 0L;
}


KstViewObject* KstViewLegend::copyObjectQuietly(KstViewObject &parent, const QString& name) const { 
  Q_UNUSED(name)
  
  KstViewLegend* viewLegend = new KstViewLegend(*this);
  parent.appendChild(viewLegend, true);

  return viewLegend;
}


void KstViewLegend::resize(const QSize& size) {
  KstBorderedViewObject::resize(size);
  if (!_parsedTitle) {
    reparseTitle();
  }
}


int KstViewLegend::ascent() const {
  return _ascent;
}


void KstViewLegend::setFontName(const QString& fontName) {
  if (_fontName != fontName) {
    setDirty();
    _fontName = fontName;
  }
}


const QString& KstViewLegend::fontName() const {
  return _fontName;
}


void KstViewLegend::save(QTextStream &ts, const QString& indent) {
  ts << indent << "<" << type() << ">" << endl;
  KstBorderedViewObject::save(ts, indent + "  ");
  
  for (KstBaseCurveList::ConstIterator j = _curves.begin(); j != _curves.end(); ++j) {
    (*j)->readLock();
    ts << indent+"  " << "<curvetag>" << Q3StyleSheet::escape((*j)->tagName()) << "</curvetag>" << endl;
    (*j)->unlock();
  }

  ts << indent << "</" << type() << ">" << endl;
}


void KstViewLegend::drawToBuffer() {
  setDirty(false);

  _backBuffer.buffer().resize(contentsRect().size());
  _backBuffer.buffer().fill(backgroundColor());
  KstPainter p;
  p.begin(&_backBuffer.buffer());
  QPen pen;
  pen.setColor(foregroundColor());
  p.setPen(pen);
  drawToPainter(p);
  p.end();
}


void KstViewLegend::drawToPainter(KstPainter& p) {
  RenderContext rc(_fontName, _absFontSize, &p);

  if (_vertical) {
    unsigned i = 0;

    if (!_title.isEmpty()) {
      p.save();
      p.translate(_legendMargin*_ascent/10, _legendMargin*_ascent/10);
      rc.x = 0;
      rc.y = _ascent;
      rc.xStart = rc.x;
      renderLabel(rc, _parsedTitle->chunk);
      i = 1;
      p.restore();
    }
    for (KstBaseCurveList::Iterator it = _curves.begin(); it != _curves.end(); ++it) {
      p.save();
      if ((*it)->parsedLegendTag()) {
        p.translate(_legendMargin*_ascent/10, _legendMargin*_ascent/10 + i * (rc.fontHeight() + _ascent / 4));
        QRect symbolBound(QPoint(0,0), QSize(16*_ascent/4, rc.fontHeight()));

        (*it)->paintLegendSymbol(&p, symbolBound);
        p.translate(9*_ascent/2, 0);
        rc.x = 0;
        rc.y = _ascent;
        rc.xStart = rc.x;
        renderLabel(rc, (*it)->parsedLegendTag()->chunk);
      }
      p.restore();
      ++i;
    }
  } else {
    p.save();
    p.translate(_legendMargin*_ascent/10, _legendMargin*_ascent/10);
    if (!_title.isEmpty()) {
      rc.x = 0;
      rc.y = _ascent;
      rc.xStart = rc.x;
      renderLabel(rc, _parsedTitle->chunk);
      p.translate(_titleWidth + _ascent,0);
    }

    for (KstBaseCurveList::Iterator it = _curves.begin(); it != _curves.end(); ++it) {
      if ((*it)->parsedLegendTag()) {
        QRect symbolBound(QPoint(0,0),
                          QSize(16*_ascent/4, rc.fontHeight()));
        (*it)->paintLegendSymbol(&p, symbolBound);
        p.translate(9*_ascent/2, 0);
        rc.x = 0;
        rc.y = _ascent;
        rc.xStart = rc.x;
        renderLabel(rc, (*it)->parsedLegendTag()->chunk);
        p.translate((*it)->legendLabelSize().width() + _ascent,0);
      }
    }
    p.restore();
  }

  QApplication::syncX();
}


void KstViewLegend::computeTextSize() {
  _textWidth = 0;
  _ascent = 0;
  _textHeight = 0;

  for (KstBaseCurveList::Iterator it = _curves.begin(); it != _curves.end(); it++) {
    if ((*it)->parsedLegendTag()) {
      RenderContext rc(_fontName, _absFontSize, 0L);
      renderLabel(rc, (*it)->parsedLegendTag()->chunk);
      if (_vertical) {
        if (rc.xMax > _textWidth) {
          _textWidth = rc.xMax;
        }
      } else {
        if (rc.fontHeight() > _textHeight) {
          _textHeight = rc.fontHeight();
        }
        _textWidth += rc.xMax;
      }
      (*it)->setLegendLabelSize(QSize(rc.xMax, rc.fontHeight()));
    } else {
      (*it)->setLegendLabelSize(QSize(0,0));
    }
  }
  RenderContext rc(_fontName, _absFontSize, 0L);
  _ascent = rc.fontAscent();

  if (_vertical) {
    if (_curves.count() > 0) {
      _textHeight = _curves.count()*rc.fontHeight() + (_curves.count()-1)*_ascent/4;
    } else {
      _textHeight = _ascent/4;
    }
  } else {
    if (_curves.count() > 0) {
      _textWidth += _curves.count()*_ascent;
    } else {
      _textWidth += _ascent;
    }
  }

  // determine title size
  if (!_title.isEmpty()) {
    if (!_parsedTitle) {
      reparseTitle();
    }
    renderLabel(rc, _parsedTitle->chunk);
    _titleWidth = rc.xMax;
    _titleHeight = rc.fontHeight();
  } else {
    _titleWidth = _titleHeight = 0;
  }
}


void KstViewLegend::updateSelf() {
  bool wasDirty(dirty());
  KstBorderedViewObject::updateSelf();
  if (wasDirty) {
    adjustSizeForText(_parent->contentsRect());
    drawToBuffer();
  }
}


void KstViewLegend::paintSelf(KstPainter& p, const QRegion& bounds) {
  if (p.type() == KstPainter::P_PRINT || p.type() == KstPainter::P_EXPORT) {
    p.save();
    QRect cr(contentsRectForPainter(p));
    cr.setSize(sizeForText(_parent->geometry()));
    setContentsRectForPainter(p, cr);    
    KstBorderedViewObject::paintSelf(p, bounds);
    
    p.translate(cr.left(), cr.top());
    if (!_transparent) {
      p.fillRect(0, 0, cr.width(), cr.height(), _backgroundColor);
    }
    drawToPainter(p);
    
    p.restore();
  } else {
    const QRect cr(contentsRect());
    if (p.makingMask()) {
      p.setRasterOp(Qt::SetROP);
    } else {
      const QRegion clip(clipRegion());
      KstBorderedViewObject::paintSelf(p, bounds - _myClipMask);
      p.setClipRegion(bounds & clip);
    }

    _backBuffer.paintInto(p, cr);
  }
}


void KstViewLegend::invalidateClipRegion() {
  KstBorderedViewObject::invalidateClipRegion();
  _myClipMask = QRegion();
}


QRegion KstViewLegend::clipRegion() {
  if (!_transparent) {
    return KstBorderedViewObject::clipRegion();
  }

  if (_clipMask.isNull() && _myClipMask.isNull()) {
    const QRect cr(contentsRect());
    QBitmap bm = _backBuffer.buffer().createHeuristicMask(false); // slow but preserves antialiasing...
    _myClipMask = QRegion(bm);
    _myClipMask.translate(cr.topLeft().x(), cr.topLeft().y());

    QBitmap bm1(_geom.bottomRight().x() + 1, _geom.bottomRight().y() + 1, true);
    if (!bm1.isNull()) {
      KstPainter p;
      p.setMakingMask(true);
      p.begin(&bm1);
      p.setViewXForm(true);
      KstBorderedViewObject::paintSelf(p, QRegion());
      p.flush();
      p.end();
      _clipMask = QRegion(bm1);
    }
  }

  return _clipMask | _myClipMask;
}


void KstViewLegend::setFontSize(int size) {
  if (_fontSize != size) {
    _absFontSize = size + KstSettings::globalSettings()->plotFontSize;
    if (_absFontSize < KstSettings::globalSettings()->plotFontMinSize) {
      _absFontSize = KstSettings::globalSettings()->plotFontMinSize;
    }

    _fontSize = size;
    setDirty();
  }
}


int KstViewLegend::fontSize() const {
  return _fontSize;
}


void KstViewLegend::adjustSizeForText(const QRect& w) {
  QRect cr(contentsRect());
  cr.setSize(sizeForText(w));
  setContentsRect(cr);
}


QSize KstViewLegend::sizeForText(const QRect& w) {
  double x_s, y_s;

  x_s = y_s = _fontSize + (double)KstSettings::globalSettings()->plotFontSize;

  int x_pix = w.width();
  int y_pix = w.height();

  if (x_pix < y_pix) {
    x_s *= x_pix/540.0;
    y_s *= y_pix/748.0;
  } else {
    y_s *= y_pix/540.0;
    x_s *= x_pix/748.0;
  }

  _absFontSize = int((x_s + y_s)/2.0);
  if (_absFontSize < KstSettings::globalSettings()->plotFontMinSize) {
    _absFontSize = KstSettings::globalSettings()->plotFontMinSize;
  }

  computeTextSize();

  int width, height;
  if (_vertical) {
    width = qMax(_textWidth + 9*_ascent/2, _titleWidth);
    height = _textHeight;
    if (_titleHeight > 0) {
      height += _titleHeight;
    }
  } else {
    height = qMax(_textHeight, _titleHeight);

    if (_titleWidth > 0) {
      width = _titleWidth + _textWidth + 9*_ascent*_curves.count()/2;
    } else {
      width = _textWidth + 9*_ascent*_curves.count()/2 - _ascent;
    }
  }

  QSize sz(width, height);
  
  sz += QSize(2 * _legendMargin * _ascent / 10, 2 * _legendMargin * _ascent / 10);

  if (_parent) {
    QRect r(position(), sz);
    sz = r.intersect(_parent->geometry()).size();
  }
    
  return sz;
}

void KstViewLegend::modifiedLegendEntry() {
  setDirty();
}

bool KstViewLegend::layoutPopupMenu(KMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent) {
  return KstViewObject::layoutPopupMenu(menu, pos, topLevelParent);
}


KstViewObjectPtr create_KstViewLegend() {
  return KstViewObjectPtr(new KstViewLegend());
}


KstViewObjectFactoryMethod KstViewLegend::factory() const {
  return &create_KstViewLegend;
}


/* FIXME: not written for legends yet. */
void KstViewLegend::writeBinary(QDataStream& str) {
}


/* FIXME: not written for legends yet. */
void KstViewLegend::readBinary(QDataStream& str) {
}


bool KstViewLegend::transparent() const {
  return KstViewObject::transparent();
}


void KstViewLegend::setTransparent(bool transparent) {
  KstViewObject::setTransparent(transparent);
}


void KstViewLegend::addCurve(KstBaseCurvePtr incurve) {
  if (!_curves.contains(incurve)) {
    _curves.append(incurve);
    connect(incurve, SIGNAL(modifiedLegendEntry()), this, SLOT(modifiedLegendEntry()));
    setDirty();
  }
}


void KstViewLegend::removeCurve(KstBaseCurvePtr incurve) {
  if (_curves.contains(incurve)) {
    _curves.remove(incurve);
    disconnect(incurve,SIGNAL(modifiedLegendEntry()), this, SLOT(modifiedLegendEntry()));
    setDirty();
  }
}


void KstViewLegend::clear() {
  if (!_curves.isEmpty()) {
    _curves.clear();
    setDirty();
  }
}


void KstViewLegend::setCurveList(Kst2DPlotPtr pl) {
  _curves = Q3DeepCopy<KstBaseCurveList>(pl->Curves);
  setDirty();
}

void KstViewLegend::reparseTitle() {
  delete _parsedTitle;
  _parsedTitle = Label::parse(_title, true, false);
  setDirty();
}


/** fill the custom widget with current properties */
bool KstViewLegend::fillConfigWidget(QWidget *w, bool isNew) const {
  ViewLegendWidget *widget = dynamic_cast<ViewLegendWidget*>(w);
  if (!widget) {
    return false;
  }

  KstBaseCurveList allCurves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
  
  if (isNew) {
    widget->_fontSize->setValue(0);
    widget->_fontColor->setColor(KstSettings::globalSettings()->foregroundColor);
    widget->_font->setCurrentFont(KstApp::inst()->defaultFont());
    widget->_margin->setValue(5);
    widget->_boxColors->setForeground(KstSettings::globalSettings()->foregroundColor);
    widget->_boxColors->setBackground(KstSettings::globalSettings()->backgroundColor);
    widget->_vertical->setChecked(true);
    widget->_transparent->setChecked(false);
    widget->_border->setValue(2);
    widget->_title->setText("");
    widget->TrackContents->setChecked(true);
 
    for (KstBaseCurveList::ConstIterator it = allCurves.begin(); it != allCurves.end(); ++it) {
      (*it)->readLock();
      widget->AvailableCurveList->insertItem((*it)->tagName());
      (*it)->unlock();
    }

  } else { // fill legend properties into widget
    widget->TrackContents->setChecked(trackContents());
    widget->_title->setText(title());
    widget->_fontSize->setValue(int(fontSize()));
    widget->_fontColor->setColor(foregroundColor());
    widget->_font->setCurrentFont(fontName());
    widget->_transparent->setChecked(transparent());
    widget->_border->setValue(borderWidth());
    widget->_boxColors->setForeground(borderColor());
    widget->_boxColors->setBackground(backgroundColor());
    widget->_margin->setValue(_legendMargin);
    widget->_vertical->setChecked(vertical());
    for (KstBaseCurveList::ConstIterator it = _curves.begin(); it != _curves.end(); ++it) {
      (*it)->readLock();
      widget->DisplayedCurveList->insertItem((*it)->tagName());
      (*it)->unlock();
    }
    for (KstBaseCurveList::ConstIterator it = allCurves.begin(); it != allCurves.end(); ++it) {
      (*it)->readLock();
      if (_curves.find(*it) == _curves.end()) {
        widget->AvailableCurveList->insertItem((*it)->tagName());
      }
      (*it)->unlock();
    }
  }
  return false;
}


/** apply properties in the custom config widget to this */
bool KstViewLegend::readConfigWidget(QWidget *w) {
  KstViewLegendList legends;
  KstViewLegendPtr legendExtra;

  ViewLegendWidget *widget = dynamic_cast<ViewLegendWidget*>(w);
  if (!widget) {
    return false;
  }

  if (widget->_thisLegend->isChecked()) {
    legends += this;
  } else if (widget->_thisWindow->isChecked()) {
    legends = topLevelParent()->findChildrenType<KstViewLegend>(true);
  } else {
    legends = globalLegendList();
  }

  // apply the curve list, but only to this legend!
  KstBaseCurveList allCurves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
  _curves.clear();
  for (unsigned i = 0; i < widget->DisplayedCurveList->count(); i++) {
    KstBaseCurveList::Iterator it = allCurves.findTag(widget->DisplayedCurveList->text(i));
    if (it != allCurves.end()) {
      _curves.append(*it);
    }
  }

  for (uint i = 0; i < legends.size(); i++) {
    legendExtra = legends[i];

    legendExtra->setFontSize(widget->_fontSize->value());
    legendExtra->setForegroundColor(widget->_fontColor->color());
    legendExtra->setFontName(widget->_font->currentFont());
    legendExtra->setTitle(widget->_title->text());

    legendExtra->setTransparent(widget->_transparent->isChecked());
    legendExtra->setBorderWidth(widget->_border->value());
    legendExtra->setBorderColor(widget->_boxColors->foreground());
    legendExtra->setBackgroundColor(widget->_boxColors->background());
    legendExtra->setLegendMargin(widget->_margin->value());
    legendExtra->setVertical(widget->_vertical->isChecked());
    legendExtra->setTrackContents(widget->TrackContents->isChecked());
  }

  setDirty();
  return true;
}


void KstViewLegend::connectConfigWidget(QWidget *parent, QWidget *w) const {
  ViewLegendWidget *widget = dynamic_cast<ViewLegendWidget*>(w);
  if (!widget) {
    return;
  }

  connect(widget, SIGNAL(changed()), parent, SLOT(modified()));
  connect(widget->AvailableCurveList, SIGNAL(changed()), parent, SLOT(modified()));
  connect(widget->DisplayedCurveList, SIGNAL(changed()), parent, SLOT(modified()));
  connect(widget->_title, SIGNAL(textChanged(const QString&)), parent, SLOT(modified()));
  connect(widget->TrackContents, SIGNAL(pressed()), parent, SLOT(modified()));
  connect(widget->_font, SIGNAL(activated(int)), parent, SLOT(modified()));
  connect(widget->_fontSize, SIGNAL(valueChanged(int)), parent, SLOT(modified()));
  connect(widget->_fontSize->child("qt_spinbox_edit"), SIGNAL(textChanged(const QString&)), parent, SLOT(modified()));
  connect(widget->_vertical, SIGNAL(pressed()), parent, SLOT(modified()));
  connect(widget->_fontColor, SIGNAL(changed(const QColor&)), parent, SLOT(modified()));  
  connect(widget->_transparent, SIGNAL(pressed()), parent, SLOT(modified()));
  connect(widget->_boxColors, SIGNAL(fgChanged(const QColor&)), parent, SLOT(modified()));
  connect(widget->_boxColors, SIGNAL(bgChanged(const QColor&)), parent, SLOT(modified()));
  connect(widget->_margin, SIGNAL(valueChanged(int)), parent, SLOT(modified()));
  connect(widget->_margin->child("qt_spinbox_edit"), SIGNAL(textChanged(const QString&)), parent, SLOT(modified()));
  connect(widget->_border, SIGNAL(valueChanged(int)), parent, SLOT(modified()));
  connect(widget->_border->child("qt_spinbox_edit"), SIGNAL(textChanged(const QString&)), parent, SLOT(modified()));
  connect(widget->_thisLegend, SIGNAL(stateChanged(int)), parent, SLOT(modified()));

}


QWidget *KstViewLegend::configWidget() {
  return new ViewLegendWidget;
}


bool KstViewLegend::vertical() const {
  return _vertical;
}


void KstViewLegend::setVertical(bool vertical) {
  if (_vertical != vertical) {
    _vertical = vertical;
    setDirty();
  }
}


void KstViewLegend::setLegendMargin(int margin) {
  int mm = qMax(0, margin);
  if (_legendMargin != mm) {
    _legendMargin = mm;
    setDirty();
  }
}


int KstViewLegend::legendMargin() const {
  return _legendMargin;
}

const QString& KstViewLegend::title() const {
  return _title;
}

void KstViewLegend::setTitle(const QString& title_in) {
  if (_title != title_in) {
    _title = title_in;
    reparseTitle(); // calls setDirty()
  }
}

bool KstViewLegend::trackContents() const {
  return _trackContents;
}

void KstViewLegend::setTrackContents( bool track) {
  _trackContents = track;
}

KstBaseCurveList& KstViewLegend::curves() {
  return _curves;
}

KstViewLegendList KstViewLegend::globalLegendList() {
  KstViewLegendList rc;
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  if (it) {
    while (it->currentItem()) {
      KstViewWindow *view = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (view) {
        KstViewLegendList sub = view->view()->findChildrenType<KstViewLegend>(true);
        rc += sub;
      }
      it->next();
    }
    app->deleteIterator(it);
  }
  return rc;
}

namespace {
KstViewObject *create_KstViewLegend() {
  return new KstViewLegend;
}


KstGfxMouseHandler *handler_KstViewLegend() {
  return 0L;
}

KST_REGISTER_VIEW_OBJECT(Legend, create_KstViewLegend, handler_KstViewLegend)
}


#include "kstviewlegend.moc"
// vim: ts=2 sw=2 et
