/***************************************************************************
                       kstviewmanager.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003-2006 The University of British Columbia
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
#include <assert.h>

// include files for Qt
#include <q3ptrstack.h>
#include <q3dragobject.h>
//Added by qt3to4:
#include <QDropEvent>
#include <QMouseEvent>

// include files for KDE
#include <k3listview.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

// application specific includes
#include "kst.h"
#include "kst2dplot.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kstdoc.h"
#include "kstobject.h"
#include "kstviewmanager.h"
#include "kstviewobjectimagedrag.h"
#include "kstviewwindow.h"
#include "plotmimesource.h"


#define RTTI_OBJ_WINDOW          4301
#define RTTI_OBJ_VIEW_OBJECT     4302
#define RTTI_OBJ_DATA_OBJECT     4303


KstViewListView::KstViewListView(QWidget *parent, const char *name) : Q3ListView(parent, name) {
}

KstViewListView::~KstViewListView() {
}

Q3DragObject* KstViewListView::dragObject() {
  Q3ListViewItem *qi = selectedItem();
  Q3DragObject* drag = 0L;

  if (qi) {
    KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);

    if (koi) {
      if (koi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
        //FIXME PORT!
        /*KMultipleDrag *multipleDrag = new KMultipleDrag(this);
        QStringList objects;
        KstViewObjectList vol;
        KstViewWindow *win;
        KstViewObjectPtr viewObject = koi->viewObject(&win);

        drag = multipleDrag;

        if (viewObject) {
          objects.append(viewObject->tagName());
          vol.append(viewObject);
          multipleDrag->addDragObject(new PlotMimeSource(win->caption(), objects, 0L));
          KstViewObjectImageDrag *imd = new KstViewObjectImageDrag(this);
          imd->setObjects(vol);
          multipleDrag->addDragObject(imd);
        }*/
      } else if(koi->rtti() == RTTI_OBJ_DATA_OBJECT) {
        Kst2DPlotPtr plot;
        KstDataObjectPtr dataObject = koi->dataObject(plot);
      
        if (dataObject) {
          Q3StoredDrag *storedDrag = new Q3StoredDrag("application/x-kst-curve-list", this);
          QStringList entries;

          drag = storedDrag;
          
          entries << dataObject->tagName();
          
          QByteArray data;
          QDataStream ds(&data, QIODevice::WriteOnly);
          ds << entries;
          storedDrag->setEncodedData(data);
        }
      }
    }
  }
  
  return drag;
}

void KstViewListView::contentsMouseMoveEvent(QMouseEvent *e) {
  viewport()->setCursor(Qt::ArrowCursor);
  
  Q3ListView::contentsMouseMoveEvent(e);
}

// ==============================================

KstViewObjectItem::KstViewObjectItem(Q3ListView *parent, KstTopLevelViewPtr x, KstViewManagerI *vm, int localUseCount)
: Q3ListViewItem(parent), _rtti(RTTI_OBJ_WINDOW), _name(x->tagName()), _vm(vm) {
  
  if (x) {
    _inUse = false;
    _removable = true;
    setText(0, x->name());
    setText(1, i18n("Window"));
    setDragEnabled(false);
    setDropEnabled(true);
    update(KstViewObjectPtr(x), true, localUseCount);
  }
}

KstViewObjectItem::KstViewObjectItem(Q3ListViewItem *parent, KstViewObjectPtr x, KstViewManagerI *vm, int localUseCount)
: Q3ListViewItem(parent), _rtti(RTTI_OBJ_VIEW_OBJECT), _name(x->tagName()), _vm(vm) {
  
  if (x) {
    _inUse = false;
    _removable = true;
    setText(0, x->tagName());
    setText(1, x->type());
    setDragEnabled(true);
    setDropEnabled(true);
    update(KstViewObjectPtr(x), true, localUseCount);
  }
}

KstViewObjectItem::KstViewObjectItem(Q3ListViewItem *parent, KstBaseCurvePtr x, KstViewManagerI *vm, int localUseCount)
: Q3ListViewItem(parent), _rtti(RTTI_OBJ_DATA_OBJECT), _name(x->tagName()), _vm(vm) {
  Q_UNUSED(localUseCount)
  
  if (x) {
    _inUse = false;
    _removable = true;
    setText(0, x->tagName());
    setText(1, x->type());
    setDragEnabled(true);
    setDropEnabled(false);
  }
}

KstViewObjectItem::~KstViewObjectItem() {
}

bool KstViewObjectItem::acceptDrop(const QMimeSource *mime) const {
  bool retVal = false;

  if (rtti() == RTTI_OBJ_WINDOW) {
    if (mime->provides(PlotMimeSource::mimeType())) {
      retVal = true;
    }
  } else if (rtti() == RTTI_OBJ_VIEW_OBJECT) {
    KstViewWindow *win;
    KstViewObjectPtr obj = viewObject(&win);
    
    if (mime->provides(PlotMimeSource::mimeType())) {
      if (obj && obj->isContainer()) {
        retVal = true;
      }
    } else {     
      if (obj) {
        if (mime->provides("application/x-kst-curve-list")) {
          if (dynamic_cast<Kst2DPlot*>(obj.data())) {
            retVal = true;
          } else if (dynamic_cast<KstViewLegend*>(obj.data())) {
            retVal = true;
          }
        }
      }
    }
  }
  
  //
  // the following should not be necessary but is due to a bug in Q3ListView...
  //
  if (retVal) {
    _vm->ViewView->viewport()->setCursor(Qt::ArrowCursor);
  } else {
    _vm->ViewView->viewport()->setCursor(Qt::ForbiddenCursor);  
  }
  
  return retVal;
}

void KstViewObjectItem::dropped(QDropEvent *e) {
  bool accepted = false;
  
  if (rtti() == RTTI_OBJ_WINDOW) {
    KstViewWindow *win;

    viewObject(&win);    
    if (win) {
      win->view()->widget()->dropped(e);
      accepted = true;
    }
  } else if (rtti() == RTTI_OBJ_VIEW_OBJECT) {
    KstViewWindow *win;
    KstViewObjectPtr obj = viewObject(&win);
    
    if (win) {
      if (e->provides(PlotMimeSource::mimeType())) {
        KstViewObjectList list;
        bool added = false;
        
        if (obj->paste(e, &list)) {
          for (KstViewObjectList::Iterator it = list.begin(); it != list.end(); ++it) {
            new KstViewObjectItem(this, *it, _vm);
            added = true;
          }
          if (added) {
            win->view()->paint(KstPainter::P_PAINT);
          }
          accepted = true;
        }
      } else if (obj && e->provides("application/x-kst-curve-list")) {
        Kst2DPlotPtr plot = dynamic_cast<Kst2DPlot*>(obj.data());
        KstViewLegendPtr legend = dynamic_cast<KstViewLegend*>(obj.data());
        if (plot || legend) {
          KstBaseCurveList curves = kstObjectSubList<KstDataObject, KstBaseCurve>(KST::dataObjectList);
          QByteArray data = e->encodedData("application/x-kst-curve-list");
          QDataStream ds(&data, QIODevice::ReadOnly);
          QStringList entries;
          bool added = false;
          
          ds >> entries;
          for (QStringList::ConstIterator i = entries.begin(); i != entries.end(); ++i) {
            KstBaseCurveList::Iterator it = curves.findTag(*i);
            if (it != curves.end()) {
              if (plot && plot->addCurve(*it)) {
                new KstViewObjectItem(this, *it, _vm);
                added = true;
              } else if (legend) {
                legend->addCurve(*it);
              }
            }
          }
          if (added) {          
            win->view()->paint(KstPainter::P_PLOT);
          }
          accepted = true;
        }
      }
    }
  }
   
  e->accept(accepted);
}

KstDataObjectPtr KstViewObjectItem::dataObject(Kst2DPlotPtr &plot) const {
  KstDataObjectPtr dataObj;
  
  plot = 0L;
  
  if (rtti() == RTTI_OBJ_VIEW_OBJECT) {
    KstViewWindow *win;
    KstViewObjectPtr viewObj = viewObject(&win);
    
    if (viewObj) {
      plot = dynamic_cast<Kst2DPlot*>(viewObj.data());
    }
  } else if (rtti() == RTTI_OBJ_DATA_OBJECT) {
    KstViewObjectItem *koi;
    Q3ListViewItem *item;
    
    dataObj = *KST::dataObjectList.findTag(tagName());      
    item = parent();
    if (item) {
      koi = static_cast<KstViewObjectItem*>(item);
      koi->dataObject(plot);
    }
  }
    
  return dataObj;
}

KstViewObjectPtr KstViewObjectItem::viewObject(KstViewWindow **win) const {
  KstViewObjectPtr viewObj;
  KstViewObjectItem *koi;
  Q3ListViewItem *item;
  
  *win = 0L;
  
  if (rtti() == RTTI_OBJ_WINDOW) {
    *win = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(tagName()));
    viewObj = (*win)->view()->findChild(tagName(), true);      
  } else {
    item = parent();
    if (item) {
      koi = static_cast<KstViewObjectItem*>(item);
      viewObj = koi->viewObject(win);
      if (win) {
        viewObj = (*win)->view()->findChild(tagName(), true);
      }
    }
  }
  
  return viewObj;
}

void KstViewObjectItem::update(KstViewObjectPtr x, bool recursive, int localUseCount) {
  Q_UNUSED(localUseCount)
  Q_UNUSED(recursive)
  
  Kst2DPlotPtr plot = dynamic_cast<Kst2DPlot*>(x.data());
  Q3PtrStack<Q3ListViewItem> trash;
  
  // garbage collect first
  for (Q3ListViewItem *i = firstChild(); i; i = i->nextSibling()) {
    bool found = false;
    KstViewObjectItem *oi = static_cast<KstViewObjectItem*>(i);
    if (oi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
      for (KstViewObjectList::ConstIterator obj = x->children().begin(); obj != x->children().end(); ++obj) {
        if ((*obj)->tagName() == oi->tagName()) {
          found = true;
          break;
        }
      }
    } else if (plot && oi->rtti() == RTTI_OBJ_DATA_OBJECT) {
      for (KstBaseCurveList::ConstIterator obj = plot->Curves.begin(); obj != plot->Curves.end(); ++obj) {
        if ((*obj)->tagName() == oi->tagName()) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      trash.push(i);
    }
  }
  
  trash.setAutoDelete(true);
  _vm->blockSignals(true);
  trash.clear();
  _vm->blockSignals(false);
  
  for (KstViewObjectList::ConstIterator obj = x->children().begin(); obj != x->children().end(); ++obj) {
    bool found = false;
    for (Q3ListViewItem *i = firstChild(); i; i = i->nextSibling()) {
      KstViewObjectItem *oi = static_cast<KstViewObjectItem*>(i);
      if (oi->tagName() == (*obj)->tagName()) {
        oi->update(*obj);
        found = true;
        break;
      }
    }
    if (!found) {
      new KstViewObjectItem(this, *obj, _vm);
    }
  } 
  
  if (plot) {
    for (KstBaseCurveList::ConstIterator obj = plot->Curves.begin(); obj != plot->Curves.end(); ++obj) {
      bool found = false;
      for (Q3ListViewItem *i = firstChild(); i; i = i->nextSibling()) {
        KstViewObjectItem *oi = static_cast<KstViewObjectItem*>(i);
        if (oi->tagName() == (*obj)->tagName()) {
          found = true;
          break;
        }
      }
      if (!found) {
        new KstViewObjectItem(this, *obj, _vm);
      }
    } 
  }
}

void KstViewObjectItem::updateButtons() {
  _vm->Edit->setEnabled(true);
  _vm->Delete->setEnabled(true);
}

void KstViewObjectItem::openChildren(bool open) {
  for (Q3ListViewItem *qi = firstChild(); qi; qi = qi->nextSibling()) {
    KstViewObjectItem *oi = static_cast<KstViewObjectItem*>(qi);
    
    oi->openChildren(open);
    qi->setOpen(open);
  }
}

// ==============================================

KstViewManagerI::KstViewManagerI(KstDoc *in_doc, QWidget* parent, Qt::WindowFlags fl)
: QWidget(parent, fl) {
  setupUi(this);

  doc = in_doc;
  delete ViewView;
  
  setAcceptDrops(TRUE);
  ViewView = new KstViewListView(this, "ViewView");
  ViewView->addColumn(i18n("Name"));
  ViewView->addColumn(i18n("Type"));
  ViewView->setAllColumnsShowFocus(TRUE);
  ViewView->setShowSortIndicator(TRUE);
  ViewView->setRootIsDecorated(TRUE);
  ViewView->setAcceptDrops(FALSE);
  ViewView->viewport()->setAcceptDrops(TRUE);
  ViewView->setSelectionMode(Q3ListView::Single);
  gridLayout->addMultiCellWidget(ViewView, 0, 0, 0, 3);
  
  connect(Close, SIGNAL(clicked()), this, SLOT(reject()));
  connect(Edit, SIGNAL(clicked()), this, SLOT(edit_I()));
  connect(Delete, SIGNAL(clicked()), this, SLOT(delete_I()));
  connect(ViewView, SIGNAL(doubleClicked(Q3ListViewItem *)), this, SLOT(edit_I()));
  connect(ViewView, SIGNAL(currentChanged(Q3ListViewItem *)), this, SLOT(currentChanged(Q3ListViewItem *)));
  connect(ViewView, SIGNAL(selectionChanged(Q3ListViewItem *)), this, SLOT(currentChanged(Q3ListViewItem *)));
  connect(ViewView, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), this, SLOT(contextMenu(Q3ListViewItem*, const QPoint&, int)));
}

KstViewManagerI::~KstViewManagerI() {
}

void KstViewManagerI::show_I() {
  show();
  raise();
  update();
}

void KstViewManagerI::updateContents() {
  update();
}

void KstViewManagerI::update() {
  KstApp *app = KstApp::inst();
    
  if (isHidden()) {
    return;
  }

  Q3ListViewItem *currentItem = ViewView->selectedItem();
  Q3PtrStack<Q3ListViewItem> trash;

  QList<QWidget*> childViews = app->childViews();
  QListIterator<KMdiChildView*> it(childViews);

  // garbage collect first
  for (Q3ListViewItem *i = ViewView->firstChild(); i; i = i->nextSibling()) {
    bool found = false;
    KstViewObjectItem *oi = static_cast<KstViewObjectItem*>(i);
    it.toFront();
    if (i->rtti() == RTTI_OBJ_WINDOW) {
      while (it.hasNext()) {
        KstViewWindow *win = dynamic_cast<KstViewWindow*>(it.next());
        if (win) {
          KstTopLevelViewPtr view = win->view();
          if (view) {
            if (view->tagName() == oi->tagName()) {
              found = true;
            }
          }
        }
        it.next();
      }
    }
    if (!found) {
      trash.push(i);
    }
  }

  trash.setAutoDelete(true);
  ViewView->blockSignals(true);
  trash.clear();
  ViewView->blockSignals(false);

  it.toFront();
  while (it.hasNext()) {
    KstViewWindow *win = dynamic_cast<KstViewWindow*>(it.next());
    if (win) {
      KstTopLevelViewPtr view = win->view();
      if (view) {
        bool found = false;

        for (Q3ListViewItem *i = ViewView->firstChild(); i; i = i->nextSibling()) {
          KstViewObjectItem *oi = static_cast<KstViewObjectItem*>(i);
          if (oi->rtti() == RTTI_OBJ_WINDOW && oi->tagName() == view->tagName()) {
            oi->update(KstViewObjectPtr(view));
            found = true;
            break;
          }
        }
        if (!found) {
          new KstViewObjectItem(ViewView, view, this);
        }
      }
    }
    it.next();
  }

  for (Q3ListViewItem *i = ViewView->firstChild(); i; i = i->nextSibling()) {
    if (i == currentItem) {
      ViewView->setCurrentItem(i);
      ViewView->setSelected(i, true);
      break;
    }
  }

  if (ViewView->selectedItem()) {
    static_cast<KstViewObjectItem*>(ViewView->currentItem())->updateButtons();
  } else {
    Edit->setEnabled(false);
    Delete->setEnabled(false);
  }
}

void KstViewManagerI::edit_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  KstViewWindow *win;
  
  if (koi) {  
    if (qi->rtti() == RTTI_OBJ_WINDOW) {
      win = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(koi->tagName()));
      if (win) {
        KstApp::inst()->renameWindow(win);
      }
    } else if (qi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
      KstViewObjectPtr obj = koi->viewObject(&win);
      if (obj && win) {
        obj->showDialog(win->view(), false);  
      }
    } else if (qi->rtti() == RTTI_OBJ_DATA_OBJECT) {
      Kst2DPlotPtr plot;
      KstDataObjectPtr obj = koi->dataObject(plot);
      if (obj) {
        obj->showDialog(false);  
      }
    }
  } else {
    KMessageBox::sorry(this, i18n("An item must be selected to edit."));
  }
}

void KstViewManagerI::delete_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  
  if (koi) {
    if (koi->removable()) {
      if (koi->rtti() == RTTI_OBJ_WINDOW) {
        KstViewWindow *win = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(koi->tagName()));
        if (win) {
          win->close(true);
          update();
        }
      } else if (koi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
        KstViewWindow *win;
        KstViewObjectPtr obj = koi->viewObject(&win);
        if (obj && win) {
          win->view()->removeChild(obj, true);
          win->view()->paint(KstPainter::P_PAINT);
          update();
        }
      } else if (koi->rtti() == RTTI_OBJ_DATA_OBJECT) {
        KstViewWindow *win;
        Kst2DPlotPtr plot;
        KstDataObjectPtr obj = koi->dataObject(plot);
        KstBaseCurvePtr curve = dynamic_cast<KstBaseCurve*>(obj.data());
        koi->viewObject(&win);
        if (curve && plot && win) {
          plot->removeCurve(curve);          
          win->view()->paint(KstPainter::P_PAINT);
          update();
        }        
      }
    }
  } else {
    KMessageBox::sorry(this, i18n("An item must be selected to delete."));
  }
}

void KstViewManagerI::activate_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  
  if (koi) {
    if (koi->rtti() == RTTI_OBJ_WINDOW) {
      KstViewWindow *win = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(koi->tagName()));
      if (win) {
        win->setVisible(true);
      }
    }
  }
}

void KstViewManagerI::cleanupDefault_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  
  if (koi) {
    if (koi->rtti() == RTTI_OBJ_WINDOW) {
      KstViewWindow *win = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(koi->tagName()));
      if (win) {
        win->view()->cleanupDefault();
      }      
    }
  }  
}

void KstViewManagerI::cleanupCustom_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  
  if (koi) {
    if (koi->rtti() == RTTI_OBJ_WINDOW) {
      KstViewWindow *win = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(koi->tagName()));
      if (win) {
        win->view()->cleanupCustom();
      }      
    }
  }  
}

void KstViewManagerI::select_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  
  if (koi) {
    if (koi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
      KstViewWindow *win;
      KstViewObjectPtr obj = koi->viewObject(&win);
      if (obj && win) {
        win->setVisible(true);
        obj->setSelected(true);
        win->view()->paint(KstPainter::P_PAINT);
        update();
      }
    }
  }
}

void KstViewManagerI::deselect_I() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
  
  if (koi) {
    if (koi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
      KstViewWindow *win;
      KstViewObjectPtr obj = koi->viewObject(&win);
      if (obj && win) {
        win->setVisible(true);
        obj->setSelected(false);
        win->view()->paint(KstPainter::P_PAINT);
        update();
      }
    }
  }
}

void KstViewManagerI::open() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  
  if (qi) {
    qi->setOpen(true);
  }
}

void KstViewManagerI::close() {
  Q3ListViewItem *qi = ViewView->selectedItem();
  
  if (qi) {
    qi->setOpen(false);
  }
}

void KstViewManagerI::open(bool open) {
  Q3ListViewItem *qi = ViewView->selectedItem();
  KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(qi);
    
  if (koi) {
    koi->openChildren(open);
    qi->setOpen(open);
  } else {
    for (qi = ViewView->firstChild(); qi; qi = qi->nextSibling()) {
      koi = static_cast<KstViewObjectItem*>(qi);

      koi->openChildren(open);
      qi->setOpen(open);
    }
  }
}

void KstViewManagerI::openAll() {
  open(true);
}

void KstViewManagerI::closeAll() {
  open(false);
}

void KstViewManagerI::contextMenu(Q3ListViewItem *i, const QPoint& p, int col) {
  Q_UNUSED(col)

  if (i) {
    KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(i);
    KMenu *menu = new KMenu(koi->text(0), this);
    int id;

    if (koi->rtti() == RTTI_OBJ_WINDOW) {
      id = menu->insertItem(i18n("&Rename..."), this, SLOT(edit_I()));
      id = menu->insertItem(i18n("&Close"), this, SLOT(delete_I()));
      id = menu->insertItem(i18n("&Activate"), this, SLOT(activate_I()));

      KMenu *submenu = new KMenu(menu);
      if (submenu) {
        menu->insertSeparator();
        submenu->insertItem("Default Tile", this, SLOT(cleanupDefault_I()));
        submenu->insertItem("Custom...", this, SLOT(cleanupCustom_I()));
        id = menu->insertItem(i18n("Cleanup Layout"), submenu);
      }
    } else if (koi->rtti() == RTTI_OBJ_VIEW_OBJECT) {
      id = menu->insertItem(i18n("&Edit..."), this, SLOT(edit_I()));
      id = menu->insertItem(i18n("&Delete"), this, SLOT(delete_I()));
      id = menu->insertItem(i18n("&Select"), this, SLOT(select_I()));
      menu->setItemEnabled(id, KstApp::inst()->currentViewMode() == KstTopLevelView::LayoutMode);
      id = menu->insertItem(i18n("Dese&lect"), this, SLOT(deselect_I()));
      menu->setItemEnabled(id, KstApp::inst()->currentViewMode() == KstTopLevelView::LayoutMode);
    } else if (koi->rtti() == RTTI_OBJ_DATA_OBJECT) {
      id = menu->insertItem(i18n("&Edit..."), this, SLOT(edit_I()));
      id = menu->insertItem(i18n("&Remove"), this, SLOT(delete_I()));
    }

    menu->insertSeparator();
    id = menu->insertItem(i18n("Expand"), this, SLOT(open()));
    menu->setItemEnabled(id, !koi->isOpen() && koi->firstChild());
    id = menu->insertItem(i18n("Collapse"), this, SLOT(close()));
    menu->setItemEnabled(id, koi->isOpen() && koi->firstChild());
    id = menu->insertItem(i18n("Expand All"), this, SLOT(openAll()));
    menu->setItemEnabled(id, koi->firstChild());
    id = menu->insertItem(i18n("Collapse All"), this, SLOT(closeAll()));
    menu->setItemEnabled(id, koi->firstChild());

    menu->popup(p);
  } else {
    KMenu *menu = new KMenu(this);
    int id;

    id = menu->insertItem(i18n("Expand All"), this, SLOT(openAll()));
    menu->setItemEnabled(id, ViewView->firstChild());
    id = menu->insertItem(i18n("Collapse All"), this, SLOT(closeAll()));
    menu->setItemEnabled(id, ViewView->firstChild());

    menu->popup(p);
  }
}

void KstViewManagerI::doUpdates() {
  emit docChanged();
}

void KstViewManagerI::currentChanged(Q3ListViewItem *i) {
  if (i) {
    KstViewObjectItem *koi = static_cast<KstViewObjectItem*>(i);
    koi->updateButtons();
  }
}

#include "kstviewmanager.moc"
// vim: ts=2 sw=2 et
