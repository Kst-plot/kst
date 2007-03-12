/***************************************************************************
                               kstviewpicture.cpp
                             -------------------
    begin                : Jun 14, 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "kstgfxpicturemousehandler.h"
#include "kst.h"
#include "kstviewobjectfactory.h"
#include "kstviewpicture.h"

#include <assert.h>
#include <math.h>

#include <qdebug.h>
#include <kglobal.h>
#include <kio/netaccess.h>
#include <klocale.h>

#include <qbitmap.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmetaobject.h>
#include <qpainter.h>
#include <qtimer.h>

KstViewPicture::KstViewPicture()
: KstBorderedViewObject("Picture") {
  _refresh = 0;
  _timer = 0L;
  setTransparent(true);
  _maintainAspect = true;
  _standardActions |= Delete | Edit;
}


KstViewPicture::KstViewPicture(const QDomElement& e)
: KstBorderedViewObject(e) {
  _refresh = 0;
  _timer = 0L;

  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement();
    if (!el.isNull()) {
      if (metaObject()->findProperty(el.tagName().toLatin1(), true) > -1) {
        setProperty(el.tagName().toLatin1(), QVariant(el.text()));
      }
    }
    n = n.nextSibling();
  }

  // always have these values
  _type = "Picture";
  setTransparent(true);
  _standardActions |= Delete | Edit;
}


KstViewPicture::KstViewPicture(const KstViewPicture& picture)
: KstBorderedViewObject(picture) {
  _refresh = picture._refresh;
  _url = picture._url;
  doRefresh();
  
  // always have these values
  _type = "Picture";
  _standardActions |= Delete | Edit;
}


KstViewPicture::~KstViewPicture() {
}


KstViewObject* KstViewPicture::copyObjectQuietly(KstViewObject& parent, const QString& name) const {
  Q_UNUSED(name)
  
  KstViewPicture* viewPicture = new KstViewPicture(*this);
  parent.appendChild(viewPicture, true);
  
  return viewPicture;
}


QRegion KstViewPicture::clipRegion() {
  if (_clipMask.isNull()) {
    _myClipMask = QRegion();
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
    QBitmap bm2(_geom.bottomRight().x() + 1, _geom.bottomRight().y() + 1, true);
    if (!bm2.isNull()) {
      KstPainter p;
      p.setMakingMask(true);
      p.begin(&bm2);
      p.setViewXForm(true);
      paintSelf(p, QRegion());
      p.flush();
      p.end();
      _myClipMask = QRegion(bm2);
    }
  }

  return _myClipMask | _clipMask;
}


void KstViewPicture::paintSelf(KstPainter& p, const QRegion& bounds) {
  p.save();
  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    if (p.makingMask()) {
      p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    } else {
      const QRegion clip(clipRegion());
      KstBorderedViewObject::paintSelf(p, bounds - _myClipMask);
      p.setClipRegion(bounds & clip);
    }
  } else {
    KstBorderedViewObject::paintSelf(p, bounds);
  }

  const QRect cr(contentsRectForPainter(p));
  if (_image.isNull()) {
    // fill with X
    p.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    p.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    p.drawRect(cr);
    p.drawLine(cr.topLeft(), cr.bottomRight());
    p.drawLine(cr.topRight(), cr.bottomLeft());
  } else {
    assert(!cr.isNull()); // Null view objects are not allowed.  I want to see
                          // how this happens so it can be fixed.

    if (_iCache.isNull() || _iCache.size() != cr.size()) {
      _iCache = _image.copy();
      if (!_iCache.isNull()) {
        _iCache = _iCache.smoothScale(cr.size());
      }
    }
    if (!_iCache.isNull()) {
      if (p.makingMask()) {
        // which indicates clipping / BW mode
        if (_iCache.hasAlphaBuffer()) {
          p.drawImage(cr.topLeft(), _iCache.createAlphaMask());
        } else {
          p.setBrush(Qt::color1);
          p.drawRect(cr);
        }
      } else {
        p.drawImage(cr.topLeft(), _iCache);
      }
    }
  }
  p.restore();
}


void KstViewPicture::save(QTextStream& ts, const QString& indent) {
  ts << indent << "<" << type() << ">" << endl;
  KstBorderedViewObject::save(ts, indent + "  ");
  ts << indent << "</" << type() << ">" << endl;
}


bool KstViewPicture::setImage(const QString& source) {
  KUrl url;

  if (QFile::exists(source) && QFileInfo(source).isRelative()) {
    url.setPath(source);
  } else {
    url = KUrl(source);
  }

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
  // FIXME: come up with a way to indicate the "widget" and fill it in here so
  //        that KIO dialogs are associated with the proper window
  if (!KIO::NetAccess::exists(url, true, 0L)) {
#else
  if (!KIO::NetAccess::exists(url, true)) {
#endif
    return false;
  }

  QString tmpFile;
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
  // FIXME: come up with a way to indicate the "widget" and fill it in here so
  //        that KIO dialogs are associated with the proper window
  if (!KIO::NetAccess::download(url, tmpFile, 0L)) {
#else
  if (!KIO::NetAccess::download(url, tmpFile)) {
#endif
    return false;
  }

  bool success = true;

  QImage ti;
  ti.setAlphaBuffer(true);
  if (ti.load(tmpFile)) {
    setImage(ti);
    _url = source;

    if (_maintainAspect == true) { restoreAspect(); }
  } else {
    success = false;
  }

  KIO::NetAccess::removeTempFile(tmpFile);
  return success;
}


void KstViewPicture::setImage(const QImage& image) {
  _url = QString::null;
  _image = image;
  _iCache = QImage();
  setDirty();
}


const QString& KstViewPicture::url() const {
  return _url;
}


const QImage& KstViewPicture::image() const {
  return _image;
}


void KstViewPicture::doRefresh() {
  if (_url.isEmpty()) {
    setRefreshTimer(0);
  } else {
    QString u = _url;
    bool rc = setImage(_url);
    _url = u;
    // FIXME: after n failures, give up?
    if (rc) {
      KstApp::inst()->paintAll(KstPainter::P_PAINT);
    }
  }
}


void KstViewPicture::setRefreshTimer(int seconds) {
  _refresh = qMax(0, seconds);
  if (_refresh) {
    if (!_timer) {
      _timer = new QTimer(this);
      connect(_timer, SIGNAL(timeout()), this, SLOT(doRefresh()));
    }
    _timer->start(_refresh * 1000, false);
  } else {
    delete _timer;
    _timer = 0L;
  }
}


bool KstViewPicture::transparent() const {
  if (!_iCache.isNull()) {
    return _iCache.hasAlphaBuffer();
  }
  
  return _image.hasAlphaBuffer();
}


int KstViewPicture::refreshTimer() const {
  return _refresh;
}


bool KstViewPicture::maintainAspect() const {
  return _maintainAspect;
}


void KstViewPicture::setMaintainAspect(bool maintain) {
  _maintainAspect = maintain;
}


void KstViewPicture::restoreSize() {
  QRect cr(contentsRect());
  cr.setSize(_image.size());
  setContentsRect(cr);
}


void KstViewPicture::restoreAspect() {
  QRect cr(contentsRect());
  QSize size = _image.size(); //start with original size.

  size.scale( cr.size().width(), cr.size().height(), QSize::ScaleMin ); //find largest rect. which will fit inside original and still preserve aspect.

  cr.setSize(size);
  setContentsRect(cr);
}

QMap<QString, QVariant> KstViewPicture::widgetHints(const QString& propertyName) const {
  QMap<QString, QVariant> map = KstBorderedViewObject::widgetHints(propertyName);
  if (!map.empty()) {
    return map;
  }

  if (propertyName == "path") {
    map.insert(QString("_kst_widgetType"), QString("KUrlRequester"));
    map.insert(QString("_kst_label"), i18n("File path"));
  } else if (propertyName == "refreshTimer") {
    map.insert(QString("_kst_widgetType"), QString("QSpinBox"));
    map.insert(QString("_kst_label"), i18n("Refresh timer"));
  } else if (propertyName == "maintainAspect") {
    map.insert(QString("_kst_widgetType"), QString("QCheckBox"));
    map.insert(QString("_kst_label"), QString::null);    
    map.insert(QString("text"), i18n("Maintain aspect ratio"));
  }
  // FIXME: uncomment once these are supported.
  /* else if (propertyName == "padding") {
  map.insert(QString("_kst_widgetType"), QString("QSpinBox"));
  map.insert(QString("_kst_label"), i18n("Padding"));
  map.insert(QString("minimum"), 0);
} else if (propertyName == "margin") {
  map.insert(QString("_kst_widgetType"), QString("QSpinBox"));
  map.insert(QString("_kst_label"), i18n("Margin"));
  map.insert(QString("minimum"), 0);
}*/
  return map;
}


namespace {
KstViewObject *create_KstViewPicture() {
  return new KstViewPicture;
}


KstGfxMouseHandler *handler_KstViewPicture() {
  return new KstGfxPictureMouseHandler;
}

KST_REGISTER_VIEW_OBJECT(Picture, create_KstViewPicture, handler_KstViewPicture)
}


#include "kstviewpicture.moc"
// vim: ts=2 sw=2 et
