/***************************************************************************
                             plugineditor.ui.h
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

// vim: ts=8 sw=4 noet

void PluginEditor::setModified(bool state)
{
    _modified = state;
}


bool PluginEditor::isModified()
{
    return _modified;
}


void PluginEditor::init()
{
    _doc = 0L;
    setModified(false);
    listView1->setDefaultRenameAction(QListView::Accept);
    _savedFileName = QString::null;
    initListView();
}

void PluginEditor::fileNew()
{
    if (isModified()) {
	switch (QMessageBox::information( this, QString::null, tr("This file has been modified. Do you wish to save it?"),  tr("&Yes"), tr("&No"), tr("&Cancel"))){
	case 0: // yes
	    PluginEditor::fileSave();
	    break;
	case 1: // no
	    break;
	case 2: // cancel
	    return;
	    break;
	default:
	    break;
	}
    }

    initListView();
    _savedFileName = QString::null;
    delete _doc;
    _doc = 0L;

    setModified(false);
}

void PluginEditor::closeEvent( QCloseEvent* e )
{
    if (isModified()) {
	switch (QMessageBox::information( this, QString::null, tr("This file has been modified. Do you wish to save it?"), tr("&Yes"), tr("&No"), tr("&Cancel"))) {
	    case 0: // yes
		PluginEditor::fileSave();
		break;
	    case 1: // no
		break;
	    case 2: // cancel
		return;
		break;
	    default:
		break;
	}
    }

    switch (QMessageBox::information( this, QString::null, tr("Are you sure you want to quit?"), tr("&Yes"), tr("&No"), tr("&Cancel"))){
	case 0: // yes
	    delete _doc;
	    _doc = 0L;
	    e->accept();
	    break;
	case 1: // no
	    e->ignore();
	    break;
	case 2: // cancel
	    e->ignore();
	    break;
	default:
	    e->ignore();
	    break;
    }
}

void PluginEditor::fileOpen()
{
    if (isModified()) {
	switch (QMessageBox::information(this, QString::null, tr("This file has been modified. Do you wish to save it?"), tr("&Yes"), tr("&No"), tr("&Cancel"))) {
	    case 0: // yes
		PluginEditor::fileSave();
		break;
	    case 1: // no
		break;
	    case 2: // cancel
		return;
		break;
	    default:
		break;
	}
    }

    QString filename = QFileDialog::getOpenFileName(QString::null, tr("XML Files (*.xml)"), this, 0L, tr("Choose a file"));

    if (filename.isEmpty()) {
	return;
    }

    delete _doc;
    _doc = new QDomDocument;
    QFile file(filename);

    if (!file.open(IO_ReadOnly)) {
	QMessageBox::information(this, QString::null, tr("Error: Could Not Open File!"),  tr("&OK"));
	file.close();
	return;
    }

    if (!_doc->setContent(&file)) {
	QMessageBox::information(this, QString::null, tr("Error: Could Not Parse XML!"), tr("&OK"));
	file.close();
	_savedFileName = filename;
	return;
    }

    file.close();
    QListViewItem *item;
    QDomElement root = _doc->documentElement();
    QDomNode node;
    node = root.firstChild();

    // reinitialize the listview
    initListView();

    while (!node.isNull()) {
	// handles INTRO tags
	if (node.isElement() && node.nodeName() == "intro") {
	    QDomNode child = node.firstChild();

	    while (!child.isNull()) {
		if (child.isElement() && child.nodeName() == "author") {
		    item = listView1->findItem("author", 0, Qt::ExactMatch);
		    item->setText(1, child.toElement().attributeNode("name").value());
		}
		if (child.isElement() && child.nodeName() == "description") {
		    item = listView1->findItem("description", 0, Qt::ExactMatch);
		    item->setText(1, child.toElement().attributeNode("text").value());
		}
		if (child.isElement() && child.nodeName() == "modulename") {
		    item = listView1->findItem("modulename", 0, Qt::ExactMatch);
		    item->setText(1, child.toElement().attributeNode("name").value());
		}
		if (child.isElement() && child.nodeName() == "version") {
		    item = listView1->findItem("major", 0, Qt::ExactMatch);
		    item->setText(1, child.toElement().attributeNode("major").value());
		    item = listView1->findItem("minor", 0, Qt::ExactMatch);
		    item->setText(1, child.toElement().attributeNode("minor").value());
		}
		if (child.isElement() && child.nodeName() == "state") {
		    item = listView1->findItem("state", 0, Qt::ExactMatch);
		    item->setText(1, child.toElement().attributeNode("devstate").value());
		}

		child = child.nextSibling();
	    }
	}

	// handles INTERFACE tag
	if (node.isElement() && node.nodeName() == "interface") {
	    QDomNode child = node.firstChild();
	    while (!child.isNull()) {
		if (child.isElement() && (child.nodeName() == "input" || child.nodeName() == "output")) {
		    QDomNode subchild = child.firstChild();
		    QString interfaceType = (child.nodeName() == "input") ? "input" : "output";

		    while (!subchild.isNull()) {

			// STRING
			if (subchild.isElement() && subchild.nodeName() == "string"){
			    QListViewItem *subchildString = new QListViewItem(
				    listView1->findItem(interfaceType, 0, Qt::ExactMatch),
				    QString::null, QString::null);

			    subchildString->setText(0, subchild.toElement().attributeNode("name").value());
			    subchildString->setRenameEnabled(0,true);

			    QListViewItem *helptext = new QListViewItem(
				    subchildString,
				    "helptext");

			    helptext->setText(1, subchild.toElement().attributeNode("helptext").value());
			    helptext->setRenameEnabled(1, true);

			    QListViewItem *type = new QListViewItem(
				    subchildString, "type");

			    type->setText(1, subchild.nodeName());
			    type->setRenameEnabled(1,true);
			}

			// TABLES AND MAPS
			if (subchild.isElement() && (subchild.nodeName() == "table" || subchild.nodeName() == "map")) {
			    QListViewItem *subchildString = new QListViewItem(
				    listView1->findItem(interfaceType, 0, Qt::ExactMatch),
				    "", "");

			    subchildString->setText(0, subchild.toElement().attributeNode("name").value());
			    subchildString->setRenameEnabled(0, true);

			    QListViewItem *type = new QListViewItem(
				    subchildString, "type");

			    type->setText(1, subchild.nodeName() + ":" + subchild.toElement().attributeNode("type").value());
			    type->setRenameEnabled(1, true);

			    QListViewItem *descr = new QListViewItem(
				    subchildString, "descr");

			    descr->setText(1, subchild.toElement().attributeNode("descr").value());
			    descr->setRenameEnabled(1, true);

			}

			subchild = subchild.nextSibling();
		    }
		}

		child = child.nextSibling();
	    }
	}

	// handles PARALIST tag
	if (node.isElement() && node.nodeName() == "paralist") {
	    QDomNode child = node.firstChild();

	    while (!child.isNull()) {

		if (child.isElement() && (child.nodeName() == "string" || child.nodeName() == "int")) {
		    QListViewItem *childString = new QListViewItem(
			    listView1->findItem("paralist", 0, Qt::ExactMatch),
			    QString::null, QString::null);

		    childString->setText(0, child.toElement().attributeNode("name").value());
		    childString->setRenameEnabled(0, true);

		    QListViewItem *helptext = new QListViewItem(
			    childString, "helptext");

		    helptext->setText(1, child.toElement().attributeNode("helptext").value());
		    helptext->setRenameEnabled(1, true);

		    QListViewItem *type = new QListViewItem(
			    childString, "type");

		    type->setText(1, child.nodeName());
		    type->setRenameEnabled(1, true);
		}
		child = child.nextSibling();
	    }
	}

	node = node.nextSibling();
    }

    _savedFileName = filename;
    setModified(false);
    listView1->setSelected(listView1->firstChild(), true);
}

void PluginEditor::fileSave()
{
    if (_savedFileName.isEmpty()) {
	fileSaveAs();
    }

    // _savedFileName can change in fileSaveAs()
    if (_savedFileName.isEmpty()) {
	return;
    }

    QFile file(_savedFileName);

    if (file.open(IO_WriteOnly)){
	QTextStream out (&file);

	// sequentially write output file

	delete _doc;
	_doc = new QDomDocument;
	QDomElement root = _doc->createElement("Module");
	_doc->appendChild(root);
	QDomElement intro = _doc->createElement("intro");
	root.appendChild(intro);

	// write INTRO

	QListViewItem *iter = listView1->findItem("intro", 0, Qt::ExactMatch);
	iter = iter->firstChild();
	while (iter) {
	    QDomElement element = _doc->createElement(iter->text(0));

	    if (iter->text(0) == "version") {
		QListViewItem *version = iter->firstChild();

		if (version->text(1).length() > 0) {
		    element.setAttribute(version->text(0), version->text(1));
		}

		version = version->nextSibling();

		if (version->text(1).length() > 0) {
		    element.setAttribute(version->text(0), version->text(1));
		}
	    }

	    if (iter->text(1).length() > 0) {
		if (iter->text(0) == "modulename" || iter->text(0) == "author") {
		    element.setAttribute("name", iter->text(1));
		}

		if (iter->text(0) == "description") {
		    element.setAttribute("text", iter->text(1));
		}

		if (iter->text(0) == "state") {
		    element.setAttribute("devstate", iter->text(1));
		}
	    }

	    if (element.attributes().length() > 0) {
		intro.appendChild(element);
	    }

	    iter = iter->nextSibling();
	}

	// Write INTERFACE
	QDomElement interface = _doc->createElement("interface");
	root.appendChild(interface);
	createTags("input", &interface);
	createTags("output", &interface);

	// Write PARALIST
	createTags("paralist", &root);

	root.save(out, 1);
    }

    file.close();
    setModified(false);
}


void PluginEditor::fileSaveAs()
{
    QString filename = QFileDialog::getSaveFileName(QString::null, tr("XML Files (*.xml)"), this, 0L, tr("Choose a file"));

    if (filename.isEmpty()) {
	return;
    }

    _savedFileName = filename;
    fileSave();
}


void PluginEditor::filePrint()
{

}


void PluginEditor::fileExit()
{

}


void PluginEditor::helpIndex()
{

}


void PluginEditor::helpContents()
{

}


void PluginEditor::helpAbout()
{

}


void PluginEditor::entryAdd()
{
    QListViewItem *cur = listView1->currentItem();
    QListViewItem *parent = cur;

    while (parent->parent() != 0) {
	parent = parent->parent();
    }

    if (parent->text(0) == "interface") {
	    QString interfaceType;

	    if (cur->text(0) == "input") {
		interfaceType = "input";
	    }

	    if (cur->text(0) == "output") {
		interfaceType = "output";
	    }

	    if (interfaceType.isEmpty()) {
		return;
	    }

	    QListViewItem *newInterface = new QListViewItem(
		listView1->findItem(interfaceType, 0, Qt::ExactMatch),
		"new " + interfaceType, QString::null);

	    newInterface->setRenameEnabled(0, true);

	    QListViewItem *helptext = new QListViewItem(
		    newInterface, "helptext");

	    helptext->setRenameEnabled(1, true);

	    QListViewItem *desc = new QListViewItem(
		    newInterface, "descr");

	    desc->setRenameEnabled(1, true);

	    QListViewItem *type = new QListViewItem(
		    newInterface, "type");

	    type->setRenameEnabled(1, true);

	    listView1->setSelected(newInterface, true);
	    newInterface->setOpen(true);
	    newInterface->startRename(0);

    } else if (cur->text(0) == "paralist") {
	QListViewItem *newParalist = new QListViewItem(
		cur, "new param", QString::null);

	newParalist->setRenameEnabled(0, true);

	QListViewItem *helptext = new QListViewItem(
		newParalist, "helptext");

	helptext->setRenameEnabled(1, true);

	QListViewItem *desc = new QListViewItem(
		newParalist, "descr");

	desc->setRenameEnabled(1, true);

	QListViewItem *type = new QListViewItem(
		newParalist, "type");

	type->setRenameEnabled(1, true);
	listView1->setSelected(newParalist, true);
	newParalist->setOpen(true);
	newParalist->startRename(0);
    }
}


void PluginEditor::removeEntry()
{
    QListViewItem *p = listView1->currentItem()->parent();
    if (p && (p->text(0) != "intro" && p->text(0) != "interface" && p->text(0) != "version")) {
	delete listView1->currentItem();
    }

    setModified(true);
}

void PluginEditor::initListView()
{
    listView1->clear();

    QListViewItem *root = new QListViewItem(listView1, "paralist", QString::null);
    root->setOpen(true);
    listView1->insertItem(root);

    root = new QListViewItem(listView1, "interface", QString::null);
    root->setOpen(true);
    QListViewItem *child = new QListViewItem(root, "input", QString::null);
    child = new QListViewItem(root, "output", QString::null);
    listView1->insertItem(root);

    root = new QListViewItem(listView1, "intro", QString::null);
    root->setOpen(true);
    child = new QListViewItem(root, "modulename", QString::null);
    child->setRenameEnabled(1,true);
    child = new QListViewItem(root, "author", QString::null);
    child->setRenameEnabled(1,true);
    child = new QListViewItem(root, "description", QString::null);
    child->setRenameEnabled(1,true);
    child = new QListViewItem(root, "version", QString::null);
    QListViewItem *subChild = new QListViewItem(child, "major", QString::null);
    subChild->setRenameEnabled(1,true);
    subChild = new QListViewItem(child, "minor", QString::null);
    subChild->setRenameEnabled(1,true);
    child = new QListViewItem(root, "state", QString::null);
    child->setRenameEnabled(1,true);
    child = new QListViewItem(root, "platforms", QString::null);
    child->setRenameEnabled(1,true);
    child = new QListViewItem(root, "language", QString::null);
    child->setRenameEnabled(1,true);
    child = new QListViewItem(root, "documentation", QString::null);
    child->setRenameEnabled(1,true);
    listView1->insertItem(root);

    listView1->setSelected(listView1->firstChild(), true);
}


void PluginEditor::renameOccured( QListViewItem *item, int col )
{
  Q_UNUSED( item )
  Q_UNUSED( col )

  setModified(true);
}


void PluginEditor::createTags( const QString & outerTag, QDomElement *parent )
{
	QListViewItem *iter = listView1->findItem(outerTag, 0, Qt::ExactMatch)->firstChild();

	QDomElement outerElement = _doc->createElement(outerTag);
	parent->appendChild(outerElement);

	while (iter) {
	    QDomElement element = _doc->createElement(QString::null);
	    element.setAttribute("name", iter->text(0));
	    QListViewItem *child = iter->firstChild();

	    while (child) {
		if (child->text(0) == "type") {
		    if (child->text(1).contains(':', false) > 0) {
			QStringList list = QStringList::split(':', child->text(1),FALSE);
			QStringList::Iterator it = list.begin();
			element.setTagName(*it);
			element.setAttribute("type",*(++it));
		    } else {
			element.setTagName(child->text(1));
		    }
		} else {
		    element.setAttribute(child->text(0), child->text(1));
		}
		child = child->nextSibling();
	    }

	    outerElement.appendChild(element);
	    iter = iter->nextSibling();
	}
}
