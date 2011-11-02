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

#include "dialogscriptinterface.h"
#include "application.h"
#include "curvedialog.h"
#include "dialoglauncher.h"
#include "equationdialog.h"
#include "histogramdialog.h"
#include "vectordialog.h"
#include "scalardialog.h"
#include "stringdialog.h"
#include "matrixdialog.h"
#include "powerspectrumdialog.h"
#include "csddialog.h"
#include "imagedialog.h"
#include "eventmonitordialog.h"
#include "basicplugindialog.h"
#include "filterfitdialog.h"
#include "datasourcedialog.h"
#include "updatemanager.h"
#include "document.h"
#include "sessionmodel.h"
#include "editablevector.h"

#include "colorbutton.h"
#include <iostream>

//viewitems
#include "arrowitem.h"
#include "boxitem.h"
#include "buttonitem.h"
#include "lineedititem.h"
#include "circleitem.h"
#include "ellipseitem.h"
#include "labelitem.h"
#include "layoutboxitem.h"
#include "legenditem.h"
#include "lineitem.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "sharedaxisboxitem.h"
#include "svgitem.h"
#include "viewitemdialog.h"
#include "plotitemdialog.h"
#include "arrowitemdialog.h"
#include "labelitemdialog.h"
#include "legenditemdialog.h"

#include "curve.h"
#include "equation.h"
#include "vector.h"
#include "matrix.h"
#include "histogram.h"
#include "psd.h"
#include "eventmonitorentry.h"
#include "image.h"
#include "csd.h"
#include "basicplugin.h"
#include "dialog.h"

#include "viewitemscriptinterface.h"
#include "labelscriptinterface.h"
#include "plotscriptinterface.h"
#include "stringscriptinterface.h"

#include "datasourcepluginmanager.h"
#include "viewitemscriptinterface.h"
#include <QAbstractButton>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QStringBuilder>


using namespace std;

#define TEXT(T) (T->property("si").isValid()?T->property("si").toString().toAscii():T->text().toAscii())
#define ORIGTEXT(T) T->text().toAscii()
#define CBTEXT(T) (T->property("si").isValid()?T->property("si").toString().toAscii():"")

namespace Kst {

DialogLauncherSI* DialogLauncherSI::self;

DialogLauncherSI::DialogLauncherSI() {
}


DialogLauncherSI::~DialogLauncherSI() {
}

ScriptInterface* DialogLauncherSI::showViewItemDialog(ViewItem* x) {
    ViewItemDialog* dialog=0;
    if(qobject_cast<PlotItem*>(x)) {
      return new PlotSI(qobject_cast<PlotItem*>(x));
    } else if(qobject_cast<ArrowItem*>(x)) {
      dialog=new ArrowItemDialog(qobject_cast<ArrowItem*>(x),kstApp->mainWindow());
    } else if(qobject_cast<LabelItem*>(x)) {
      return new LabelSI(qobject_cast<LabelItem*>(x));
    } else if(qobject_cast<LegendItem*>(x)) {
      dialog=new LegendItemDialog(qobject_cast<LegendItem*>(x),kstApp->mainWindow());
    } else {
      return new ViewItemSI(qobject_cast<ViewItem*>(x));
    }
    dialog->hide();
    return new DialogSI(dialog,x);
}

ScriptInterface* DialogLauncherSI::newArrow() {
    ArrowItem* bi=new ArrowItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return showViewItemDialog(bi);
}

ScriptInterface* DialogLauncherSI::newBox() {
    BoxItem* bi=new BoxItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newButton() {
    ButtonItem* bi=new ButtonItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newLineEdit() {
    LineEditItem* bi=new LineEditItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newCircle() {
    CircleItem* bi=new CircleItem(kstApp->mainWindow()->tabWidget()->currentView());
    bi->setViewRect(-0.1/2.0, -0.1/2.0, 0.1/2.0, 0.1/2.0);
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newEllipse() {
    EllipseItem* bi=new EllipseItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newLabel() {
    LabelItem* bi=new LabelItem(kstApp->mainWindow()->tabWidget()->currentView(),"");
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new LabelSI(bi);
}

ScriptInterface* DialogLauncherSI::newLine() {
    LineItem* bi=new LineItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newPicture(QByteArray picf) {
    PictureItem* bi=new PictureItem(kstApp->mainWindow()->tabWidget()->currentView(),QImage(QString::fromLocal8Bit(picf)));
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    bi->setViewRect(0.9,0.9,1.0,1.0,true);
    bi->setVisible(1);
    bi->updateViewItemParent();
    return new ViewItemSI(bi);
}

ScriptInterface* DialogLauncherSI::newPlot() {
    PlotItem* bi=new PlotItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return new PlotSI(bi);
    //return showViewItemDialog(bi);
}

ScriptInterface* DialogLauncherSI::newSharedAxisBox() {
    SharedAxisBoxItem* bi=new SharedAxisBoxItem(kstApp->mainWindow()->tabWidget()->currentView());
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    return showViewItemDialog(bi);
}

ScriptInterface* DialogLauncherSI::newSvgItem(QByteArray path) {
    SvgItem* bi=new SvgItem(kstApp->mainWindow()->tabWidget()->currentView(),path);
    kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
    bi->setViewRect(0.9,0.9,1.0,1.0,true);
    bi->setVisible(1);
    bi->updateViewItemParent();
    return new ViewItemSI(bi);
}

DialogSI* DialogLauncherSI::showVectorDialog(QByteArray &vectorname, ObjectPtr objectPtr) {
    VectorDialog *dialog = new VectorDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    if (!vectorname.isEmpty()) {
        dialog->setField(vectorname);
    }
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showMatrixDialog(QByteArray &, ObjectPtr objectPtr) {
    MatrixDialog *dialog = new MatrixDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showScalarDialog(QByteArray &, ObjectPtr objectPtr) {
    ScalarDialog *dialog = new ScalarDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showStringDialog(QByteArray &, ObjectPtr objectPtr) {
    StringDialog *dialog = new StringDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    dialog->hide();
    return new DialogSI(dialog);
}

ScriptInterface* DialogLauncherSI::showStringGenDialog(QByteArray &, ObjectPtr objectPtr,ObjectStore*store) {
    if(!objectPtr) {
        objectPtr=ObjectPtr(store->createObject<String>());
    }

    return new StringGenSI(kst_cast<String>(objectPtr));
}


DialogSI* DialogLauncherSI::showCurveDialog(ObjectPtr objectPtr, VectorPtr vector) {
    CurveDialog *dialog = new CurveDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    if (vector) {
        dialog->setVector(vector);
    }
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showMultiCurveDialog(QList<ObjectPtr> curves) {
    CurveDialog *dialog = new CurveDialog(curves.at(0), kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    dialog->editMultiple(curves);
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showImageDialog(ObjectPtr objectPtr, MatrixPtr matrix) {
    ImageDialog *dialog = new ImageDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    if (matrix) {
        dialog->setMatrix(matrix);
    }
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showMultiImageDialog(QList<ObjectPtr> images) {
    ImageDialog *dialog = new ImageDialog(images.at(0), kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    dialog->editMultiple(images);
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showEquationDialog(ObjectPtr objectPtr) {
    EquationDialog *dialog = new EquationDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showHistogramDialog(ObjectPtr objectPtr, VectorPtr vector) {
    HistogramDialog *dialog = new HistogramDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    if (vector) {
        dialog->setVector(vector);
    }
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showPowerSpectrumDialog(ObjectPtr objectPtr, VectorPtr vector) {
    PowerSpectrumDialog *dialog = new PowerSpectrumDialog(objectPtr, kstApp->mainWindow());

    /*you shouln't need mainWindow() but someone did a hack... *_*/
    if (vector) {
        dialog->setVector(vector);
    }
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showCSDDialog(ObjectPtr objectPtr, VectorPtr vector) {
    CSDDialog *dialog = new CSDDialog(objectPtr, kstApp->mainWindow());
    /*you shouln't need mainWindow() but someone did a hack... *_*/

    if (vector) {
        dialog->setVector(vector);
    }
    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showEventMonitorDialog(ObjectPtr objectPtr) {
    EventMonitorDialog *dialog = new EventMonitorDialog(objectPtr, kstApp->mainWindow());
    /*you shouln't need mainWindow() but someone did a hack... *_*/

    dialog->hide();
    return new DialogSI(dialog);
}


DialogSI* DialogLauncherSI::showBasicPluginDialog(QByteArray pluginName_ba, ObjectPtr objectPtr, VectorPtr vectorX, VectorPtr vectorY, PlotItemInterface *plotItem) {
    QString pluginName=pluginName_ba;
    if (DataObject::pluginType(pluginName) == DataObjectPluginInterface::Generic) {
        BasicPluginDialog *dialog = new BasicPluginDialog(pluginName, objectPtr, kstApp->mainWindow());
        dialog->hide();

        return new DialogSI(dialog);
    } else {
        FilterFitDialog *dialog = new FilterFitDialog(pluginName, objectPtr, kstApp->mainWindow());

        if (!objectPtr) {
            if (vectorX) {
                dialog->setVectorX(vectorX);
            }
            if (vectorY) {
                dialog->setVectorY(vectorY);
            }
            if (plotItem) {
                dialog->setPlotMode((PlotItem*)plotItem);
            }
        }
        dialog->hide();
        return new DialogSI(dialog);
    }
}

DialogSI* DialogLauncherSI::showMultiObjectDialog(QList<ObjectPtr> objects) {
    if (objects.count() > 0) {
        if (CurvePtr curve = kst_cast<Curve>(objects.at(0))) {
            return self->showMultiCurveDialog(objects);
        } else if (ImagePtr image = kst_cast<Image>(objects.at(0))) {
            return self->showMultiImageDialog(objects);
        }
    }
    return 0;
}

DialogSI* DialogLauncherSI::showObjectDialog(ObjectPtr objectPtr) {
    if (CurvePtr curve = kst_cast<Curve>(objectPtr)) {
        return self->showCurveDialog(curve);
    } else if (EquationPtr equation = kst_cast<Equation>(objectPtr)) {
        return self->showEquationDialog(equation);
    } else if (HistogramPtr histogram = kst_cast<Histogram>(objectPtr)) {
        return self->showHistogramDialog(histogram);
    } else if (PSDPtr psd = kst_cast<PSD>(objectPtr)) {
        return self->showPowerSpectrumDialog(psd);
    } else if (EventMonitorEntryPtr eventMonitorEntry = kst_cast<EventMonitorEntry>(objectPtr)) {
        return self->showEventMonitorDialog(eventMonitorEntry);
    } else if (ImagePtr image = kst_cast<Image>(objectPtr)) {
        return self->showImageDialog(image);
    } else if (CSDPtr csd = kst_cast<CSD>(objectPtr)) {
        return self->showCSDDialog(csd);
    } else if (VectorPtr vector = kst_cast<Vector>(objectPtr)) {
        QByteArray tmp;
        return self->showVectorDialog(tmp, vector);
    } else if (MatrixPtr matrix = kst_cast<Matrix>(objectPtr)) {
        QByteArray tmp;
        return self->showMatrixDialog(tmp, matrix);
    } else if (BasicPluginPtr plugin = kst_cast<BasicPlugin>(objectPtr)) {
        return self->showBasicPluginDialog(plugin->pluginName().toAscii(), plugin);
    }  else if (ScalarPtr scalar = kst_cast<Scalar>(objectPtr)) {
        QByteArray tmp;
        return self->showScalarDialog(tmp, scalar);
    } else if (StringPtr string = kst_cast<String>(objectPtr)) {
        QByteArray tmp;
        return self->showStringDialog(tmp, string);
    }
    return 0;
}

QString DialogSI::toCamelCase(QString x)
{
    x.replace('&',"");
    x.replace(':',"");
    x=x.toLower();
    while(x.contains(' ')) {
        int y=x.indexOf(' ');
        x.remove(y,1);
        if(y!=x.size()) {
            x[y]=x[y].toUpper();
        }
    }
    if(x.size()) {
        x[0]=x[0].toLower();
    }
    return x;
}

DialogSI::DialogSI(QWidget* t,ViewItem*v) : _dialog(t), _valid(1), _subDialog(0),_vi(v)
{
    QObjectList c=t->children();
    for(int i=0;i<c.size();i++) {
        QLabel* label=qobject_cast<QLabel*>(c[i]);
        if(label&&label->buddy()) {
            if(label->buddy()->inherits("QLineEdit")||label->buddy()->inherits("QSpinBox")||
                    label->buddy()->inherits("QDoubleSpinBox")||label->buddy()->inherits("QTextEdit")||
                    label->buddy()->inherits("QComboBox")) {
                _labelsWithBuddies<<label;
                _camelTextLabels.push_back(toCamelCase(TEXT(_labelsWithBuddies.back())).toAscii());
                if(_labelsWithBuddies.back()->property("si").isNull()) {
//                    std::cerr<<qPrintable(_labelsWithBuddies.back()->objectName())<<"->setProperty(\"si\",\""<<qPrintable(QString(TEXT(_labelsWithBuddies.back())))<<"\");\n";
                }
                else if(_labelsWithBuddies.back()->property("si").toString()!=TEXT(_labelsWithBuddies.back())) {
                    qDebug()<<"Warning:"<<ORIGTEXT(_labelsWithBuddies.back())<<"!="<<_labelsWithBuddies.back()->property("si").toString();
                }
                if(_camelTextLabels.size()) {
                    _camelTextLabels.back()[0]=QString(QString(_camelTextLabels.back())[0].toUpper())[0].toAscii();
                }
            }
        }

        if(qobject_cast<ColorButton*>(c[i])) {
            if(!TEXT(qobject_cast<ColorButton*>(c[i])).isEmpty()) {
                _colorButtons<<qobject_cast<ColorButton*>(c[i]);
                if(_colorButtons.back()->property("si").isNull()) {
//                    std::cerr<<qPrintable(_colorButtons.back()->objectName())<<"->setProperty(\"si\",\""<<qPrintable(QString(TEXT(_colorButtons.back())))<<"\");\n";
                }
                else if(_colorButtons.back()->property("si").toString()!=TEXT(_colorButtons.back())) {
                    qDebug()<<"Warning:"<<ORIGTEXT(_colorButtons.back())<<"!="<<_colorButtons.back()->property("si").toString();
                }
                _camelColorButtons.push_back(toCamelCase(TEXT(_colorButtons.back())).toAscii());
                if(_camelColorButtons.size()) {
                    _camelColorButtons.back()[0]=QString(QString(_camelColorButtons.back())[0].toUpper())[0].toAscii();
                }
            } else {
                qDebug()<<"Warning: ColorButton with no text!!"<<c[i]->objectName();
            }
        } else if(qobject_cast<QAbstractButton*>(c.at(i))) {
            if(!TEXT(qobject_cast<QAbstractButton*>(c[i])).isEmpty()) {
                _buttons<<qobject_cast<QAbstractButton*>(c[i]);
                if(_buttons.back()->property("si").isNull()) {
//                    std::cerr<<qPrintable(_buttons.back()->objectName())<<"->setProperty(\"si\",\""<<qPrintable(QString(TEXT(_buttons.back())))<<"\");\n";
                }
                else if(_buttons.back()->property("si").toString()!=TEXT(_buttons.back())) {
                    qDebug()<<"Warning:"<<ORIGTEXT(_buttons.back())<<"!="<<_buttons.back()->property("si").toString();
                }
                _camelButtons.push_back(toCamelCase(TEXT(_buttons.back())).toAscii());
                _camelButtonsRev.push_back(toCamelCase(TEXT(_buttons.back())).toAscii());
                if(_camelButtons.size()) {
                    _camelButtons.back()[0]=QString(QString(_camelButtons.back())[0].toUpper())[0].toAscii();
                }
            }
        }

        if(c[i]->inherits("QGroupBox")) {
            QGroupBox* gb=qobject_cast<QGroupBox*>(c[i]);
            if(gb->isCheckable()) {
                _groupBoxes<<gb;
                if(_groupBoxes.back()->property("si").isNull()) {
//                    std::cerr<<qPrintable(_groupBoxes.back()->objectName())<<"->setProperty(\"si\",\""<<qPrintable(_groupBoxes.back()->title())<<"\");\n";
                }
                else if(_groupBoxes.back()->property("si").toString()!=_groupBoxes.back()->title()) {
                    qDebug()<<"Warning:"<<_groupBoxes.back()->title()<<"!="<<_groupBoxes.back()->property("si").toString();
                }
            }
        }

        c<<c.at(i)->children();
    }
    for(int i=0;i<c.size();i++) {
        if(qobject_cast<QComboBox*>(c[i])) {
            bool ok=1;
            for(int j=0;j<_labelsWithBuddies.size();j++) {
                if(_labelsWithBuddies[j]->buddy()==c[i]) {
                    ok=0;
                    break;
                }
            }
            if(ok) {
                _buddylessComboBoxes.push_back(qobject_cast<QComboBox*>(c[i]));
//                if(_buddylessComboBoxes.back()->property("si").isNull()) {
//                    std::cerr<<qPrintable(_buddylessComboBoxes.back()->objectName())<<"->setProperty(\"si\",\""<<qPrintable(QString(CBTEXT(_buddylessComboBoxes.back())))<<"\");\n";
//                }
//                else if(!CBTEXT(_buddylessComboBoxes.back()).size()) {
//                    qDebug()<<"Buddyless combobox"<<_buddylessComboBoxes.back()->objectName()<<"has no name."
//                               "You must add the property 'si' with a useful script name to every UI item inside a dialog for it to appear in kstScript...";
//                }
            }
        }
    }
    connect(t,SIGNAL(destroyed()),this,SLOT(invalidate()));

    /////////////////////////////////////////////////////////////////////////////
    // Initialize our maps. We map texts to functions, and texts to objects. On
    // doCommand(...) we call the function the text is mapped to with the object as
    // the second parameter with this as the this value.

    if(qobject_cast<PictureItem*>(_vi)) {
        _fnMap.insert("setPicture()",&DialogSI::setPicture);
        _objMap.insert("setPicture()",0);
    }
    for(int i=0;i<_labelsWithBuddies.size();i++) {
        if(_labelsWithBuddies[i]->buddy()->inherits("QComboBox")) {
            QByteArray c=toCamelCase("set index of "+TEXT(_labelsWithBuddies[i])).toAscii()+"()";
            _fnMap.insert(c,&DialogSI::setComboBoxIndex);
            _objMap.insert(c,_labelsWithBuddies[i]->buddy());
        }
        QByteArray c=toCamelCase("set "+TEXT(_labelsWithBuddies[i])).toAscii()+"()";
        QByteArray g=toCamelCase("get "+TEXT(_labelsWithBuddies[i])).toAscii()+"()";
        if(_labelsWithBuddies[i]->buddy()->inherits("QLineEdit")) {
            _fnMap.insert(c,&DialogSI::setLineEditText);
            _fnMap.insert(g,&DialogSI::getLineEditText);
        } else if(_labelsWithBuddies[i]->buddy()->inherits("QTextEdit")) {
            _fnMap.insert(c,&DialogSI::setTextEditText);
            _fnMap.insert(g,&DialogSI::getTextEditText);
        } else if(_labelsWithBuddies[i]->buddy()->inherits("QSpinBox")) {
            _fnMap.insert(c,&DialogSI::setSpinBoxValue);
            _fnMap.insert(g,&DialogSI::getSpinBoxValue);
        } else if(_labelsWithBuddies[i]->buddy()->inherits("QDoubleSpinBox")) {
            _fnMap.insert(c,&DialogSI::setDoubleSpinBoxValue);
            _fnMap.insert(g,&DialogSI::getDoubleSpinBoxValue);
        } else if(_labelsWithBuddies[i]->buddy()->inherits("QComboBox")) {
            _fnMap.insert(c,&DialogSI::setComboBoxEditValue);
            _fnMap.insert(g,&DialogSI::getComboBoxEditValue);
        } else {
            qDebug()<<"Label buddy not supported... Dialog script interface broken.";
            _fnMap.insert(c,&DialogSI::noSuchFn);
            _fnMap.insert(g,&DialogSI::noSuchFn);
        }
        _objMap.insert(c,_labelsWithBuddies[i]->buddy());
        _objMap.insert(g,_labelsWithBuddies[i]->buddy());

    }
    for(int i=0;i<_buttons.size();i++) {
        if(_buttons[i]->isCheckable()) {
            QByteArray c=toCamelCase("check "+TEXT(_buttons[i])).toAscii()+"()";
            _fnMap.insert(c,&DialogSI::checkButton);
            _objMap.insert(c,_buttons[i]);
            c=toCamelCase("uncheck "+TEXT(_buttons[i])).toAscii()+"()";
            _fnMap.insert(c,&DialogSI::uncheckButton);
            _objMap.insert(c,_buttons[i]);
            c=toCamelCase(TEXT(_buttons[i])+" is checked").toAscii()+"()";
            _fnMap.insert(c,&DialogSI::getButtonIsChecked);
            _objMap.insert(c,_buttons[i]);
        } else {
            QByteArray c=toCamelCase("press "+TEXT(_buttons[i])).toAscii()+"()";
            _fnMap.insert(c,&DialogSI::pressButton);
            _objMap.insert(c,_buttons[i]);
        }
    }
    for(int i=0;i<_groupBoxes.size();i++) {
        QByteArray c=toCamelCase("check "+_groupBoxes[i]->title()).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::checkGroupBox);
        _objMap.insert(c,_groupBoxes[i]);
        c=toCamelCase("uncheck "+_groupBoxes[i]->title()).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::uncheckGroupBox);
        _objMap.insert(c,_groupBoxes[i]);
        c=toCamelCase(_groupBoxes[i]->title()+" is checked").toAscii()+"()";
        _fnMap.insert(c,&DialogSI::getGroupBoxIsChecked);
        _objMap.insert(c,_groupBoxes[i]);
    }

    for(int i=0;i<_colorButtons.size();i++) {
        QByteArray c=toCamelCase("set "+TEXT(_colorButtons[i])).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::setColour);
        _objMap.insert(c,_colorButtons[i]);
        c=toCamelCase("get "+TEXT(_colorButtons[i])).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::getColour);
        _objMap.insert(c,_colorButtons[i]);
    }

    for(int i=0;i<_buddylessComboBoxes.size();i++) {
        QByteArray c=toCamelCase("set index of "+CBTEXT(_buddylessComboBoxes[i])).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::setComboBoxIndex);
        _objMap.insert(c,_buddylessComboBoxes[i]);
        c=toCamelCase("get index of "+CBTEXT(_buddylessComboBoxes[i])).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::getComboBoxIndex);
        _objMap.insert(c,_buddylessComboBoxes[i]);

        c=toCamelCase("set "+CBTEXT(_buddylessComboBoxes[i])).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::setComboBoxEditValue);
        _objMap.insert(c,_buddylessComboBoxes[i]);
        c=toCamelCase("get "+CBTEXT(_buddylessComboBoxes[i])).toAscii()+"()";
        _fnMap.insert(c,&DialogSI::getComboBoxEditValue);
        _objMap.insert(c,_buddylessComboBoxes[i]);
    }

}

DialogSI::~DialogSI()
{
    if(_valid) {
        Dialog* d=qobject_cast<Dialog*>(_dialog);
        DataSourceDialog* dsd=qobject_cast<DataSourceDialog*>(_dialog); //why isn't this a Kst::Dialog!?
        if(d) {
            if(d->buttonBox()->button(QDialogButtonBox::Ok)&&
                    d->buttonBox()->button(QDialogButtonBox::Ok)->isEnabled()) {
                d->buttonBox()->button(QDialogButtonBox::Ok)->animateClick(0);
                qApp->processEvents();
            } else if(d->buttonBox()->button(QDialogButtonBox::Cancel)&&
                      d->buttonBox()->button(QDialogButtonBox::Cancel)->isEnabled()) {
                d->buttonBox()->button(QDialogButtonBox::Cancel)->animateClick(0);
                qApp->processEvents();
            } else {
                qDebug()<<"Warning: no 'ok' button on dialog!!! Script may have had no effect.";
            }
        } else if(dsd) {
            if(dsd->_buttonBox->button(QDialogButtonBox::Ok)&&
                    dsd->_buttonBox->button(QDialogButtonBox::Ok)->isEnabled()) {
                dsd->_buttonBox->button(QDialogButtonBox::Ok)->animateClick(0);
                qApp->processEvents();
            } else if(dsd->_buttonBox->button(QDialogButtonBox::Cancel)&&
                      dsd->_buttonBox->button(QDialogButtonBox::Cancel)->isEnabled()) {
                dsd->_buttonBox->button(QDialogButtonBox::Cancel)->animateClick(0);
                qApp->processEvents();
            } else {
                qDebug()<<"Warning: no 'ok' button on dsdialog!!! Script may have had no effect.";
            }
        }
    }
}

QByteArray DialogSI::getHandle() {
    QByteArray x;
    if(qobject_cast<DataDialog*>(_dialog)&&qobject_cast<DataDialog*>(_dialog)->dataObject()) {
        return ("Finished editing "%qobject_cast<DataDialog*>(_dialog)->dataObject()->Name()).toLatin1();
    } else if(qobject_cast<ViewItemDialog*>(_dialog)&&
              qobject_cast<ViewItemDialog*>(_dialog)->_item) {
        return ("Finished editing "%qobject_cast<ViewItemDialog*>(_dialog)->_item->Name()).toLatin1();
    } else {
        if(qobject_cast<DataDialog*>(_dialog)) {
            ObjectPtr z=qobject_cast<DataDialog*>(_dialog)->createNewDataObject();
            if(z.isPtrValid()) {
                qobject_cast<DataDialog*>(_dialog)->setDataObject(z);
                return ("(created) Finished editing "%z->Name()).toLatin1();
            } else {
                return "No handle returned.";
            }
        }
    }
    return x;
}

void DialogSI::invalidate()
{
    _valid=0;
}

QByteArrayList DialogSI::commands()
{
    if(!_valid) {
        QByteArrayList b;
        b<<"INVALID_DIALOG";
        return b;
    }
    if(_subDialog) {
        QByteArrayList b;
        b<<"leaveSub()"<<_subDialog->commands();
        return b;
    }

    QByteArrayList ret;
    for(int i=0;i<_fnMap.size();i++) {
        ret<<_fnMap.keys()[i];
    }
    return ret;
}

QString DialogSI::doCommand(QString x)
{
    if(!_valid) {
        return "Dialog is invalid";
    }
    if(_subDialog) {
        if(x=="leaveSub()") {
            delete _subDialog;
            _subDialog=0;
            return "Done.";
        }
        return _subDialog->doCommand(x);
    }
    QByteArray x_hack=x.toAscii();
    x_hack.remove(x_hack.indexOf('('),9999999);
    x_hack+="()";
    DialogSIMemberFn fn = _fnMap.value(x_hack,&DialogSI::noSuchFn);
    QWidget* obj = _objMap.value(x_hack,0);
    x_hack=x.toAscii(); // (!!)
    return CALL_MEMBER_FN(*this,fn)(x_hack,obj);
}

void DialogSI::initDataSourceDialogSI()
{
    ScalarDialog* scd=qobject_cast<ScalarDialog*>(_dialog);
    MatrixDialog* md=qobject_cast<MatrixDialog*>(_dialog);
    VectorDialog* vd=qobject_cast<VectorDialog*>(_dialog);
    StringDialog* std=qobject_cast<StringDialog*>(_dialog);

    if(scd&&scd->_scalarTab->_configure->isEnabled()) {
        _subDialog=new DialogSI(new DataSourceDialog(scd->editMode(), scd->_scalarTab->_dataSource, _dialog));
    } else if(md&&md->_matrixTab->_configure->isEnabled()) {
        _subDialog=new DialogSI(new DataSourceDialog(md->editMode(), md->_matrixTab->_dataSource, _dialog));
    } else if(vd&&vd->_vectorTab->_configure->isEnabled()) {
        _subDialog=new DialogSI(new DataSourceDialog(vd->editMode(), vd->_vectorTab->_dataSource, _dialog));
    } else if(std&&std->_stringTab->_configure->isEnabled()) {
        _subDialog=new DialogSI(new DataSourceDialog(std->editMode(), std->_stringTab->_dataSource, _dialog));
    }

}


void DialogSI::waitForValidation() {
  Dialog *d = qobject_cast<Dialog*>(_dialog);
  if (d) {
    d->waitForValidation();
  }
}


QByteArray DialogSI::pressConfigure(QByteArray&y,QWidget*obj){
    waitForValidation();
    initDataSourceDialogSI();
    return "Done";
}

QByteArray DialogSI::setPicture(QByteArray&y,QWidget*obj){
    waitForValidation();
    y.remove(0,y.indexOf("(")+1);
    if(y.contains(')')) {
        y.remove(y.lastIndexOf(')'),1);
    }
    QString d(y);
    QImage qimage(d);
    if(qimage.isNull()) {
        return "No such thing.";
    }
    static_cast<PictureItem*>(_vi)->setImage(qimage);
    static_cast<PictureItem*>(_vi)->update();
    return "Done.";
}

QByteArray DialogSI::setLineEditText(QByteArray&y,QWidget*obj) {
    waitForValidation();
    if(!qobject_cast<QLineEdit*>(obj)->isEnabled()) {
        return "Option disabled.";
    }
    y.remove(0,y.indexOf("(")+1);
    y.remove(y.lastIndexOf(')'),9999999);
    qobject_cast<QLineEdit*>(obj)->setText(y);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::setTextEditText(QByteArray&y,QWidget*obj){
    waitForValidation();
    if(!qobject_cast<QTextEdit*>(obj)->isEnabled()) {
        return "Option disabled.";
    }
    y.remove(0,y.indexOf("(")+1);
    y.remove(y.lastIndexOf(')'),9999999);
    qobject_cast<QTextEdit*>(obj)->setPlainText(y);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::setSpinBoxValue(QByteArray&y,QWidget*obj){
    waitForValidation();
    if(!qobject_cast<QSpinBox*>(obj)->isEnabled()) {
        return "Option disabled.";
    }
    y.remove(0,y.indexOf("(")+1);
    y.remove(y.lastIndexOf(')'),9999999);
    bool ok;
    int z=y.toInt(&ok);
    if(ok) {
        qobject_cast<QSpinBox*>(obj)->setValue(z);
        qApp->processEvents();
    }
    return "Done";
}

QByteArray DialogSI::setDoubleSpinBoxValue(QByteArray&y,QWidget*obj){
    waitForValidation();
    if(!qobject_cast<QDoubleSpinBox*>(obj)->isEnabled()) {
        return "Option disabled.";
    }
    y.remove(0,y.indexOf("(")+1);
    if(y.contains(')')) {
        y.remove(y.lastIndexOf(')'),999999);
    }
    bool ok;
    float z=y.toDouble(&ok);
    if(ok) {
        qobject_cast<QDoubleSpinBox*>(obj)->setValue(z);
        qApp->processEvents();
    } else {
        return "invalid number";
    }
    return "Done";
}

QByteArray DialogSI::setComboBoxEditValue(QByteArray&y,QWidget*obj){
    waitForValidation();
    if(!qobject_cast<QComboBox*>(obj)->isEnabled()) {
        return "Option disabled.";
    }
    QComboBox* cb=qobject_cast<QComboBox*>(obj);
    if(!cb->isEditable()) {
        return "Not editable.";
    }
    y.remove(0,y.indexOf("(")+1);
    y.remove(y.lastIndexOf(')'),9999999);
    cb->setEditText(y);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::setComboBoxIndex(QByteArray&y,QWidget*obj){
    waitForValidation();
    if(!obj->isEnabled()) {
      return "Option disabled.";
    }
    QString x=y;
    x.remove(0,y.indexOf("(")+1);
    if(x.contains(')')) {
      x.remove(x.lastIndexOf(')'),INT_MAX);
    }
    bool ok;
    int a=x.toInt(&ok);
    if(ok) {
        qobject_cast<QComboBox*>(obj)->setCurrentIndex(a);
        qApp->processEvents();
    }
    return "Done";
}

QByteArray DialogSI::getComboBoxIndex(QByteArray&y,QWidget*obj){
    return QByteArray::number(qobject_cast<QComboBox*>(obj)->currentIndex());
}

QByteArray DialogSI::getLineEditText(QByteArray&y,QWidget*obj){
    return qobject_cast<QLineEdit*>(obj)->text().toAscii();
}

QByteArray DialogSI::getTextEditText(QByteArray&y,QWidget*obj){
    return dynamic_cast<QTextEdit*>(obj)->toPlainText().toAscii();
}

QByteArray DialogSI::getSpinBoxValue(QByteArray&y,QWidget*obj){
    return QByteArray::number(qobject_cast<QSpinBox*>(obj)->value());
}

QByteArray DialogSI::getDoubleSpinBoxValue(QByteArray&y,QWidget*obj){
    return QByteArray::number(qobject_cast<QDoubleSpinBox*>(obj)->value());
}

QByteArray DialogSI::getComboBoxEditValue(QByteArray&y,QWidget*obj){
    if(qobject_cast<QComboBox*>(obj)->currentText().isEmpty()) {
        return "Index "+QByteArray::number(qobject_cast<QComboBox*>(obj)->currentIndex());
    } else {
        return qobject_cast<QComboBox*>(obj)->currentText().toAscii();
    }
}

QByteArray DialogSI::checkButton(QByteArray&y,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    if(!qobject_cast<QAbstractButton*>(obj)->isChecked()) qobject_cast<QAbstractButton*>(obj)->animateClick(0);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::uncheckButton(QByteArray&y,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    if(qobject_cast<QAbstractButton*>(obj)->isChecked()) qobject_cast<QAbstractButton*>(obj)->animateClick(0);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::getButtonIsChecked(QByteArray&y,QWidget*obj){
    if(!qobject_cast<QAbstractButton*>(obj)->isEnabled()) return "Option disabled.";
    return qobject_cast<QAbstractButton*>(obj)->isChecked()?"true":"false";
}

QByteArray DialogSI::pressButton(QByteArray&y,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    qobject_cast<QAbstractButton*>(obj)->animateClick(0);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::checkGroupBox(QByteArray&y,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    qobject_cast<QGroupBox*>(obj)->setChecked(1);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::uncheckGroupBox(QByteArray&y,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    qobject_cast<QGroupBox*>(obj)->setChecked(0);
    qApp->processEvents();
    return "Done";
}

QByteArray DialogSI::getGroupBoxIsChecked(QByteArray&y,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    return qobject_cast<QGroupBox*>(obj)->isChecked()?"true":"false";
}

QByteArray DialogSI::getColour(QByteArray&x,QWidget*obj){
    return qobject_cast<ColorButton*>(obj)->color().name().toAscii();

}

QByteArray DialogSI::setColour(QByteArray&x,QWidget*obj){
    if(!obj->isEnabled()) return "Option disabled.";
    x.remove(0,x.indexOf('(')+1);
    if(x.contains(')')) {
        x.remove(x.lastIndexOf(')'),99999);
    }
    QString z=x;
    QColor y(z);
    bool ok;
    ok=y.isValid();
    if(ok) {
        qobject_cast<ColorButton*>(obj)->setColor(y);
        qApp->processEvents();
    }
    return "Done";
}

}
