/***************************************************************************
                       kstdatamanger.cpp  -  Part of KST
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
#include <assert.h>

// include files for Qt
#include <q3ptrstack.h>
#include <qtoolbox.h>

#include <qtoolbutton.h>
#include <qstylefactory.h>
#include <qstyle.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QPixmap>
#include <Q3Frame>

static QStyle *windowsStyle = 0;

// include files for KDE
#include "ksdebug.h"
#include <klistview.h>
#include <kmessagebox.h>
#include <kinputdialog.h>
#include <kstandarddirs.h>
#include <kcombobox.h>

#include <klistviewsearchline.h>

// application specific includes
#include "datasourcemetadatadialog.h"
#include "kst2dplot.h"
#include "kstcurvedialog.h"
#include "kstcsddialog.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kstdatamanager.h"
#include "kstdoc.h"
#include "ksteqdialog.h"
#include "ksteventmonitor.h"
#include "ksthsdialog.h"
#include "kstimagedialog.h"
#include "kstmatrixdialog.h"
#include "kstplugindialog.h"
#include "kstpsddialog.h"
#include "kstvectordialog.h"
#include "kstviewwindow.h"
#include "matrixselector.h"
#include "vectorselector.h"
#include "plugincollection.h"


static QMap<int,Kst2DPlotPtr> PlotMap;

#define RTTI_OBJ_VECTOR          4201
#define RTTI_OBJ_OBJECT          4202
#define RTTI_OBJ_DATA_VECTOR     4203
#define RTTI_OBJ_STATIC_VECTOR   4204
#define RTTI_OBJ_MATRIX          4205
#define RTTI_OBJ_DATA_MATRIX     4206
#define RTTI_OBJ_STATIC_MATRIX   4207

KstObjectItem::KstObjectItem(Q3ListView *parent, KstRVectorPtr x, KstDataManagerI *dm, int localUseCount)
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_DATA_VECTOR), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  setText(1, i18n("Data Vector"));
  x = 0L; // keep the counts in sync
  update(true, localUseCount);
}


KstObjectItem::KstObjectItem(Q3ListView *parent, KstSVectorPtr x, KstDataManagerI *dm, int localUseCount)
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_STATIC_VECTOR), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  setText(1, i18n("Static Vector"));
  x = 0L; // keep the counts in sync
  update(true, localUseCount);
}


KstObjectItem::KstObjectItem(KListViewItem *parent, KstVectorPtr x, KstDataManagerI *dm, int localUseCount)
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_VECTOR), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  setText(1, i18n("Slave Vector"));
  x = 0L; // keep the counts in sync
  update(true, localUseCount);
}


KstObjectItem::KstObjectItem(Q3ListView *parent, KstDataObjectPtr x, KstDataManagerI *dm, int localUseCount)
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_OBJECT), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  for (KstVectorMap::Iterator i = x->outputVectors().begin();
      i != x->outputVectors().end();
      ++i) {
    KstObjectItem *item = new KstObjectItem(this, i.data(), _dm);
    connect(item, SIGNAL(updated()), this, SIGNAL(updated()));
  }
  for (KstMatrixMap::Iterator i = x->outputMatrices().begin();
       i != x->outputMatrices().end();
       ++i) {
    KstObjectItem *item = new KstObjectItem(this, i.data(), _dm);
    connect(item, SIGNAL(updated()), this, SIGNAL(updated()));       
  }
  x = 0L; // keep the counts in sync
  update(false, localUseCount);
}


KstObjectItem::KstObjectItem(Q3ListView *parent, KstRMatrixPtr x, KstDataManagerI *dm, int localUseCount) 
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_DATA_MATRIX), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  setText(1, i18n("Data Matrix"));
  x = 0L; // keep the counts in sync
  update(true, localUseCount);
}


KstObjectItem::KstObjectItem(Q3ListView *parent, KstSMatrixPtr x, KstDataManagerI *dm, int localUseCount) 
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_STATIC_MATRIX), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  setText(1, i18n("Static Matrix"));
  x = 0L; // keep the counts in sync
  update(true, localUseCount);
}


KstObjectItem::KstObjectItem(KListViewItem *parent, KstMatrixPtr x, KstDataManagerI *dm, int localUseCount) 
: QObject(), KListViewItem(parent), _rtti(RTTI_OBJ_MATRIX), _tag(x->tag()), _dm(dm) {
  assert(x);
  _inUse = false;
  setText(0, x->tag().tag());
  setText(1, i18n("Slave Matrix"));
  x = 0L; // keep the counts in sync
  update(true, localUseCount);
}


KstObjectItem::~KstObjectItem() {
}


KstDataObjectPtr KstObjectItem::dataObject() {
  return *KST::dataObjectList.findTag(_tag.tag());
}


void KstObjectItem::update(bool recursive, int localUseCount) {
  switch (_rtti) {
    case RTTI_OBJ_DATA_VECTOR:
    {
      KST::vectorList.lock().readLock();
      KstRVectorPtr x = kst_cast<KstRVector>(*KST::vectorList.findTag(_tag));
      KST::vectorList.lock().unlock();
      if (x) {
        x->readLock();
        // getUsage: subtract 1 for KstRVectorPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        QString field;
        if (inUse) {
          field = QString::number(x->length());
        } else {
          field = "-";
        }
        if (text(3) != field) {
          setText(3, field);
        }
        field = i18n("%3: %4 [%1..%2]").arg(x->reqStartFrame())
            .arg(x->reqStartFrame() + x->reqNumFrames())
            .arg(x->filename())
            .arg(x->field());
        if (text(4) != field) {
          setText(4, field);
        }
        _removable = x->getUsage() == 2;
        x->unlock();
      }
      // Hmmm what happens if this if() fails??  We become inconsistent?
      break;
    }
    case RTTI_OBJ_STATIC_VECTOR:
    {
      KST::vectorList.lock().readLock();
      KstSVectorPtr x = kst_cast<KstSVector>(*KST::vectorList.findTag(_tag));
      KST::vectorList.lock().unlock();
      if (x) {
        x->readLock();
        // getUsage: subtract 1 for KstRVectorPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        QString field;
        if (inUse) {
          field = QString::number(x->length());
        } else {
          field = "-";
        }
        if (text(3) != field) {
          setText(3, field);
        }
        field = i18n("%1 to %2").arg(x->min()).arg(x->max());
        if (text(4) != field) {
          setText(4, field);
        }
        _removable = x->getUsage() == 2;
        x->unlock();
      }
      // Hmmm what happens if this if() fails??  We become inconsistent?
      break;
    }
    case RTTI_OBJ_VECTOR:
    {
      KST::vectorList.lock().readLock();
      KstVectorPtr x = *KST::vectorList.findTag(_tag);
      KST::vectorList.lock().unlock();
      if (x) {
        x->readLock();
        // getUsage:
        //  subtract 1 for KstVectorPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        QString field = QString::number(x->length());
        if (text(3) != field) {
          setText(3, field);
        }
        field = i18n("[%1..%2]").arg(x->min()).arg(x->max());
        if (text(4) != field) {
          setText(4, field);
        }
        x->unlock();
        _removable = false;
      }
      break;
    }
    case RTTI_OBJ_OBJECT:
    {
      KST::dataObjectList.lock().readLock();
      KstDataObjectPtr x = *KST::dataObjectList.findTag(_tag.tag());
      KST::dataObjectList.lock().unlock();
      if (x) {
        x->readLock();
        QString field = x->typeString();
        if (text(1) != field) {
          setText(1, field);
        }
        // getUsage:
        //  subtract 1 for KstDataObjectPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        if (x->sampleCount() > 0) {
          field = QString::number(x->sampleCount());
          if (text(3) != field) {
            setText(3, field);
          }
        } else {
          if (text(3) != "-") {
            setText(3, "-");
          }          
        }
        field = x->propertyString();
        if (text(4) != field) {
          setText(4, field);
        }
        if (recursive) {
          Q3PtrStack<Q3ListViewItem> trash;
          KstVectorMap vl = x->outputVectors();
          KstVectorMap::Iterator vlEnd = vl.end();

          for (Q3ListViewItem *i = firstChild(); i; i = i->nextSibling()) {
            KstObjectItem *oi = static_cast<KstObjectItem*>(i);
            if (vl.findTag(oi->tag().tag()) == vlEnd) {
              trash.push(i);
            }
          }
          trash.setAutoDelete(true);
          trash.clear();

          // get the output vectors
          for (KstVectorMap::Iterator p = vl.begin(); p != vlEnd; ++p) {
            bool found = false;
            QString tn = p.data()->tag().tag();
            for (Q3ListViewItem *i = firstChild(); i; i = i->nextSibling()) {
              KstObjectItem *oi = static_cast<KstObjectItem*>(i);
              if (oi->tag().tag() == tn) {
                oi->update();
                found = true;
                break;
              }
            }
            if (!found) {
              KstObjectItem *item = new KstObjectItem(this, p.data(), _dm);
              connect(item, SIGNAL(updated()), this, SIGNAL(updated()));
            }
          }
          
          KstMatrixMap ml = x->outputMatrices();
          KstMatrixMap::Iterator mlEnd = ml.end();
          // also get the output matrices
          for (KstMatrixMap::Iterator p = ml.begin(); p != mlEnd; ++p) {
            bool found = false;
            QString tn = p.data()->tag().tag();
            for (Q3ListViewItem *i = firstChild(); i; i = i->nextSibling()) {
              KstObjectItem *oi = static_cast<KstObjectItem*>(i);
              if (oi->tag().tag() == tn) {
                oi->update();
                found = true;
                break;
              }
            }
            if (!found) {
              KstObjectItem *item = new KstObjectItem(this, p.data(), _dm);
              connect(item, SIGNAL(updated()), this, SIGNAL(updated()));
            }
          }
        }
        _removable = x->getUsage() == 1;
        x->unlock();
      }
      break;
    }
    case RTTI_OBJ_DATA_MATRIX:
    {
      KST::matrixList.lock().readLock();
      KstRMatrixPtr x = kst_cast<KstRMatrix>(*KST::matrixList.findTag(_tag));
      KST::matrixList.lock().unlock();
      if (x) {
        x->readLock();
          // getUsage: subtract 1 for KstRMatrixPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        QString field = QString::number(x->sampleCount());
        if (text(3) != field) {
          setText(3, field);
        }
        field = i18n("%1: %2 (%3 by %4)").arg(x->filename()).arg(x->field())
                .arg(x->xNumSteps())
                .arg(x->yNumSteps()); 
        if (text(4) != field) {
          setText(4, field);
        }
        _removable = x->getUsage() == 2;
        x->unlock();
      } 
      break;
    }
    case RTTI_OBJ_STATIC_MATRIX:
    {
      KST::matrixList.lock().readLock();
      KstSMatrixPtr x = kst_cast<KstSMatrix>(*KST::matrixList.findTag(_tag));
      KST::matrixList.lock().unlock();
      if (x) {
        x->readLock();
          // getUsage: subtract 1 for KstRMatrixPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        QString field = QString::number(x->sampleCount());
        if (text(3) != field) {
          setText(3, field);
        }
        field = i18n("%1 to %2").arg(x->gradZMin()).arg(x->gradZMax());
        if (text(4) != field) {
          setText(4, field);
        }
        _removable = x->getUsage() == 2;
        x->unlock();
      }
      break;
    }
    case RTTI_OBJ_MATRIX:
    {
      KST::matrixList.lock().readLock();
      KstMatrixPtr x = *KST::matrixList.findTag(_tag);
      KST::matrixList.lock().unlock();
      if (x) {
        x->readLock();
          // getUsage:
          //  subtract 1 for KstVectorPtr x
        bool inUse = (x->getUsage() - 1 - localUseCount) > 0;
        if (inUse != _inUse) {
          _inUse = inUse;
          setPixmap(2, inUse ? _dm->yesPixmap() : QPixmap());
        }
        QString field = QString::number(x->sampleCount());
        if (text(3) != field) {
          setText(3, field);
        }
        field = i18n("[%1..%2]").arg(x->minValue()).arg(x->maxValue());
        if (text(4) != field) {
          setText(4, field);
        }
        x->unlock();
        _removable = false;
      }
      break;
    }
    default:
      assert(0);
  }
}


void KstObjectItem::updateButtons() {
  _dm->Edit->setEnabled(RTTI_OBJ_VECTOR != _rtti && RTTI_OBJ_MATRIX != _rtti);
  _dm->Delete->setEnabled(RTTI_OBJ_VECTOR != _rtti && RTTI_OBJ_MATRIX != _rtti);
}


void KstObjectItem::reload() {
  if (_rtti == RTTI_OBJ_DATA_VECTOR) {
    KstReadLocker ml(&KST::vectorList.lock());
    KstVectorList::Iterator v = KST::vectorList.findTag(_tag);
    if (v != KST::vectorList.end()) {
      KstRVectorPtr r = kst_cast<KstRVector>(*v);
      if (r) {
        r->writeLock();
        r->reload();
        r->unlock();
      }
    }
  } else if (_rtti == RTTI_OBJ_DATA_MATRIX) {
    KstReadLocker ml(&KST::matrixList.lock());
    KstMatrixList::Iterator m = KST::matrixList.findTag(_tag);
    if (m != KST::matrixList.end()) {
      KstRMatrixPtr r = kst_cast<KstRMatrix>(*m);
      if (r) {
        r->writeLock();
        r->reload();
        r->unlock();
      }
    }
  }
}


void KstObjectItem::makeCurve() {
  KstCurveDialogI::globalInstance()->show();
  KstCurveDialogI::globalInstance()->setVector(_tag.tag());
}

void KstObjectItem::makeCSD() {
  KstCsdDialogI::globalInstance()->show();
  KstCsdDialogI::globalInstance()->setVector(_tag.tag());
}

void KstObjectItem::makePSD() {
  KstPsdDialogI::globalInstance()->show();
  KstPsdDialogI::globalInstance()->setVector(_tag.tag());
}


void KstObjectItem::makeHistogram() {
  KstHsDialogI::globalInstance()->show();
  KstHsDialogI::globalInstance()->setVector(_tag.tag());
}


void KstObjectItem::makeImage() {
  KstImageDialogI::globalInstance()->show();
  KstImageDialogI::globalInstance()->setMatrix(_tag.tag());
}


void KstObjectItem::showMetadata() {
  if (_rtti == RTTI_OBJ_DATA_VECTOR) {
    DataSourceMetaDataDialog *dlg = new DataSourceMetaDataDialog(_dm, 0, false, Qt::WDestructiveClose);
    KstReadLocker vl(&KST::vectorList.lock());
    KstVectorList::Iterator m = KST::vectorList.findTag(_tag);
    KstRVectorPtr r = kst_cast<KstRVector>(*m);
    KstDataSourcePtr dsp;
    if (r) {
      r->readLock();
      dsp = r->dataSource();
      r->unlock();
    }
    dlg->setDataSource(dsp);
    dlg->show();
  } else if (_rtti == RTTI_OBJ_DATA_MATRIX) {
    DataSourceMetaDataDialog *dlg = new DataSourceMetaDataDialog(_dm, 0, false, Qt::WDestructiveClose);
    KstReadLocker ml(&KST::matrixList.lock());
    KstMatrixList::Iterator m = KST::matrixList.findTag(_tag);
    KstRMatrixPtr r = kst_cast<KstRMatrix>(*m);
    KstDataSourcePtr dsp;
    if (r) {
      r->readLock();
      dsp = r->dataSource();
      r->unlock();
    }
    dlg->setDataSource(dsp);
    dlg->show();
  }
}


void KstObjectItem::activateHint(int id) {
  KstDataObjectPtr d = dataObject();
  const KstCurveHintList* hints = d->curveHints();
  int cnt = 0;
  for (KstCurveHintList::ConstIterator i = hints->begin(); i != hints->end(); ++i) {
    if (cnt == id) {
      KstBaseCurvePtr c = (*i)->makeCurve(KST::suggestCurveName(d->tag(), false), KstColorSequence::next());
      if (c) {
        KST::dataObjectList.lock().writeLock();
        KST::dataObjectList.append(c.data());
        KST::dataObjectList.lock().unlock();
        emit updated();
      } else {
        KMessageBox::sorry(KstApp::inst(), i18n("Unable to create quick curve."));
      }
      break;
    }
    ++cnt;
  }
}


void KstObjectItem::addToPlot(int id) {
  Kst2DPlotPtr p = PlotMap[id];
  KstBaseCurvePtr c = kst_cast<KstBaseCurve>(dataObject());
  if (p && c) {
    p->addCurve(c);
    p->setDirty();
    paintPlot(p);
    emit updated();
  }
}


void KstObjectItem::removeFromPlot(int id) {
  Kst2DPlotPtr p = PlotMap[id];
  KstBaseCurvePtr c = kst_cast<KstBaseCurve>(dataObject());
  if (p && c) {
    p->removeCurve(c);
    p->setDirty();
    paintPlot(p);
    emit updated();
  }
}


void KstObjectItem::paintPlot(Kst2DPlotPtr p) {
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  while (it->currentItem()) {
    KstViewWindow *v = dynamic_cast<KstViewWindow*>(it->currentItem());
    if (v && v->view()->contains(kst_cast<KstViewObject>(p))) {
      v->view()->paint(KstPainter::P_PLOT);
      break;
    }
    it->next();
  }
  app->deleteIterator(it);
}


const QPixmap& KstDataManagerI::yesPixmap() const {
  return _yesPixmap;
}

KstDataManagerI::KstDataManagerI(KstDoc *in_doc, QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: QDialog(parent, name, modal, fl) {
  doc = in_doc;

  _yesPixmap = QPixmap(locate("data", "kst/pics/yes.png"));

  connect(Edit, SIGNAL(clicked()), this, SLOT(edit_I()));
  connect(Delete, SIGNAL(clicked()), this, SLOT(delete_I()));
  connect(Purge, SIGNAL(clicked()), doc, SLOT(purge()));
  connect(DataView, SIGNAL(doubleClicked(Q3ListViewItem *)),
      this, SLOT(edit_I()));
  connect(DataView, SIGNAL(currentChanged(Q3ListViewItem *)),
      this, SLOT(currentChanged(Q3ListViewItem *)));
  connect(DataView, SIGNAL(selectionChanged()),
      this, SLOT(selectionChanged()));
  connect(DataView, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), this, SLOT(contextMenu(Q3ListViewItem*, const QPoint&, int)));

  _searchWidget = new KListViewSearchLineWidget(DataView, SearchBox);
  Q3ValueList<int> cols;
  cols.append(0);
  _searchWidget->createSearchLine(DataView);
  _searchWidget->searchLine()->setSearchColumns(cols);

  Q3MainWindow *main = static_cast<Q3MainWindow*>(parent);
  main->setUsesTextLabel(true);

  _primitive = new Q3ToolBar(i18n("Primitive Objects"), main, this);
  _primitive->setFrameStyle(Q3Frame::NoFrame);
  _primitive->setOrientation(Qt::Vertical);
  _primitive->setBackgroundMode(Qt::PaletteBase);

  _data = new Q3ToolBar(i18n("Data Objects"), main, this);
  _data->setFrameStyle(Q3Frame::NoFrame);
  _data->setOrientation(Qt::Vertical);
  _data->setBackgroundMode(Qt::PaletteBase);

  _fits = new Q3ToolBar(i18n("Fit Objects"), main, this);
  _fits->setFrameStyle(Q3Frame::NoFrame);
  _fits->setOrientation(Qt::Vertical);
  _fits->setBackgroundMode(Qt::PaletteBase);

  _filters = new Q3ToolBar(i18n("Filter Objects"), main, this);
  _filters->setFrameStyle(Q3Frame::NoFrame);
  _filters->setOrientation(Qt::Vertical);
  _filters->setBackgroundMode(Qt::PaletteBase);

  ToolBox->setUpdatesEnabled(false);

  _primitive->setUpdatesEnabled(false);
  _primitive->clear();

  _data->setUpdatesEnabled(false);
  _data->clear();

  _fits->setUpdatesEnabled(false);
  _fits->clear();

  _filters->setUpdatesEnabled(false);
  _filters->clear();

  //Create canonical actions...
//   createObjectAction(i18n("Scalar"), _primitive, KstScalarDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Vector"), _primitive, KstVectorDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Matrix"), _primitive, KstMatrixDialogI::globalInstance(), SLOT(show()));
//   createObjectAction(i18n("String"), _primitive, KstStringDialogI::globalInstance(), SLOT(show()));

  createObjectAction(i18n("Curve"), _data, KstCurveDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Equation"), _data, KstEqDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Histogram"), _data, KstHsDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Power Spectrum"), _data, KstPsdDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Event Monitor"), _data, KstEventMonitorI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("Image"), _data, KstImageDialogI::globalInstance(), SLOT(show()));
  createObjectAction(i18n("CSD"), _data, KstCsdDialogI::globalInstance(), SLOT(show()));

  //Create plugin actions...
  setupPluginActions();

  //TODO sort the actions in each box alphabetically?

  QWidget *priw = new QWidget(_primitive);
  priw->setBackgroundMode(Qt::PaletteBase);
  _primitive->setStretchableWidget(priw);

  QWidget *datw = new QWidget(_data);
  datw->setBackgroundMode(Qt::PaletteBase);
  _data->setStretchableWidget(datw);

  QWidget *fitw = new QWidget(_fits);
  fitw->setBackgroundMode(Qt::PaletteBase);
  _fits->setStretchableWidget(fitw);

  QWidget *filw = new QWidget(_filters);
  filw->setBackgroundMode(Qt::PaletteBase);
  _filters->setStretchableWidget(filw);

  ToolBox->setUpdatesEnabled(true);

  _primitive->setUpdatesEnabled(true);
  _data->setUpdatesEnabled(true);
  _fits->setUpdatesEnabled(true);
  _filters->setUpdatesEnabled(true);

  ToolBox->addItem(_primitive, i18n("Primitive Objects"));
  ToolBox->addItem(_data, i18n("Data Objects"));
  ToolBox->addItem(_fits, i18n("Fit Objects"));
  ToolBox->addItem(_filters, i18n("Filter Objects"));
}


KstDataManagerI::~KstDataManagerI() {
}


void KstDataAction::addedTo(QWidget *actionWidget, QWidget *container) {
  Q_UNUSED(container)
  if ( ::qt_cast<QToolButton*>(actionWidget) ) {
    if (!windowsStyle)
      windowsStyle = QStyleFactory::create("windows");
    actionWidget->setStyle(windowsStyle);

    ( (QToolButton*)actionWidget )->setUsesTextLabel(true);
    ( (QToolButton*)actionWidget )->setTextPosition(QToolButton::BesideIcon);
    actionWidget->setBackgroundMode(Qt::PaletteBase);
  }
}


void KstDataManagerI::createObjectAction(const QString &txt, Q3ToolBar *bar,
                                         QObject *receiver, const char *slot) {
  KstDataAction *a = new KstDataAction(txt, QKeySequence(), bar);
  a->addTo(bar);
  if (receiver && slot) {
    connect(a, SIGNAL(activated()), receiver, slot);
  }
}


void KstDataManagerI::setupPluginActions() {

  //The new KstDataObject plugins...
  {
    const KstPluginInfoList newPlugins = KstDataObject::pluginInfoList();
    KstPluginInfoList::ConstIterator it = newPlugins.begin();
    for (; it != newPlugins.end(); ++it) {

      KstDataObjectPtr ptr = KstDataObject::plugin(it.key());
      if (!ptr)
        continue;

      switch(it.data()) {
      case KstDataObject::Generic:
        createObjectAction(it.key(), _data, ptr, SLOT(showNewDialog()));
        break;
      case KstDataObject::Primitive:
        createObjectAction(it.key(), _primitive, ptr, SLOT(showNewDialog()));
        break;
      case KstDataObject::Fit:
        createObjectAction(it.key(), _fits, ptr, SLOT(showNewDialog()));
        break;
      case KstDataObject::Filter:
        createObjectAction(it.key(), _filters, ptr, SLOT(showNewDialog()));
        break;
      default:
        break;
      }
    }
  }

  //The old C style plugins...
  QStringList oldPlugins;
  const QMap<QString,QString> readable =
    PluginCollection::self()->readableNameList();
  QMap<QString,QString>::const_iterator it = readable.begin();
  for (; it != readable.end(); ++it) {
    oldPlugins << it.key();
  }

  {
    QStringList::ConstIterator it = oldPlugins.begin();
    for (; it != oldPlugins.end(); ++it) {
      if (KstSharedPtr<Plugin> p = PluginCollection::self()->plugin(readable[*it])) {
        if (p->data()._isFit)
          createObjectAction(*it, _fits, this, SLOT(showOldPlugin()));
        else if (p->data()._isFilter)
          createObjectAction(*it, _filters, this, SLOT(showOldPlugin()));
        else
          createObjectAction(*it, _data, this, SLOT(showOldPlugin()));
      }
    }
  }
}


void KstDataManagerI::showOldPlugin() {

  if (QAction *a = ::qt_cast<QAction*>(sender())) {
    const QMap<QString,QString> readable =
      PluginCollection::self()->readableNameList();
    KstPluginDialogI::globalInstance()->showNew(readable[a->text()]);
  }
}


void KstDataManagerI::doubleClicked(Q3ListViewItem *i) {
  if (i && DataView->selectedItems().contains(i)) {
    edit_I();
  }
}

void KstDataManagerI::show_I() {
  show();
  raise();
  update();
}


void KstDataManagerI::updateContents() {
  if (!isShown()) {
    return;
  }

  for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
    KstObjectItem *oi = static_cast<KstObjectItem*>(i);
    oi->update();
  }

  _searchWidget->searchLine()->updateSearch();
}


void KstDataManagerI::update() {
  if (!isShown()) {
    return;
  }

  Q3ListViewItem *currentItem = DataView->selectedItem();
  Q3PtrStack<Q3ListViewItem> trash;

  KST::dataObjectList.lock().writeLock();
  KST::vectorList.lock().writeLock();
  KST::matrixList.lock().writeLock();

  // garbage collect first
  for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
    KstObjectItem *oi = static_cast<KstObjectItem*>(i);
    if (i->rtti() == RTTI_OBJ_OBJECT) {
      if (KST::dataObjectList.findTag(oi->tag().tag()) == KST::dataObjectList.end()) {
        trash.push(i);
      }
    } else if (i->rtti() == RTTI_OBJ_DATA_MATRIX || 
               i->rtti() == RTTI_OBJ_MATRIX ||
               i->rtti() == RTTI_OBJ_STATIC_MATRIX) {
      if (KST::matrixList.findTag(oi->tag().tag()) == KST::matrixList.end()) {
        trash.push(i);  
      }
    } else {
      if (KST::vectorList.findTag(oi->tag().tag()) == KST::vectorList.end()) {
        trash.push(i);
      }
    }
  }

  trash.setAutoDelete(true);
  DataView->blockSignals(true);
  trash.clear();
  DataView->blockSignals(false);

  // update the data objects
  for (KstDataObjectList::iterator it = KST::dataObjectList.begin();
                                    it != KST::dataObjectList.end();
                                                               ++it) {
    KstReadLocker dol(*it);
    bool found = false;
    for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
      KstObjectItem *oi = static_cast<KstObjectItem*>(i);
      if (oi->rtti() == RTTI_OBJ_OBJECT && oi->tag().tag() == (*it)->tag().tag()) {
        oi->update();
        found = true;
        break;
      }
    }
    if (!found) {
      KstObjectItem *i = new KstObjectItem(DataView, *it, this);
      connect(i, SIGNAL(updated()), this, SLOT(doUpdates()));
    }
  }

  KST::dataObjectList.lock().unlock();

  // update the data vectors
  KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  for (KstRVectorList::iterator it = rvl.begin(); it != rvl.end(); ++it) {
    KstReadLocker vl(*it);
    bool found = false;
    for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
      KstObjectItem *oi = static_cast<KstObjectItem*>(i);
      if (oi->rtti() == RTTI_OBJ_DATA_VECTOR && oi->tag().tag() == (*it)->tag().tag()) {
        oi->update(true, 1);
        found = true;
        break;
      }
    }
    if (!found) {
      KstObjectItem *i = new KstObjectItem(DataView, *it, this, 1);
      connect(i, SIGNAL(updated()), this, SLOT(doUpdates()));
    }
  }

  // update the static vectors
  KstSVectorList svl = kstObjectSubList<KstVector,KstSVector>(KST::vectorList);
  for (KstSVectorList::iterator it = svl.begin(); it != svl.end(); ++it) {
    KstReadLocker vl(*it);
    bool found = false;
    for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
      KstObjectItem *oi = static_cast<KstObjectItem*>(i);
      if (oi->rtti() == RTTI_OBJ_STATIC_VECTOR && oi->tag().tag() == (*it)->tag().tag()) {
        oi->update(true, 1);
        found = true;
        break;
      }
    }
    if (!found) {
      KstObjectItem *i = new KstObjectItem(DataView, *it, this, 1);
      connect(i, SIGNAL(updated()), this, SLOT(doUpdates()));
    }
  }

  KST::vectorList.lock().unlock();

  // update the data matrices 
  KstRMatrixList rml = kstObjectSubList<KstMatrix,KstRMatrix>(KST::matrixList);
  for (KstRMatrixList::iterator it = rml.begin(); it != rml.end(); ++it) {
    KstReadLocker ml(*it);
    bool found = false;
    for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
      KstObjectItem *oi = static_cast<KstObjectItem*>(i);
      if (oi->rtti() == RTTI_OBJ_DATA_MATRIX && oi->tag().tag() == (*it)->tag().tag()) {
        oi->update(true, 1);
        found = true;
        break;
      }
    }
    if (!found) {
      KstObjectItem *i = new KstObjectItem(DataView, *it, this, 1);
      connect(i, SIGNAL(updated()), this, SLOT(doUpdates()));
    }
  }

  // update the static matrices
  KstSMatrixList sml = kstObjectSubList<KstMatrix,KstSMatrix>(KST::matrixList);
  for (KstSMatrixList::iterator it = sml.begin(); it != sml.end(); ++it) {
    KstReadLocker ml(*it);
    bool found = false;
    for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
      KstObjectItem *oi = static_cast<KstObjectItem*>(i);
      if (oi->rtti() == RTTI_OBJ_STATIC_MATRIX && oi->tag().tag() == (*it)->tag().tag()) {
        oi->update(true, 1);
        found = true;
        break;
      }
    }
    if (!found) {
      KstObjectItem *i = new KstObjectItem(DataView, *it, this, 1);
      connect(i, SIGNAL(updated()), this, SLOT(doUpdates()));
    }
  }

  KST::matrixList.lock().unlock();

  // is this really necessary?  I would think not...
  for (Q3ListViewItem *i = DataView->firstChild(); i; i = i->nextSibling()) {
    if (i == currentItem) {
      DataView->setCurrentItem(i);
      DataView->setSelected(i, true);
      break;
    }
  }

  if (DataView->selectedItem()) {
    static_cast<KstObjectItem*>(DataView->currentItem())->updateButtons();
  } else {
    Edit->setEnabled(false);
    Delete->setEnabled(false);
  }
}


void KstDataManagerI::edit_I() {
  Q3ListViewItem *qi = 0;

  if (!DataView->selectedItems().isEmpty()) {
    qi = DataView->selectedItems().at(0);
  } else {
    // This error message is odd to say the least.
    //KMessageBox::sorry(this, i18n("A data item must be selected to edit."));
    return;
  }

  switch (qi->rtti()) {
    case RTTI_OBJ_DATA_VECTOR:
      emit editDataVector(qi->text(0));
      break;
    case RTTI_OBJ_STATIC_VECTOR:
      emit editStaticVector(qi->text(0));
      break;
    case RTTI_OBJ_OBJECT:
      static_cast<KstObjectItem*>(qi)->dataObject()->showDialog(false);
      break;
    case RTTI_OBJ_DATA_MATRIX:
      emit editDataMatrix(qi->text(0));  
      break;
    case RTTI_OBJ_STATIC_MATRIX:
      emit editStaticMatrix(qi->text(0));  
      break;
  default:
      break;
  }
}


void KstDataManagerI::delete_I() {
  Q3ListViewItem *qi = DataView->selectedItems().at(0);
  if (!qi) {
    return;
  }
  KstObjectItem *koi = static_cast<KstObjectItem*>(qi);

  if (koi->removable()) {
    if (qi->rtti() == RTTI_OBJ_OBJECT) {
      doc->removeDataObject(koi->tag().tag());
    } else if (qi->rtti() == RTTI_OBJ_DATA_VECTOR) {
      KST::vectorList.lock().writeLock();
      KST::vectorList.removeTag(koi->tag().tag());
      KST::vectorList.lock().unlock();
      doUpdates();
    } else if (qi->rtti() == RTTI_OBJ_STATIC_VECTOR) {
      KST::vectorList.lock().writeLock();
      KST::vectorList.removeTag(koi->tag().tag());
      KST::vectorList.lock().unlock();
      doUpdates();
    } else if (qi->rtti() == RTTI_OBJ_DATA_MATRIX) {
      KST::matrixList.lock().writeLock();
      KST::matrixList.removeTag(koi->tag().tag());
      KST::matrixList.lock().unlock();  
      doUpdates();
    } else if (qi->rtti() == RTTI_OBJ_STATIC_MATRIX) {
      KST::matrixList.lock().writeLock();
      KST::matrixList.removeTag(koi->tag().tag());
      KST::matrixList.lock().unlock();  
      doUpdates();
    }
    update();
  } else {
    // Don't prompt for base curves
    KstBaseCurvePtr bc = kst_cast<KstBaseCurve>(koi->dataObject());
    if (bc || KMessageBox::warningYesNo(this, i18n("There are other objects in memory that depend on %1.  Do you wish to delete them too?").arg(koi->tag().tag())) == KMessageBox::Yes) {

      if (qi->rtti() == RTTI_OBJ_OBJECT) {
        koi->dataObject()->deleteDependents();
        doc->removeDataObject(koi->tag().tag());
      } else if (qi->rtti() == RTTI_OBJ_DATA_VECTOR) {
        KstRVectorPtr x = kst_cast<KstRVector>(*KST::vectorList.findTag(koi->tag().tag()));
        if (x) {
          x->deleteDependents();
          x = 0L;
          KST::vectorList.lock().writeLock();
          KST::vectorList.removeTag(koi->tag().tag());
          KST::vectorList.lock().unlock();
          doUpdates();
        } else {
          KMessageBox::sorry(this, i18n("Unknown error deleting data vector."));
        }
      } else if (qi->rtti() == RTTI_OBJ_STATIC_VECTOR) {
        KstSVectorPtr x = kst_cast<KstSVector>(*KST::vectorList.findTag(koi->tag().tag()));
        if (x) {
          x->deleteDependents();
          x = 0L;
          KST::vectorList.lock().writeLock();
          KST::vectorList.removeTag(koi->tag().tag());
          KST::vectorList.lock().unlock();
          doUpdates();
        } else {
          KMessageBox::sorry(this, i18n("Unknown error deleting static vector."));
        }
      } else if (qi->rtti() == RTTI_OBJ_DATA_MATRIX) {
        KstRMatrixPtr x = kst_cast<KstRMatrix>(*KST::matrixList.findTag(koi->tag().tag()));
        if (x) {
          x->deleteDependents();
          x = 0L;
          KST::matrixList.lock().writeLock();
          KST::matrixList.removeTag(koi->tag().tag());
          KST::matrixList.lock().unlock();
          doUpdates();
        } else {
          KMessageBox::sorry(this, i18n("Unknown error deleting data matrix."));
        }
      } else if (qi->rtti() == RTTI_OBJ_STATIC_MATRIX) {
        KstSMatrixPtr x = kst_cast<KstSMatrix>(*KST::matrixList.findTag(koi->tag().tag()));
        if (x) {
          x->deleteDependents();
          x = 0L;
          KST::matrixList.lock().writeLock();
          KST::matrixList.removeTag(koi->tag().tag());
          KST::matrixList.lock().unlock();
          doUpdates();
        } else {
          KMessageBox::sorry(this, i18n("Unknown error deleting static matrix."));
        }  
      }
      KstApp::inst()->paintAll(KstPainter::P_PLOT);
      update();
    } else {
      KMessageBox::sorry(this, i18n("Cannot delete objects with dependencies."));
    }
  }
}


// Menu IDs:
// 100->499 reserved for plots
// 500->999 reserved for filters

void KstDataManagerI::contextMenu(Q3ListViewItem *i, const QPoint& p, int col) {
  Q_UNUSED(col)

  if (!i) {
    return;
  }

  KstObjectItem *koi = static_cast<KstObjectItem*>(i);
  KstBaseCurvePtr c;
  KstImagePtr img;

  KPopupMenu *m = new KPopupMenu(this);

  m->insertTitle(koi->text(0));

  int id;
  if (koi->rtti() != RTTI_OBJ_VECTOR && koi->rtti() != RTTI_OBJ_MATRIX) {
    id = m->insertItem(i18n("&Edit..."), this, SLOT(edit_I()));
  }

  if (koi->dataObject()) {
    const KstCurveHintList* hints = koi->dataObject()->curveHints();
    if (!hints->isEmpty()) {
      KPopupMenu *hintMenu = new KPopupMenu(this);
      int cnt = 0;
      for (KstCurveHintList::ConstIterator i = hints->begin(); i != hints->end(); ++i) {
        hintMenu->insertItem((*i)->curveName(), koi, SLOT(activateHint(int)), 0, cnt);
        cnt++;
      }
      id = m->insertItem(i18n("&Quick Curve"), hintMenu);
    }
  }

  if (koi->rtti() == RTTI_OBJ_DATA_VECTOR) {
    id = m->insertItem(i18n("&Make Curve..."), koi, SLOT(makeCurve()));
    id = m->insertItem(i18n("Make &Power Spectrum..."), koi, SLOT(makePSD()));
    id = m->insertItem(i18n("Make Cumulative &Spectral Decay..."), koi, SLOT(makeCSD()));
    id = m->insertItem(i18n("Make &Histogram..."), koi, SLOT(makeHistogram()));
    id = m->insertItem(i18n("&Reload"), koi, SLOT(reload()));
    id = m->insertItem(i18n("Meta &Data"), koi, SLOT(showMetadata()));
  } else if (koi->rtti() == RTTI_OBJ_VECTOR) {
    id = m->insertItem(i18n("&Make Curve..."), koi, SLOT(makeCurve()));
    id = m->insertItem(i18n("Make &Power Spectrum..."), koi, SLOT(makePSD()));
    id = m->insertItem(i18n("Make Cumulative &Spectral Decay..."), koi, SLOT(makeCSD()));
    id = m->insertItem(i18n("Make &Histogram..."), koi, SLOT(makeHistogram()));
  } else if (koi->rtti() == RTTI_OBJ_DATA_MATRIX) {
    id = m->insertItem(i18n("Make &Image..."), koi, SLOT(makeImage()));  
    id = m->insertItem(i18n("&Reload"), koi, SLOT(reload()));
    id = m->insertItem(i18n("Meta &Data"), koi, SLOT(showMetadata()));
  } else if (koi->rtti() == RTTI_OBJ_MATRIX || koi->rtti() == RTTI_OBJ_STATIC_MATRIX) {
    id = m->insertItem(i18n("Make &Image..."), koi, SLOT(makeImage()));
  } else if ((c = kst_cast<KstBaseCurve>(koi->dataObject()))) {
    KPopupMenu *addMenu = new KPopupMenu(this);
    KPopupMenu *removeMenu = new KPopupMenu(this);
    PlotMap.clear();
    id = 300;
    bool haveAdd = false, haveRemove = false;

    KstApp *app = KstApp::inst();
    KMdiIterator<KMdiChildView*> *it = app->createIterator();
    while (it->currentItem()) {
      KstViewWindow *v = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (v) {
        Kst2DPlotList plots = v->view()->findChildrenType<Kst2DPlot>();
        for (Kst2DPlotList::Iterator i = plots.begin(); i != plots.end(); ++i) {
          Kst2DPlotPtr plot = *i;
          if (!plot->Curves.contains(c)) {
            addMenu->insertItem(i18n("%1 - %2").arg(v->caption()).arg(plot->tag().tag()), koi, SLOT(addToPlot(int)), 0, id);
            haveAdd = true;
          } else {
            removeMenu->insertItem(i18n("%1 - %2").arg(v->caption()).arg(plot->tag().tag()), koi, SLOT(removeFromPlot(int)), 0, id);
            haveRemove = true;
          }
          PlotMap[id++] = plot;
        }
      }
      it->next();
    }

    app->deleteIterator(it);

    id = m->insertItem(i18n("&Add to Plot"), addMenu);
    m->setItemEnabled(id, haveAdd);
    id = m->insertItem(i18n("&Remove From Plot"), removeMenu);
    m->setItemEnabled(id, haveRemove);
  } 

  if (koi->rtti() != RTTI_OBJ_VECTOR && koi->rtti() != RTTI_OBJ_MATRIX) {
    // no slave vectors or matrices get this
    id = m->insertItem(i18n("&Delete"), this, SLOT(delete_I()));
  }

  m->popup(p);
}


void KstDataManagerI::doUpdates() {
//  doc->forceUpdate();
//  doc->setModified();
  emit docChanged();
}


void KstDataManagerI::currentChanged(Q3ListViewItem *i) {
  if (i && !DataView->selectedItems().isEmpty()) {
    KstObjectItem *koi = static_cast<KstObjectItem*>(i);
    koi->updateButtons();
  } else {
    Edit->setEnabled(false);
    Delete->setEnabled(false);
  }
}


void KstDataManagerI::selectionChanged() {
  if (!DataView->selectedItems().isEmpty()) {
    KstObjectItem *koi = static_cast<KstObjectItem*>(DataView->selectedItems().first());
    koi->updateButtons();
  } else {
    Edit->setEnabled(false);
    Delete->setEnabled(false);
  }
}


#include "kstdatamanager.moc"
// vim: ts=2 sw=2 et
