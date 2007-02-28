//Added by qt3to4:
#include <QDropEvent>
#include <QMouseEvent>
/***************************************************************************
                       kstdatamanger.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of British Columbia
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
#ifndef KSTVIEWMANAGERI_H
#define KSTVIEWMANAGERI_H

class KstDoc;

#include "kstviewmanager.h"
#include "kst2dplot.h"
#include "kstplotgroup.h"
#include "kstviewobject.h"
#include "kstviewwindow.h"

class KstViewListView : public Q3ListView {
  public:
    KstViewListView(QWidget * parent = 0, const char * name = 0);
   ~KstViewListView();
   
  protected:
    virtual Q3DragObject* dragObject();
    virtual void contentsMouseMoveEvent(QMouseEvent *e);
};

class KstViewObjectItem : public Q3ListViewItem {
  public:
    KstViewObjectItem(Q3ListView *parent, KstTopLevelViewPtr x, KstViewManagerI *dm, int localUseCount = 0);
    KstViewObjectItem(Q3ListViewItem *parent, KstViewObjectPtr x, KstViewManagerI *dm, int localUseCount = 0);
    KstViewObjectItem(Q3ListViewItem *parent, KstBaseCurvePtr x, KstViewManagerI *dm, int localUseCount = 0);
    virtual ~KstViewObjectItem();

    virtual bool acceptDrop(const QMimeSource *mime) const;
    virtual void dropped(QDropEvent *e);
    virtual int rtti() const { return _rtti; }

    void update(KstViewObjectPtr x, bool recursive = true, int localUseCount = 0);
    const QString& tagName() const { return _name; }
    KstDataObjectPtr dataObject(Kst2DPlotPtr &plot) const;
    KstViewObjectPtr viewObject(KstViewWindow **win) const;
    bool removable() const { return _removable; }
    void updateButtons();
    void openChildren(bool open);
         
  private:
    int _rtti;
    QString _name;
    KstViewManagerI *_vm;
    bool _removable;
    bool _inUse;
};

class KstViewManagerI: public KstViewManager {
  Q_OBJECT
  public:
    KstViewManagerI(KstDoc *doc, QWidget* parent = 0, const char *name = 0, 
                    bool modal = false, Qt::WFlags fl = 0);
    virtual ~KstViewManagerI();
        
  public slots:
    void update();
    void updateContents();
    void show_I();
    void edit_I();
    void delete_I();
    void activate_I();
    void cleanupDefault_I();
    void cleanupCustom_I();
    void select_I();
    void deselect_I();
    void open();
    void close();
    void openAll();
    void closeAll();   
  
  private slots:
    void doUpdates();
    void contextMenu(Q3ListViewItem *i, const QPoint& p, int c);
    void currentChanged(Q3ListViewItem *);
  
  private:
    void open(bool open);  

    KstDoc *doc;

  protected:

  signals:
    void docChanged();
};

#endif
// vim: ts=2 sw=2 et
