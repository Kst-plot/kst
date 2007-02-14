/**********************************************************************
** 
**
** Definition of QCanvas classes
**
** Created : 991211
**
** Copyright (C) 1999-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the canvas module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QCANVAS_H
#define QCANVAS_H

#ifndef QT_H
#include "qscrollview.h"
#include "qpixmap.h"
#include "qptrlist.h"
#include "qbrush.h"
#include "qpen.h"
#include "qvaluelist.h"
#include "qpointarray.h"
#endif // QT_H

#if !defined( QT_MODULE_CANVAS ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_CANVAS )
#define QM_EXPORT_CANVAS
#define QM_TEMPLATE_EXTERN_CANVAS
#else
#define QM_EXPORT_CANVAS Q_EXPORT
#define QM_TEMPLATE_EXTERN_CANVAS Q_TEMPLATE_EXTERN
#endif

#ifndef QT_NO_CANVAS


class QCanvasSprite;
class QCanvasPolygonalItem;
class QCanvasRectangle;
class QCanvasPolygon;
class QCanvasEllipse;
class QCanvasText;
class QCanvasLine;
class QCanvasChunk;
class QCanvas;
class QCanvasItem;
class QCanvasView;
class QCanvasPixmap;


class QM_EXPORT_CANVAS QCanvasItemList : public QValueList<QCanvasItem*> {
public:
    void sort();
    void drawUnique( QPainter& painter );
};


class QCanvasItemExtra;

class QM_EXPORT_CANVAS QCanvasItem : public Qt
{
public:
    QCanvasItem(QCanvas* canvas);
    virtual ~QCanvasItem();

    double x() const
	{ return myx; }
    double y() const
	{ return myy; }
    double z() const
	{ return myz; } // (depth)

    virtual void moveBy(double dx, double dy);
    void move(double x, double y);
    void setX(double a) { move(a,y()); }
    void setY(double a) { move(x(),a); }
    void setZ(double a) { myz=a; changeChunks(); }

    bool animated() const;
    virtual void setAnimated(bool y);
    virtual void setVelocity( double vx, double vy);
    void setXVelocity( double vx ) { setVelocity(vx,yVelocity()); }
    void setYVelocity( double vy ) { setVelocity(xVelocity(),vy); }
    double xVelocity() const;
    double yVelocity() const;
    virtual void advance(int stage);

    virtual bool collidesWith( const QCanvasItem* ) const=0;

    QCanvasItemList collisions(bool exact /* NO DEFAULT */ ) const;

    virtual void setCanvas(QCanvas*);

    virtual void draw(QPainter&)=0;

    void show();
    void hide();

    virtual void setVisible(bool yes);
    bool isVisible() const
	{ return (bool)vis; }
    virtual void setSelected(bool yes);
    bool isSelected() const
	{ return (bool)sel; }
    virtual void setEnabled(bool yes);
    bool isEnabled() const
	{ return (bool)ena; }
    virtual void setActive(bool yes);
    bool isActive() const
	{ return (bool)act; }
#ifndef QT_NO_COMPAT
    bool visible() const
	{ return (bool)vis; }
    bool selected() const
	{ return (bool)sel; }
    bool enabled() const
	{ return (bool)ena; }
    bool active() const
	{ return (bool)act; }
#endif

    enum RttiValues {
	Rtti_Item = 0,
	Rtti_Sprite = 1,
	Rtti_PolygonalItem = 2,
	Rtti_Text = 3,
	Rtti_Polygon = 4,
	Rtti_Rectangle = 5,
	Rtti_Ellipse = 6,
	Rtti_Line = 7,
	Rtti_Spline = 8
    };

    virtual int rtti() const;
    static int RTTI;

    virtual QRect boundingRect() const=0;
    virtual QRect boundingRectAdvanced() const;

    QCanvas* canvas() const
	{ return cnv; }

protected:
    void update() { changeChunks(); }

private:
    // For friendly subclasses...

    friend class QCanvasPolygonalItem;
    friend class QCanvasSprite;
    friend class QCanvasRectangle;
    friend class QCanvasPolygon;
    friend class QCanvasEllipse;
    friend class QCanvasText;
    friend class QCanvasLine;

    virtual QPointArray chunks() const;
    virtual void addToChunks();
    virtual void removeFromChunks();
    virtual void changeChunks();
    virtual bool collidesWith( const QCanvasSprite*,
			       const QCanvasPolygonalItem*,
			       const QCanvasRectangle*,
			       const QCanvasEllipse*,
			       const QCanvasText* ) const = 0;
    // End of friend stuff

    QCanvas* cnv;
    static QCanvas* current_canvas;
    double myx,myy,myz;
    QCanvasItemExtra *ext;
    QCanvasItemExtra& extra();
    uint ani:1;
    uint vis:1;
    uint val:1;
    uint sel:1;
    uint ena:1;
    uint act:1;
};


class QCanvasData;

class QM_EXPORT_CANVAS QCanvas : public QObject
{
    Q_OBJECT
public:
    QCanvas( QObject* parent = 0, const char* name = 0 );
    QCanvas(int w, int h);
    QCanvas( QPixmap p, int h, int v, int tilewidth, int tileheight );

    virtual ~QCanvas();

    virtual void setTiles( QPixmap tiles, int h, int v,
			   int tilewidth, int tileheight );
    virtual void setBackgroundPixmap( const QPixmap& p );
    QPixmap backgroundPixmap() const;

    virtual void setBackgroundColor( const QColor& c );
    QColor backgroundColor() const;

    virtual void setTile( int x, int y, int tilenum );
    int tile( int x, int y ) const
	{ return grid[x+y*htiles]; }

    int tilesHorizontally() const
	{ return htiles; }
    int tilesVertically() const
	{ return vtiles; }

    int tileWidth() const
	{ return tilew; }
    int tileHeight() const
	{ return tileh; }

    virtual void resize(int width, int height);
    int width() const
	{ return awidth; }
    int height() const
	{ return aheight; }
    QSize size() const
	{ return QSize(awidth,aheight); }
    QRect rect() const
	{ return QRect( 0, 0, awidth, aheight ); }
    bool onCanvas( int x, int y ) const
	{ return x>=0 && y>=0 && x<awidth && y<aheight; }
    bool onCanvas( const QPoint& p ) const
	{ return onCanvas(p.x(),p.y()); }
    bool validChunk( int x, int y ) const
	{ return x>=0 && y>=0 && x<chwidth && y<chheight; }
    bool validChunk( const QPoint& p ) const
	{ return validChunk(p.x(),p.y()); }

    int chunkSize() const
	{ return chunksize; }
    virtual void retune(int chunksize, int maxclusters=100);

    bool sameChunk(int x1, int y1, int x2, int y2) const
	{ return x1/chunksize==x2/chunksize && y1/chunksize==y2/chunksize; }
    virtual void setChangedChunk(int i, int j);
    virtual void setChangedChunkContaining(int x, int y);
    virtual void setAllChanged();
    virtual void setChanged(const QRect& area);
    virtual void setUnchanged(const QRect& area);

    // These call setChangedChunk.
    void addItemToChunk(QCanvasItem*, int i, int j);
    void removeItemFromChunk(QCanvasItem*, int i, int j);
    void addItemToChunkContaining(QCanvasItem*, int x, int y);
    void removeItemFromChunkContaining(QCanvasItem*, int x, int y);

    QCanvasItemList allItems();
    QCanvasItemList collisions( const QPoint&) const;
    QCanvasItemList collisions( const QRect&) const;
    QCanvasItemList collisions( const QPointArray& pa, const QCanvasItem* item,
				bool exact) const;

    void drawArea(const QRect&, QPainter* p, bool double_buffer=FALSE);

    // These are for QCanvasView to call
    virtual void addView(QCanvasView*);
    virtual void removeView(QCanvasView*);
    void drawCanvasArea(const QRect&, QPainter* p=0, bool double_buffer=TRUE);
    void drawViewArea( QCanvasView* view, QPainter* p, const QRect& r, bool dbuf );

    // These are for QCanvasItem to call
    virtual void addItem(QCanvasItem*);
    virtual void addAnimation(QCanvasItem*);
    virtual void removeItem(QCanvasItem*);
    virtual void removeAnimation(QCanvasItem*);

    virtual void setAdvancePeriod(int ms);
    virtual void setUpdatePeriod(int ms);

    virtual void setDoubleBuffering(bool y);

signals:
    void resized();

public slots:
    virtual void advance();
    virtual void update();

protected:
    virtual void drawBackground(QPainter&, const QRect& area);
    virtual void drawForeground(QPainter&, const QRect& area);

private:
    void init(int w, int h, int chunksze=16, int maxclust=100);

    QCanvasChunk& chunk(int i, int j) const;
    QCanvasChunk& chunkContaining(int x, int y) const;

    QRect changeBounds(const QRect& inarea);
    void drawChanges(const QRect& inarea);

    void ensureOffScrSize( int osw, int osh );
    QPixmap offscr;
    int awidth,aheight;
    int chunksize;
    int maxclusters;
    int chwidth,chheight;
    QCanvasChunk* chunks;

    QCanvasData* d;

    void initTiles(QPixmap p, int h, int v, int tilewidth, int tileheight);
    ushort *grid;
    ushort htiles;
    ushort vtiles;
    ushort tilew;
    ushort tileh;
    bool oneone;
    QPixmap pm;
    QTimer* update_timer;
    QColor bgcolor;
    bool debug_redraw_areas;
    bool dblbuf;

    friend void qt_unview(QCanvas* c);

#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    QCanvas( const QCanvas & );
    QCanvas &operator=( const QCanvas & );
#endif
};

class QCanvasViewData;

class QM_EXPORT_CANVAS QCanvasView : public QScrollView
{
    Q_OBJECT
public:

    QCanvasView(QWidget* parent=0, const char* name=0, WFlags f=0);
    QCanvasView(QCanvas* viewing, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~QCanvasView();

    QCanvas* canvas() const
	{ return viewing; }
    void setCanvas(QCanvas* v);

    const QWMatrix &worldMatrix() const;
    const QWMatrix &inverseWorldMatrix() const;
    bool setWorldMatrix( const QWMatrix & );

protected:
    void drawContents( QPainter*, int cx, int cy, int cw, int ch );
    QSize sizeHint() const;

private:
    void drawContents( QPainter* );
    QCanvas* viewing;
    QCanvasViewData* d;
    friend void qt_unview(QCanvas* c);

private slots:
    void cMoving(int,int);
    void updateContentsSize();

private:
#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    QCanvasView( const QCanvasView & );
    QCanvasView &operator=( const QCanvasView & );
#endif
};


class QM_EXPORT_CANVAS QCanvasPixmap : public QPixmap
{
public:
#ifndef QT_NO_IMAGEIO
    QCanvasPixmap(const QString& datafilename);
#endif
    QCanvasPixmap(const QImage& image);
    QCanvasPixmap(const QPixmap&, const QPoint& hotspot);
    ~QCanvasPixmap();

    int offsetX() const
	{ return hotx; }
    int offsetY() const
	{ return hoty; }
    void setOffset(int x, int y) { hotx = x; hoty = y; }

private:
#if defined(Q_DISABLE_COPY)
    QCanvasPixmap( const QCanvasPixmap & );
    QCanvasPixmap &operator=( const QCanvasPixmap & );
#endif
    void init(const QImage&);
    void init(const QPixmap& pixmap, int hx, int hy);

    friend class QCanvasSprite;
    friend class QCanvasPixmapArray;
    friend bool qt_testCollision(const QCanvasSprite* s1, const QCanvasSprite* s2);

    int hotx,hoty;

    QImage* collision_mask;
};


class QM_EXPORT_CANVAS QCanvasPixmapArray
{
public:
    QCanvasPixmapArray();
#ifndef QT_NO_IMAGEIO
    QCanvasPixmapArray(const QString& datafilenamepattern, int framecount=0);
#endif
    // this form is deprecated
    QCanvasPixmapArray(QPtrList<QPixmap>, QPtrList<QPoint> hotspots);

    QCanvasPixmapArray(QValueList<QPixmap>, QPointArray hotspots = QPointArray() );
    ~QCanvasPixmapArray();

#ifndef QT_NO_IMAGEIO
    bool readPixmaps(const QString& datafilenamepattern, int framecount=0);
    bool readCollisionMasks(const QString& filenamepattern);
#endif

    // deprecated
    bool operator!(); // Failure check.
    bool isValid() const;

    QCanvasPixmap* image(int i) const
	{ return img ? img[i] : 0; }
    void setImage(int i, QCanvasPixmap* p);
    uint count() const
	{ return (uint)framecount; }

private:
#if defined(Q_DISABLE_COPY)
    QCanvasPixmapArray( const QCanvasPixmapArray & );
    QCanvasPixmapArray &operator=( const QCanvasPixmapArray & );
#endif
#ifndef QT_NO_IMAGEIO
    bool readPixmaps(const QString& datafilenamepattern, int framecount, bool maskonly);
#endif

    void reset();
    int framecount;
    QCanvasPixmap** img;
};


class QM_EXPORT_CANVAS QCanvasSprite : public QCanvasItem
{
public:
    QCanvasSprite(QCanvasPixmapArray* array, QCanvas* canvas);

    void setSequence(QCanvasPixmapArray* seq);

    virtual ~QCanvasSprite();

    void move(double x, double y);
    virtual void move(double x, double y, int frame);
    void setFrame(int);
    enum FrameAnimationType { Cycle, Oscillate };
    virtual void setFrameAnimation(FrameAnimationType=Cycle, int step=1, int state=0);
    int frame() const
	{ return frm; }
    int frameCount() const
	{ return images->count(); }

    int rtti() const;
    static int RTTI;

    bool collidesWith( const QCanvasItem* ) const;

    QRect boundingRect() const;

    // is there a reason for these to be protected? Lars
//protected:

    int width() const;
    int height() const;

    int leftEdge() const;
    int topEdge() const;
    int rightEdge() const;
    int bottomEdge() const;

    int leftEdge(int nx) const;
    int topEdge(int ny) const;
    int rightEdge(int nx) const;
    int bottomEdge(int ny) const;
    QCanvasPixmap* image() const
	{ return images->image(frm); }
    virtual QCanvasPixmap* imageAdvanced() const;
    QCanvasPixmap* image(int f) const
	{ return images->image(f); }
    virtual void advance(int stage);

public:
    void draw(QPainter& painter);

private:
#if defined(Q_DISABLE_COPY)
    QCanvasSprite( const QCanvasSprite & );
    QCanvasSprite &operator=( const QCanvasSprite & );
#endif
    void addToChunks();
    void removeFromChunks();
    void changeChunks();

    int frm;
    ushort anim_val;
    uint anim_state:2;
    uint anim_type:14;
    bool collidesWith( const QCanvasSprite*,
		       const QCanvasPolygonalItem*,
		       const QCanvasRectangle*,
		       const QCanvasEllipse*,
		       const QCanvasText* ) const;

    friend bool qt_testCollision( const QCanvasSprite* s1,
				  const QCanvasSprite* s2 );

    QCanvasPixmapArray* images;
};

class QPolygonalProcessor;

class QM_EXPORT_CANVAS QCanvasPolygonalItem : public QCanvasItem
{
public:
    QCanvasPolygonalItem(QCanvas* canvas);
    virtual ~QCanvasPolygonalItem();

    bool collidesWith( const QCanvasItem* ) const;

    virtual void setPen(QPen p);
    virtual void setBrush(QBrush b);

    QPen pen() const
	{ return pn; }
    QBrush brush() const
	{ return br; }

    virtual QPointArray areaPoints() const=0;
    virtual QPointArray areaPointsAdvanced() const;
    QRect boundingRect() const;

    int rtti() const;
    static int RTTI;

protected:
    void draw(QPainter &);
    virtual void drawShape(QPainter &) = 0;

    bool winding() const;
    void setWinding(bool);

    void invalidate();
    bool isValid() const
	{ return (bool)val; }

private:
    void scanPolygon( const QPointArray& pa, int winding,
		      QPolygonalProcessor& process ) const;
    QPointArray chunks() const;

    bool collidesWith( const QCanvasSprite*,
		       const QCanvasPolygonalItem*,
		       const QCanvasRectangle*,
		       const QCanvasEllipse*,
		       const QCanvasText* ) const;

    QBrush br;
    QPen pn;
    uint wind:1;
};


class QM_EXPORT_CANVAS QCanvasRectangle : public QCanvasPolygonalItem
{
public:
    QCanvasRectangle(QCanvas* canvas);
    QCanvasRectangle(const QRect&, QCanvas* canvas);
    QCanvasRectangle(int x, int y, int width, int height, QCanvas* canvas);

    ~QCanvasRectangle();

    int width() const;
    int height() const;
    void setSize(int w, int h);
    QSize size() const
	{ return QSize(w,h); }
    QPointArray areaPoints() const;
    QRect rect() const
	{ return QRect(int(x()),int(y()),w,h); }

    bool collidesWith( const QCanvasItem* ) const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(QPainter &);
    QPointArray chunks() const;

private:
    bool collidesWith(   const QCanvasSprite*,
			 const QCanvasPolygonalItem*,
			 const QCanvasRectangle*,
			 const QCanvasEllipse*,
			 const QCanvasText* ) const;

    int w, h;
};


class QM_EXPORT_CANVAS QCanvasPolygon : public QCanvasPolygonalItem
{
public:
    QCanvasPolygon(QCanvas* canvas);
    ~QCanvasPolygon();
    void setPoints(QPointArray);
    QPointArray points() const;
    void moveBy(double dx, double dy);

    QPointArray areaPoints() const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(QPainter &);
    QPointArray poly;
};


class QM_EXPORT_CANVAS QCanvasSpline : public QCanvasPolygon
{
public:
    QCanvasSpline(QCanvas* canvas);
    ~QCanvasSpline();

    void setControlPoints(QPointArray, bool closed=TRUE);
    QPointArray controlPoints() const;
    bool closed() const;

    int rtti() const;
    static int RTTI;

private:
    void recalcPoly();
    QPointArray bez;
    bool cl;
};


class QM_EXPORT_CANVAS QCanvasLine : public QCanvasPolygonalItem
{
public:
    QCanvasLine(QCanvas* canvas);
    ~QCanvasLine();
    void setPoints(int x1, int y1, int x2, int y2);

    QPoint startPoint() const
	{ return QPoint(x1,y1); }
    QPoint endPoint() const
	{ return QPoint(x2,y2); }

    int rtti() const;
    static int RTTI;

    void setPen(QPen p);

protected:
    void drawShape(QPainter &);
    QPointArray areaPoints() const;

private:
    int x1,y1,x2,y2;
};


class QM_EXPORT_CANVAS QCanvasEllipse : public QCanvasPolygonalItem
{

public:
    QCanvasEllipse( QCanvas* canvas );
    QCanvasEllipse( int width, int height, QCanvas* canvas );
    QCanvasEllipse( int width, int height, int startangle, int angle,
		    QCanvas* canvas );

    ~QCanvasEllipse();

    int width() const;
    int height() const;
    void setSize(int w, int h);
    void setAngles(int start, int length);
    int angleStart() const
	{ return a1; }
    int angleLength() const
	{ return a2; }
    QPointArray areaPoints() const;

    bool collidesWith( const QCanvasItem* ) const;

    int rtti() const;
    static int RTTI;

protected:
    void drawShape(QPainter &);

private:
    bool collidesWith( const QCanvasSprite*,
		       const QCanvasPolygonalItem*,
		       const QCanvasRectangle*,
		       const QCanvasEllipse*,
		       const QCanvasText* ) const;
    int w, h;
    int a1, a2;
};


class QCanvasTextExtra;

class QM_EXPORT_CANVAS QCanvasText : public QCanvasItem
{
public:
    QCanvasText(QCanvas* canvas);
    QCanvasText(const QString&, QCanvas* canvas);
    QCanvasText(const QString&, QFont, QCanvas* canvas);

    virtual ~QCanvasText();

    void setText( const QString& );
    void setFont( const QFont& );
    void setColor( const QColor& );
    QString text() const;
    QFont font() const;
    QColor color() const;

    void moveBy(double dx, double dy);

    int textFlags() const
	{ return flags; }
    void setTextFlags(int);

    QRect boundingRect() const;

    bool collidesWith( const QCanvasItem* ) const;

    int rtti() const;
    static int RTTI;

protected:
    virtual void draw(QPainter&);

private:
#if defined(Q_DISABLE_COPY)
    QCanvasText( const QCanvasText & );
    QCanvasText &operator=( const QCanvasText & );
#endif
    void addToChunks();
    void removeFromChunks();
    void changeChunks();

    void setRect();
    QRect brect;
    QString txt;
    int flags;
    QFont fnt;
    QColor col;
    QCanvasTextExtra* extra;

    bool collidesWith(   const QCanvasSprite*,
			 const QCanvasPolygonalItem*,
			 const QCanvasRectangle*,
			 const QCanvasEllipse*,
			 const QCanvasText* ) const;
};

#define Q_DEFINED_QCANVAS
#include "qwinexport.h"
#endif // QT_NO_CANVAS

#endif // QCANVAS_H
