/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "view.h"
#include "scene.h"
#include "viewitem.h"
#include "layoutboxitem.h"
#include "mainwindow.h"
#include "application.h"
#include "applicationsettings.h"
#include "viewdialog.h"
#include "viewgridlayout.h"
#include "document.h"
#include "plotitemmanager.h"
#include "plotitem.h"
#include "dialogdefaults.h"

#include <math.h>

#include <QDebug>
#include <QTimer>
#include <QUndoStack>
#include <QResizeEvent>
#include <QMenu>
#include <QWidgetAction>
#include <QGLWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QKeyEvent>

namespace Kst {

View::View() : QGraphicsView(kstApp->mainWindow())
{
  init();
}

View::View(QWidget* parent) : QGraphicsView(parent)
{
  init();
}


void View::init()
{
  _viewMode = Data;
  _mouseMode = Default;
  _layoutBoxItem = 0;
  _gridSpacing = QSizeF(20,20);
  _showGrid = false;
  _snapToGridHorizontal = false;
  _snapToGridVertical = false;
  _plotBordersDirty = false;
  _printing = false;
  _dataMode = false;
  _fontRescale = 1.0;
  _childMaximized = false;
  _undoStack = new QUndoStack(this);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setScene(new Scene(this));
  scene()->installEventFilter(this);
  setInteractive(true);
  setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  setContextMenuPolicy(Qt::DefaultContextMenu);

  _useOpenGL = ApplicationSettings::self()->useOpenGL();
  if (_useOpenGL) {
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setViewport(new QGLWidget);
  } else {
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setViewport(0);
  }

  connect(ApplicationSettings::self(), SIGNAL(modified()), this, SLOT(updateSettings()));
  loadSettings();

  _editAction = new QAction(tr("Edit"), this);
  _editAction->setShortcut(Qt::Key_E);
//   registerShortcut(_editAction);
  connect(_editAction, SIGNAL(triggered()), this, SLOT(edit()));

  _autoLayoutAction = new QAction(tr("Automatic"), this);
  connect(_autoLayoutAction, SIGNAL(triggered()), this, SLOT(createLayout()));

  _customLayoutAction = new QAction(tr("Custom"), this);
  connect(_customLayoutAction, SIGNAL(triggered()), this, SLOT(createCustomLayout()));

  connect(this, SIGNAL(viewModeChanged(View::ViewMode)), PlotItemManager::self(), SLOT(clearFocusedPlots()));
}


View::~View() {
  QList<PlotItem*> list = PlotItemManager::plotsForView(this);
  foreach (PlotItem *plotItem, list) {
    delete plotItem;
  }
  delete _undoStack;
  delete _layoutBoxItem;
}


bool View::useOpenGL() const {
  return _useOpenGL;
}


void View::setUseOpenGL(bool useOpenGL) {
  //This is an expensive operation...
  if (_useOpenGL == useOpenGL)
    return;

  _useOpenGL = useOpenGL;
  if (useOpenGL) {
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setViewport(new QGLWidget);
  } else {
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setViewport(0);
  }
}


QUndoStack *View::undoStack() const {
  return _undoStack;
}


ViewItem *View::selectedViewItem() const {
  QList<QGraphicsItem*> items = scene()->selectedItems();
  if (items.isEmpty())
    return 0;

  //return the first item
  return qgraphicsitem_cast<ViewItem*>(items.first());
}


void View::save(QXmlStreamWriter &xml) {
  QList<QGraphicsItem*> items = scene()->items();
  xml.writeAttribute("width", QVariant(sceneRect().width()).toString());
  xml.writeAttribute("height", QVariant(sceneRect().height()).toString());
  foreach(QGraphicsItem* viewItem, items) {
    if (!viewItem->parentItem()) {
      qgraphicsitem_cast<ViewItem*>(viewItem)->save(xml);
    }
  }
}


View::ViewMode View::viewMode() const {
  return _viewMode;
}


void View::setViewMode(ViewMode mode) {
  ViewMode oldMode = _viewMode;
  _viewMode = mode;
  emit viewModeChanged(oldMode);
}


View::MouseMode View::mouseMode() const {
  return _mouseMode;
}


void View::setMouseMode(MouseMode mode) {

  //Clear the creation polygons if we're currently
  //in Create mouse mode.
  MouseMode oldMode = _mouseMode;

  if (oldMode == Create) {
    _creationPolygonPress.clear();
    _creationPolygonRelease.clear();
    _creationPolygonMove.clear();
  }

  _mouseMode = mode;

  if (_mouseMode != Create) {
    setCursor(Qt::ArrowCursor);
    setDragMode(QGraphicsView::RubberBandDrag);
  } else {
    setDragMode(QGraphicsView::NoDrag);
  }

  emit mouseModeChanged(oldMode);
}


QPolygonF View::creationPolygon(CreationEvents events) const {
  if (events == View::MousePress)
     return _creationPolygonPress;
  if (events == View::MouseRelease)
     return _creationPolygonRelease;
  if (events == View::MouseMove)
     return _creationPolygonMove;
  return QPolygonF();
}


void View::setShowGrid(bool showGrid) {
  //Don't repaint unless absolutely necessary
  if (_showGrid == showGrid)
    return;

  _showGrid = showGrid;
  invalidateScene(sceneRect(), QGraphicsScene::BackgroundLayer);
}


void View::setGridSpacing(const QSizeF &gridSpacing) {
  //Don't repaint unless absolutely necessary
  if (_gridSpacing == gridSpacing)
    return;

  _gridSpacing = gridSpacing;
  invalidateScene(sceneRect(), QGraphicsScene::BackgroundLayer);
}


QPointF View::snapPoint(const QPointF &point) {
  qreal x = point.x();
  qreal y = point.y();
  if (_snapToGridHorizontal && gridSpacing().width() > 0)
    x -= fmod(point.x(), gridSpacing().width());

  if (_snapToGridVertical && gridSpacing().height() > 0)
    y -= fmod(point.y(), gridSpacing().height());

  return QPointF(x, y);
}


bool View::event(QEvent *event) {
  if (event->type() == QEvent::Shortcut) {
    QShortcutEvent *e = static_cast<QShortcutEvent*>(event);

    QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));
    QList<QGraphicsItem*> list = scene()->items(mousePos);
    if (list.isEmpty()) {
      if (e->key() == _editAction->shortcut()) {
        _editAction->trigger();
      }
    } else {
      foreach (QGraphicsItem *item, list) {
        ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
        if (!viewItem)
          continue;

        // Qt bug: http://bugreports.qt.nokia.com/browse/QTBUG-8188
        // also see PlotRenderItem::hoverEnterEvent, there is a workaround.
        //if (viewItem                         && viewItem->tryShortcut(e->key())) {
        if (viewItem && viewItem->hasFocus() && viewItem->tryShortcut(e->key())) {
          return true;
        }
      }
    }
  } else if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent *e = static_cast<QMouseEvent*>(event);
    if (e->button() == Qt::RightButton) {
      contextMenuEvent();
      return true;
    }
  }

  return QGraphicsView::event(event);
}


bool View::eventFilter(QObject *obj, QEvent *event) {
  if (obj != scene() || _mouseMode != Create)
    return QGraphicsView::eventFilter(obj, event);

  switch (event->type()) {
  case QEvent::GraphicsSceneMousePress:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      if (e->button() == Qt::LeftButton) {
        _creationPolygonPress << snapPoint(e->buttonDownScenePos(Qt::LeftButton));
        emit creationPolygonChanged(MousePress);
        return true; //filter this otherwise something can grab our mouse...
      }
    }
  case QEvent::GraphicsSceneMouseRelease:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      if (e->button() != Qt::LeftButton) break;
      _creationPolygonRelease << snapPoint(e->scenePos());
      emit creationPolygonChanged(MouseRelease);
      break;
    }
  case QEvent::GraphicsSceneMouseMove:
    {
      QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(event);
      _creationPolygonMove << snapPoint(e->scenePos());
      emit creationPolygonChanged(MouseMove);
      break;
    }
  case QEvent::KeyPress:
    {
      QKeyEvent *e = static_cast<QKeyEvent*>(event);
      if (e->key() == Qt::Key_Escape) {
        emit creationPolygonChanged(EscapeEvent);
      }
    }
  default:
    break;
  }

  return QGraphicsView::eventFilter(obj, event);
}


void View::createCustomLayout() {
  bool ok;
  int columns = QInputDialog::getInteger(this, tr("Kst"),
                                      tr("Select Number of Columns"),1, 0,
                                      10, 1, &ok);
  if (ok) {
    createLayout(columns);
  }
}


void View::createLayout(int columns) {
  PlotItemManager::self()->clearFocusedPlots();

  LayoutCommand *layout = new LayoutCommand(new LayoutBoxItem(this));
  layout->createLayout(columns);

  if (_layoutBoxItem) {
    _layoutBoxItem->setEnabled(false);
    delete _layoutBoxItem;
    _layoutBoxItem = 0;
  }
}


void View::appendToLayout(CurvePlacement::Layout layout, ViewItem* item, int columns) {
  AppendLayoutCommand *appendlayout = new AppendLayoutCommand(new LayoutBoxItem(this));
  appendlayout->appendLayout(layout, item, columns);
  if (_layoutBoxItem) {
    LayoutBoxItem *layoutBox = _layoutBoxItem;
    _layoutBoxItem->setEnabled(false);
    delete layoutBox;
  }

}

void View::processResize(QSize size) {

  setPlotBordersDirty(true);
  if (size != sceneRect().size()) {
    QRectF oldSceneRect = sceneRect();

    viewport()->resize(size);

    setSceneRect(QRectF(0.0, 0.0, size.width() - 4.0, size.height() - 4.0));

    updateBrush();

    setCacheMode(QGraphicsView::CacheBackground);

    foreach (QGraphicsItem *item, items()) {
      if (item->parentItem())
        continue;

      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      Q_ASSERT(viewItem);

      viewItem->updateChildGeometry(oldSceneRect, sceneRect());
    }
  }
  updateFont();
}

void View::resizeEvent(QResizeEvent *event) {
  if (event) {
    QGraphicsView::resizeEvent(event);
  }
  setPlotBordersDirty(true);
  if (size() != sceneRect().size()) {
    QRectF oldSceneRect = sceneRect();

    setSceneRect(QRectF(0.0, 0.0, width() - 4.0, height() - 4.0));

    updateBrush();

    setCacheMode(QGraphicsView::CacheBackground);

    foreach (QGraphicsItem *item, items()) {
      if (item->parentItem())
        continue;

      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      Q_ASSERT(viewItem);

      viewItem->updateChildGeometry(oldSceneRect, sceneRect());
    }
  }
  updateFont();
}


void View::forceChildResize(QRectF oldRect, QRectF newRect) {
  foreach (QGraphicsItem *item, items()) {
    if (item->parentItem())
      continue;

    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    Q_ASSERT(viewItem);

    viewItem->updateChildGeometry(oldRect, newRect);
  }
}


void View::updateBrush() {
  if (!ApplicationSettings::self()->gradientStops().empty()) {
    QLinearGradient l(0.0, height() - 4.0, 0.0, 0.0);
    l.setStops(ApplicationSettings::self()->gradientStops());
    setBackgroundBrush(l);
  } else {
    setBackgroundBrush(ApplicationSettings::self()->backgroundBrush());
  }
}


void View::drawBackground(QPainter *painter, const QRectF &rect) {
  if (isPrinting()) {
    QBrush currentBrush(backgroundBrush());
    setBackgroundBrush(Qt::white);
    QGraphicsView::drawBackground(painter, rect);
    setBackgroundBrush(currentBrush);
    return;
  }

  QGraphicsView::drawBackground(painter, rect);

  if (!showGrid())
    return;

  painter->save();
  QColor c = Qt::gray;
  c.setAlphaF(c.alphaF() * 0.2);
  painter->setPen(c);
  // http://techbase.kde.org/Development/Tutorials/Graphics/Performance says
  // setOpacity turns off hardware acceleration.
  // they suggest setting alpha in the color instead, which
  // we have done, above.
  //painter->setOpacity(0.2);

  const QRectF r = sceneRect();
  qreal spacing = gridSpacing().width();

  //FIXME We should probably only draw those lines that intercept rect

  //vertical lines
  qreal x = r.left() + spacing;
  while (x < r.right() && spacing > 0) {
    QLineF line(QPointF(x, r.top()), QPointF(x, r.bottom()));
    painter->drawLine(line);
    x += spacing;
  }

  spacing = gridSpacing().height();

  //horizontal lines
  qreal y = r.top() + spacing;
  while (y < r.bottom() && spacing > 0) {
    QLineF line(QPointF(r.left(), y), QPointF(r.right(), y));
    painter->drawLine(line);
    y += spacing;
  }

  painter->restore();
}


void View::updateSettings() {
  setUseOpenGL(ApplicationSettings::self()->useOpenGL());
  setShowGrid(ApplicationSettings::self()->showGrid());

  setSnapToGrid(ApplicationSettings::self()->snapToGrid());

  setGridSpacing(QSizeF(ApplicationSettings::self()->gridHorizontalSpacing(),
                        ApplicationSettings::self()->gridVerticalSpacing()));

  QFont oldFont = _defaultFont;
  updateFont();
  if (oldFont != _defaultFont) {
    forceChildResize(sceneRect(), sceneRect());
  }
}


void View::loadSettings() {
  setUseOpenGL(ApplicationSettings::self()->useOpenGL());

  setShowGrid(ApplicationSettings::self()->showGrid());

  setSnapToGrid(ApplicationSettings::self()->snapToGrid());

  setGridSpacing(QSizeF(ApplicationSettings::self()->gridHorizontalSpacing(),
                        ApplicationSettings::self()->gridVerticalSpacing()));

  updateFont();
  updateBrush();

}


void View::updateChildGeometry(const QRectF &oldSceneRect) {
  foreach (QGraphicsItem *item, items()) {
    if (item->parentItem())
      continue;

    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    Q_ASSERT(viewItem);

    viewItem->updateChildGeometry(oldSceneRect, sceneRect());
  }
}


// the view's default font size will be the application's default font size,
// scaled by the size of the window compared to the reference plot size.
void View::updateFont() {
  _defaultFont = ApplicationSettings::self()->defaultFont();
}


// returns a font of size pointSize, rescaled if the window size
// is different than the reference window size.
QFont View::defaultFont(double pointSize) const {
  QFont font(_defaultFont);

  qreal fontSize = (qreal)(height() + width()) /
                   (ApplicationSettings::self()->referenceViewHeight() +
                    ApplicationSettings::self()->referenceViewWidth()) *
                   pointSize * _fontRescale;

  if (fontSize < ApplicationSettings::self()->minimumFontSize()) {
    fontSize = ApplicationSettings::self()->minimumFontSize();
  }

  if (fontSize < 0)
    return font;

#ifdef Q_OS_WIN
  // On Windows more and more memory gets allocated when fontsize
  // is to detailed, somewhere some strange caching happens.
  const double fontPrecision = 4;
  fontSize = floor(fontSize * fontPrecision + 0.5) / fontPrecision;
#endif

  font.setPointSizeF(fontSize); 
  
  return font;
}

// Set the font sizes of all plots in the view to a default size, scaled
// by the default global font scale, and the application minimum font scale.
void View::resetPlotFontSizes(double pointSize) {
  if (pointSize < 0.1) {
    pointSize = _dialogDefaults->value("plot/globalFontScale",16.0).toDouble();
  }
  double count = PlotItemManager::self()->plotsForView(this).count();
  double newPointSize = pointSize/sqrt(count) + ApplicationSettings::self()->minimumFontSize();
  if (newPointSize<pointSize) {
    pointSize = newPointSize;
  }
  double legendPointSize = qMax(pointSize*0.6, ApplicationSettings::self()->minimumFontSize());

  foreach(PlotItem* plotItem, PlotItemManager::self()->plotsForView(this)) {
    plotItem->setGlobalFontScale(pointSize);
    plotItem->rightLabelDetails()->setFontScale(pointSize);
    plotItem->leftLabelDetails()->setFontScale(pointSize);
    plotItem->bottomLabelDetails()->setFontScale(pointSize);
    plotItem->topLabelDetails()->setFontScale(pointSize);
    plotItem->numberLabelDetails()->setFontScale(pointSize);
    if (plotItem->showLegend()) {
      plotItem->legend()->setFontScale(legendPointSize);
    }
  }
}

// copy the font settings of the first plotItem in the view into
// plot, then break.
void View::configurePlotFontDefaults(PlotItem *plot) {
  if (plot) {
    bool configured = false;
    foreach(PlotItem* plotItem, PlotItemManager::self()->plotsForView(this)) {
      if (plot != plotItem) {
        configured = true;
        plot->setGlobalFont(plotItem->globalFont());
        plot->setGlobalFontScale(plotItem->globalFontScale());
        plot->setGlobalFontColor(plotItem->globalFontColor());

        plot->leftLabelDetails()->setFontUseGlobal(plotItem->leftLabelDetails()->fontUseGlobal());
        plot->leftLabelDetails()->setFont(plotItem->leftLabelDetails()->font());
        plot->leftLabelDetails()->setFontScale(plotItem->leftLabelDetails()->fontScale());
        plot->leftLabelDetails()->setFontColor(plotItem->leftLabelDetails()->fontColor());

        plot->rightLabelDetails()->setFontUseGlobal(plotItem->rightLabelDetails()->fontUseGlobal());
        plot->rightLabelDetails()->setFont(plotItem->rightLabelDetails()->font());
        plot->rightLabelDetails()->setFontScale(plotItem->rightLabelDetails()->fontScale());
        plot->rightLabelDetails()->setFontColor(plotItem->rightLabelDetails()->fontColor());

        plot->topLabelDetails()->setFontUseGlobal(plotItem->topLabelDetails()->fontUseGlobal());
        plot->topLabelDetails()->setFont(plotItem->topLabelDetails()->font());
        plot->topLabelDetails()->setFontScale(plotItem->topLabelDetails()->fontScale());
        plot->topLabelDetails()->setFontColor(plotItem->topLabelDetails()->fontColor());

        plot->bottomLabelDetails()->setFontUseGlobal(plotItem->bottomLabelDetails()->fontUseGlobal());
        plot->bottomLabelDetails()->setFont(plotItem->bottomLabelDetails()->font());
        plot->bottomLabelDetails()->setFontScale(plotItem->bottomLabelDetails()->fontScale());
        plot->bottomLabelDetails()->setFontColor(plotItem->bottomLabelDetails()->fontColor());

        plot->numberLabelDetails()->setFontUseGlobal(plotItem->numberLabelDetails()->fontUseGlobal());
        plot->numberLabelDetails()->setFont(plotItem->numberLabelDetails()->font());
        plot->numberLabelDetails()->setFontScale(plotItem->numberLabelDetails()->fontScale());
        plot->numberLabelDetails()->setFontColor(plotItem->numberLabelDetails()->fontColor());

        plot->setItemPen(plotItem->pen());
        plot->setItemBrush(plotItem->brush());
        plot->xAxis()->copyProperties(plotItem->xAxis());
        plot->yAxis()->copyProperties(plotItem->yAxis());

        break;
      }
    }
  }
}


void View::contextMenuEvent() {
  QMenu menu;

  addTitle(&menu);
  menu.addAction(_editAction);

  QMenu layoutMenu;
  layoutMenu.setTitle(tr("Cleanup Layout"));
  layoutMenu.addAction(_autoLayoutAction);
  layoutMenu.addAction(_customLayoutAction);
  menu.addMenu(&layoutMenu);

  menu.exec(QCursor::pos());
}


void View::addTitle(QMenu *menu) const {
  QWidgetAction *action = new QWidgetAction(menu);
  action->setEnabled(false);

  QLabel *label = new QLabel(tr("View Menu"), menu);
  label->setAlignment(Qt::AlignCenter);
  label->setStyleSheet("QLabel {"
                       "border-bottom: 2px solid lightGray;"
                       "font: bold large;"
                       "padding: 3px;"
                       "margin: 1px;"
                       "}");
  action->setDefaultWidget(label);
  menu->addAction(action);
}


void View::edit() {
  ViewDialog *editDialog = new ViewDialog(this, kstApp->mainWindow());
  editDialog->show();
}


void View::viewChanged() {
  //kstApp->mainWindow()->document()->setChanged(true);
}

}

// vim: ts=2 sw=2 et
