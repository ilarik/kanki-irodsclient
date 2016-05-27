/**
 * @file rodsmainwindow.cpp
 * @brief Implementation of class RodsMainWindow
 *
 * The RodsMainWindow class extends the Qt window class QMainWindow
 * and implements an iRODS Grid Browser window.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMainWindow header
#include "rodsmainwindow.h"

// generated UI class Ui::RodsMainWindow header
#include "ui_rodsmainwindow.h"

RodsMainWindow::RodsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RodsMainWindow)
{
    this->conn = NULL;
    this->queueWindow = NULL;
    this->findWindow = NULL;
    this->model = NULL;

    // instantiate and configure Qt UI
    this->ui->setupUi(this);

    // set initial settings
    this->allowOverwrite = false;
    this->verifyChecksum = true;
    this->ui->allowOverwrite->setChecked(this->allowOverwrite);
    this->ui->verifyChecksum->setChecked(this->verifyChecksum);

    // initialize progress bar display
    this->progress = new QProgressDialog(this);

    // initialize error log
    this->errorLogWindow = new RodsErrorLogWindow();
    connect(this, &RodsMainWindow::logError, this->errorLogWindow, &RodsErrorLogWindow::logError);
    connect(this->errorLogWindow, &RodsErrorLogWindow::errorsPresent, this, &RodsMainWindow::errorsReported);

    // enable drag & drop
    this->ui->rodsObjTree->viewport()->setAcceptDrops(true);
    this->ui->rodsObjTree->setDragEnabled(true);
    this->ui->rodsObjTree->setAcceptDrops(true);
    this->ui->rodsObjTree->setDropIndicatorShown(true);
}

RodsMainWindow::~RodsMainWindow()
{
    // if there is a connection object
    if (this->conn)
    {
        // disconnect from iRODS server
        this->conn->disconnect();

        delete (this->conn);
    }

    // delete object tree model if exists
    if (this->model)
        delete (this->model);

    // delete queue window if exists
    if (this->queueWindow)
        delete (this->queueWindow);

    // delete all metadata editors
    for (std::map<std::string, RodsMetadataWindow*>::iterator i = this->metaEditors.begin();
         i != this->metaEditors.end(); i++)
    {
        RodsMetadataWindow *window = i->second;

        delete (window);
        this->metaEditors.erase(i);
    }

    // tear down ui
    delete (this->ui);
}

void RodsMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  //  event->acceptProposedAction();
}

void RodsMainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    if (urls.empty())
        return;

    QString filePath = urls.first().toLocalFile();

    if (filePath.isEmpty())
        return;
}

void RodsMainWindow::closeEvent(QCloseEvent *event)
{
    (void)event;

    // call Qt to quit application (exit event loop)
    qApp->quit();
}

void RodsMainWindow::setConnection(Kanki::RodsConnection *newConn)
{
    // set connection pointer to new connection
    this->conn = newConn;
}

void RodsMainWindow::enterConnectedState()
{
    std::string statusMsg = "Connected to iRODS server ";
    statusMsg += this->conn->rodsHost();

    // if the connection is SSL enabled, get info
    if (conn->isSSL())
    {
        const SSL_CIPHER *cipher = this->conn->cipherInfo();

        statusMsg += " - SSL security enabled - ";

        // if we can get current cipher info from OpenSSL
        if (cipher)
        {
            statusMsg += "encryption method: " + std::string(cipher->name) + " - ";
            statusMsg += "cipher strength: " + QVariant(cipher->strength_bits).toString().toStdString();
            statusMsg += " bits - ";
        }

        // otherwise alert user that something's funky!
        else {
            statusMsg += "(SSL CIPHER INFO UNAVAILABLE !)";
        }
    }

    else {
        statusMsg += " - SSL security disabled!";
    }

    this->ui->statusBar->showMessage(statusMsg.c_str(), 10000);
    this->setWindowTitle(this->windowTitle() + " (Zone: " + QString(this->conn->rodsZone().c_str()) + ")");

    // if there exists a previous model, delete it
    if (this->model)
        delete (this->model);

    // instantiate new model for new connection
    this->model = new RodsObjTreeModel(this->conn, this->conn->rodsHome());

    // connect to model refresh interface slot
    connect(this, &RodsMainWindow::refreshObjectModelAtIndex, this->model,
            &RodsObjTreeModel::refreshAtIndex);

    // setup model with tree view and expand first item
    this->ui->rodsObjTree->setModel(this->model);
    this->ui->rodsObjTree->expand(this->model->index(0, 0, QModelIndex()));

    // resize columns after tree view data has been initialized
    for (int i = 0; i < this->model->columnCount(QModelIndex()); i++)
        this->ui->rodsObjTree->resizeColumnToContents(i);

    this->refreshResources();

    this->ui->actionConnect->setDisabled(true);
    this->ui->actionDisconnect->setDisabled(false);
    this->ui->actionMetadata->setDisabled(false);
    this->ui->actionMount->setDisabled(false);
    this->ui->actionRefresh->setDisabled(false);
    this->ui->actionQueueStat->setDisabled(false);
    this->ui->actionNewColl->setDisabled(false);
    this->ui->actionDelete->setDisabled(false);
    this->ui->actionUpload->setDisabled(false);
    this->ui->actionUploadDirectory->setDisabled(false);
    this->ui->actionDownload->setDisabled(false);
    this->ui->rodsObjTree->setDisabled(false);
    this->ui->viewSize->setDisabled(false);
    this->ui->verifyChecksum->setDisabled(false);
    this->ui->allowOverwrite->setDisabled(false);
    this->ui->storageResc->setDisabled(false);
    this->ui->actionFind->setDisabled(false);
}

void RodsMainWindow::enterDisconnectedState()
{
    // disable tree view
    this->ui->rodsObjTree->setDisabled(true);
    this->ui->viewSize->setDisabled(true);

    // reverse state of connect and disconnect actions
    this->ui->actionConnect->setDisabled(false);
    this->ui->actionDisconnect->setDisabled(true);
    this->ui->actionMetadata->setDisabled(true);
    this->ui->actionMount->setDisabled(true);
    this->ui->actionRefresh->setDisabled(true);
    this->ui->actionFind->setDisabled(true);
    this->ui->actionQueueStat->setDisabled(true);
    this->ui->actionNewColl->setDisabled(true);
    this->ui->actionDelete->setDisabled(true);
    this->ui->actionUpload->setDisabled(true);
    this->ui->actionUploadDirectory->setDisabled(true);
    this->ui->actionDownload->setDisabled(true);
    this->ui->actionFind->setDisabled(true);

    // disable settings controls
    this->ui->verifyChecksum->setDisabled(true);
    this->ui->allowOverwrite->setDisabled(true);
    this->ui->storageResc->setDisabled(true);

    // display disconnected message
    this->ui->statusBar->showMessage("Disconnected", 5000);
    this->setWindowTitle("iRODS Grid Browser");
}


void RodsMainWindow::unregisterMetadataWindow(std::string objPath)
{
    // try to find metadata editor instance
    std::map<std::string, RodsMetadataWindow*>::iterator i = this->metaEditors.find(objPath);

    // if found, unregister it
    if (i != this->metaEditors.end())
    {
        delete (i->second);
        this->metaEditors.erase(i);
    }
}

void RodsMainWindow::unregisterQueueWindow()
{
    // sanity check
    if (this->queueWindow)
    {
        // delete and unregister queue window object
        delete (this->queueWindow);
        this->queueWindow = NULL;
    }
}

void RodsMainWindow::mountPath()
{
    bool ok = false;

    // make dialog for prompting user
    QString path = QInputDialog::getText(this, "Mount Path", "iRODS path:",
                                         QLineEdit::Normal, "/", &ok);

    // user decided to go ahead
    if (ok)
    {
        std::vector<Kanki::RodsObjEntryPtr> tmp;
        int status = 0;

        // try to read collection to see that it's readable (by the user)
        if ((status = this->conn->readColl(path.toStdString(), &tmp)) >= 0)
        {
                // get the model in use and add the mount point to the model
                RodsObjTreeModel *model = static_cast<RodsObjTreeModel*>(this->ui->rodsObjTree->model());
                model->addMountPoint(path.toStdString());
        }

        else
            this->reportError("Mount collection error", "Read collection failed", status);
    }
}

void RodsMainWindow::startModalProgressDialog(QString text, int value, int maxValue)
{
    this->progress->setLabelText(text);
    this->progress->setWindowModality(Qt::WindowModal);
    this->progress->setFixedWidth(this->width() / 3);
    this->progress->setEnabled(true);
    this->progress->setMaximum(maxValue);
    this->progress->setValue(value);
    this->progress->show();
}

void RodsMainWindow::setProgress(QString text, int currentValue)
{
    this->progress->setValue(currentValue);
    this->progress->setLabelText(text);
}

void RodsMainWindow::setProgressMarquee(QString text)
{
    this->progress->setLabelText(text);
    this->progress->setMaximum(0);
    this->progress->setMinimum(0);
}

void RodsMainWindow::endModalProgressDialog()
{
    this->progress->hide();
}

void RodsMainWindow::doQueueStatOpen()
{
    // if not yet instantiated, do it
    if (!this->queueWindow)
    {
        this->queueWindow = new RodsQueueWindow(this->conn);

        connect(this->queueWindow, &RodsQueueWindow::unregister, this,
                &RodsMainWindow::unregisterQueueWindow);
    }

    // show and raise queue window to front
    queueWindow->show();
    queueWindow->raise();

    // set application active window to queue window
    QApplication::setActiveWindow(queueWindow);
}

void RodsMainWindow::doMetadataEditorOpen()
{
    QModelIndex curIndex = this->ui->rodsObjTree->currentIndex();

    // sanity check for index validity
    if (curIndex.isValid())
    {
        RodsObjTreeItem *selection = static_cast<RodsObjTreeItem*>(curIndex.internalPointer());

        // if we have a valid selection
        if (selection)
        {
            Kanki::RodsObjEntryPtr objEntry = selection->getObjEntryPtr();

            // if we are talking about a proper item
            if (objEntry)
            {
                RodsMetadataWindow *metaWindow = NULL;

                // if we already don't have an editor window, make one
                if (this->metaEditors.find(objEntry->getObjectFullPath()) != this->metaEditors.end())
                    metaWindow = this->metaEditors.at(objEntry->getObjectFullPath());

                else {
                    metaWindow = new RodsMetadataWindow(conn, selection->getObjEntryPtr());

                    connect(metaWindow, &RodsMetadataWindow::unregister, this,
                            &RodsMainWindow::unregisterMetadataWindow);

                    this->metaEditors[objEntry->getObjectFullPath()] = metaWindow;
                }

                // show window and bring it to front
                metaWindow->show();
                metaWindow->raise();
                QApplication::setActiveWindow(metaWindow);
            }
        }
    }
}

void RodsMainWindow::doDownload()
{
    QModelIndex curIndex = this->ui->rodsObjTree->currentIndex();
    QStringList destPathSelection;
    std::string destPath;

    if (curIndex.isValid())
    {
        RodsObjTreeItem *selection = static_cast<RodsObjTreeItem*>(curIndex.internalPointer());
        Kanki::RodsObjEntryPtr objEntry = selection->getObjEntryPtr();

        if (objEntry)
        {
            QFileDialog destSelection(this, "Select Destination Directory");
            destSelection.setFileMode(QFileDialog::Directory);
            destSelection.setViewMode(QFileDialog::Detail);
            destSelection.setLabelText(QFileDialog::Accept, "Destination");

            // get destination from selection dialog
            if (destSelection.exec())
                destPathSelection = destSelection.selectedFiles();

            // if user cancelled
            if (!destPathSelection.count())
                return;

            // create worker thread for downloading
            RodsDownloadThread *downloadWorker = new RodsDownloadThread(this->conn,
                                                                        objEntry,
                                                                        destPathSelection.at(0).toStdString(),
                                                                        this->verifyChecksum,
                                                                        this->allowOverwrite);

            QString title = QString("Downloading '") + objEntry->getObjectName().c_str() + "'";
            RodsTransferWindow *transferWindow = new RodsTransferWindow(title);

            // connect worker thread signals to transfer window slots
            connect(downloadWorker, &RodsDownloadThread::setupProgressDisplay, transferWindow,
                    &RodsTransferWindow::setupMainProgressBar);
            connect(downloadWorker, &RodsDownloadThread::progressUpdate, transferWindow,
                    &RodsTransferWindow::updateMainProgress);
            connect(downloadWorker, &RodsDownloadThread::setupSubProgressDisplay, transferWindow,
                    &RodsTransferWindow::setupSubProgressBar);
            connect(downloadWorker, &RodsDownloadThread::subProgressUpdate, transferWindow,
                    &RodsTransferWindow::updateSubProgress);
            connect(downloadWorker, &RodsDownloadThread::progressMarquee, transferWindow,
                    &RodsTransferWindow::progressMarquee);

            // error reporting signal connects to the error log window slot
            connect(downloadWorker, &RodsDownloadThread::reportError, this->errorLogWindow,
                    &RodsErrorLogWindow::logError);

            // connect thread finished signal to Qt object deletion mechanisms
            connect(downloadWorker, &RodsDownloadThread::finished,
                    &QObject::deleteLater);
            connect(downloadWorker, &RodsDownloadThread::finished, transferWindow,
                    &QObject::deleteLater);

            // we use a brute force terminate signal for now
            connect(transferWindow, &RodsTransferWindow::cancelRequested, downloadWorker,
                    &QThread::terminate);

            transferWindow->show();
            transferWindow->raise();
            QApplication::setActiveWindow(transferWindow);

            // start worker thread
            downloadWorker->start();
        }
    }
}

void RodsMainWindow::doUpload(bool uploadDirectory)
{
    QStringList fileNames;
    std::string destCollPath;

    // get current collection from selection
    destCollPath = getCurrentRodsCollPath();

    // get system file selection dialog trough qt
    QFileDialog fileSelection(this, "Select items to upload");

    if (uploadDirectory)
    {
        fileSelection.setFileMode(QFileDialog::Directory);
        fileSelection.setLabelText(QFileDialog::Accept, "Upload directory");
    }

    else {
        fileSelection.setFileMode(QFileDialog::ExistingFiles);
        fileSelection.setLabelText(QFileDialog::Accept, "Upload file(s)");
    }

    fileSelection.setViewMode(QFileDialog::Detail);

    // get file names from dialog
    if (fileSelection.exec())
        fileNames = fileSelection.selectedFiles();

    // if user cancelled
    if (!fileNames.count())
        return;

    // create a worker thread for the upload
    RodsUploadThread *uploadWorker = NULL;

    if (uploadDirectory)
        uploadWorker = new RodsUploadThread(this->conn, fileNames.at(0).toStdString(),
                                            destCollPath, this->currentResc);
    else
        uploadWorker = new RodsUploadThread(this->conn, fileNames, destCollPath,
                                            this->currentResc);

    QString title = QString("Uploading to '") + destCollPath.c_str() + "'";
    RodsTransferWindow *transferWindow = new RodsTransferWindow(title);

    // connect upload signals to transfer window slots
    connect(uploadWorker, &RodsUploadThread::setupProgressDisplay, transferWindow,
            &RodsTransferWindow::setupMainProgressBar);
    connect(uploadWorker, &RodsUploadThread::progressMarquee, transferWindow,
            &RodsTransferWindow::progressMarquee);
    connect(uploadWorker, &RodsUploadThread::progressUpdate, transferWindow,
            &RodsTransferWindow::updateMainProgress);

    // error reporting signal connects to the error log window slot
    connect(uploadWorker, &RodsUploadThread::reportError, this->errorLogWindow,
            &RodsErrorLogWindow::logError);

    // refresh signal connects directly to object model refresh slot
    connect(uploadWorker, &RodsUploadThread::refreshObjectModel, this->model,
            &RodsObjTreeModel::refreshAtPath);

    // connect thread finished signal to Qt object deletion mechanisms
    connect(uploadWorker, &RodsUploadThread::finished, &QObject::deleteLater);
    connect(uploadWorker, &RodsUploadThread::finished, transferWindow,
            &QObject::deleteLater);

    // we use a brute force terminate signal for now
    connect(transferWindow, &RodsTransferWindow::cancelRequested, uploadWorker, &QThread::terminate);

    transferWindow->show();
    transferWindow->raise();
    QApplication::setActiveWindow(transferWindow);

    // start upload thread execution
    uploadWorker->start();
}

void RodsMainWindow::doRefreshTreeView(QModelIndex atIndex)
{
    QModelIndex curIndex = atIndex;

    // first process pending Qt events
    qApp->processEvents();

    // get current index and pointer to model object
    if (!curIndex.isValid())
        curIndex = this->ui->rodsObjTree->currentIndex();

    // if no valid index (no selection), assume initial mount point
    if (!curIndex.isValid())
        curIndex = this->model->index(0, 0, QModelIndex());

    // get object pointers for selected item and tree view model
    RodsObjTreeItem *selection = static_cast<RodsObjTreeItem*>(curIndex.internalPointer());

    // if item is a proper item
    if (selection->getObjEntryPtr())
    {
        // if a collection was selected, refresh it
        if (selection->getObjEntryPtr()->objType == COLL_OBJ_T)
            this->refreshObjectModelAtIndex(curIndex);

        // if a data object was selected, refresh parent collection
        else if (selection->getObjEntryPtr()->objType == DATA_OBJ_T)
            this->refreshObjectModelAtIndex(this->model->parent(curIndex));
    }

    else
        this->refreshObjectModelAtIndex(curIndex);
}

void RodsMainWindow::doRodsConnect()
{
    // create a thread for connecting
    RodsConnectThread *connThread = new RodsConnectThread();

    // connect connect thread signals to main window slots
    connect(connThread, &RodsConnectThread::progressUpdate, this,
            &RodsMainWindow::setProgress);
    connect(connThread, &RodsConnectThread::reportError, this,
            &RodsMainWindow::reportError);
    connect(connThread, &RodsConnectThread::setConnection, this,
            &RodsMainWindow::setConnection);

    // when failure is reported enter disconnected state
    connect(connThread, &RodsConnectThread::failure, this,
            &RodsMainWindow::endModalProgressDialog);
    connect(connThread, &RodsConnectThread::failure, this,
            &RodsMainWindow::enterDisconnectedState);

    // when connect thread was successful, enter connected state
    connect(connThread, &RodsConnectThread::success, this,
            &RodsMainWindow::endModalProgressDialog);
    connect(connThread, &RodsConnectThread::success, this,
            &RodsMainWindow::enterConnectedState);

    // after therad completion have the object deleted
    connect(connThread, &RodsConnectThread::finished, &QObject::deleteLater);

    // connect cancel signal to thread termination
    connect(this->progress, &QProgressDialog::canceled, connThread,
            &QThread::terminate);

    // enter into a modal state and start worker thread
    this->startModalProgressDialog("Preparing to Connect...", 1, 3);
    connThread->start();
}

void RodsMainWindow::doRodsDisconnect()
{
    // sanity check that there is a connection object
    if (this->conn)
    {
        // disconnect from iRODS
        this->conn->disconnect();

        // delete connection object
        delete(this->conn);
        this->conn = NULL;

        // enter disconnected state
        this->enterDisconnectedState();
    }
}

void RodsMainWindow::doCreateCollection()
{
    int status = 0;
    bool ok = false;

    // make dialog for prompting user
    QString collName = QInputDialog::getText(this, "New Collection", "Collection name:",
                                             QLineEdit::Normal, "New Collection", &ok);

    // if user pressed cancel, do nothing
    if (!ok)
        return;

    // if we have a valid collection name
    if (!collName.isEmpty())
    {
        // get current collection path from selection
        std::string curCollPath = this->getCurrentRodsCollPath();

        // construct collection path (TODO: sanitize string)
        std::string collPath = curCollPath + "/" + collName.toStdString();

        // make new collection into path
        if ((status = this->conn->makeColl(collPath, false)) < 0)
            this->reportError("Create collection error", "Create collection failed", status);

        // refresh tree view on success
        else
            this->doRefreshTreeView();
    }

    else
        this->reportError("Emtpy collection name entered!", QString(), 0);
}

void RodsMainWindow::doDelete()
{
    int status = 0;
    QModelIndex curIndex = this->ui->rodsObjTree->currentIndex();

    // first process pending Qt events
    qApp->processEvents();

    // proceed only if we have a valid index
    if (curIndex.isValid())
    {
        // get selected item from index
        RodsObjTreeItem *selection = static_cast<RodsObjTreeItem*>(curIndex.internalPointer());
        RodsObjTreeModel *model = static_cast<RodsObjTreeModel*>(this->ui->rodsObjTree->model());

        // we do delete only for proper items
        if (selection->getObjEntryPtr())
        {
            QMessageBox confirm;
            Kanki::RodsObjEntryPtr itemData = selection->getObjEntryPtr();

            // we must also get the parent object for collection path
            RodsObjTreeItem *parentItem = selection->parent();
            QModelIndex parent = curIndex.parent();

            // configure message box
            QPushButton *deleteButton = confirm.addButton(tr("Delete"), QMessageBox::ActionRole);
            QPushButton *cancelButton = confirm.addButton(tr("Cancel"), QMessageBox::ActionRole);
            confirm.setDefaultButton(cancelButton);
            confirm.setIcon(QMessageBox::Question);

            // if we are deleting a collection
            if (itemData->objType == COLL_OBJ_T)
            {
                confirm.setText("Delete iRODS Collection");

                std::string confirmText = "Are you sure you want to recursively delete '" +
                        itemData->getObjectFullPath() + "' ?";

                confirm.setInformativeText(confirmText.c_str());
                confirm.exec();

                if (confirm.clickedButton() == cancelButton)
                    return;

                // try to delete collection
                if ((status = this->conn->removeColl(itemData->objName)) < 0)
                    this->reportError("Delete collection error", "iRODS API error", status);

                // on success refresh view
                else {
                    model->removeRows(curIndex.row(), 1, parent);
                }
            }

            else if (itemData->objType == DATA_OBJ_T)
            {
                confirm.setText("Delete iRODS Data Object");

                std::string confirmText = "Are you sure you want to delete '" +
                        itemData->getObjectName() + "' ?";

                confirm.setInformativeText(confirmText.c_str());
                confirm.exec();

                if (confirm.clickedButton() != deleteButton)
                    return;

                // sanity check
                if (parentItem)
                {
                    // get full object path for remove operation
                    std::string objPath = itemData->getObjectFullPath();

                    // try to remove data object
                    if ((status = this->conn->removeObj(objPath)) < 0)
                        this->reportError("Delete object error", "iRODS API error", status);

                    // on success refresh
                    else {
                        model->removeRows(curIndex.row(), 1, parent);
                    }
                }
            }
        }
    }
}

void RodsMainWindow::reportError(QString msgStr, QString errorStr, int errorCode)
{
    // create new message box object and detail string
    QMessageBox errorMsg;
    QString detailStr = errorStr + "\n" + "Status: " + QVariant(errorCode).toString();

    // set strings to message box and block until user clicked ok
    errorMsg.setText(msgStr);
    errorMsg.setDetailedText(detailStr);
    errorMsg.setIcon(QMessageBox::Critical);
    errorMsg.exec();

    // log the error
    this->logError(msgStr, errorStr, errorCode);
}

std::string RodsMainWindow::getCurrentRodsCollPath()
{
    std::string currentCollPath;

    // get index for current selection
    QModelIndex curIndex = this->ui->rodsObjTree->currentIndex();

    // if no index, current collection is irods home
    if (!curIndex.isValid())
        currentCollPath = this->conn->rodsHome();

    else {
        // get selected item and its parent item
        RodsObjTreeItem *selection = static_cast<RodsObjTreeItem*>(curIndex.internalPointer());
        RodsObjTreeItem *parent = selection->parent();

        // choose depending on selected object type, collection or data object
        if (selection->getObjEntryPtr() && selection->getObjEntryPtr()->objType == COLL_OBJ_T)
            currentCollPath = selection->getObjEntryPtr()->objName;

        else if (parent->getObjEntryPtr())
            currentCollPath = parent->getObjEntryPtr()->objName;

        else
            currentCollPath = selection->mountPoint();
    }

    return (currentCollPath);
}

QModelIndex RodsMainWindow::getCurrentRodsObjIndex()
{
    QModelIndex index = this->ui->rodsObjTree->currentIndex();

    // in the case there's no valid selection, index at first mount
    if (!index.isValid())
        return (this->model->index(0, 0, QModelIndex()));

    return (index);
}

void RodsMainWindow::showAbout()
{
    QString versionStr = "Version: " VERSION "\n\n";

    if (strlen(BUILD_TAG))
        versionStr += "Build: " BUILD_TAG "\n\n";

    if (strlen(BUILD_HOST))
        versionStr += "Build host: " BUILD_HOST "\n\n";

    versionStr += LICENSE;

    QMessageBox::about(this, "About Kanki irodsclient", versionStr);
}

void RodsMainWindow::refreshResources()
{
    int status = 0;

    // sanity checks for a ready connection
    if (!this->conn || !this->conn->isReady())
        return;

    std::string defResc = this->conn->rodsDefResc();
    this->currentResc = defResc;

    // setup a genquery for resource names and comments
    Kanki::RodsGenQuery rescQuery(this->conn);
    rescQuery.addQueryAttribute(COL_R_RESC_NAME);
    rescQuery.addQueryAttribute(COL_R_RESC_COMMENT);
    rescQuery.addQueryAttribute(COL_R_RESC_PARENT);

    // we omit bundleResc
    rescQuery.addQueryCondition(COL_R_RESC_NAME, Kanki::RodsGenQuery::isNotEqual,
                                "bundleResc");

    // try to execute genquery
    if ((status = rescQuery.execute()) < 0)
        this->reportError("Error while refreshing available iRODS storage resources",
                         "iRODS GenQuery error", status);

    // on success, add found resources with their comments to the combo box
    else {
        std::vector<std::string> resources = rescQuery.getResultSetForAttr(COL_R_RESC_NAME);
        std::vector<std::string> comments = rescQuery.getResultSetForAttr(COL_R_RESC_COMMENT);
        std::vector<std::string> parents = rescQuery.getResultSetForAttr(COL_R_RESC_PARENT);

        for (unsigned int i = 0; i < resources.size() && i < comments.size() && i < parents.size(); i ++)
        {
            QString parentStr = parents.at(i).c_str();

            if (!parentStr.length())
            {
                QString rescStr = resources.at(i).c_str();
                QString rescDesc = rescStr;

                // add comment if there is one and truncate long ones
                if (comments.at(i).length())
                {
                    rescDesc += " (";

                    if (comments.at(i).length() > 32)
                        rescDesc += comments.at(i).substr(0,31).c_str() + QString("...");

                    else
                        rescDesc += comments.at(i).c_str();

                    rescDesc += QString(")");
                }

                // add new item and select it if it's the user default
                this->ui->storageResc->addItem(rescDesc, rescStr);

                // select (first by default) or user-defined default resource
                if ((i == 0) || (!defResc.compare(resources.at(i))))
                {
                    this->ui->storageResc->setCurrentIndex(i);
                    this->currentResc = rescStr.toStdString();
                }
            }
        }
    }
}

void RodsMainWindow::openErrorLog()
{
    this->errorLogWindow->show();
    this->errorLogWindow->raise();

    QApplication::setActiveWindow(this->errorLogWindow);
    this->ui->statusBar->clearMessage();
    this->ui->actionErrorLog->setText("Error Log");
}

void RodsMainWindow::errorsReported(unsigned int errorCount)
{
    this->ui->actionErrorLog->setDisabled(false);
    this->ui->actionErrorLog->setToolTip("Errors reported in this session (" +
                                         QVariant(errorCount).toString() + ")");

    this->ui->statusBar->showMessage("Errors reported, see error log!");
    this->ui->actionErrorLog->setText("Error Log (NEW!)");
}

void RodsMainWindow::openFindWindow()
{
    // create and setup new instance if necessary
    if (!this->findWindow)
    {
        this->findWindow = new RodsFindWindow(this->conn);

        connect(this->findWindow, &RodsFindWindow::unregister,
                this, &RodsMainWindow::unregisterFindWindow);
        connect(this->findWindow, &RodsFindWindow::selectObj,
                this, &RodsMainWindow::selectRodsObject);
    }

    // show and activate window
    this->findWindow->show();
    this->findWindow->raise();
    QApplication::setActiveWindow(this->findWindow);

}

void RodsMainWindow::unregisterFindWindow()
{
    // sanity check
    if (this->findWindow)
    {
        // delete object and clear pointer
        delete (this->findWindow);
        this->findWindow = NULL;
    }
}

void RodsMainWindow::selectRodsObject(QString objPath)
{

}

void RodsMainWindow::on_actionConnect_triggered()
{
    this->doRodsConnect();
}

void RodsMainWindow::on_actionDisconnect_triggered()
{
    this->doRodsDisconnect();
}

void RodsMainWindow::on_actionMetadata_triggered()
{
    this->doMetadataEditorOpen();
}

void RodsMainWindow::on_actionUpload_triggered()
{
    this->doUpload();
}

void RodsMainWindow::on_actionDelete_triggered()
{
    this->doDelete();
}

void RodsMainWindow::on_actionRefresh_triggered()
{
    this->doRefreshTreeView();
}

void RodsMainWindow::on_actionNewColl_triggered()
{
    this->doCreateCollection();
}

void RodsMainWindow::on_actionQueueStat_triggered()
{
    this->doQueueStatOpen();
}

void RodsMainWindow::on_actionDownload_triggered()
{
    this->doDownload();
}

void RodsMainWindow::on_viewSize_valueChanged(int value)
{
    this->ui->rodsObjTree->setIconSize(QSize(value, value));
}

void RodsMainWindow::on_actionMount_triggered()
{
    this->mountPath();
}

void RodsMainWindow::on_verifyChecksum_toggled(bool checked)
{
    this->verifyChecksum = checked;
}

void RodsMainWindow::on_allowOverwrite_toggled(bool checked)
{
    this->allowOverwrite = checked;
}

void RodsMainWindow::on_actionAbout_triggered()
{
    this->showAbout();
}

void RodsMainWindow::on_actionUploadDirectory_triggered()
{
    this->doUpload(true);
}

void RodsMainWindow::on_storageResc_activated(const QString &arg1)
{
    (void)arg1;

    // fetch resource name and if valid, set it as current resc
    QString rescStr = this->ui->storageResc->currentData().toString();

    if (rescStr.length())
        this->currentResc = rescStr.toStdString();
}

void RodsMainWindow::on_actionErrorLog_triggered()
{
    this->openErrorLog();
}

void RodsMainWindow::on_actionFind_triggered()
{
    this->openFindWindow();
}
