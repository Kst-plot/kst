/***************************************************************************
                      kstiface_impl.cpp  -  Part of KST
                             -------------------
    begin                : Thu Jun 19 2003
    copyright            : (C) 2003 The University of Toronto
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
#include <qfileinfo.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3ValueList>

// include files for KDE

// application specific includes
#include "kst.h"
#include "kst2dplot.h"
#include "kstcolorsequence.h"
#include "kstdataobjectcollection.h"
#include "kstdoc.h"
#include "kstequation.h"
#include "ksteventmonitorentry.h"
#include "ksthistogram.h"
#include "kstiface_impl.h"
#include "kstimage.h"
#include "kstcplugin.h"
#include "kstpsd.h"
#include "psdcalculator.h"
#include "kstrmatrix.h"
#include "kstrvector.h"
#include "kstsmatrix.h"
#include "kstsvector.h"
#include "kstvcurve.h"
#include "kstviewwindow.h"
#include "plugincollection.h"

#include "kmessagebox.h"

KstIfaceImpl::KstIfaceImpl(KstDoc *doc, KstApp *app)
: DCOPObject("KstIface"), _doc(doc), _app(app) {
  assert(doc);
}


KstIfaceImpl::~KstIfaceImpl() {
}


void KstIfaceImpl::showDataManager() {
  _app->showDataManager();
}


QStringList KstIfaceImpl::stringList() {
  KstReadLocker ml(&KST::stringList.lock());
  return KST::stringList.tagNames();
}


QStringList KstIfaceImpl::scalarList() {
  KstReadLocker ml(&KST::scalarList.lock());
  return KST::scalarList.tagNames();
}


QStringList KstIfaceImpl::vectorList() {
  KstReadLocker ml(&KST::vectorList.lock());
  return KST::vectorList.tagNames();
}


QStringList KstIfaceImpl::objectList() {
  KstReadLocker ml(&KST::dataObjectList.lock());
  return KST::dataObjectList.tagNames();
}


QStringList KstIfaceImpl::curveList() {
  QStringList rc;

  KstBaseCurveList bcl = kstObjectSubList<KstDataObject,KstBaseCurve>(KST::dataObjectList);

  for (KstBaseCurveList::Iterator it = bcl.begin(); it != bcl.end(); ++it) {
    (*it)->readLock();
    rc += (*it)->tagName();
    (*it)->unlock();
  }

  return rc;
}


QString KstIfaceImpl::activeWindow() {
  KstApp *app = KstApp::inst();
  KMdiChildView *view = app->activeWindow();
  if (view) {
    return view->caption();
  }
  return QString::null;
}


QStringList KstIfaceImpl::windowList() {
  QStringList rc;
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  while (it->currentItem()) {
    rc += it->currentItem()->caption();
    it->next();
  }
  app->deleteIterator(it);
  return rc;
}


QStringList KstIfaceImpl::plotList() {
  QStringList rc;
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  while (it->currentItem()) {
    rc += plotList(it->currentItem()->caption());
    it->next();
  }
  app->deleteIterator(it);
  return rc;
}


QStringList KstIfaceImpl::plotList(const QString& window) {
  QStringList rc;
  KstApp *app = KstApp::inst();
  KMdiChildView *c = app->findWindow(window);
  KstViewWindow *v = dynamic_cast<KstViewWindow*>(c);
  if (v) {
    Kst2DPlotList l = v->view()->findChildrenType<Kst2DPlot>(false);
    for (Kst2DPlotList::Iterator i = l.begin(); i != l.end(); ++i) {
      rc += (*i)->tagName();
    }
  }
  return rc;
}


QStringList KstIfaceImpl::pluginList() {
  QStringList rc;
  PluginCollection *pc = PluginCollection::self();
  const QMap<QString,Plugin::Data>& pluginList = pc->pluginList();
  QMap<QString,Plugin::Data>::ConstIterator it;

  for (it = pluginList.begin(); it != pluginList.end(); ++it) {
    if (it.data()._isFilter == false) {
      rc += it.data()._name;
    }
  }

  return rc;
}


QStringList KstIfaceImpl::filterList() {
  QStringList rc;
  PluginCollection *pc = PluginCollection::self();
  const QMap<QString,Plugin::Data>& pluginList = pc->pluginList();
  QMap<QString,Plugin::Data>::ConstIterator it;

  for (it = pluginList.begin(); it != pluginList.end(); ++it) {
    if (it.data()._isFilter) {
      rc += it.data()._name;
    }
  }

  return rc;
}


bool KstIfaceImpl::plotEquation(double start, double end, int numSamples, const QString& equation, const QString& plotName, const QColor& color) {
  Kst2DPlotPtr plot;
  QString ptag;
  KstApp *app = KstApp::inst();

  if (equation.isEmpty()) {
    return false;
  }

  QString etag = KST::suggestEQName(QString(equation).replace(QRegExp("[\\[\\]\\s]"), "_"));
  ptag = "P-" + plotName;

  if (!plotName.isEmpty()) {
    //find the plot, or P-plotName
    KMdiIterator<KMdiChildView*> *iter = app->createIterator();
    bool found = false;
    while ((iter->currentItem()) && !found) {
      KMdiChildView *childview = iter->currentItem();
      KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
      if (viewwindow && !found) {
        Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
        Kst2DPlotList::Iterator plot_iter = plotlist.findTag(plotName);
        if (plot_iter != plotlist.end()) {
          plot = *plot_iter;
          found = true;
        } else {
          Kst2DPlotList::Iterator plot_iter = plotlist.findTag(ptag);
          if (plot_iter != plotlist.end()) {
            plot = *plot_iter;
            found = true;
          }
        }
      }
      iter->next();
    }
    app->deleteIterator(iter);
  }

  //if the plot does not exist, create it
  if (!plot) {
    //put the plot in the active window
    KMdiChildView *activewin = app->activeWindow();
    if (!activewin) {
      QString windowname = app->newWindow("W1");
      activewin = app->findWindow(windowname);
    }
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(activewin);
    if (viewwindow) {
      KstTopLevelViewPtr pTLV = viewwindow->view();
      plot = pTLV->createObject<Kst2DPlot>(ptag);
    }
  }

  KstEquationPtr eq = new KstEquation(etag, equation, start, end, numSamples);

  if (!eq->isValid()) {
    return false;
  }
  
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(plot->Curves);

  KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(eq->tag(), true), eq->vX(), eq->vY(), 0L, 0L, 0L, 0L, color.isValid() ? color : KstColorSequence::next(vcurves,plot->backgroundColor()));
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(eq));
  KST::dataObjectList.append(KstDataObjectPtr(vc));
  KST::dataObjectList.lock().unlock();

  plot->addCurve(KstBaseCurvePtr(vc));

  _doc->forceUpdate();
  _doc->setModified();

  return true;
}


bool KstIfaceImpl::plotEquation(double start, double end, int numSamples, const QString& equation, const QString& plotName) {
  return plotEquation(start, end, numSamples, equation, plotName, QColor());
}


bool KstIfaceImpl::plotEquation(const QString& xvector, const QString& equation, const QString& plotName) {
  return plotEquation(xvector, equation, plotName, QColor());
}


bool KstIfaceImpl::plotEquation(const QString& xvector, const QString& equation, const QString& plotName, const QColor& color) {
  KstVectorPtr v;
  Kst2DPlotPtr plot;
  QString etag, ptag;
  KST::vectorList.lock().readLock();
  KstVectorList::Iterator it = KST::vectorList.findTag(xvector);
  KST::vectorList.lock().unlock();
  KstApp *app = KstApp::inst();

  if (equation.isEmpty() || it == KST::vectorList.end()) {
    return false;
  }

  v = *it;

  etag = KST::suggestEQName(QString(equation).replace(QRegExp("[\\[\\]\\s]"), "_"));
  ptag = "P-" + plotName;

  if (!plotName.isEmpty()) {
    //find the plot, or P-plotName
    KMdiIterator<KMdiChildView*> *iter = app->createIterator();
    bool found = false;
    while (iter->currentItem() && !found) {
      KMdiChildView *childview = iter->currentItem();
      KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
      if (viewwindow && !found) {
        Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
        Kst2DPlotList::Iterator plot_iter = plotlist.findTag(plotName);
        if (plot_iter != plotlist.end()) {
          plot = *plot_iter;
          found = true;
        }
        else {
          Kst2DPlotList::Iterator plot_iter = plotlist.findTag(ptag);
          if (plot_iter != plotlist.end()) {
            plot = *plot_iter;
            found = true;
          }
        }
      }
      iter->next();
    }
    app->deleteIterator(iter);
  }

  //if the plot does not exist, create it
  if (!plot) {
    //put the plot in the active window
    KMdiChildView *activewin = app->activeWindow();
    if (!activewin) {
      QString windowname = app->newWindow("W1");
      activewin = app->findWindow(windowname);
    }
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(activewin);
    if (viewwindow) {
      KstTopLevelViewPtr pTLV = viewwindow->view();
      plot = pTLV->createObject<Kst2DPlot>(ptag);
    }
  }


  KstEquationPtr eq = new KstEquation(etag, equation, v, true);

  if (!eq->isValid()) {
    return false;
  }

  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(plot->Curves);
  
  KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(eq->tag(), true), eq->vX(), eq->vY(), 0L, 0L, 0L, 0L, color.isValid() ? color : KstColorSequence::next(vcurves,plot->backgroundColor()));
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(eq));
  KST::dataObjectList.append(KstDataObjectPtr(vc));
  KST::dataObjectList.lock().unlock();

  plot->addCurve(KstBaseCurvePtr(vc));

  _doc->forceUpdate();
  _doc->setModified();

  return true;
}

QString KstIfaceImpl::generateScalar(const QString& name, double value) {
  KstScalarPtr s = new KstScalar(KstObjectTag(name, KstObjectTag::globalTagContext), 0L, value); // FIXME: do tag context properly
  KstReadLocker rl(s);
  s->setOrphan(true);
  s->setEditable(true);
  return s->tagName();
}


QString KstIfaceImpl::generateVector(const QString& name, double from, double to, int points) {
  KstVectorPtr v = new KstSVector(from, to, points, KstObjectTag(name, KstObjectTag::globalTagContext)); // FIXME: do tag context properly
  KstReadLocker rl(v);
  return v->tagName();
}


bool KstIfaceImpl::saveVector(const QString& vector, const QString& filename) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator it = KST::vectorList.findTag(vector);

  if (it == KST::vectorList.end() || filename.isEmpty()) {
    return false;
  }

  QFile f(filename);
  if (!f.open(QIODevice::WriteOnly)) {
    return false;
  }

  return 0 == KstData::self()->vectorToFile(*it, &f);
}


QStringList KstIfaceImpl::inputVectors(const QString& objectName) {
  KstReadLocker ml(&KST::dataObjectList.lock());
  KstDataObjectList::Iterator oi = KST::dataObjectList.findTag(objectName);
  QStringList rc;

  if (oi != KST::dataObjectList.end()) {
    (*oi)->readLock();
    rc = (*oi)->inputVectors().tagNames();
    (*oi)->unlock();
  }

  return rc;
}


QStringList KstIfaceImpl::inputScalars(const QString& objectName) {
  KstReadLocker ml(&KST::dataObjectList.lock());
  KstDataObjectList::Iterator oi = KST::dataObjectList.findTag(objectName);
  QStringList rc;

  if (oi != KST::dataObjectList.end()) {
    (*oi)->readLock();
    rc = (*oi)->inputScalars().tagNames();
    (*oi)->unlock();
  }

  return rc;
}


QStringList KstIfaceImpl::outputVectors(const QString& objectName) {
  KstReadLocker ml(&KST::dataObjectList.lock());
  KstDataObjectList::Iterator oi = KST::dataObjectList.findTag(objectName);
  QStringList rc;

  if (oi != KST::dataObjectList.end()) {
    (*oi)->readLock();
    rc = (*oi)->outputVectors().tagNames();
    (*oi)->unlock();
  }

  return rc;
}


QStringList KstIfaceImpl::outputScalars(const QString& objectName) {
  KstReadLocker ml(&KST::dataObjectList.lock());
  KstDataObjectList::Iterator oi = KST::dataObjectList.findTag(objectName);
  QStringList rc;

  if (oi != KST::dataObjectList.end()) {
    (*oi)->readLock();
    rc = (*oi)->outputScalars().tagNames();
    (*oi)->unlock();
  }

  return rc;
}


double KstIfaceImpl::scalar(const QString& name) {
  KstReadLocker ml(&KST::scalarList.lock());
  KstScalarList::Iterator it = KST::scalarList.findTag(name);

  if (it == KST::scalarList.end()) {
    return 0.0;
  }

  KstReadLocker l2(*it);

  return (*it)->value();
}


bool KstIfaceImpl::setScalar(const QString& name, double value) {
  KstReadLocker ml(&KST::scalarList.lock());
  KstScalarList::Iterator it = KST::scalarList.findTag(name);

  if (it == KST::scalarList.end() || !(*it)->editable()) {
    return false;
  }

  KstWriteLocker l2(*it);
  *(*it) = value;
  return true;
}


int KstIfaceImpl::vectorSize(const QString& name) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator it = KST::vectorList.findTag(name);
  int rc = 0;

  if (it != KST::vectorList.end()) {
    (*it)->readLock();
    rc = (*it)->length();
    (*it)->unlock();
  }

  return rc;
}


bool KstIfaceImpl::setString(const QString& name, const QString& value) {
  KstReadLocker ml(&KST::stringList.lock());
  KstStringList::Iterator it = KST::stringList.findTag(name);

  if (it == KST::stringList.end()) {
    return false;
  }

  KstWriteLocker l2(*it);
  *(*it) = value;
  return true;
}


const QString& KstIfaceImpl::string(const QString& name) {
  KstReadLocker ml(&KST::stringList.lock());
  KstStringList::Iterator it = KST::stringList.findTag(name);

  if (it != KST::stringList.end()) {
    KstReadLocker ml2(*it);
    return (*it)->value();
  }

  return QString::null;
}


double KstIfaceImpl::vector(const QString& name, int index) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator it = KST::vectorList.findTag(name);
  double rc = 0.0;

  if (it != KST::vectorList.end() && index >= 0) {
    (*it)->readLock();
    if (index < (*it)->length()) {
      rc = (*it)->value(index);
    }
    (*it)->unlock();
  }

  return rc;
}


bool KstIfaceImpl::setVector(const QString& name, int index, double value) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator it = KST::vectorList.findTag(name);

  if (it != KST::vectorList.end() && index >= 0) {
    (*it)->writeLock();
    if (index < (*it)->length()) {
      (*it)->value()[index] = value;
      (*it)->unlock();
      return true;
    }
    (*it)->unlock();
  }

  return false;
}


bool KstIfaceImpl::resizeVector(const QString& name, int newSize) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator it = KST::vectorList.findTag(name);

  if (it != KST::vectorList.end()) {
    (*it)->writeLock();
    bool rc = (*it)->resize(newSize);
    if (rc) {
      rc = (*it)->length() == newSize;
    }
    (*it)->unlock();
    return rc;
  }

  return false;
}


bool KstIfaceImpl::clearVector(const QString& name) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator it = KST::vectorList.findTag(name);

  if (it != KST::vectorList.end()) {
    (*it)->writeLock();
    (*it)->zero();
    (*it)->unlock();
    return true;
  }

  return false;
}


QString KstIfaceImpl::createWindow(const QString& name) {
  return KstApp::inst()->newWindow(name);
}


QString KstIfaceImpl::createPlot(const QString& name) {
  QString window = activeWindow();

  //if no window exists, create one
  if (window.isEmpty()) {
    window = KstApp::inst()->newWindow(false);
  }

  return createPlot(window, name);
}


QString KstIfaceImpl::createPlot(const QString& window, const QString& name) {
  QString plotName;

  KstViewWindow *pView = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(window));
  if (pView) {
    plotName = pView->createObject<Kst2DPlot>(name, false);
    _doc->forceUpdate();
    _doc->setModified();
  }

  return plotName;
}


bool KstIfaceImpl::deletePlot(const QString& name) {
   QString window = activeWindow();
   if (!window.isEmpty()) {
     return deletePlot(window, name);
   }

   return false;
}


bool KstIfaceImpl::deletePlot(const QString& window, const QString& name) {
  KstViewWindow *pView = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(window));
  if (pView) {
    KstTopLevelViewPtr pTLV = pView->view();
    KstViewObjectList objects = pTLV->findChildrenType<KstViewObject>(true);

    for (KstViewObjectList::Iterator it = objects.begin(); it != objects.end(); ++it) {
      KstViewObjectPtr object = *it;
      if (object->tagName() == name) {
        pTLV->removeChild(object, true);
        _doc->forceUpdate();
        _doc->setModified();
        return true;
      }
    }
  }

  return false;
}


QStringList KstIfaceImpl::plotContents(const QString& name) {
  //iterate through the windows until plot is found
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter = app->createIterator();
  while (iter->currentItem()) {
    KMdiChildView *childview = iter->currentItem();
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
    if (viewwindow) {
      Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
      Kst2DPlotList::Iterator plot_iter=plotlist.findTag(name);
      if (plot_iter != plotlist.end()) {
        app->deleteIterator(iter);
        return (*plot_iter)->Curves.tagNames();
      }
    }
    iter->next();
  }
  app->deleteIterator(iter);
  return QStringList();
}


bool KstIfaceImpl::addCurveToPlot(KMdiChildView *win, const QString& plot, const QString& curve) {
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(win);

  if (w) {
    KstTopLevelViewPtr view = kst_cast<KstTopLevelView>(w->view());
    if (view) {
      Kst2DPlotList plots = view->findChildrenType<Kst2DPlot>(true);
      if (plots.findTag(plot) != plots.end()) {
        KstBaseCurveList bcl = kstObjectSubList<KstDataObject,KstBaseCurve>(KST::dataObjectList);
        KstBaseCurveList::Iterator ci = bcl.findTag(curve);
        Kst2DPlotPtr p = *(plots.findTag(plot));
        if (p && ci != bcl.end()) {
          p->addCurve(*ci);
          _doc->forceUpdate();
          return true;
        }
      }
    }
  }

  return false;
}


bool KstIfaceImpl::addCurveToPlot(const QString& window, const QString& plot, const QString& curve) {
  return addCurveToPlot(KstApp::inst()->findWindow(window), plot, curve);
}


bool KstIfaceImpl::addCurveToPlot(const QString& plot, const QString& curve) {
  return addCurveToPlot(KstApp::inst()->activeWindow(), plot, curve);
}


bool KstIfaceImpl::removeCurveFromPlot(KMdiChildView *win, const QString& plot, const QString& curve) {
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(win);

  if (w) {
    KstTopLevelViewPtr view = kst_cast<KstTopLevelView>(w->view());
    if (view) {
      Kst2DPlotList plots = view->findChildrenType<Kst2DPlot>(true);
      if (plots.findTag(plot) != plots.end()) {
        Kst2DPlotPtr p = *(plots.findTag(plot));
        KstBaseCurveList bcl = kstObjectSubList<KstDataObject,KstBaseCurve>(KST::dataObjectList);
        KstBaseCurveList::Iterator ci = bcl.findTag(curve);
        if (p && ci != bcl.end()) {
          p->removeCurve(*ci);
          _doc->forceUpdate();
          return true;
        }
      }
    }
  }

  return false;
}

bool KstIfaceImpl::removeCurveFromPlot(const QString& window, const QString& plot, const QString& curve) {
  return removeCurveFromPlot(KstApp::inst()->findWindow(window), plot, curve);
}


bool KstIfaceImpl::removeCurveFromPlot(const QString& plot, const QString& curve) {
  return removeCurveFromPlot(KstApp::inst()->activeWindow(), plot, curve);
}


QString KstIfaceImpl::createCurve(const QString& name, const QString& xVector, const QString& yVector, const QString& xErrorVector, const QString& yErrorVector) {
  return createCurve(name, xVector, yVector, xErrorVector, yErrorVector, KstColorSequence::next());
}


QString KstIfaceImpl::createCurve(const QString& name, const QString& xVector, const QString& yVector, const QString& xErrorVector, const QString& yErrorVector, const QColor& color) {
  QString n = name;
  KST::vectorList.lock().readLock();
  KstVectorPtr vx = *KST::vectorList.findTag(xVector);
  KstVectorPtr vy = *KST::vectorList.findTag(yVector);
  KstVectorPtr ex = *KST::vectorList.findTag(xErrorVector);
  KstVectorPtr ey = *KST::vectorList.findTag(yErrorVector);
  KST::vectorList.lock().unlock();

  KST::dataObjectList.lock().writeLock();
  while (KST::dataObjectList.findTag(n) != KST::dataObjectList.end()) {
    n += "'";
  }

  KstVCurvePtr c = new KstVCurve(n, vx, vy, ex, ey, ex, ey, color);
  KST::dataObjectList.append(KstDataObjectPtr(c));
  KST::dataObjectList.lock().unlock();
  _doc->forceUpdate();
  _doc->setModified();

  return c->tagName();
}


bool KstIfaceImpl::printImage(const QString& windowname, const QString& url) {
  KstApp *app = KstApp::inst();
  KMdiChildView *win = app->findWindow(windowname);
  bool bRetVal = false;

  if (!url.isEmpty() && win) {
    app->immediatePrintWindowToPng(win, url);
    bRetVal = true;
  }

  return bRetVal;
}


bool KstIfaceImpl::printImage(const QString& url) {
  KstApp *app = KstApp::inst();
  bool bRetVal = false;

  if (!url.isEmpty()) {
    app->immediatePrintActiveWindowToPng( url );
    bRetVal = true;
  }

  return bRetVal;
}


bool KstIfaceImpl::printPostScript(const QString& windowname, const QString& url) {
  KstApp *app = KstApp::inst();
  KMdiChildView *win = app->findWindow(windowname);
  bool bRetVal = false;

  if (!url.isEmpty() && win) {
    app->immediatePrintWindowToFile(win, url);
    bRetVal = true;
  }

  return bRetVal;
}


bool KstIfaceImpl::printPostScript(const QString& url) {
  KstApp *app = KstApp::inst();
  bool bRetVal = false;

  if (!url.isEmpty()) {
    app->immediatePrintActiveWindowToFile(url);
    bRetVal = true;
  }

  return bRetVal;
}


void KstIfaceImpl::reloadVectors() {
  _app->reload();
}


void KstIfaceImpl::reloadVector(const QString& vector) {
  KstReadLocker ml(&KST::vectorList.lock());
  KstVectorList::Iterator v = KST::vectorList.findTag(vector);
  if (v != KST::vectorList.end()) {
    (*v)->writeLock();
    KstRVectorPtr r = kst_cast<KstRVector>(*v);
    if (r) {
      r->reload();
    }
    (*v)->unlock();
  }
}


QString KstIfaceImpl::loadVector(const QString& file, const QString& field) {
  KstDataSourcePtr src;
  /* generate or find the kstfile */
  KST::dataSourceList.lock().writeLock();
  KstDataSourceList::Iterator it = KST::dataSourceList.findReusableFileName(file);

  if (it == KST::dataSourceList.end()) {
    src = KstDataSource::loadSource(file);
    if (!src || !src->isValid()) {
      KST::dataSourceList.lock().unlock();
      return QString::null;
    }
    if (src->isEmpty()) {
      KST::dataSourceList.lock().unlock();
      return QString::null;
    }
    KST::dataSourceList.append(src);
  } else {
    src = *it;
  }
  src->writeLock();
  KST::dataSourceList.lock().unlock();

  KST::vectorList.lock().readLock();
  QString vname = "V" + QString::number(KST::vectorList.count() + 1);

  while (KstData::self()->vectorTagNameNotUnique(vname, false)) {
    vname = "V" + QString::number(KST::vectorList.count() + 1);
  }
  KST::vectorList.lock().unlock();

  KstVectorPtr p = new KstRVector(src, field, KstObjectTag(vname, KstObjectTag::globalTagContext), 0, -1, 0, false, false); // FIXME: do tag context properly

  src->unlock();

  if (p) {
    _doc->forceUpdate();
    _doc->setModified();
    return p->tagName();
  }

  return QString::null;
}


const QString& KstIfaceImpl::fileName() {
  return _doc->absFilePath();
}


bool KstIfaceImpl::save() {
  if (_doc->title() != "Untitled") {
    return _doc->saveDocument(_doc->absFilePath());
  }
  return false;
}


bool KstIfaceImpl::saveAs(const QString& fileName) {
  bool rc = _doc->saveDocument(fileName);
  if (rc) {
    QFileInfo saveAsInfo(fileName);
    _doc->setTitle(saveAsInfo.fileName());
    _doc->setAbsFilePath(saveAsInfo.absFilePath());

    _app->setCaption(kapp->caption() + ": " + _doc->title());
  }
  return rc;
}


void KstIfaceImpl::newFile() {
  _doc->newDocument();
}


bool KstIfaceImpl::open(const QString& fileName) {
  return _app->openDocumentFile(fileName);
}


bool KstIfaceImpl::setMDIMode(int mode){
  KstApp *app = KstApp::inst();
  bool returnval = true;

  switch (mode) {
    case KMdi::ToplevelMode:
      app->switchToToplevelMode();
    break;
    case KMdi::ChildframeMode:
      app->switchToChildframeMode();
    break;
    case KMdi::TabPageMode:
      app->switchToTabPageMode();
    break;
    case KMdi::IDEAlMode:
      app->switchToIDEAlMode();
    break;
    default:
      returnval = false;
    break;
  }

  return returnval;
}

bool KstIfaceImpl::setTileMode(int mode) {
  KstApp *app = KstApp::inst();
  bool returnval = false;

  if (app->mdiMode() == KMdi::ChildframeMode) {
    KMdiChildView *activewin = app->activeWindow();
    if (activewin) {
      returnval = true;
      switch (mode) {
        case 1:
          app->cascadeWindows();
        break;
        case 2:
          app->cascadeMaximized();
        break;
        case 3:
          app->expandVertical();
        break;
        case 4:
          app->expandHorizontal();
        break;
        case 5:
          app->tilePragma();
        break;
        case 6:
          app->tileAnodine();
        break;
        case 7:
          app->tileVertically();
        break;
        default:
          returnval = false;
        break;
      }
    }
  }

  return returnval;
}

bool KstIfaceImpl::closeWindow(const QString& windowname) {
  KstApp *app = KstApp::inst();
  KMdiChildView *win = app->findWindow(windowname);

  if (win) {
    app->closeWindow(win, true);
    return true;
  }

  return false;
}


bool KstIfaceImpl::activateWindow(const QString& windowname) {
  KstApp *app = KstApp::inst();
  KMdiChildView *win = app->findWindow(windowname);

  if (win) {
    win->activate();
    return true;
  }

  return false;
}

bool KstIfaceImpl::positionWindow(int topleftX, int topleftY, int width, int height) {
  KstApp *app = KstApp::inst();
  KMdiChildView *win = app->activeWindow();

  if (win) {
    if (topleftX >= 0 && topleftY >= 0 && width >= 0 && height >= 0){
      QRect rectangle(topleftX, topleftY, width, height);
      win->setExternalGeometry(rectangle);
      return true;
    }
  }

  return false;
}

bool KstIfaceImpl::maximizePlot(const QString& plotName) {
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter = app->createIterator();
  while (iter->currentItem()) {
    KMdiChildView *childview = iter->currentItem();
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
    if (viewwindow) {
      Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
      Kst2DPlotList::Iterator plot_iter = plotlist.findTag(plotName);
      if (plot_iter != plotlist.end()) {
        app->deleteIterator(iter);
        (*plot_iter)->setMaximized(true);
        return true;
      }
    }
    iter->next();
  }
  app->deleteIterator(iter);
  return false;
}

bool KstIfaceImpl::unmaximizePlot(const QString& plotName) {
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter = app->createIterator();
  while (iter->currentItem()) {
    KMdiChildView *childview = iter->currentItem();
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
    if (viewwindow) {
      Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
      Kst2DPlotList::Iterator plot_iter = plotlist.findTag(plotName);
      if (plot_iter != plotlist.end()) {
        app->deleteIterator(iter);
        (*plot_iter)->setMaximized(false);
        return true;
      }
    }
    iter->next();
  }
  app->deleteIterator(iter);
  return false;
}

bool KstIfaceImpl::toggleMaximizePlot(const QString& plotName) {
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter = app->createIterator();
  while (iter->currentItem()) {
    KMdiChildView *childview = iter->currentItem();
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
    if (viewwindow) {
      Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
      Kst2DPlotList::Iterator plot_iter = plotlist.findTag(plotName);
      if (plot_iter != plotlist.end()) {
        app->deleteIterator(iter);
        (*plot_iter)->zoomToggle();
        return true;
      }
    }
    iter->next();
  }
  app->deleteIterator(iter);
  return false;
}

bool KstIfaceImpl::setPlotAxes(const QString& plotName,
    int XLower,
    int XUpper,
    int YLower,
    int YUpper) {
  //find the plot
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter = app->createIterator();
  while (iter->currentItem()) {
    KMdiChildView *childview = iter->currentItem();
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
    if (viewwindow) {
      Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
      Kst2DPlotList::Iterator plot_iter=plotlist.findTag(plotName);
      if (plot_iter != plotlist.end()) {
        app->deleteIterator(iter);
      
        (*plot_iter)->setXScaleMode(FIXED);
        (*plot_iter)->setYScaleMode(FIXED);
        (*plot_iter)->setScale(XLower, YLower,
                              XUpper, YUpper);  //set the scale
        
        // repaint the plot 
        (*plot_iter)->setDirty();
        viewwindow->view()->paint(KstPainter::P_PLOT);
        return true;
      }
    }
    iter->next();
  }
  app->deleteIterator(iter);
  return false;
}

QString KstIfaceImpl::createHistogram(const QString& name,
    const QString& vector,
    double min,
    double max,
    int numBins,
    int normalizationType,
    const QColor& color) {

  QStringList objList = createHistogram(name, vector, min, max, numBins, normalizationType);

  if (objList.isEmpty())
  {
    return QString::null;
  }
  
  // also create the curve for the histogram
  QString n = objList[0] + "-C";
  KST::vectorList.lock().readLock();
  KstVectorPtr vx = *KST::vectorList.findTag(objList[1]);
  KstVectorPtr vy = *KST::vectorList.findTag(objList[2]);
  KST::vectorList.lock().unlock();

  KST::dataObjectList.lock().readLock();
  while (KST::dataObjectList.findTag(n) != KST::dataObjectList.end()) {
    n += "'";
  }
  KST::dataObjectList.lock().unlock();

  KstVCurvePtr c = new KstVCurve(n, vx, vy, 0L, 0L, 0L, 0L, color);
  c->setHasPoints(false);
  c->setHasLines(false);
  c->setHasBars(true);
  c->setBarStyle(1);
  
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(c));
  KST::dataObjectList.lock().unlock();

  _doc->forceUpdate();
  _doc->setModified();
  return c->tagName(); //return the curve name so user can plot it
}

QStringList KstIfaceImpl::createHistogram(const QString& name,
                                          const QString& vector,
                                          double min,
                                          double max,
                                          int numBins,
                                          int normalizationType)
{
  //get the vector
  KST::vectorList.lock().readLock();
  KstVectorList::Iterator iter = KST::vectorList.findTag(vector);
  KST::vectorList.lock().unlock();

  if (iter == KST::vectorList.end()) {
    return QStringList();
  }

  //set the normalization type
  KstHsNormType normtype;

  switch (normalizationType) {
    case 1:
      normtype = KST_HS_NUMBER;
      break;
    case 2:
      normtype = KST_HS_PERCENT;
      break;
    case 3:
      normtype = KST_HS_FRACTION;
      break;
    case 4:
      normtype = KST_HS_MAX_ONE;
    default:
      normtype = KST_HS_NUMBER;
      break;
  }

  //suggest a name if not supplied
  QString htag_end;
  if (name.isEmpty())
    htag_end = QString(vector);
  else
    htag_end = QString(name);

  //count number of histograms and make a unique name
  KstHistogramList hlist = kstObjectSubList<KstDataObject,KstHistogram>(KST::dataObjectList);
  int i = hlist.count() + 1;
  QString stringnum;
  stringnum = stringnum.setNum(i);

  QString htag = stringnum + "-" + htag_end;

  while (KstData::self()->dataTagNameNotUnique(htag, false)) {
    stringnum.setNum(++i);
    htag = stringnum + "-" + htag_end;
  }

  KstHistogramPtr histogram = new KstHistogram(htag, *iter, min, max, numBins,
                                               normtype);

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(histogram));
  KST::dataObjectList.lock().unlock();

  QStringList returnList;
  returnList.push_back(histogram->tagName());
  returnList.push_back(histogram->xVTag());
  returnList.push_back(histogram->yVTag());
  
  _doc->forceUpdate();
  _doc->setModified();
  
  return returnList;
}

QString KstIfaceImpl::createPowerSpectrum(const QString & name,
    const QString& vector,
    bool appodize,
    bool removeMean,
    int fftLength,
    const QString& rateUnits,
    double sampleRate,
    const QString& vectorUnits,
    const QColor& color) {

  QStringList objList = createPowerSpectrum(name, vector, appodize, removeMean,
                                            fftLength, rateUnits, sampleRate, 
                                            vectorUnits);
  if (objList.isEmpty())
  {
    return QString::null;
  }
  
  KST::vectorList.lock().readLock();
  KstVectorPtr vx = *KST::vectorList.findTag(objList[1]);
  KstVectorPtr vy = *KST::vectorList.findTag(objList[2]);
  KST::vectorList.lock().unlock();
  
  QString n = objList[0] + "-C";
  KST::dataObjectList.lock().readLock();
  while (KST::dataObjectList.findTag(n) != KST::dataObjectList.end()) {
    n += "'";
  }
  KST::dataObjectList.lock().unlock();
  
  // create curve as well (but don't plot the curve)
  KstVCurvePtr vc = new KstVCurve(n, vx, vy, 
                                  0L, 0L, 0L, 0L, 
                                  color.isValid() ? color : QColor("darkBlue"));

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(vc));
  KST::dataObjectList.lock().unlock();

  _doc->forceUpdate();
  _doc->setModified();

  return vc->tagName(); //return the curve name so user can plot it
}
                                       
QStringList KstIfaceImpl::createPowerSpectrum(const QString& name,
                                              const QString& vector,
                                              bool appodize,
                                              bool removeMean,
                                              int fftLength,
                                              const QString& rateUnits,
                                              double sampleRate,
                                              const QString& vectorUnits)
{
  //get the vector
  KST::vectorList.lock().readLock();
  KstVectorList::Iterator iter = KST::vectorList.findTag(vector);
  KST::vectorList.lock().unlock();

  if (iter == KST::vectorList.end()) {
    return QStringList();
  }

  //suggest a name if not supplied
  QString pstag_end;
  if (name.isEmpty())
    pstag_end = vector;
  else
    pstag_end = name;

  //count number of power spectra and make a unique name
  KstPSDList pslist = kstObjectSubList<KstDataObject,KstPSD>(KST::dataObjectList);
  int i = pslist.count() + 1;
  QString stringnum = QString::number(i);

  QString pstag = stringnum + "-" + pstag_end;

  while (KstData::self()->dataTagNameNotUnique(pstag, false)) {
    stringnum.setNum(++i);
    pstag = stringnum + "-" + pstag_end;
  }

  KstPSDPtr powerspectrum = new KstPSD(pstag, *iter, sampleRate, true, fftLength,
                                       appodize, removeMean, vectorUnits, rateUnits, WindowOriginal);
  
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(powerspectrum));
  KST::dataObjectList.lock().unlock();
  
  QStringList returnList;
  returnList.push_back(powerspectrum->tagName());
  returnList.push_back(powerspectrum->xVTag());
  returnList.push_back(powerspectrum->yVTag());
  
  _doc->forceUpdate();
  _doc->setModified();
  
  return returnList;
}

QString KstIfaceImpl::createEvent(const QString& name,
    const QString& expression,
    const QString& description,
    int debugLogType,
    const QString& email) {

  //suggest a name if not supplied
  QString evtag_end;
  if (name.isEmpty())
    evtag_end = QString(expression);
  else
    evtag_end = QString(name);

  //count number of events and make a unique name
  KstEventMonitorEntryList evlist = kstObjectSubList<KstDataObject,EventMonitorEntry>(KST::dataObjectList);
  int i = evlist.count() + 1;
  QString stringnum;
  stringnum = stringnum.setNum(i);

  QString evtag = "E" + stringnum + "-" + evtag_end;

  while (KstData::self()->dataTagNameNotUnique(evtag, false)) {
    stringnum.setNum(++i);
    evtag = "E" + stringnum + "-" + evtag_end;
  }

  EventMonitorEntryPtr event = new EventMonitorEntry(evtag);

  event->setEvent(expression);
  event->setDescription(description);

  if (debugLogType <= 3 && debugLogType >= 1) {
    event->setLogKstDebug(true);
    switch (debugLogType) {
      case 1:
        event->setLevel(KstDebug::Notice);
      break;
      case 2:
        event->setLevel(KstDebug::Warning);
      break;
      case 3:
        event->setLevel(KstDebug::Error);
      break;
      default:
        event->setLevel(KstDebug::Notice);
      break;
    }
  }
  else {
    event->setLogKstDebug(false);
  }

  if (email.isEmpty()) {
    event->setLogEMail(false);
  } else {
    event->setLogEMail(true);
    event->setEMailRecipients(email);
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(event));
  KST::dataObjectList.lock().unlock();

  _doc->forceUpdate();
  _doc->setModified();

  return evtag;

}

QStringList KstIfaceImpl::createPlugin(const QString& pluginName,
                                const QStringList& scalarInputs,
                                const QStringList& vectorInputs) {

  PluginCollection *plug_collect = PluginCollection::self();
  KstSharedPtr<Plugin> plug_ptr = plug_collect->plugin(pluginName);


  if (!plug_ptr) {
    return QStringList();
  }

  Plugin::Data plug_data = plug_ptr->data();
  Q3ValueList<Plugin::Data::IOValue>::Iterator IOIter;
  QStringList::ConstIterator scalarParamIter;
  QStringList::ConstIterator vectorParamIter;

  KstCPluginPtr kstplug_ptr = new KstCPlugin;

  //iterate through inputs
  IOIter = plug_data._inputs.begin();
  scalarParamIter = scalarInputs.begin();
  vectorParamIter = vectorInputs.begin();

  while (IOIter != plug_data._inputs.end()) {
    if ((*IOIter)._type == Plugin::Data::IOValue::TableType
         && (*IOIter)._subType == Plugin::Data::IOValue::FloatSubType) {
      if (vectorParamIter != vectorInputs.end()) {
        KST::vectorList.lock().readLock();
        KstVectorList::Iterator iter = KST::vectorList.findTag(*vectorParamIter);
        KST::vectorList.lock().unlock();
        if (iter != KST::vectorList.end()) {
          kstplug_ptr->writeLock();
          kstplug_ptr->inputVectors().insert((*IOIter)._name, *iter);
          kstplug_ptr->unlock();
        }
        else {
          return QStringList();
        }
        vectorParamIter++;
      }
      else {
        return QStringList();
      }
    }
    else if ((*IOIter)._type == Plugin::Data::IOValue::FloatType) {
      if (scalarParamIter != scalarInputs.end()) {
        KST::scalarList.lock().readLock();
        KstScalarList::Iterator iter = KST::scalarList.findTag(*scalarParamIter);
        KST::scalarList.lock().unlock();
        if (iter != KST::scalarList.end()) {
          kstplug_ptr->writeLock();
          kstplug_ptr->inputScalars().insert((*IOIter)._name, *iter);
          kstplug_ptr->unlock();
        }
        else {
          return QStringList();
        }
        scalarParamIter++;
      }
      else {
        return QStringList();
      }
    }
    else {
      return QStringList();
    }
    IOIter++;
  }

  //set the plugin
  if (!(kstplug_ptr->setPlugin(plug_ptr))) {
    return QStringList();
  }

  //name the plugin

  KstCPluginList kstpluglist = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList);
  int i = kstpluglist.count() + 1;
  QString stringnum;
  stringnum = stringnum.setNum(i);

  QString plugtag = "P" + stringnum + "-" + pluginName;

  while (KstData::self()->dataTagNameNotUnique(plugtag, false)) {
    stringnum.setNum(++i);
    plugtag = "P" + stringnum + "-" + pluginName;
  }
  kstplug_ptr->setTagName(KstObjectTag(plugtag, KstObjectTag::globalTagContext));  // FIXME: tag context

  //try to rename the outputs.  If something is inconsistent just ignore it and
  //leave default output names.
  KstVectorMap::Iterator kstVectorIter = kstplug_ptr->outputVectors().begin();
  KstScalarMap::Iterator kstScalarIter = kstplug_ptr->outputScalars().begin();

  IOIter = plug_data._outputs.begin();
  while (IOIter != plug_data._inputs.end()) {
    if ((*IOIter)._type == Plugin::Data::IOValue::TableType
         && (*IOIter)._subType == Plugin::Data::IOValue::FloatSubType) {
      if (kstVectorIter != kstplug_ptr->outputVectors().end()) {
        QString vectorTag = plugtag + (*IOIter)._name;
        while (KstData::self()->dataTagNameNotUnique(vectorTag, false)) {
          vectorTag += '\'';
        }
        kstplug_ptr->writeLock();
        (*kstVectorIter)->setTagName(KstObjectTag(vectorTag, KstObjectTag::globalTagContext)); // FIXME: tag context
        kstplug_ptr->unlock();
        kstVectorIter++;
      }
    }
    else if ((*IOIter)._type == Plugin::Data::IOValue::FloatType) {
      if (kstScalarIter != kstplug_ptr->outputScalars().end()) {
        QString scalarTag = plugtag + (*IOIter)._name;
        while (KstData::self()->dataTagNameNotUnique(scalarTag, false)) {
          scalarTag += '\'';
        }
        kstplug_ptr->writeLock();
        (*kstScalarIter)->setTagName(KstObjectTag(scalarTag, KstObjectTag::globalTagContext));  // FIXME: tag context
        kstplug_ptr->unlock();
        kstScalarIter++;
      }
    }
    IOIter++;
  }

  //return the output list, vectors followed by scalars
  QStringList outputList;

  kstVectorIter = kstplug_ptr->outputVectors().begin();
  kstScalarIter = kstplug_ptr->outputScalars().begin();
  while (kstVectorIter != kstplug_ptr->outputVectors().end()) {
    outputList += (*kstVectorIter)->tagName();
  }

  while (kstScalarIter != kstplug_ptr->outputScalars().end()) {
    outputList += (*kstScalarIter)->tagName();
  }
  return outputList;
}


QString KstIfaceImpl::loadMatrix(const QString& name, const QString& file, const QString& field,
                                int xStart, int yStart, int xNumSteps, int yNumSteps, 
                                int skipFrames, bool boxcarFilter) {
  KstDataSourcePtr src;
  /* generate or find the kstfile */
  KST::dataSourceList.lock().writeLock();
  KstDataSourceList::Iterator it = KST::dataSourceList.findReusableFileName(file);

  if (it == KST::dataSourceList.end()) {
    src = KstDataSource::loadSource(file);
    if (!src || !src->isValid()) {
      KST::dataSourceList.lock().unlock();
      return QString::null;
    }
    if (src->isEmpty()) {
      KST::dataSourceList.lock().unlock();
      return QString::null;
    }
    KST::dataSourceList.append(src);
  } else {
    src = *it;
  }
  src->writeLock();
  KST::dataSourceList.lock().unlock();

  // make sure field is valid
  if (!src->isValidMatrix(field)) {
    src->unlock();
    return QString::null;  
  }
  
  // make sure name is unique, else generate a unique one
  KST::matrixList.lock().readLock();
 
  QString matrixName;
  if (name.isEmpty()) {
    matrixName = "M" + QString::number(KST::matrixList.count() + 1); 
  } else {
    matrixName = name;  
  }

  while (KstData::self()->matrixTagNameNotUnique(matrixName, false)) {
    matrixName = "M" + QString::number(KST::matrixList.count() + 1);
  }
  KST::matrixList.lock().unlock();

  KstMatrixPtr p = new KstRMatrix(src, field,
      KstObjectTag(matrixName, KstObjectTag::globalTagContext), xStart, yStart,
      xNumSteps, yNumSteps, boxcarFilter, skipFrames > 0, skipFrames); // FIXME: do tag context properly

  src->unlock();

  if (p) {
    _doc->forceUpdate();
    _doc->setModified();
    return p->tagName();
  }

  return QString::null;
}

  
QString KstIfaceImpl::createGradient(const QString& name, bool xDirection, double zAtMin, double zAtMax, 
                                     int xNumSteps, int yNumSteps, double xMin, double yMin, 
                                     double xStepSize, double yStepSize) {
  // make sure tag name is unique
  // make sure name is unique, else generate a unique one
  KST::matrixList.lock().readLock();

  QString matrixName;
  if (name.isEmpty()) {
    matrixName = "M" + QString::number(KST::matrixList.count() + 1); 
  } else {
    matrixName = name;  
  }

  while (KstData::self()->matrixTagNameNotUnique(matrixName, false)) {
    matrixName = "M" + QString::number(KST::matrixList.count() + 1);
  }
  KST::matrixList.lock().unlock();
  
  // create the gradient matrix
  KstMatrixPtr p = new KstSMatrix(KstObjectTag(matrixName, KstObjectTag::globalTagContext),
      xNumSteps, yNumSteps, xMin, yMin, xStepSize, yStepSize, zAtMin, zAtMax,
      xDirection);  // FIXME: do tag context properly
  

  if (p) {
    _doc->forceUpdate();
    _doc->setModified();
    return p->tagName();
  }

  return QString::null;
}
                

QString KstIfaceImpl::createImage(const QString &name,
                                  const QString &in_matrix,
                                  double lowerZ,
                                  double upperZ,
                                  const QString &paletteName,
                                  int numContours,
                                  const QColor& contourColor,
                                  uint imageType) {
  //get the matrix
  KstMatrixList matrices = kstObjectSubList<KstDataObject, KstMatrix>(KST::dataObjectList);
  KstMatrixPtr matrix = *matrices.findTag(in_matrix);
  if (!matrix) {
    return QString::null;
  }

  //make a name if necessary
  QString imgtag;
  if (name.isEmpty()) {
    imgtag = KST::suggestImageName(matrix->tag());
  } else {
    QString imgtag_end = QString(name);
    //count number of data objects and make a unique name
    int i = KST::dataObjectList.count() + 1;
    imgtag = QString::number(i) + "-" + imgtag_end;
  }
  while (KstData::self()->dataTagNameNotUnique(imgtag, false)) {
    imgtag += "\'";
  }

  //determine the image type
  KstImagePtr image;
  if (imageType == 0) {
    //need a colormap
    if (lowerZ > upperZ) {
      return QString::null;
    }
    KPalette* pal = new KPalette(paletteName);
    matrix->readLock();
    image = new KstImage(imgtag, matrix, lowerZ, upperZ, false, pal);
    matrix->unlock();
  } else if (imageType == 1) {
    //need a contourmap
    if (numContours < 1) {
      return QString::null;
    }
    matrix->readLock();
    image = new KstImage(imgtag, matrix, numContours, contourColor.isValid() ? contourColor : QColor("darkBlue"), 0);
    matrix->unlock();
  } else if (imageType == 2) {
    //need both contourmap and colormap
    if (lowerZ > upperZ) {
      return QString::null;
    }
    if (numContours < 1) {
      return QString::null;
    }
    KPalette* pal = new KPalette(paletteName);
    matrix->readLock();
    image = new KstImage(imgtag, matrix, lowerZ, upperZ, false, pal,
                         numContours, contourColor.isValid() ? contourColor : QColor("darkBlue"), 0);
    matrix->unlock();
  } else {
    return QString::null;
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(KstDataObjectPtr(image));
  KST::dataObjectList.lock().unlock();

  _doc->forceUpdate();
  _doc->setModified();

  return imgtag;

}

bool KstIfaceImpl::addPlotMarker(const QString &plotName, double markerValue) {
  //find the plot
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter = app->createIterator();
  while (iter->currentItem()) {
    KMdiChildView *childview = iter->currentItem();
    KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
    if (viewwindow) {
      Kst2DPlotList plotlist = viewwindow->view()->findChildrenType<Kst2DPlot>(false);
      Kst2DPlotList::Iterator plot_iter=plotlist.findTag(plotName);
      if (plot_iter != plotlist.end() && (*plot_iter)->setPlotMarker(markerValue)) {
        app->deleteIterator(iter);
        // repaint the plot 
        (*plot_iter)->setDirty();
        viewwindow->view()->paint(KstPainter::P_PLOT);
        return true;
      }
    }
    iter->next();
  }
  app->deleteIterator(iter);
  return false;
}


bool KstIfaceImpl::changeDataFile(const QString& fileName) {
  QStringList vl;
  KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  for (KstRVectorList::ConstIterator i = rvl.begin(); i != rvl.end(); ++i) {
    vl += (*i)->tagName();
  }
  return changeDataFiles(vl, fileName);
}


bool KstIfaceImpl::changeDataFile(const QString& vector, const QString& fileName) {
  return changeDataFile(vector, fileName, true);
}


bool KstIfaceImpl::changeDataFile(const QString& vector, const QString& fileName, bool update) {
  KST::vectorList.lock().readLock();
  KstRVectorPtr rvp = kst_cast<KstRVector>(*KST::vectorList.findTag(vector));
  KST::vectorList.lock().unlock();
  if (!rvp) {
    return false;
  }

  KST::dataSourceList.lock().writeLock();
  KstDataSourceList::Iterator it = KST::dataSourceList.findReusableFileName(fileName);
  KstDataSourcePtr file;
  QString invalidSources;

  if (it == KST::dataSourceList.end()) {
    file = KstDataSource::loadSource(fileName);
    if (!file || !file->isValid() || file->isEmpty()) {
      KST::dataSourceList.lock().unlock();
      return false;
    }
    KST::dataSourceList.append(file);
  } else {
    file = *it;
  }

  KST::dataSourceList.lock().unlock();

  rvp->writeLock();
  file->writeLock();

  if (!file->isValidField(vector)) {
    file->unlock();
    rvp->unlock();
    return false;
  }

  rvp->changeFile(file);
  
  file->unlock();
  bool rc = rvp->isValid();
  rvp->unlock();

  if (update) {
    KstApp::inst()->forceUpdate();
  }

  return rc;
}


bool KstIfaceImpl::changeDataFiles(const QStringList& vectors, const QString& fileName) {
  bool rc = true;
  bool any = false;
  for (QStringList::ConstIterator i = vectors.begin(); i != vectors.end(); ++i) {
    bool thisrc = changeDataFile(*i, fileName, false);
    any = thisrc || any;
    rc = thisrc && rc;
  }

  KstApp::inst()->forceUpdate();

  return rc;
}


// vim: ts=2 sw=2 et
