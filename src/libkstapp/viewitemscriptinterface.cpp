/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "viewitemscriptinterface.h"
#include "viewitem.h"
#include "lineedititem.h"
#include "buttonitem.h"

#include <QStringBuilder>

namespace Kst {

QString LayoutTabSI::doCommand(QString x) {
    if(x.startsWith("getLayout")) {
        x.remove(0,9);
        QSizeF f;
        if(x.contains("Margin")) {
            f=vi->layoutMargins();
        } else if(x.contains("Spacing")) {
            f=vi->layoutSpacing();
        } else {
            return "";
        }
        if(x.contains("Horizontal")) {
            return QString::number(f.width());
        } else if(x.contains("Vertical")) {
            return QString::number(f.height());
        }
    } else if(x.startsWith("setLayout")) {
        x.remove(0,9);
        QSizeF f;
        if(x.contains("Margin")) {
            f=vi->layoutMargins();
        } else if(x.contains("Spacing")) {
            f=vi->layoutSpacing();
        } else {
            return "";
        }
        if(x.contains("Horizontal")) {
            x.remove(0,x.indexOf("("));
            x.remove(x.indexOf(")"),99999);
            f.setWidth(x.toInt());
        } else if(x.contains("Vertical")) {
            x.remove(0,x.indexOf("("));
            x.remove(x.indexOf(")"),99999);
            f.setWidth(x.toInt());
        } else {
            return "";
        }

        if(x.contains("Margin")) {
            vi->setLayoutMargins(f);
        } else if(x.contains("Spacing")) {
            vi->setLayoutSpacing(f);
        }
        return "Done";
    }
    return "";
}

QString FillTabSI::doCommand(QString x) {
    if(!x.startsWith("setFillColor")&&!x.startsWith("setIndexOfFillStyle(")) {
        return "";
    }
    QBrush b=item->brush();
    QColor this_color = (x.startsWith("setFillColor(")&&x!="setFillColor()") ? QColor(x.remove("setFillColor(").remove(')')) : b.color();
    Qt::BrushStyle this_style = b.style();
    if(x.startsWith("setIndexOfFillStyle(")) {
        x.remove("setIndexOfFillStyle(").remove(')');
        Qt::BrushStyle arr[]={Qt::NoBrush,Qt::SolidPattern,Qt::Dense1Pattern,Qt::Dense2Pattern,Qt::Dense3Pattern,Qt::Dense4Pattern,Qt::Dense5Pattern,
                              Qt::Dense6Pattern,Qt::Dense7Pattern,Qt::HorPattern,Qt::VerPattern,Qt::CrossPattern,Qt::BDiagPattern,Qt::FDiagPattern,
                              Qt::DiagCrossPattern};
        this_style=arr[x.toInt()];
    }
    b.setColor(this_color);
    b.setStyle(this_style);
    item->setBrush(b);
    return "Done";
}

QString StrokeTabSI::doCommand(QString x) {
    if(!x.startsWith("setIndexOfStrokeStyle")&&!x.startsWith("setIndexOfStrokeBrushStyle(")&&!x.startsWith("setIndexOfStrokeJoinStyle")&&
            !x.startsWith("setIndexOfStrokeCapStyle")&&!x.startsWith("setStrokeWidth")&&!x.startsWith("setStrokeBrushColor")) {
        return "";
    }

    QPen p=item->pen();
    QBrush b = p.brush();

    Qt::PenStyle this_style = p.style();
    qreal this_width = p.widthF();
    QColor this_brushColor = b.color();
    Qt::BrushStyle this_brushStyle = b.style();

    Qt::PenJoinStyle this_joinStyle = p.joinStyle();
    Qt::PenCapStyle this_capStyle = p.capStyle();

    if(x.startsWith("setIndexOfStrokeStyle(")) {
        x.remove("setIndexOfStrokeStyle(").remove(')');
        Qt::PenStyle arr[]={Qt::NoPen,Qt::SolidLine,Qt::DashLine,Qt::DotLine,Qt::DashDotLine,Qt::DashDotDotLine,Qt::CustomDashLine};
        this_style=arr[x.toInt()];
    } else if(x.startsWith("setIndexOfStrokeBrushStyle(")) {
        x.remove("setIndexOfStrokeBrushStyle(").remove(')');
        Qt::BrushStyle arr[]={Qt::NoBrush,Qt::SolidPattern,Qt::Dense1Pattern,Qt::Dense2Pattern,Qt::Dense3Pattern,Qt::Dense4Pattern,Qt::Dense5Pattern,
                              Qt::Dense6Pattern,Qt::Dense7Pattern,Qt::HorPattern,Qt::VerPattern,Qt::CrossPattern,Qt::BDiagPattern,Qt::FDiagPattern,
                              Qt::DiagCrossPattern};
        this_brushStyle=arr[x.toInt()];
    } else if(x.startsWith("setIndexOfStrokeJoinStyle(")) {
        x.remove("setIndexOfStrokeJoinStyle(").remove(')');
        Qt::PenJoinStyle arr[]={Qt::MiterJoin,Qt::BevelJoin,Qt::RoundJoin,Qt::SvgMiterJoin};
        this_joinStyle=arr[x.toInt()];
    } else if(x.startsWith("setIndexOfStrokeCapStyle(")) {
        x.remove("setIndexOfStrokeCapStyle(").remove(')');
        Qt::PenCapStyle arr[]={Qt::FlatCap,Qt::SquareCap,Qt::RoundCap};
        this_capStyle=arr[x.toInt()];
    } else if(x.startsWith("setStrokeWidth(")) {
        this_width=x.remove("setStrokeWidth(").remove(')').toFloat();
    } else if(x.startsWith("setStrokeBrushColor(")) {
        this_brushColor=QColor(x.remove("setStrokeBrushColor(").remove(')'));
    }

    p.setStyle(this_style);
    p.setWidthF(this_width);

    b.setColor(this_brushColor);
    b.setStyle(this_brushStyle);

    p.setJoinStyle(this_joinStyle);
    p.setCapStyle(this_capStyle);
    p.setBrush(b);
#ifdef Q_OS_WIN32
    if (p.isCosmetic()) {
        p.setWidth(1);
    }
#endif

    item->storePen(p);
    return "Done";
}

/* Note: this is not the cleanest way to parse commands, and shouldn't be coppied. *
 * Look at vectorscriptinterface instead.  Its the way all the cool kids do it.    */
QString DimensionTabSI::doCommand(QString x) {

    QString command = x.left(x.indexOf('('));
    double parameter_d = x.remove(command).remove('(').remove(')').toDouble();
    QString parameter_s = x.remove(command).remove('(').remove(')');

    if (command == "setLockPosToData") {
      if (parameter_s.toLower() == "true") {
        item->setLockPosToData(true);
      } else {
        item->setLockPosToData(false);
      }
    }

    if (command == "updateParent") {
      item->updateViewItemParent();
      return "done";
    }

    if (command == "parentTopLevel") {
      item->updateViewItemParent(true);
      return "done";
    }

    if(command=="fixAspectRatioIsChecked") {
        return item->lockAspectRatio()?"true":"false";
    }

    if (command == "position") {
      QString retval;
      if(item->dataPosLockable() && item->lockPosToData()) {
        retval = QString("(%1, %2)").arg(item->dataRelativeRect().center().x()).arg(item->dataRelativeRect().center().y());
      } else {
        retval = QString("(%1, %2)").arg(item->relativeCenter().x()).arg(item->relativeCenter().y());
      }
      return retval;
    }

    if (command == "dimensions") {
      QString retval;
      if(item->dataPosLockable() && item->lockPosToData()) {
        retval = QString("(%1, %2)").arg(item->dataRelativeRect().width()).arg(item->dataRelativeRect().height());
      } else {
        retval = QString("(%1, %2)").arg(item->relativeWidth()).arg(item->relativeHeight());
      }
      return retval;
    }

    if (command == "setPos") {
      QStringList args = ScriptInterface::getArgs(x);

      item->setItemPos(args.at(0).toDouble(), args.at(1).toDouble());

      QTransform transform;

      item->setTransform(transform);
      item->updateRelativeSize();

      return "Done";
    }

    if (command == "setSize") {
      QStringList args = ScriptInterface::getArgs(x);

      if (args.size() == 1) {
        item->setItemSize(args.at(0).toDouble());
      } else {
        item->setItemSize(args.at(0).toDouble(), args.at(1).toDouble());
      }

      QTransform transform;

      item->setTransform(transform);
      item->updateRelativeSize();

      return "Done";
    }

    if (command == "lockAspectRatio") {
      if (parameter_s.toLower() == "true") {
        item->setLockAspectRatio(true);
      } else {
        item->setLockAspectRatio(false);
      }
      return "Done";
    }

    if (command == "setRotation") {

      QTransform transform;
      transform.rotate(parameter_d);

      item->setTransform(transform);
      item->updateRelativeSize();
      return "Done.";

    }

    if(!command.contains("setRotation")) {
        return "";
    }
    QRectF parentRect = item->parentRect();
    qreal parentWidth = parentRect.width();
    qreal parentHeight = parentRect.height();
    qreal parentX = parentRect.x();
    qreal parentY = parentRect.y();

    qreal aspectRatio;
    if (item->rect().width()==0) {
      aspectRatio = 1.0;
    } else {
      aspectRatio = qreal(item->rect().height()) / qreal(item->rect().width());
    }

    qreal relativeWidth = (command == "setGeoX") ? parameter_d :item->relativeWidth();
    qreal relativeHeight = (command == "setGeoY") ? parameter_d :item->relativeHeight();

    bool fixedAspect = item->lockAspectRatio();
    if (command == "uncheckFixAspectRatio") {
      fixedAspect = false;
    } else if (command == "checkFixAspectRatio") {
      fixedAspect = true;
    }

    qreal width = relativeWidth * parentWidth;
    qreal height;
    if (fixedAspect) {
        height = width * aspectRatio;
        item->setLockAspectRatio(true);
    } else {
        height = relativeHeight * parentHeight;
        item->setLockAspectRatio(false);
    }

    double x0 = parentX + ((command == "setPosX")? parameter_d : item->relativeCenter().x())*parentWidth;
    double y0 = parentY + ((command == "setPosY")? parameter_d : item->relativeCenter().y())*parentHeight;

    item->setPos(x0,y0);

    item->setViewRect(-width/2, -height/2, width, height);

    qreal rotation = (command == "setRotation") ? parameter_d :item->rotationAngle();

    QTransform transform;
    transform.rotate(rotation);

    item->setTransform(transform);
    item->updateRelativeSize();
    return "Done.";
}

ViewItemSI::ViewItemSI(ViewItem *it) : layout(new LayoutTabSI), dim(new DimensionTabSI), fill(new FillTabSI), stroke(new StrokeTabSI) {
    layout->vi=it;
    dim->item=it;
    fill->item=it;
    stroke->item=it;
}

QString ViewItemSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, dim->item);

  if (v.isEmpty()) {
    v=layout->doCommand(x);
  }
  if (v.isEmpty()) {
    v=dim->doCommand(x);
  }
  if (v.isEmpty()) {
    v=fill->doCommand(x);
  }
  if (v.isEmpty()) {
    v=stroke->doCommand(x);
  }
  if (v.isEmpty()&&x.startsWith("setText(")) {
    if(qobject_cast<ButtonItem*>(layout->vi)) {
      qobject_cast<ButtonItem*>(layout->vi)->setText(x.remove("setText(").remove(')'));
      v="Done";
    } else if(qobject_cast<LineEditItem*>(layout->vi)) {
      qobject_cast<LineEditItem*>(layout->vi)->setText(x.remove("setText(").remove(')'));
      v="Done";
    }
  }
  return v.isEmpty()?"No command":v;
}

bool ViewItemSI::isValid() {
    return dim->item;
}

ScriptInterface* ViewItemSI::newBox() {
    BoxItem* bi=new BoxItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}


ScriptInterface* ViewItemSI::newButton() {
    ButtonItem* bi=new ButtonItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* ViewItemSI::newLineEdit() {
    LineEditItem* bi=new LineEditItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* ViewItemSI::newCircle() {
    CircleItem* bi=new CircleItem(kstApp->mainWindow()->tabWidget()->currentView());
    bi->setViewRect(-0.1/2.0, -0.1/2.0, 0.1/2.0, 0.1/2.0);
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* ViewItemSI::newEllipse() {
    EllipseItem* bi=new EllipseItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* ViewItemSI::newLine() {
    LineItem* bi=new LineItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* ViewItemSI::newPicture(QByteArray picf) {
    PictureItem* bi=new PictureItem(kstApp->mainWindow()->tabWidget()->currentView(),QImage(QString::fromLocal8Bit(picf)));
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    bi->setViewRect(0.9,0.9,1.0,1.0,true);
    bi->setVisible(1);
    //bi->updateViewItemParent();
    return new ViewItemSI(bi);
}

#ifndef KST_NO_SVG
ScriptInterface* ViewItemSI::newSvgItem(QByteArray path) {
    SvgItem* bi=new SvgItem(kstApp->mainWindow()->tabWidget()->currentView(),path);
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    bi->setViewRect(0.9,0.9,1.0,1.0,true);
    bi->setVisible(1);
    //bi->updateViewItemParent();
    return new ViewItemSI(bi);
}
#endif



}
