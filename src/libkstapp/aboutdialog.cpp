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

#include "aboutdialog.h"

#include <QDesktopServices>
#include <QDebug>

namespace Kst {

AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  // Sorted alphabetically, first group is 2.0 contributors
  const QString msg = tr(
  "<qt><h2>Kst 2.0 - A data viewing program.</h2>\n<hr>\n"
  "Copyright &copy; 2000-2008 Barth Netterfield<br>"
  "<a href=\"http://kst.kde.org/\">http://kst.kde.org/</a><br>"
  "Please report bugs to: <a href=\"http://bugs.kde.org/\">http://bugs.kde.org/</a><br>"
  "Authors:<ul>"
  "<li>Barth Netterfield</li>"
  "<li><a href=\"http://www.staikos.net/\">Staikos Computing Services Inc.</a></li>"
  "<li>Ted Kisner</li>"
  "<li>The University of Toronto</li>"
  "</ul><ul>"
  "<li>Matthew Truch</li>"
  "<li>Nicolas Brisset</li>"
  "<li>Rick Chern</li>"
  "<li>Sumus Technology Limited</li>"
  "<li>The University of British Columbia</li>"
  "</ul>"
  );
  text->setText(msg);
  connect(text, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(launchURL(const QUrl&)));
}


AboutDialog::~AboutDialog() {
}


void AboutDialog::launchURL(const QUrl &link) {
  QDesktopServices::openUrl ( link );
}

}
// vim: ts=2 sw=2 et
