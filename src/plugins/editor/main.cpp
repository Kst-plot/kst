/***************************************************************************
                                main.cpp
    begin                : Tue Nov 11 2003
    copyright            : (C) 2003 The University of Toronto
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
// vim: noet ts=8 sw=4

#include <qapplication.h>
#include "plugineditor.h"

int main( int argc, char **argv) {
    QApplication a(argc, argv);
    PluginEditor *ab = new PluginEditor;

    ab->show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
