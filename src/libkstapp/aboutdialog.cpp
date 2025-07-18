/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2009 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>
#include "aboutdialog.h"

#ifdef KST_HAVE_REVISION_H
#include "kstrevision.h"
#include "authors.h"
#endif

#include <QDesktopServices>
#include <QDebug>
#include <QTextEdit>

#include <QStringList>

namespace Kst {

AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

#ifdef KST_HAVE_REVISION_H
   QStringList utf8Authors = QString::fromUtf8(kst_authors).trimmed().split(';');
   QStringList authors;
   foreach(const QString& a, utf8Authors) {
     if (!a.startsWith('#')) {
      authors << a;
     }
   }
#else
// qmake support
  QStringList authors = QStringList()
    << "Barth Netterfield"
    << "Joshua Netterfield"
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
    << "Zongyi Zang";
#endif

  //authors.sort();
  authors.replaceInStrings("<", "&lt;");
  authors.replaceInStrings(">", "&gt;");
  authors.replaceInStrings(QRegExp("^(.*)"), "<li>\\1</li>");

  QString version = QString(KSTVERSION)
#ifdef KST_REVISION
      +KST_REVISION
#endif
      ;

  QStringList msg = QStringList()
  << tr("<qt><h2>Kst %1").arg(version)
  << tr("</h2>")
  << tr("<h2>A data viewing program.</h2>")
  << tr("Copyright &copy; 2000-2024 Barth Netterfield<br><hr>")
  << tr("Homepage: <a href=\"http://kst-plot.kde.org/\">http://kst-plot.kde.org/</a><br>")
  << tr("Please report bugs with the 'Bug Report Wizard' of the 'Help' menu.<br>")
  << tr("<br>Authors and contributors:")
  << QString("<ul>")
  << authors
  << QString("</ul>");
  
  text->setText(msg.join("\n"));
  connect(text, SIGNAL(anchorClicked(QUrl)), this, SLOT(launchURL(QUrl)));
}


AboutDialog::~AboutDialog() {
}


void AboutDialog::launchURL(const QUrl &link) {
  QDesktopServices::openUrl ( link );
}

}
// vim: ts=2 sw=2 et
