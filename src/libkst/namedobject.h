/***************************************************************************
                 namedobject.h: adds naming features to objects...
                             -------------------
    begin                : May 29, 2008
    copyright            : (C) 2008 C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// name system: see object names devel doc for intended behavior


#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include <QString>
#include <QXmlStreamWriter>
#include <QFont>
#include "kstcore_export.h"

namespace Kst {

// short name index variables
KSTCORE_EXPORT extern int _vectornum; // vectors
KSTCORE_EXPORT extern int _pluginnum; // plugins
KSTCORE_EXPORT extern int _csdnum; // csd
KSTCORE_EXPORT extern int _curvecnum; // curves
KSTCORE_EXPORT extern int _equationnum; // equations
KSTCORE_EXPORT extern int _histogramnum; // histograms
KSTCORE_EXPORT extern int _imagenum; // images
KSTCORE_EXPORT extern int _psdnum; // psd
KSTCORE_EXPORT extern int _scalarnum; // scalars
KSTCORE_EXPORT extern int _stringnum; // text string
KSTCORE_EXPORT extern int _matrixnum; // matrix
KSTCORE_EXPORT extern int _plotnum; // plot item
KSTCORE_EXPORT extern int _legendnum; // legend
KSTCORE_EXPORT extern int _viewitemnum; // view item (drawable)
KSTCORE_EXPORT extern int _datasourcenum; // datasource

KSTCORE_EXPORT extern int max_vectornum; // vectors
KSTCORE_EXPORT extern int max_pluginnum; // plugins
KSTCORE_EXPORT extern int max_csdnum; // csd
KSTCORE_EXPORT extern int max_curvenum; // curves
KSTCORE_EXPORT extern int max_equationnum; // equations
KSTCORE_EXPORT extern int max_histogramnum; // histograms
KSTCORE_EXPORT extern int max_imagenum; // images
KSTCORE_EXPORT extern int max_psdnum; // psd
KSTCORE_EXPORT extern int max_scalarnum; // scalars
KSTCORE_EXPORT extern int max_stringnum; // string
KSTCORE_EXPORT extern int max_matrixnum; // matrix
KSTCORE_EXPORT extern int max_plotnum; // plot item
KSTCORE_EXPORT extern int max_legendnum; // legend
KSTCORE_EXPORT extern int max_viewitemnum; // view item
KSTCORE_EXPORT extern int max_datasourcenum; // datasource

struct SizeCache {
    int nameWidthPixels;
    int fontSize;
    QString name;
};

KSTCORE_EXPORT void resetNameIndexes();

class KSTCORE_EXPORT NamedObject 
{
public: 
     NamedObject();
     virtual ~NamedObject();

     enum ShortNameIndex {
      VECTORNUM    = 0x0001,
      PLUGINNUM    = 0x0002,
      CSDNUM       = 0x0004,
      CURVENUM     = 0x0008,
      EQUATIONNUM  = 0x0010,
      HISTOGRAMNUM = 0x0020,
      IMAGENUM     = 0x0040,
      PSDNUM       = 0x0080,
      SCALARNUM    = 0x0100,
      STRINGNUM    = 0x0200,
      MATRIXNUM    = 0x0400,
      PLOTNUM      = 0x0800,
      LEGENDNUM    = 0x1000,
      VIEWITEMNUM  = 0x2000,
      DATASOURCENUM= 0x4000
    };

    // name system: see object names devel doc
    QString Name() const; // eg GYRO1 (V1)
    QString CleanedName() const; // all \_ replaced with _
    QString descriptiveName() const; // eg GYRO1: automatic or manual
    QString shortName() const; // eg V1: always automatically generated
    QString lengthLimitedName(int length = 20) const; // Name, but with descriptiveName truncated
    QString sizeLimitedName(const QFont&font,const int&width) const; // Name, shrunk to fit in width with font
    QString sizeLimitedName(const QWidget *widget) const; // Name, shrunk to fit in widget
    virtual QString descriptionTip() const = 0; // description for tooltips
    void setDescriptiveName(QString new_name); // auto if new_name.isEmpty()
    bool descriptiveNameIsManual() const;
    static void processShortNameIndexAttributes(QXmlStreamAttributes &attrs);

    // Reset all name indexes.  Should only be used by ObjectStore when clearing the store entirely.
    static void resetNameIndex();

  protected:
    virtual QString _automaticDescriptiveName() const= 0;
    virtual void _initializeShortName() = 0;
    QString _manualDescriptiveName;
    QString _shortName;
    virtual void saveNameInfo(QXmlStreamWriter &s, unsigned I = 0xffff);

    // object indices used for saving/resorting shortnames
    int _initial_vectornum; // vectors
    int _initial_pluginnum; // plugins
    int _initial_csdnum; // csd
    int _initial_curvenum; // curves
    int _initial_equationnum; // equations
    int _initial_histogramnum; // histograms
    int _initial_imagenum; // images
    int _initial_psdnum; // psd
    int _initial_scalarnum; // scalars
    int _initial_stringnum; // text string
    int _initial_matrixnum; // matrix

    int _initial_plotnum; // plot item
    int _initial_legendnum; // legend
    int _initial_viewitemnum; // view item
    int _initial_datasourcenum; // datasource
  private:
    SizeCache *_sizeCache;
};

KSTCORE_EXPORT bool shortNameLessThan(NamedObject *n1, NamedObject *n2);

}
#endif
