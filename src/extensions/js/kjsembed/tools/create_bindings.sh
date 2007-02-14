#!/bin/sh

function bind_class()
{
    xsltproc doxygen2imp_h.xsl "xml/class$1.xml" > "$2_imp.h"
    xsltproc doxygen2imp_cpp.xsl "xml/class$1.xml" > "$2_imp.cpp"
    xsltproc doxygen2cons_h.xsl "xml/class$1.xml" > "$2_cons.h"
    xsltproc doxygen2cons_cpp.xsl "xml/class$1.xml" > "$2_cons.cpp"
}

doxygen

bind_class QComboBox qcombobox
bind_class Qt qnamespace
bind_class KPopupTitle kpopupmenu

#bind_class QDir qdir
#bind_class QListViewItem qlistview

#bind_class QTimer qtimer
#bind_class QBrush qbrush
#bind_class QPen qpen
#bind_class QPainter qpainter
