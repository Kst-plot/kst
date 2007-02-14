/***************************************************************************
                              kstviewlegend.h
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

#ifndef KSTVIEWLEGEND_H
#define KSTVIEWLEGEND_H

#include "kstbackbuffer.h"
#include "kstborderedviewobject.h"
#include "kstscalar.h"
#include "labelparser.h"
#include "kstbasecurve.h"

#include <qguardedptr.h>

class Kst2DPlot;
typedef KstSharedPtr<Kst2DPlot> Kst2DPlotPtr;
class KstViewLegend;
typedef KstSharedPtr<KstViewLegend> KstViewLegendPtr;
class KstViewLegend;
typedef KstObjectList<KstSharedPtr<KstViewLegend> > KstViewLegendList;

class KstViewLegend : public KstBorderedViewObject {
  Q_OBJECT
  Q_PROPERTY(QString font READ fontName WRITE setFontName)
  Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize)
  Q_PROPERTY(bool transparent READ transparent WRITE setTransparent)
  Q_PROPERTY(bool trackContents READ trackContents WRITE setTrackContents)
  Q_PROPERTY(int legendMargin READ legendMargin WRITE setLegendMargin)
  Q_PROPERTY(bool vertical READ vertical WRITE setVertical)
  Q_PROPERTY(QString title READ title WRITE setTitle)
  public:
    KstViewLegend();
    KstViewLegend(const QDomElement& e);
    KstViewLegend(const KstViewLegend& legend);
    virtual ~KstViewLegend();

    static KstViewLegendList globalLegendList();

    virtual KstViewObject* copyObjectQuietly(KstViewObject &parent, const QString& name = QString::null) const;
    void setTitle(const QString& title);
    const QString& title() const;
    void reparseTitle();

    int ascent() const;

    void setFontName(const QString& fontName);
    const QString& fontName() const;

    void setFontSize(int size);
    int fontSize() const;

    void save(QTextStream& ts, const QString& indent = QString::null);

    void setTransparent(bool transparent);
    bool transparent() const;

    void updateSelf();
    void paintSelf(KstPainter& p, const QRegion& bounds);
    void resize(const QSize&);
    QRegion clipRegion();
    
    QWidget *configWidget();

    // handle custom widget, if any: is called by KstEditViewObjectDialogI
    bool fillConfigWidget(QWidget *w, bool isNew) const;
    bool readConfigWidget(QWidget *w);
    void connectConfigWidget(QWidget *parent, QWidget *w) const;
    
    void addCurve(KstBaseCurvePtr curve);
    void removeCurve(KstBaseCurvePtr curve);
    void clear();

    void setCurveList(Kst2DPlotPtr plot);
    
    bool vertical() const;
    void setVertical(bool vertical);

    void setLegendMargin(int margin);
    int legendMargin() const;

    KstBaseCurveList& curves();

    void invalidateClipRegion();
    bool trackContents() const;
    void setTrackContents(bool track);
    void adjustSizeForText(const QRect& w);
    QSize sizeForText(const QRect& w);

  public slots:
    void modifiedLegendEntry();

  protected:
    KstViewObjectFactoryMethod factory() const;
    bool layoutPopupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent);
    void readBinary(QDataStream& str);
    void writeBinary(QDataStream& str);

  private:
    void drawToBuffer();
    void drawToPainter(KstPainter& p);
    void computeTextSize();

    double _rotation;
    QString _fontName;
    KstScalarList _scalarsUsed;

    bool _replace : 1;
    bool _vertical : 1;
    int _absFontSize; // points
    int _fontSize; // size relative to reference size.....
    int _textWidth, _textHeight, _ascent;
    int _titleWidth, _titleHeight;
    int _legendMargin;
    KstBackBuffer _backBuffer;
    KstBaseCurveList _curves;
    QRegion _myClipMask;
    bool _trackContents;

    QString _title;
    Label::Parsed *_parsedTitle;
};

typedef KstSharedPtr<KstViewLegend> KstViewLegendPtr;

#endif
// vim: ts=2 sw=2 et
