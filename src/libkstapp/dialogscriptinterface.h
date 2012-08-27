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

#include "objectstore.h"
#include "editablevector.h"
#include "scriptinterface.h"
#include "updatemanager.h"
#include <QLabel>

#ifndef DIALOGSCRIPTINTERFACE_H
#define DIALOGSCRIPTINTERFACE_H

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#endif

class QLocalServer;
class QLocalSocket;
class QComboBox;
class QGroupBox;
class QAbstractButton;

typedef QList<QByteArray> QByteArrayList;

namespace Kst{

class Dialog;
class DialogSI;
class PlotItemInterface;
class ViewItem;
class ColorButton;
class ScriptInterface;

/** Based on DialogLauncherGUI */
class DialogLauncherSI {
  public:
    DialogLauncherSI();
    virtual ~DialogLauncherSI();

    static DialogLauncherSI* self;

    //primitives
    DialogSI* showVectorDialog(QByteArray &vectorname, ObjectPtr objectPtr = 0);
    DialogSI* showMatrixDialog(QByteArray &matrixName, ObjectPtr objectPtr = 0);
    DialogSI* showScalarDialog(QByteArray &scalarname, ObjectPtr objectPtr = 0);
    DialogSI* showStringDialog(QByteArray &stringname, ObjectPtr objectPtr = 0);
    ScriptInterface* showStringGenDialog(QByteArray &, ObjectPtr objectPtr,ObjectStore*store);

    //standard objects
    DialogSI* showCurveDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);
    DialogSI* showMultiCurveDialog(QList<ObjectPtr> curves);
    DialogSI* showImageDialog(ObjectPtr objectPtr = 0, MatrixPtr matrix = 0);
    DialogSI* showMultiImageDialog(QList<ObjectPtr> images);

    //standard data objects
    DialogSI* showEquationDialog(ObjectPtr objectPtr = 0);
    DialogSI* showHistogramDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);
    DialogSI* showPowerSpectrumDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);
    DialogSI* showCSDDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);
    DialogSI* showEventMonitorDialog(ObjectPtr objectPtr = 0);

    //view items [grr...]
    ScriptInterface* showViewItemDialog(ViewItem* x);
    ScriptInterface* newArrow();
    ScriptInterface* newBox();
    ScriptInterface* newButton();
    ScriptInterface* newLineEdit();
    ScriptInterface* newCircle();
    ScriptInterface* newEllipse();
    ScriptInterface* newLabel();
    ScriptInterface* newLine();
    ScriptInterface* newPicture(QByteArray file);
    ScriptInterface* newPlot();
    ScriptInterface* newSharedAxisBox();
#ifndef KST_NO_SVG
    ScriptInterface* newSvgItem(QByteArray path);
#endif

    //plugins
    DialogSI* showBasicPluginDialog(QByteArray pluginName, ObjectPtr objectPtr = 0, VectorPtr vectorX = 0, VectorPtr vectorY = 0, PlotItemInterface *plotItem = 0 );

    //show appropriate dialog
    DialogSI* showObjectDialog(ObjectPtr objectPtr = 0);
    DialogSI* showMultiObjectDialog(QList<ObjectPtr> objects);

};

typedef QByteArray (DialogSI::*DialogSIMemberFn)(QByteArray&,QWidget*);

class DialogSI : public ScriptInterface
{
    Q_OBJECT
    QWidget* _dialog;   // the hidden dialog being scrapped

    QList<QLabel*> _labelsWithBuddies;  // from the dialog
    QByteArrayList _camelTextLabels;    // the names of the above labels, in camel case, starting with a capital

    QList<QAbstractButton*> _buttons;   // from the dialog
    QByteArrayList _camelButtons;       // the names of the above buttons, in camel case, starting with a capital
    QByteArrayList _camelButtonsRev;    // the names of the above buttons, in camel case, starting lowercase

    QList<ColorButton*> _colorButtons;  // from the dialog
    QByteArrayList _camelColorButtons;  // the names of the above buttons, in camel case, starting with a capital

    QList<QGroupBox*> _groupBoxes;      // from the dialog
    QList<QComboBox*> _buddylessComboBoxes; // from the dialog

    bool _valid;    // false if the dialog is 0, or was deleted.
    DialogSI* _subDialog;   // "Configure" buttons in primitives
    ViewItem* _vi;  // If dialog represents a viewitem, the viewitem, else 0
public:
    friend class ScriptServer;
    static QString toCamelCase(QString x);  // Awesome Button -> awesomeButton
    QByteArrayList commands();
    QString doCommand(QString);
    void initDataSourceDialogSI();
    bool isValid() { return _valid; }
    QByteArray getHandle();
    void endEditUpdate() { UpdateManager::self()->doUpdates(true); }
public slots:
    void invalidate();
public:
    DialogSI(QWidget* t,ViewItem* vi=0);
    ~DialogSI();

protected:
    QMap<QByteArray,DialogSIMemberFn> _fnMap;
    QMap<QByteArray,QWidget*> _objMap;

    QByteArray noSuchFn(QByteArray&c,QWidget*) { qDebug()<<"Called noSuchFn() for"<<c; return "No such command"; }

    void waitForValidation();
    //
    // these are commands processed by DialogSI
    //
    QByteArray pressConfigure(QByteArray&,QWidget*);
    QByteArray setPicture(QByteArray&,QWidget*);

    QByteArray setLineEditText(QByteArray&,QWidget*);
    QByteArray setTextEditText(QByteArray&,QWidget*);
    QByteArray setSpinBoxValue(QByteArray&,QWidget*);
    QByteArray setDoubleSpinBoxValue(QByteArray&,QWidget*);
    QByteArray setComboBoxEditValue(QByteArray&,QWidget*);

    QByteArray setComboBoxIndex(QByteArray&,QWidget*);
    QByteArray getComboBoxIndex(QByteArray&,QWidget*);

    QByteArray getLineEditText(QByteArray&,QWidget*);
    QByteArray getTextEditText(QByteArray&,QWidget*);
    QByteArray getSpinBoxValue(QByteArray&,QWidget*);
    QByteArray getDoubleSpinBoxValue(QByteArray&,QWidget*);
    QByteArray getComboBoxEditValue(QByteArray&,QWidget*);

    QByteArray checkButton(QByteArray&,QWidget*);
    QByteArray uncheckButton(QByteArray&,QWidget*);
    QByteArray getButtonIsChecked(QByteArray&,QWidget*);

    QByteArray pressButton(QByteArray&,QWidget*);

    QByteArray checkGroupBox(QByteArray&,QWidget*);
    QByteArray uncheckGroupBox(QByteArray&,QWidget*);
    QByteArray getGroupBoxIsChecked(QByteArray&,QWidget*);

    QByteArray getColour(QByteArray&,QWidget*);
    QByteArray setColour(QByteArray&,QWidget*);
};



}
#endif // DIALOGSCRIPTINTERFACE_H
