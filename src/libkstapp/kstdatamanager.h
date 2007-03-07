/***************************************************************************
                       kstdatamanger.h  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003 C. Barth Netterfield
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
#ifndef KSTDATAMANAGERI_H
#define KSTDATAMANAGERI_H

#include <QDialog>

#include <qaction.h>
//Added by qt3to4:
#include <QPixmap>

class KstDoc;
class Q3ToolBar;
class K3ListViewSearchLineWidget;

#include "ui_kstdatamanager4.h"

#include "kstrvector.h"
#include "kstsvector.h"
#include "kstrmatrix.h"
#include "kstsmatrix.h"

class KstDataAction : public QAction {
  Q_OBJECT
  public:
    KstDataAction(const QString &menuText, const QKeySequence &accel, QObject *parent, const char *name = 0)
      : QAction(menuText, parent) { setObjectName(name); setShortcut(accel); }

  protected:
    void addedTo(QWidget *actionWidget, QWidget *container);
};

class KstDataManagerI : public QDialog, public Ui::KstDataManager {
  Q_OBJECT
  public:
    KstDataManagerI(KstDoc *doc, QWidget* parent = 0, Qt::WindowFlags fl = 0);
    virtual ~KstDataManagerI();

    const QPixmap& yesPixmap() const;

  public slots:
    void update();
    void updateContents();
    void show_I();
    void edit_I();
    void delete_I();

  private slots:
    void doUpdates();
    void contextMenu(Q3ListViewItem *i, const QPoint& p, int c);
    void currentChanged(Q3ListViewItem *);
    void selectionChanged();
    void doubleClicked(Q3ListViewItem *);
    void showOldPlugin();

  private:
    void createObjectAction(const QString &txt, Q3ToolBar *bar,
                            QObject *receiver = 0, const char *slot = 0);
    void setupPluginActions();

  private:
    KstDoc *doc;
    Q3ToolBar *_primitive;
    Q3ToolBar *_data;
    Q3ToolBar *_fits;
    Q3ToolBar *_filters;
    K3ListViewSearchLineWidget *_searchWidget;

  protected:
    QPixmap _yesPixmap;

  signals:
    void editDataVector(const QString &);
    void editStaticVector(const QString &);
    void editDataMatrix(const QString &);
    void editStaticMatrix(const QString &);
    void docChanged();
};

class KstObjectItem : public QObject, public K3ListViewItem {
  Q_OBJECT
  public:
    KstObjectItem(Q3ListView *parent, KstRVectorPtr x, KstDataManagerI *dm, int localUseCount = 0);
    KstObjectItem(Q3ListView *parent, KstSVectorPtr x, KstDataManagerI *dm, int localUseCount = 0);
    KstObjectItem(K3ListViewItem *parent, KstVectorPtr x, KstDataManagerI *dm, int localUseCount = 0);
    KstObjectItem(Q3ListView *parent, KstDataObjectPtr x, KstDataManagerI *dm, int localUseCount = 0);
    KstObjectItem(Q3ListView *parent, KstRMatrixPtr x, KstDataManagerI *dm, int localUseCount = 0);
    KstObjectItem(Q3ListView *parent, KstSMatrixPtr x, KstDataManagerI *dm, int localUseCount = 0);
    KstObjectItem(K3ListViewItem *parent, KstMatrixPtr x, KstDataManagerI *dm, int localUseCount = 0);
    virtual ~KstObjectItem();

    void update(bool recursive = true, int localUseCount = 0);
    virtual int rtti() const { return _rtti; }

    const KstObjectTag& tag() const { return _tag; }
    KstDataObjectPtr dataObject();
    bool removable() const { return _removable; }
    void updateButtons();

  public slots:
    void addToPlot(int);
    void activateHint(int);
    void removeFromPlot(int);
    void makeCurve();
    void makeCSD();
    void makeHistogram();
    void makePSD();
    void makeImage();
    void reload();
    void showMetadata();

  signals:
    void updated();

  private:
    int _rtti;
    KstObjectTag _tag;
    KstDataManagerI *_dm;
    bool _removable;
    bool _inUse;
    void paintPlot(Kst2DPlotPtr p); //used by add to/remove from plot slots
};

#endif
// vim: ts=2 sw=2 et
