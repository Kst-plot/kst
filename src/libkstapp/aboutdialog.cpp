/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2009 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"
#include "aboutdialog.h"

#include <QDesktopServices>
#include <QDebug>

#include <QStringList>

namespace Kst {

AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

    QStringList authors = QStringList()
    << "Barth Netterfield"
    << "Matthew Truch"
    << "Nicolas Brisset"
    << "Staikos Computing Services Inc."
    << "Rick Chern"
    << "Sumus Technology Limited"
    << "Ted Kisner"
    << "The University of British Columbia"
    << "The University of Toronto"
    << "Andrew Walker"
    << "Peter Kümmel"
    << "Zongyi Zang"
    ;

    authors.sort();
    authors.replaceInStrings(QRegExp("^(.*)"), "<li>\\1</li>");

  QStringList msg = QStringList()
  << tr("<qt><h2>Kst "KSTVERSION" - A data viewing program.</h2>")
  << tr("Copyright &copy; 2000-2010 Barth Netterfield<br><hr>")
  << tr("Homepage: <a href=\"http://kst.kde.org/\">http://kst.kde.org/</a><br>")
  << tr("Please report bugs with the 'Bug Report Wizard' of the 'Help' menu.<br>")
  << tr("<br>Authors and contributors (sorted alphabetically):")
  << QString("<ul>")
  << authors
  << QString("</ul>");
  
  text->setText(msg.join("\n"));
  connect(text, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(launchURL(const QUrl&)));
}


AboutDialog::~AboutDialog() {
}


void AboutDialog::launchURL(const QUrl &link) {
  QDesktopServices::openUrl ( link );
}

}
// vim: ts=2 sw=2 et
