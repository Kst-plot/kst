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

#ifndef VIEWITEMDIALOG_H
#define VIEWITEMDIALOG_H

#include "dialog.h"
#include "editmultiplewidget.h"

#include <QPointer>
#include <QLabel>

#include "kst_export.h"

namespace Kst {

class ViewItem;
class FillTab;
class StrokeTab;
class LayoutTab;
class DimensionsTab;
class EditMultipleWidget;

class KST_EXPORT ViewItemDialog : public Dialog
{
  Q_OBJECT
  public:
    enum EditMode { Single, Multiple };
    ViewItemDialog(ViewItem *item, QWidget *parent = 0);
    virtual ~ViewItemDialog();

    void setSupportsMultipleEdit(bool enabled);

    void setTagString(const QString& tagString) { _tagString->setText(tagString); }
    QString tagString() const { return _tagString->text(); }

    void addMultipleEditOption(QString name, QString descriptionTip, QString shortName);
    QList<ViewItem*> selectedMultipleEditObjects();
    void clearMultipleEditOptions();

    EditMode editMode() const { return _mode; }

    DialogPage *dimensionsPage;
    DimensionsTab *_dimensionsTab;
  public Q_SLOTS:
    void setSingleEdit();
    void setMultipleEdit();

  Q_SIGNALS:
    void editMultipleMode();
    void editSingleMode();

  private Q_SLOTS:
    void fillChanged();
    void strokeChanged();
    void layoutChanged();
    void dimensionsChanged();
    void slotEditMultiple();

  private:
    void setupFill();
    void setupStroke();
    void setupLayout();
    void setupChildViewOptions();
    void setupDimensions();

    void saveFill(ViewItem *item);
    void saveStroke(ViewItem *item);
    void saveLayout(ViewItem *item);
    void saveDimensions(ViewItem *item);

  private:
    QPointer<ViewItem> _item;

    QMap <QString, QString> _multiNameShortName;
    QLabel *_tagStringLabel;
    QLineEdit *_tagString;
    EditMultipleWidget *_editMultipleWidget;
    QPushButton *_editMultipleButton;
    EditMode _mode;
    QWidget *_editMultipleBox;
    
    FillTab *_fillTab;
    StrokeTab *_strokeTab;
    LayoutTab *_layoutTab;
};

}

#endif

// vim: ts=2 sw=2 et
