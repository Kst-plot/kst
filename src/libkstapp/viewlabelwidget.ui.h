/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void ViewLabelWidget::init()
{
    connect(_scalars, SIGNAL(selectionChanged(const QString &)),
	    this, SLOT(insertScalarInText(const QString &)));
    _horizontal->insertItem(i18n("Left"));
    _horizontal->insertItem(i18n("Right"));
    _horizontal->insertItem(i18n("Center"));

    connect(_strings, SIGNAL(selectionChanged(const QString &)),
	    this, SLOT(insertStringInText(const QString &)));

    QWhatsThis::add(_text, i18n("<qt>The syntax for labels is a derivative of a subset of LaTeX.  Supported syntax is: <b>\\[greeklettername]</b> and <b>\\[Greeklettername]</b>, <b>\\approx</b>, <b>\\cdot</b>, <b>\\ge</b>, <b>\\geq</b>, <b>\\inf</b>, <b>\\int</b>, <b>\\le</b>, <b>\\leq</b>, <b>\\ne</b>, <b>\\n</b>, <b>\\partial</b>, <b>\\prod</b>, <b>\\pm</b>, <b>\\textcolor{color name}{colored text}</b>, <b>\\textbf{bold text}</b>, <b>\\textit{italicized text}</b>, <b>\\t</b>, <b>\\sum</b>, <b>\\sqrt</b>, <b>\\underline{underlined text}</b>, <b>x^y</b>, <b>x_y</b>.  Data references and embedded expressions should be enclosed in square brackets.  Example: <i>[=[MyScalar]/2]</i>"));
}


void ViewLabelWidget::insertScalarInText(const QString &S)
{
    _text->insert("["+S+"]");
}

void ViewLabelWidget::insertStringInText(const QString &S)
{
    _text->insert("["+S+"]");
}

// vim: ts=8 sw=4 noet
