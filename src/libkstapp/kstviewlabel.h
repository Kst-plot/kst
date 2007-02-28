/***************************************************************************
                              kstviewlabel.h
                             ----------------
    begin                : Apr 10 2004
    copyright            : (C) 2000 by cbn
                           (C) 2004 by The University of Toronto
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

#ifndef KSTVIEWLABEL_H
#define KSTVIEWLABEL_H

#include "dataref.h"
#include "kstbackbuffer.h"
#include "kstborderedviewobject.h"
#include "kstscalar.h"
#include "labelparser.h"

#include <qpointer.h>
#include <q3valuevector.h>

class KstViewLabel : public KstBorderedViewObject {
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(double rotation READ rotation WRITE setRotation)  
  Q_PROPERTY(QString font READ fontName WRITE setFontName)
  Q_PROPERTY(int dataPrecision READ dataPrecision WRITE setDataPrecision)
  Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize)
  Q_PROPERTY(bool transparent READ transparent WRITE setTransparent)
  Q_PROPERTY(int horizontalJustification READ horizJustifyWrap WRITE setHorizJustifyWrap)
  public:
    KstViewLabel(const QString& txt = QString::null, KstLJustifyType justify = 0L, float rotation = 0.0);
    KstViewLabel(const QDomElement& e);
    KstViewLabel(const KstViewLabel& box);
    ~KstViewLabel();

    virtual KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
    void setText(const QString& text);
    const QString& text() const;

    void setRotation(double rotation);
    double rotation() const;

    void setJustification(KstLJustifyType Justify);
    KstLJustifyType justification() const { return _justify; }

    // wraps for Q_PROPERTIES
    int horizJustifyWrap() const;
    // 0 = left, 1 = right, 2 = centre
    void setHorizJustifyWrap(int justify);

    int ascent() const;

    /** Interpret special characters, default = true */
    void setInterpreted(bool interpreted);
    bool interpreted() const;

    void setFontName(const QString& fontName);
    const QString& fontName() const;

    void setFontSize(int size);
    int fontSize() const;

    void save(Q3TextStream& ts, const QString& indent = QString::null);

    void setDoScalarReplacement(bool in_do);
    bool doScalarReplacement() const;

    void setDataPrecision(int prec);
    int dataPrecision() const;

    void setTransparent(bool transparent);
    bool transparent() const;

    void updateSelf();
    void paintSelf(KstPainter& p, const QRegion& bounds);
    void resize(const QSize&);
    QRegion clipRegion();
    void invalidateClipRegion();

    void setLabelMargin(int margin);
    int labelMargin() const;

    //virtual QMap<QString, QVariant> widgetHints(const QString& propertyName) const;
    QWidget *configWidget();

    // handle custom widget, if any: is called by KstEditViewObjectDialogI
    bool fillConfigWidget(QWidget *w, bool isNew) const;
    bool readConfigWidget(QWidget *w);
    void connectConfigWidget(QWidget *parent, QWidget *w) const;
    
    KstObject::UpdateType update(int counter);
    void adjustSizeForText(const QRect& w);
    QSize sizeForText(const QRect& w);

  public slots:
    void reparse();

  protected:
    KstViewObjectFactoryMethod factory() const;
    bool layoutPopupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent);
    void readBinary(QDataStream& str);
    void writeBinary(QDataStream& str);

  private:
    void drawToBuffer(Label::Parsed *lp);
    void drawToPainter(Label::Parsed *lp, QPainter& p);
    void computeTextSize(Label::Parsed*);

    double _rotation;
    QString _txt;
    QString _fontName;

    bool _replace : 1;
    bool _interpret : 1;
    int _absFontSize; // points
    int _fontSize; // size relative to reference size.....
    int _dataPrecision : 6;
    int _textWidth, _textHeight, _ascent;
    KstLJustifyType _justify;
    KstBackBuffer _backBuffer;
    Label::Parsed *_parsed;
    QRegion _myClipMask;
    int _labelMargin;

    struct DataCache {
      DataCache() : valid(false) {}
      bool valid;
      Q3ValueVector<DataRef> data;
      void update();
    };
    DataCache _cache;
};

typedef KstSharedPtr<KstViewLabel> KstViewLabelPtr;
typedef KstObjectList<KstSharedPtr<KstViewLabel> > KstViewLabelList;

#endif
// vim: ts=2 sw=2 et
