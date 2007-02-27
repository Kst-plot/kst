/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidgetplugin.h>
#include <kdeversion.h>
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
#include <kdemacros.h>
#endif
#include <kinstance.h>

#include "curveappearancewidget.h"
#include "curveplacementwidget.h"
#include "datarangewidget.h"
#include "editmultiplewidget.h"
#include "fftoptionswidget.h"
#include "kst_export.h"
#include "matrixselector.h"
#include "plotlistbox.h"
#include "kstcombobox.h"
#include "scalarselector.h"
#include "stringselector.h"
#include "vectorlistview.h"
#include "vectorselector.h"

#ifndef KDE_Q_EXPORT_PLUGIN
#define KDE_Q_EXPORT_PLUGIN(PLUGIN) \
Q_EXTERN_C KST_EXPORT const char* qt_ucm_query_verification_data(); \
Q_EXTERN_C KST_EXPORT QUnknownInterface* ucm_instantiate(); \
Q_EXPORT_PLUGIN(PLUGIN)
#endif

class KstWidgets : public QWidgetPlugin {
public:
	KstWidgets() : QWidgetPlugin() {
		new KInstance("kstwidgets");
	}

	virtual ~KstWidgets() {
	}

	virtual QStringList keys() const {
		QStringList rc;
		rc += "CurveAppearanceWidget";
		rc += "CurvePlacementWidget";
		rc += "EditMultipleWidget";
		rc += "KstDataRange";
		rc += "KstFFTOptions";
		rc += "MatrixSelector";
		rc += "PlotListBox";
		rc += "VectorListView";
		rc += "ScalarSelector";
		rc += "StringSelector";
		rc += "VectorSelector";
		rc += "KstComboBox";
		return rc;
	}

	virtual QWidget *create(const QString &key, QWidget *parent = 0, const char *name = 0);

	virtual QIcon iconSet(const QString &key) const {
		Q_UNUSED(key)
		return QIcon();
	}

	virtual bool isContainer(const QString &key) const {
		Q_UNUSED(key)
		return false;
	}

	virtual QString group(const QString &key) const {
		Q_UNUSED(key)
		return "Kst";
	}

	virtual QString includeFile(const QString &key) const {
		if (key == QString::fromLatin1("CurveAppearanceWidget"))
			return "curveappearancewidget.h";
		if (key == QString::fromLatin1("CurvePlacementWidget"))
			return "curveplacementwidget.h";
		if (key == QString::fromLatin1("EditMultipleWidget"))
			return "editmultiplewidget.h";
		if (key == QString::fromLatin1("MatrixSelector"))
			return "matrixselector.h";
		if (key == QString::fromLatin1("ScalarSelector"))
			return "scalarselector.h";
		if (key == QString::fromLatin1("StringSelector"))
			return "stringselector.h";
		if (key == QString::fromLatin1("VectorSelector"))
			return "vectorselector.h";
		if (key == QString::fromLatin1("KstFFTOptions"))
			return "fftoptionswidget.h";
		if (key == QString::fromLatin1("KstDataRange"))
			return "datarangewidget.h";
		if (key == QString::fromLatin1("PlotListBox"))
			return "plotlistbox.h";
		if (key == QString::fromLatin1("KstComboBox"))
			return "kstcombobox.h";
		if (key == QString::fromLatin1("VectorListView"))
			return "vectorlistview.h";
		return QString::null;
	}

	virtual QString toolTip(const QString &key) const {
		Q_UNUSED(key)
		return QString::null;
	}

	virtual QString whatsThis(const QString &key) const {
		Q_UNUSED(key)
		return QString::null;
	}
};

QWidget *KstWidgets::create(const QString &key, QWidget *parent, const char *name) {
	if (key == QString::fromLatin1("CurveAppearanceWidget"))
		return new CurveAppearanceWidget(parent, name);
	if (key == QString::fromLatin1("EditMultipleWidget"))
		return new EditMultipleWidget(parent, name);
	if (key == QString::fromLatin1("ScalarSelector"))
		return new ScalarSelector(parent, name);
	if (key == QString::fromLatin1("StringSelector"))
		return new StringSelector(parent, name);
	if (key == QString::fromLatin1("MatrixSelector"))
		return new MatrixSelector(parent, name);
	if (key == QString::fromLatin1("VectorSelector"))
		return new VectorSelector(parent, name);
	if (key == QString::fromLatin1("CurvePlacementWidget"))
		return new CurvePlacementWidget(parent, name);
	if (key == QString::fromLatin1("KstFFTOptions"))
		return new KstFFTOptions(parent, name);
	if (key == QString::fromLatin1("KstDataRange"))
		return new KstDataRange(parent, name);
	if (key == QString::fromLatin1("PlotListBox"))
		return new PlotListBox(parent, name);
	if (key == QString::fromLatin1("VectorListView"))
		return new VectorListView(parent, name);
	if (key == QString::fromLatin1("KstComboBox"))
		return new KstComboBox(parent, name);
	return 0L;
}

KDE_Q_EXPORT_PLUGIN(KstWidgets)

