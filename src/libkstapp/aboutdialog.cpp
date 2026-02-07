/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2026 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "aboutdialog.h"
#include <config.h>

#include <QDebug>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QTextEdit>

#include <QStringList>

namespace Kst {

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
  setupUi(this);

  // qmake support
  QStringList authors = QStringList() << "Barth Netterfield"
                                      << "Nicolas Brisset"
                                      << "Rick Chern"
                                      << "Mike Fenton"
                                      << "Eli Filder"
                                      << "Ted Kisner"
                                      << "Peter Kümmel"
                                      << "Joshua Netterfield"
                                      << "Matthew Truch"
                                      << "Andrew Walker"
                                      << "Zongyi Zang"
                                      << "Staikos Computing Services Inc."
                                      << "Sumus Technology Limited"
                                      << "The University of British Columbia"
                                      << "The University of Toronto";
  // authors.sort();
  authors.replaceInStrings("<", "&lt;");
  authors.replaceInStrings(">", "&gt;");
  authors.replaceInStrings(QRegularExpression("^(.*)"), "<li>\\1</li>");

  QString version = QString(KSTVERSION);

  QStringList msg =
      QStringList()
      << tr("<qt><h2>Kst %1").arg(version) << tr("</h2>")
      << tr("<h2>A data viewing program.</h2>")
      << tr("Copyright &copy; 2000-2026 Barth Netterfield<br><hr>")
      << tr("Homepage: <a "
            "href=\"http://kst-plot.kde.org/\">http://kst-plot.kde.org/</"
            "a><br>")
      << tr("Please report bugs with the 'Bug Report Wizard' of the 'Help' "
            "menu.<br>")
      << tr("<br>Authors and contributors:") << QString("<ul>") << authors
      << QString("</ul>");

  text->setText(msg.join("\n"));
  connect(text, SIGNAL(anchorClicked(QUrl)), this, SLOT(launchURL(QUrl)));
}

AboutDialog::~AboutDialog() {}

void AboutDialog::launchURL(const QUrl &link) {
  QDesktopServices::openUrl(link);
}

} // namespace Kst
// vim: ts=2 sw=2 et
