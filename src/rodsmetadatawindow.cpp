/**
 * @file rodsmetadatawindow.cpp
 * @brief Implementation of class RodsMetadataWindow
 *
 * The RodsMetadataWindow class extends the Qt window class QMainWindow
 * and implements an iRODS object metadata manager window.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMetadataWindow header
#include "rodsmetadatawindow.h"

// generated UI class Ui::RodsMetadataWindow header
#include "ui_rodsmetadatawindow.h"

RodsMetadataWindow::RodsMetadataWindow(Kanki::RodsConnection *theConn, Kanki::RodsObjEntryPtr theObjEntry, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RodsMetadataWindow),
    conn(theConn),
    objEntry(theObjEntry)
{
    ui->setupUi(this);

    // create metadata and schema objects for rods object metadata
    this->metadata = new Kanki::RodsObjMetadata(this->conn, this->objEntry);
    this->schema = new RodsMetadataSchema();

    // set window title to specify object
    std::string title = "iRODS Metadata Manager (Object: " + this->objEntry->getObjectFullPath() + ")";
    this->setWindowTitle(title.c_str());

    // create new metadata model object and link it to the view
    RodsMetadataModel *rodsMetaModel = new RodsMetadataModel(this->objEntry, this->metadata, this->schema);
    ui->treeView->setModel(rodsMetaModel);

    // create new metadata mapper object
    metaMapper = new QDataWidgetMapper(this);
    metaMapper->setOrientation(Qt::Horizontal);
    metaMapper->setModel(rodsMetaModel);

    // set metadata mappings to editor widgets
    metaMapper->addMapping(this->ui->attrNameField, RodsMetadataModel::metaName);
    metaMapper->addMapping(this->ui->attrValueField, RodsMetadataModel::metaValue);
    metaMapper->addMapping(this->ui->attrUnitField, RodsMetadataModel::metaUnit);

    // connect selection changed event signal to handler slot in this class
    connect(this->ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(metaTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));

    // refresh view
    this->refresh();
}

RodsMetadataWindow::~RodsMetadataWindow()
{
    // destroy mapper first, to prevent access
    delete (this->metaMapper);
    delete (this->ui);

    // cleanup after everything else
    delete (this->metadata);
    delete (this->schema);
}

void RodsMetadataWindow::closeEvent(QCloseEvent *event)
{
    (void)event;

    this->unregister(this->objEntry->getObjectFullPath());
}

void RodsMetadataWindow::metaTreeSelectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
    (void)newSelection;
    (void)oldSelection;

    // get current index from selection model
    const QModelIndex index = this->ui->treeView->selectionModel()->currentIndex();

    // must check index is valid !
    if (index.isValid())
    {
        const RodsMetadataItem *item = static_cast<RodsMetadataItem*>(index.internalPointer());

        // if selection was a namespace
        if (item->childCount() > 0)
        {
            this->ui->metaEditBox->setDisabled(true);
        }

        // if selection was proper metadata item
        else {
            // enable metadata inspector
            this->ui->metaEditBox->setEnabled(true);

            // set root and current index for metadata mapper
            this->metaMapper->setRootIndex(index.parent());
            this->metaMapper->setCurrentModelIndex(index);
        }
    }
}

void RodsMetadataWindow::refresh()
{
    int status = 0;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // try to refresh object metadata
    if ((status = metadata->refresh()) < 0)
    {
        this->reportError("Error while refreshing metadata - iRODS API error", status);
    }

    QApplication::restoreOverrideCursor();

    // get metadata model object and refresh it
    RodsMetadataModel *model = static_cast<RodsMetadataModel*>(this->ui->treeView->model());
    model->refresh();

    // by default open all namespaces
    this->ui->treeView->expandAll();

    // resize columns after tree view data has been initialized
    for (int i = 0; i < model->columnCount(QModelIndex()); i++)
        this->ui->treeView->resizeColumnToContents(i);
}

void RodsMetadataWindow::addAttribute()
{
    QInputDialog attrName(this);
    QStringList attrs;
    QString selection;
    bool ok = false;
    int status = 0;

    attrs = this->schema->getAttributeList();
    selection = attrName.getItem(this, "Select Schema Attribute", "Attribute:", attrs, 0, true, &ok);

    // if user didn't press cancel and entered a proper attribute string
    if (ok && selection.length())
    {
        if ((status = this->metadata->addAttribute(selection.toStdString(), "MISSING_METADATA")) < 0)
            this->reportError("Error while adding an attribute - iRODS API error", status);

        else
            this->refresh();
    }
}

void RodsMetadataWindow::removeAttribute()
{
    int status = 0;
    QModelIndex curIndex = this->ui->treeView->currentIndex();

    // proceed only if we have a valid index
    if (curIndex.isValid())
    {
        RodsMetadataItem *item = static_cast<RodsMetadataItem*>(curIndex.internalPointer());

        // if the selection was a proper attribute
        if (!item->childCount())
        {
            // remove attribute from metadata
            if ((status = this->metadata->removeAttribute(item->getName().toStdString(),
                                                          item->getValue().toStdString(),
                                                          item->getUnit().toStdString())) < 0)
            {
                this->reportError("Error while removing attribute - iRODS API error", status);
            }

            // if successful, refresh view
            else {
                this->refresh();
            }
        }
    }
}

void RodsMetadataWindow::reportError(QString msg, int status) const
{
    // error reporting via a Qt message box
    QMessageBox errMsg;
    QString errStr = msg + " " + QVariant(status).toString();

    errMsg.setText(errStr);
    errMsg.setIcon(QMessageBox::Critical);
    errMsg.exec();
}

void RodsMetadataWindow::on_actionRefresh_triggered()
{
    this->refresh();
}

void RodsMetadataWindow::on_actionAddAttribute_triggered()
{
    this->addAttribute();
}

void RodsMetadataWindow::on_actionRemove_Attribute_triggered()
{
    this->removeAttribute();
}
