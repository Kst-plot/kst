/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "labelbuilder.h"

#include "objectstore.h"

namespace Kst {

LabelBuilder::LabelBuilder(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _store(store), _helpBox(0) {

  setupUi(this);

   _label->setWhatsThis(i18n("<qt>The syntax for labels is a derivative of a subset of LaTeX.  "
                             "Supported syntax is: <b>\\[greeklettername]</b> and <b>\\[Greeklettername]</b>, "
                             "<b>\\approx</b>, <b>\\cdot</b>, <b>\\ge</b>, <b>\\geq</b>, <b>\\inf</b> ,"
                             "<b>\\int</b>, <b>\\le</b>, <b>\\leq</b>, <b>\\ne</b>, <b>\\n</b>, "
                             "<b>\\partial</b>, <b>\\prod</b>, <b>\\pm</b>, "
                             "<b>\\textcolor{color name}{colored text}</b>, <b>\\textbf{bold text}</b>, "
                             "<b>\\textit{italicized text}</b>, <b>\\t</b>, <b>\\sum</b>, <b>\\sqrt</b>, "
                             "<b>\\underline{underlined text}</b>, <b>\\overline{overlined text}</b>, "
                             "<b>x^y</b>, <b>x_y</b>.  "
                             "Scalars, equations, and vector elements can be embedded.  "
                             "Scalar: <i>[V1/Mean]</i>.  Vector Element: <i>[V1[4]]</i>.  "
                             "Equation: <i>[=[V1/Mean]^2]</i>.  A [ character can be inserted as <i>\\[</i>."));

  _label->setToolTip(tr("Label text.  A subset of LaTeX is supported.  Click 'Help' for help."));

  connect(_label, SIGNAL(textChanged()), this, SIGNAL(labelChanged()));
  connect(_help, SIGNAL(clicked()), this, SLOT(showHelp()));

  _label->setFocus();
}


LabelBuilder::~LabelBuilder() {
  if (_helpBox) {
    delete _helpBox;
    _helpBox = 0;
  }
}

void LabelBuilder::showHelp() {
  if (!_helpBox) {
    _helpBox = new ModelessInfoBox(this);
  }

  _helpBox->show();
  _helpBox->setText("<qt>"
                    "<P ALIGN=LEFT STYLE=\"margin-bottom: 0in\"><FONT SIZE=4><B>Scalars &amp; equations</B></FONT><br>"
                     "Scalars and scalar equations can be displayed live in labels.  When the scalar "
                     "is updated, the label is updated.  Scalar names are autocompleted.  The format is:</P>"

                    "<P STYLE=\"margin-bottom: 0in\"><B>Scalar:</B> <FONT FACE=\"Courier New, monospace\">[</FONT><I>scalar"
                    "name</I><FONT FACE=\"Courier New, monospace\">]</FONT>, eg <FONT FACE=\"Courier New, monospace\">[GYRO1:Mean"
                    "(X4)]</FONT></P>"
                    "<P STYLE=\"margin-bottom: 0in\"><B>Vector Element:</B>"
                    "<FONT FACE=\"Courier New, monospace\">[</FONT><I>vectorName</I><FONT FACE=\"Courier New, monospace\">[</FONT><I>index</I><FONT FACE=\"Courier New, monospace\">]]</FONT><FONT FACE=\"Times New Roman, serif\">,"
                    "eg, </FONT><FONT FACE=\"Courier New, monospace\">[GYRO1 (V2)[4]]</FONT>"
                    "</P>"
                    "<P STYLE=\"margin-bottom: 0in\"><B>Equation:</B> <FONT FACE=\"Courier New, monospace\">[=</FONT><I>equation</I><FONT FACE=\"Courier New, monospace\">]</FONT>,"
                    "eg <FONT FACE=\"Courier New, monospace\">[=[GYRO1:Mean"
                    "(X4)]/[GYRO1:Sigma (X4)]]</FONT></P>"

                    "<P ALIGN=LEFT STYLE=\"margin-bottom: 0in\"><FONT SIZE=4><B>Supported LaTeX Subset</B></FONT><br>"
                    "Labels in <i>kst</i> "
                    "support a derrivitive subset of LaTeX. For example, to display the equation for the area of a "
                    "circle, you could set the label to A=2\\pir^2.  Unlike LaTeX, it is not necessary to enter math mode using '$'.  Also, "
                    "unlike LaTeX, variables are not automatically displayed in italic font.  If desired, this must "
                    "be done explicitly using \\textit{}.  Supported sequences are:</P>"

                    "<P STYLE=\"margin-bottom: 0in\"><B>Greek letters:</B>  \\<I>name</I> or "
                    "\\<I>Name</I>.  eg: <FONT FACE=\"Courier New, monospace\">\\alpha</FONT></P>"
                    "<P STYLE=\"margin-bottom: 0in\"><B>Other symbols:</B>  <FONT FACE=\"Courier New, monospace\">\\approx</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\cdot</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\ge</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\geq</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\inf</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\approx</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\cdot</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\ge</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\geq</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\inf</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\int</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\le</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\leq</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\ne</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\n</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\partial</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\prod</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\pm</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\sum</FONT><FONT FACE=\"Times New Roman, serif\">,</FONT><FONT FACE=\"Courier New, monospace\">"
                    "\\sqrt</FONT></P>"
                   "<P STYLE=\"margin-bottom: 0in\"><B>Font effects:</B> <FONT FACE=\"Courier New, monospace\">\\textcolor{</FONT><I>color"
                   "name</I><FONT FACE=\"Courier New, monospace\">}{</FONT><I>colored "
                   "text</I><FONT FACE=\"Courier New, monospace\">}</FONT>, <FONT FACE=\"Courier New, monospace\">\\textbf{</FONT><I>bold "
                   "text</I><FONT FACE=\"Courier New, monospace\">}</FONT>,"
                   "<FONT FACE=\"Courier New, monospace\">\\textit{</FONT><I>italicized "
                   "text</I><FONT FACE=\"Courier New, monospace\">}</FONT>,"
                   "<FONT FACE=\"Courier New, monospace\">\\underline{</FONT><I>underlined "
                   "text</I><FONT FACE=\"Courier New, monospace\">}</FONT>,"
                   "<FONT FACE=\"Courier New, monospace\">\\overline{</FONT><I>overlined "
                   "text</I><FONT FACE=\"Courier New, monospace\">}</FONT>."

                   "<P STYLE=\"margin-bottom: 0in\"><B>Other:</B><I>x</I><FONT FACE=\"Courier New, monospace\">^</FONT><I>y</I>,"
                   "<I>x</I><FONT FACE=\"Courier New, monospace\">_</FONT><I>y</I>, <FONT FACE=\"Courier New, monospace\">\\t</FONT>,"
                   "<FONT FACE=\"Courier New, monospace\">\\n</FONT>, <FONT FACE=\"Courier New, monospace\">\\[</FONT></P>"

                    );
  _helpBox->setWidth(100);

}


void LabelBuilder::setObjectStore(ObjectStore *store) {
  _store = store;
  _label->setObjectStore(store);
}


QString LabelBuilder::labelText() const {
  return _label->toPlainText();
}


void LabelBuilder::setLabelText(const QString &label) {
  _label->setPlainText(label);
}


}

// vim: ts=2 sw=2 et
