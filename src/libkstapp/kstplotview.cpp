#include "kstplotview.h"
#include "kstmainwindow.h"
#include "kstapplication.h"

#include <QGraphicsScene>

KstPlotView::KstPlotView()
    : QGraphicsView(kstApp->mainWindow()) {
 QGraphicsScene *scene = new QGraphicsScene(this);
 scene->addText("Hello, Kst Plot!");
 setScene(scene);
}


KstPlotView::~KstPlotView() {
}

#include "kstplotview.moc"

// vim: ts=2 sw=2 et
